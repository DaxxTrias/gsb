#pragma once
#include <PxActor.h>
#include <atomic>
#include <memory>
#include <vector>

struct bodyData {
	physx::PxVec3 pos;
	physx::PxVec3 vel;
	float mass;
};

extern std::shared_ptr<std::vector<bodyData>> bodys;
extern std::atomic<bool> keepRunning;

int updatePhysicsThread();

void stopUpdatePhysicsThread();