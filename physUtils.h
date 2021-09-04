#pragma once
#include <PxActor.h>
#include <atomic>
#include <memory>
#include <vector>

struct bodyData {
	physx::PxVec3 pos;
	float mass;
};

std::vector<bodyData> generateBodyData();
bodyData getPlyByMass(std::vector<bodyData> &bodys);