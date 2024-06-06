#include "physics.h"
#include "gameHooks.h"
#include <PxRigidActor.h>
#include <PxScene.h>
#include <PxAggregate.h>
#include <PxRigidBody.h>
#include <PxRigidStatic.h>
#include <thread>
#include <atomic>
#include <iostream>
#include <unordered_map>
#include <exception>

struct CachedPoseData {
    physx::PxVec3 pos;
    float mass;
    bool isValid;
};

std::unordered_map<int, CachedPoseData> poseCache;
std::shared_ptr<std::vector<bodyData>> bodys = std::make_shared<std::vector<bodyData>>();
std::atomic<bool> keepRunning(true);

CachedPoseData getCachedPose(physx::PxRigidActor* rigid, int index) {
    if (poseCache.find(index) != poseCache.end()) {
        return poseCache[index];
    }

    CachedPoseData data = {};
    try {
        if (rigid == nullptr) {
            // lua GC constantly shuffles memory around, we should expect a certain amount of nulls when using this particular method
            //todo: ie look for alternate player positioning coords (playerEnt maybe?)
            data.isValid = false;
            throw std::runtime_error("rigid pointer is null");
        }
        physx::PxTransform pose = {};
        if (data.isValid)
        {
            pose = rigid->getGlobalPose();
        }
        if (!pose.isValid()) {
            data.isValid = false;
            return data;
        }
        data.pos = pose.p;
        physx::PxRigidBody* body = rigid->is<physx::PxRigidBody>();
        data.mass = (body != nullptr) ? body->getMass() : -1;
        data.isValid = true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught while processing rigid actor: " << e.what() << std::endl;
        data.isValid = false;
    }

    poseCache[index] = data;
    return data;
}

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

                physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>(); // now this is getting an access violation. null deref?
                if (rigid == nullptr || (uint64_t)rigid > 0xFFFF'FFFF'FFFF'0000) {
                    continue;
                }

                CachedPoseData cachedPose = getCachedPose(rigid, i);
                if (!cachedPose.isValid) {
                    continue;
                }

                updating->push_back({ cachedPose.pos, cachedPose.mass });
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

        // Clear the cache for the next iteration
        poseCache.clear();
    }

    return 0;
}

void stopUpdatePhysicsThread() {
    keepRunning = false;
}
