#pragma once
#include <PxActor.h>
#include "pxVec3.h"
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxRigidBody.h>
#include <atomic>
#include <memory>
#include <vector>

struct bodyData {
    physx::PxVec3 pos;
    physx::PxVec3 vel;
    float mass;
};

std::vector<bodyData> generateBodyData();
bodyData getPlyByMass(std::vector<bodyData>& bodys);

// Define the extern variables if not already defined
extern std::atomic<bool> killSwitch;
extern PhysListArray* physList;
extern std::atomic<uint32_t> maxObjects;
extern uint64_t objectManager;