#include <cstdint>
#include <foundation/Px.h>
#include <foundation/PxVec3.h>

class localPlayer
{
public:
	char pad_0000[1408]; //0x0000
	physx::PxVec3 unkvec; //0x0580
	physx::PxVec3 unkvec2; //0x058C
	char pad_0598[576]; //0x0598
	physx::PxVec3 unkvec3; //0x07D8 somehow related to XYZ
	char pad_07E4[20]; //0x07E4
	int32_t N000038B5; //0x07F8 playerSpawnedInTime?
	char pad_07FC[176]; //0x07FC
	bool playerFlying; //0x08AC
	char pad_08AD[3]; //0x08AD
	int32_t playerVerticality; //0x08B0
	float playerStrafing; //0x08B4
	float playerForward; //0x08B8
	char pad_08BC[3]; //0x08BC
	bool playerJetpacking; //0x08BF
	char pad_08C0[24]; //0x08C0
	float playerStamina; //0x08D8
	char pad_08DC[10]; //0x08DC
	bool playerCrawling; //0x08E6
	bool playerCrouched; //0x08E7
	char pad_08E8[6]; //0x08E8
	bool playerGrounded; //0x08EE
	char pad_08EF[1]; //0x08EF
	bool playerGravityHolding; //0x08F0
	char pad_08F1[6]; //0x08F1
	bool playerSprinting; //0x08F7
	bool playerSprinting2; //0x08F8
	char pad_08F9[215]; //0x08F9
	physx::PxVec3 delayedPlayerXYZ; //0x09D0 seems to trigger when entering/exiting pilot seat
	char pad_09DC[516]; //0x09DC
	float charAttachment; //0x0BE0
	float charAttachment2; //0x0BE4
	float charAttachment3; //0x0BE8
	float charAttachment4; //0x0BEC
	char pad_0BF0[92]; //0x0BF0
	physx::PxVec3 playerVelocity; //0x0C4C
	physx::PxVec3 playerRotation; //0x0C58
	char pad_0C60[8]; //0x0C60
	int32_t N00000201; //0x0C68 jump related
	int32_t N00003B74; //0x0C6C
	char pad_0C70[68]; //0x0C70
	int32_t N00003B5F; //0x0CB4 something jetpack related?
	int32_t N00000208; //0x0CB8 see module+2F4B33D
	char pad_0CBC[2140]; //0x0CBC
}; //Size: 0x1518
//static_assert(sizeof(localPlayer) == 0x1518);