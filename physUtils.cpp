#include "physUtils.h"
#include "gameHooks.h"
#include <PxActor.h>
#include <PxRigidBody.h>

inline bool cmpf(float A, float B, float epsilon = 0.005f) {
	return (fabs(A - B) < epsilon);
}

std::vector<bodyData> generateBodyData() {
	std::vector<bodyData> bodys;

	if (physList != nullptr) {
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
				physx::PxVec3 pos = rigid->getGlobalPose().p;
				bool isBody = actor->is<physx::PxRigidBody>() != nullptr;
				bool isStatic = actor->is<physx::PxRigidBody>() != nullptr;
				float mass = -1;
				if (isBody) {
					physx::PxRigidBody* body = actor->is<physx::PxRigidBody>();
					mass = body->getMass();
					bodys.push_back({ pos, mass });
				}
			}
			else {
				if ((physList[i].id & 0xFFFFFF) != (i & 0xFFFFFF)) {
					break;
				}
			}
		}
	}

	return bodys;
}

bodyData getPlyByMass(std::vector<bodyData>& bodys) {
	for (bodyData body : bodys) {
		if (cmpf(body.mass, 2.287f, 0.01f)) {
			return body;
		}
	}
}