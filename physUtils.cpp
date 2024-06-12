#include "physUtils.h"
#include "gameHooks.h"
#include <PxActor.h>
#include <PxRigidBody.h>
#include <PxRigidStatic.h>
#include "killSwitch.h"

inline bool cmpf(float A, float B, float epsilon = 0.005f) {
    return (fabs(A - B) < epsilon);
}

std::vector<bodyData> generateBodyData() {
    std::vector<bodyData> bodys;

    if (killSwitch.load())
        return bodys;

    if (physList != nullptr) {
        for (int i = 0; i < maxObjects; i++) {
            if (physList[i].entry != nullptr
                && (physList[i].id & 0xFFFFFF) == i
                && ((physList[i].entry->id & 0xFFFFFF) == (physList[i].id & 0xFFFFFF))) {

                physx::PxActor* actor = physList[i].entry->actor;
                if (actor == nullptr) {
                    return {};
                }

                physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>(); // crashes when opening SSC?
                if (rigid == nullptr || (uint64_t)rigid > 0xFFFF'FFFF'FFFF'0000) {
                    return {};
                }
                else
                    rigid->getGlobalPose().isValid();

                physx::PxVec3 pos = {};

                try
                {
                    pos = rigid->getGlobalPose().p;
                }
                catch (const std::exception& e)
                {
                    throw;
                    return {};
                }

                bool isStatic = actor->is<physx::PxRigidStatic>() != nullptr;
                bool isBody = actor->is<physx::PxRigidBody>() != nullptr;

                if (isBody) {
                    physx::PxRigidBody* body = actor->is<physx::PxRigidBody>();
                    float mass = body->getMass();
                    if (mass > 1.0f)
                    {
                        int actorType = actor->getType();
                        if (actorType == physx::PxActorType::eRIGID_DYNAMIC)
						{
                            //fprintf(stdout, "Dynamic actor\n");
						}
						else if (actorType == physx::PxActorType::eRIGID_STATIC)
						{
                            fprintf(stdout, "Static actor\n");
						}
						else if (actorType == physx::PxActorType::eARTICULATION_LINK)
						{
                            fprintf(stdout, "Articulation link\n");
						}
						else if (actorType == physx::PxActorType::eACTOR_COUNT)
						{
                            fprintf(stdout, "Actor count\n");
						}
						else
						{
                            fprintf(stdout, "Unknown actor type\n");
						}
                    }
                    physx::PxVec3 vel = body->getLinearVelocity();
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
// 2nd entrat looks a lot like personal ship (pawnVehicle?)
bodyData getPlyByMass(std::vector<bodyData>& bodys) {
    //todo: in instances of duo play, this method can result in thinking the other person is the main player.
    for (bodyData body : bodys) {
        if (cmpf(body.mass, 2.287f, 0.01f)) {
            return body;
        }
    }
    return {}; // Return a default-constructed bodyData if no match is found
}