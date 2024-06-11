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
        CachedPoseData cachedData = poseCache[index];
        if (cachedData.isValid) {
            return cachedData;
        }
    }

    CachedPoseData data = {};
    data.isValid = false;

    // lua GC constantly shuffles memory around, we should expect a certain amount of nulls when using this particular method
    //todo: ie look for alternate player positioning coords (playerEnt maybe?)
    if (rigid == nullptr) {
        // Lua GC constantly shuffles memory around, we should expect a certain amount of nulls when using this particular method
        std::cerr << "rigid pointer is null" << std::endl;
        return data;
    }

    try {
        // Revalidate the rigid pointer before accessing
        if (!rigid->getGlobalPose().isValid()) {
            std::cerr << "Invalid global pose for rigid actor" << std::endl;
            return data;
        }

        // Copy the global pose to avoid relying on the pointer
        physx::PxTransform pose = rigid->getGlobalPose(); // still does a crash
        if (!pose.isValid()) {
            std::cerr << "Invalid global pose for rigid actor" << std::endl;
            return data;
        }

        data.pos = pose.p;

        physx::PxRigidBody* body = rigid->is<physx::PxRigidBody>();
        if (body != nullptr) {
            data.mass = body->getMass();
            data.vel = body->getLinearVelocity();
        }
        else {
            data.mass = -1;
            data.vel = physx::PxVec3(0, 0, 0);
        }

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
        for (uint64_t i = 0; i < maxObjects; ++i) {

            if (physList[i].entry != nullptr
                && (physList[i].id & 0xFFFFFF) == i
                && ((physList[i].entry->id & 0xFFFFFF) == (physList[i].id & 0xFFFFFF)))
            {
                physx::PxActor* actor = nullptr;

                try {
                    actor = physList[i].entry->actor;
                    if (actor == nullptr) {
                        continue;
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Exception caught while accessing actor: " << e.what() << std::endl;
                    continue;
                }

                // Determine if the actor is a PxRigidActor
                physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>();
                if (rigid == nullptr || (uintptr_t)rigid > 0xFFFF'FFFF'FFFF'0000) {
                    continue;
                }

                try {
                    if (rigid == nullptr) {
                        continue;
                    }
                    CachedPoseData cachedPose = getCachedPose(rigid, i);
                    if (!cachedPose.isValid) {
                        continue;
                    }

                    updating->push_back(bodyData{ cachedPose.pos, cachedPose.vel, cachedPose.mass });
                }
                catch (const physx::PxErrorCallback& e) {
                    std::cerr << "PhysX exception caught while processing actor: " << std::endl;
                    continue;
                }
                catch (const std::exception& e) {
                    std::cerr << "Exception caught while processing actor: " << e.what() << std::endl;
                    continue;
                }
                catch (...) {
                    std::cerr << "Unknown exception caught while processing actor." << std::endl;
                    continue;
                }
            }
        }

        bodys = updating;

        // Clear the cache for the next iteration
        poseCache.clear();
    }

    return 0;
}

void stopUpdatePhysicsThread() {
    keepRunning = false;
}
