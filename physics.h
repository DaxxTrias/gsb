#pragma once
#include <PxActor.h>
#include <atomic>
#include <memory>
#include <vector>
#include <mutex>

struct bodyData {
	physx::PxVec3 pos;
	physx::PxVec3 vel;
	float mass;
};

extern std::shared_ptr<std::vector<bodyData>> bodys;
extern std::atomic<bool> keepRunning;
extern std::mutex bodysMutex;

int updatePhysicsThread();

void stopUpdatePhysicsThread();