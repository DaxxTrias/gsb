#include "physUtils.h"
#include <PxActor.h>
#include "killSwitch.h"
#include "gamehooks.h"
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxRigidBody.h>
#include <iostream>
#include <vector>
#include <atomic>
#include <memory>

inline bool cmpf(float A, float B, float epsilon = 0.005f) {
	return (fabs(A - B) < epsilon);
}

std::vector<bodyData> generateBodyData() {
    std::vector<bodyData> bodys;

    if (killSwitch.load())
        return bodys;

    if (physList != nullptr) {
        for (uint32_t i = 0; i < maxObjects; ++i) {
            auto& entry = physList[i];

            if (entry.entry == nullptr)
                continue;
            if (entry.id == 0)
				continue;
            if ((entry.id & 0xFFFFFF) != i ||
                ((entry.entry->id & 0xFFFFFF) != (entry.id & 0xFFFFFF)))
                continue;

            physx::PxActor* actor = entry.entry->actor;
            if (actor == nullptr) {
                continue;
            }

            // Determine if the actor is a PxRigidStatic or PxRigidBody
            bool isStatic = actor->is<physx::PxRigidStatic>() != nullptr;
            bool isBody = actor->is<physx::PxRigidBody>() != nullptr;

            if (!isStatic && !isBody) {
                continue;
            }

            physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>();
            if (rigid == nullptr || (uintptr_t)rigid > 0xFFFF'FFFF'FFFF'0000) {
                continue;
            }

            physx::PxVec3 pos = {};

            try {
                if (rigid != nullptr && !rigid->getGlobalPose().isValid()) {
                    continue;
                }
                pos = rigid->getGlobalPose().p;
            }
            catch (const std::exception& e) {
                std::cerr << "Exception caught while getting global pose: " << e.what() << std::endl;
                continue;
            }
            if (isBody) {
                physx::PxRigidBody* body = static_cast<physx::PxRigidBody*>(actor);
                float mass = body->getMass();
                physx::PxVec3 vel = body->getLinearVelocity();
                const char* rigidName = body->getName();
                if (rigidName != nullptr) {
                    bodys.push_back(bodyData{ pos, vel, mass, rigidName });
                }
				else {
					bodys.push_back(bodyData{ pos, vel, mass, "" });
				}
			}
			else {
				bodys.push_back(bodyData{ pos, physx::PxVec3(0, 0, 0), -1, "" });
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
    for (bodyData body : bodys) {
        if (cmpf(body.mass, 2.287f, 0.01f)) {
            return body;
        }
    }
    return {}; // Return a default-constructed bodyData if no match is found
}
