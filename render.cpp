#include "render.h"
#include <foundation/PxVec2.h>

static DirectX::XMMATRIX statProj{
			1, 0, 0, 0,
			0, 1.78f, 0, 0,
			0, 0, -1.9f, -1,
			0, 0, 0, 0
};

float ViewportWidth;
float ViewportHeight;

float worldView[4][4];
static physx::PxVec3 camPos;
static DirectX::XMMATRIX WorldViewProj;

float* getStaticGameWorldView() {
	return (float*)worldView;
}

void setDisplaySize(float w, float h)
{
	ViewportHeight = h;
	ViewportWidth = w;
}

DirectX::XMMATRIX& getStaticProjMat()
{
	return statProj;
}

void updateWorldViewProj() {
	worldView[3][0] = 0;
	worldView[3][3] = 1;
	DirectX::XMMATRIX localWorld = DirectX::XMMatrixTranspose(*((DirectX::FXMMATRIX*)worldView));
	WorldViewProj = DirectX::XMMatrixMultiply(localWorld, statProj);
}

void setCamPos(physx::PxVec3 camPosNew) {
	camPos = camPosNew;
}

physx::PxVec2 worldToScreen(physx::PxVec3 worldPos) {
	physx::PxVec3 localPos = camPos - worldPos;
	DirectX::XMVECTOR Pos = DirectX::XMVectorSet(localPos.x, localPos.y, localPos.z, 1.0f);

	float mx = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[0] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[0] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[0] + WorldViewProj.r[3].m128_f32[0];
	float my = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[1] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[1] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[1] + WorldViewProj.r[3].m128_f32[1];
	float mz = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[2] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[2] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[2] + WorldViewProj.r[3].m128_f32[2];
	float mw = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[3] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[3] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[3] + WorldViewProj.r[3].m128_f32[3];

	float xx, yy;
	xx = ((mx / mw) * (ViewportWidth / 2.0f)) + (ViewportWidth / 2.0f);
	yy = (ViewportHeight / 2.0f) - ((my / mw) * (ViewportHeight / 2.0f)); //- or + depends on the game

	if (mw > 0.0f) {
		return physx::PxVec2(ViewportWidth - xx, ViewportHeight - yy);
	}
	return physx::PxVec2(-100, -100);
}

physx::PxVec2 worldToScreenIgnoreDirection(physx::PxVec3 worldPos) {
	physx::PxVec3 localPos = camPos - worldPos;
	DirectX::XMVECTOR Pos = DirectX::XMVectorSet(localPos.x, localPos.y, localPos.z, 1.0f);

	float mx = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[0] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[0] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[0] + WorldViewProj.r[3].m128_f32[0];
	float my = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[1] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[1] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[1] + WorldViewProj.r[3].m128_f32[1];
	float mz = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[2] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[2] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[2] + WorldViewProj.r[3].m128_f32[2];
	float mw = Pos.m128_f32[0] * WorldViewProj.r[0].m128_f32[3] + Pos.m128_f32[1] * WorldViewProj.r[1].m128_f32[3] + Pos.m128_f32[2] * WorldViewProj.r[2].m128_f32[3] + WorldViewProj.r[3].m128_f32[3];

	float xx, yy;
	xx = ((mx / mw) * (ViewportWidth / 2.0f)) + (ViewportWidth / 2.0f);
	yy = (ViewportHeight / 2.0f) - ((my / mw) * (ViewportHeight / 2.0f)); //- or + depends on the game

	if (mw > 0.0f) {
		return physx::PxVec2(ViewportWidth - xx, ViewportHeight - yy);
	}

	physx::PxVec2 screenPos(ViewportWidth - xx, ViewportHeight - yy);
	screenPos.normalize();
	screenPos *= -ViewportWidth;

	return screenPos;
}