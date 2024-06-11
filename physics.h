#pragma once
#include <PxActor.h>
#include <atomic>
#include <memory>
#include <vector>
#include <string>

struct bodyData {
	physx::PxVec3 pos;
	physx::PxVec3 vel;
	float mass;
	std::string name;
};

extern std::shared_ptr<std::vector<bodyData>> bodys;
extern std::atomic<bool> keepRunning;

int updatePhysicsThread();

void stopUpdatePhysicsThread();