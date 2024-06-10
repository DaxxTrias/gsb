#pragma once
#include "physUtils.h"

void drawAsteroidESP(const bodyData& ply);
void drawStats(const bodyData& ply);
void drawCrosshair();
physx::PxVec3 createPxVec3(float x, float y, float z);