#include "physics.h"
#include "gameHooks.h"
#include <PxRigidActor.h>
#include <PxScene.h>
#include <PxAggregate.h>
#include <PxRigidBody.h>
#include <PxRigidStatic.h>
#include <thread>
#include <atomic>
//#include <mutex>

std::shared_ptr<std::vector<bodyData>> bodys = std::make_shared<std::vector<bodyData>>();
std::atomic<bool> keepRunning(true);

int updatePhysicsThread() {
    while (keepRunning) {
        if (physList == nullptr) {
            Sleep(1);
            continue;
        }
        
        std::shared_ptr<std::vector<bodyData>> updating = std::make_shared<std::vector<bodyData>>();

        for (int i = 0; i < 0xFFFFFF; i++) {
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

                physx::PxVec3 pos;
                if (rigid->getGlobalPose().isValid())
                {
                    pos = rigid->getGlobalPose().p;
                }
                else
                {
                    // Handle invalid pose
                    continue;
                }

                bool isBody = actor->is<physx::PxRigidBody>() != nullptr;
                bool isStatic = actor->is<physx::PxRigidStatic>() != nullptr;
                float mass = -1;
                
                if (isBody) {
                    physx::PxRigidBody* body = actor->is<physx::PxRigidBody>();
                    mass = body->getMass();
                    updating->push_back({pos, mass});
                }
            }
            else {
                if ((physList[i].id & 0xFFFFFF) != (i & 0xFFFFFF)) {
                    break;
                }
            }
        }

        // Ensure thread-safe update of bodys
        {
            //std::lock_guard<std::mutex> lock(bodys_mutex);
            bodys = updating;
        }
    }

    return 0;
}

void stopUpdatePhysicsThread() {
    keepRunning = false;
}
