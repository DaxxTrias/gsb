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
#include <stdexcept>
#include <Windows.h>
#include "killSwitch.h"

struct CachedPoseData {
    physx::PxVec3 pos;
    physx::PxVec3 vel;
    float mass;
    bool isValid;
};

std::unordered_map<int, CachedPoseData> poseCache;
std::shared_ptr<std::vector<bodyData>> bodys = std::make_shared<std::vector<bodyData>>();
std::atomic<bool> keepRunning(true);

CachedPoseData getCachedPose(physx::PxRigidActor* rigid, uint64_t indx) {
    int index = static_cast<int>(indx);

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
        data.vel = (body != nullptr) ? body->getLinearVelocity() : physx::PxVec3(0, 0, 0);
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
    while (keepRunning && !killSwitch.load()) {
        if (physList == nullptr) {
            Sleep(10);
            continue;
        }

        std::shared_ptr<std::vector<bodyData>> updating = std::make_shared<std::vector<bodyData>>();

        //todo: is this the npScene->RigidActors array?
        for (uint64_t i = 0; i < maxObjects; i++) {
            if (physList[i].entry == nullptr)
                continue;
            if (physList[i].entry != nullptr
                && (physList[i].id & 0xFFFFFF) == i
                && ((physList[i].entry->id & 0xFFFFFF) == (physList[i].id & 0xFFFFFF))) {

                // access violation here as well not handled properly
                if (physList[i].entry == nullptr || physList[i].entry->actor == nullptr) {
                    continue;
                }
                physx::PxActor* actor = {};

                try {
                    actor = physList[i].entry->actor;
                }
                catch (const std::exception& e) {
                    std::cerr << "Exception caught while processing actor: " << e.what() << std::endl;
                    continue; // Skip to the next iteration
                }

                try {
                    //todo: we should attempt to capture entity list size rather then use a hard limit
                    physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>();
                    if (rigid == nullptr || (uint64_t)rigid > 0xFFFF'FFFF'FFFF'0000) {
                        continue;
                    }

                    CachedPoseData cachedPose = getCachedPose(rigid, i);
                    if (!cachedPose.isValid) {
                        continue;
                    }

                    //updating->push_back({ cachedPose.pos, cachedPose.mass, cachedPose.vel });
                    updating->push_back(bodyData{ cachedPose.pos, cachedPose.vel, cachedPose.mass });
                }
                catch (const physx::PxErrorCallback& e) {
                    std::cerr << "PhysX exception caught while processing actor: " << std::endl;
                    continue; // Skip to the next iteration
                }
                catch (const std::exception& e) {
                    std::cerr << "Exception caught while processing actor: " << e.what() << std::endl;
                    continue; // Skip to the next iteration
                }
                catch (...) {
                    std::cerr << "Unknown exception caught while processing actor." << std::endl;
                    continue; // Skip to the next iteration
                }
            }
            else {
                if ((physList[i].id & 0xFFFFFF) != (i & 0xFFFFFF)) {
                    break;
                }
            }
        }
        {
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