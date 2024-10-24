#include "physUtils.h"
#include "gameHooks.h"
#include <PxActor.h>
#include <PxScene.h>
#include <PxRigidBody.h>
#include <PxRigidStatic.h>
#include "killSwitch.h"


using namespace physx;

inline bool cmpf(float A, float B, float epsilon = 0.005f) {
    return (fabs(A - B) < epsilon);
}

std::vector<bodyData> generateBodyData() {
    std::vector<bodyData> bodys;

    if (killSwitch.load())
        return bodys;

    uint32_t maxObjx = maxObjects * 5;

    if (physList != nullptr) {
        for (uint64_t i = 0; i <= maxObjx; i++) {
            if (physList[i].entry != nullptr
                && (physList[i].id & 0xFFFFFF) == i
                && ((physList[i].entry->id & 0xFFFFFF) == (physList[i].id & 0xFFFFFF))) {
                physx::PxActor* actor = physList[i].entry->actor;
                if (actor == nullptr) {
                    continue;
                }
                physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>();
                if (rigid == nullptr || (uint64_t)rigid > 0xFFFF'FFFF'FFFF'0000) {
                    continue;
                }
                // todo: this can cause a fatal crash when exiting from game to menu
                physx::PxVec3 pos = rigid->getGlobalPose().p;

                bool isStatic = actor->is<PxRigidStatic>() != nullptr;
                bool isBody = actor->is<PxRigidBody>() != nullptr;

                if (isBody) {
                    PxRigidBody* body = actor->is<PxRigidBody>();
                    float mass = (body != nullptr) ? body->getMass() : -1;
                    PxVec3 vel = (body != nullptr) ? body->getLinearVelocity() : physx::PxVec3(0, 0, 0);
                    if (mass > 1.0f && mass < 5.0f)
                    {
                        int actorType = actor->getType();
                        if (actorType == PxActorType::eRIGID_DYNAMIC)
						{
                            fprintf(stdout, "Rigid Dynamic Body Mass: %f\n", mass);
                            //uint32_t nbShapes = body->getNbShapes();
                            //const char* name = actor->getName();
                            //fprintf(stdout, "Dynamic actor: %s\n", name);
                            //fprintf(stdout, "Dynamic actor\n");
                            PxScene *scene = actor->getScene();
                            fprintf(stdout, "Scene: %p\n", scene);

                            PxVec3 sceneGravity = scene->getGravity();
                            fprintf(stdout, "Scene Gravity: %f, %f, %f\n", sceneGravity.x, sceneGravity.y, sceneGravity.z);

                            //PxU32 NbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
                            //PxU32 NbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
                            //fprintf(stdout, "nBActors: %u\n", NbActors);
						}
						else if (actorType == PxActorType::eRIGID_STATIC)
						{
                            fprintf(stdout, "Static actor\n");
						}
						else if (actorType == PxActorType::eARTICULATION_LINK)
						{
                            fprintf(stdout, "Articulation link\n");
						}
						else if (actorType == PxActorType::eACTOR_COUNT)
						{
                            fprintf(stdout, "Actor count\n");
						}
						else
						{
                            fprintf(stdout, "Unknown actor type\n");
						}
                    }
                    bodys.push_back(bodyData{ pos, vel, mass });
                }
                else if (isStatic) {
                    break;
                }
            }
            else {
                if ((physList[i].id & 0xFFFFFF) != (i & 0xFFFFFF)) {
                    break;
                }
            }
        }
    }

    return bodys;
}


//todo: we should investigate if this player location shuffles around
// im assuming it does, but if is more static maybe we can cache the data to not double loop the entity list
// everytime we look for an entity and the player, every frame
//followup: first position in ent table looks like player and doesnt appear to move.
// 2nd entrant looks a lot like personal ship (pawnVehicle?)
bodyData getPlyByMass(std::vector<bodyData>& bodys) {
    //todo: in instances of duo play, this method can result in thinking the other person is the main player.
    for (bodyData body : bodys) {
        if (cmpf(body.mass, 2.287f, 0.01f)) {
            return body;
        }
    }
    return {}; // Return a default-constructed bodyData if no match is found
}