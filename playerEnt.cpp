#include <cstdint>
#include <foundation/Px.h>
#include <foundation/PxVec2.h>
#include <foundation/PxVec3.h>

class playerEnt
{
public:
	char pad_0000[19]; //0x0000
	physx::PxVec2 rotationVelocity2; //0x0013
	float pitchRoll2_1; //0x0014
	float pitchRoll2_2; //0x0018
	float pitchRoll2_3; //0x001C
	float pitchRoll2_4; //0x0020
	float pitchRoll2_5; //0x0024
	float pitchRoll2_6; //0x0028
	char pad_002C[124]; //0x002C
	double playerX2; //0x00A8
	double playerY2; //0x00B0
	double playerZ2; //0x00B8
	double playerX3; //0x00C0
	double playerY3; //0x00C8
	double playerZ3; //0x00D0
	char pad_00D8[92]; //0x00D8
	float walkingVelocityX; //0x0134
	float walkingVelocityY; //0x0138
	float walkingVelocityZ; //0x013C
	float groundedPlayerX; //0x0140
	float groundedPlayerY; //0x0144
	float groundedPlayerZ; //0x0148
	char pad_014C[31]; //0x014C
	physx::PxVec3 rotationVelocity; //0x016B
	float pitchRoll1; //0x016C
	float pitchRoll2; //0x0170
	float pitchRoll3; //0x0174
	float pitchRoll4; //0x0178
	float pitchRoll5; //0x017C
	float pitchRoll6; //0x0180
	char pad_0184[36]; //0x0184
	float N000005B9; //0x01A8 something related to ships and XYZ coords?
	char pad_01AC[8]; //0x01AC
	int16_t N00002E73; //0x01B4
	int8_t collisionDetection; //0x01B6
	char pad_01B7[1]; //0x01B7
	int16_t isMoving; //0x01B8
	int16_t N00004528; //0x01BA
	char pad_01BC[12]; //0x01BC
	double playerX; //0x01C8
	double playerY; //0x01D0
	double playerZ; //0x01D8
	char pad_01E0[48]; //0x01E0
	float spawnedDuration; //0x0210
	char pad_0214[364]; //0x0214
}; //Size: 0x0380
//static_assert(sizeof(playerEnt) == 0x380);