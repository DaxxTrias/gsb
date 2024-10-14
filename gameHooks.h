#pragma once
#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <PxActor.h>
#include <atomic>
#include <cstdint>

struct Actor
{
    __int64 qword0;
    void *actorName;
    __int64 qword10;
    char gap18[54];
    unsigned __int16 unsigned___int164E;
    __int64 field_50;
    __int64 field_58;
    __int64 field_60;
    __int64 field_68;
    __int64 field_70;
    __int64 field_78;
    __int64 field_80;
    __int64 field_88;
    __int64 field_90;
    __int64 field_98;
    __int64 field_A0;
    __int64 field_A8;
    __int64 field_B0;
    __int64 field_B8;
    __int64 field_C0;
    __int64 field_C8;
    __int64 field_D0;
    __int64 field_D8;
    __int64 field_E0;
    __int64 field_E8;
    __int64 field_F0;
    __int64 field_F8;
    __int64 field_100;
    __int64 field_108;
    __int64 field_110;
    __int64 field_118;
    __int64 field_120;
    __int64 field_128;
    __int64 field_130;
    __int64 field_138;
    __int64 field_140;
    __int64 field_148;
    __int64 field_150;
    __int64 field_158;
    __int64 field_160;
    __int64 field_168;
    __int64 field_170;
    __int64 field_178;
    __int64 field_180;
    __int64 field_188;
    __int64 field_190;
    __int64 field_198;
    __int64 field_1A0;
    __int64 field_1A8;
    __int64 field_1B0;
    __int64 field_1B8;
    __int64 field_1C0;
    __int64 field_1C8;
    __int64 field_1D0;
    __int64 field_1D8;
    __int64 field_1E0;
    __int64 field_1E8;
    __int64 field_1F0;
    __int64 field_1F8;
    __int64 field_200;
    __int64 field_208;
    __int64 field_210;
    __int64 field_218;
    __int64 field_220;
    __int64 field_228;
    __int64 field_230;
    __int64 field_238;
    __int64 field_240;
    __int64 field_248;
    __int64 field_250;
    __int64 field_258;
    __int64 field_260;
    __int64 field_268;
    __int64 field_270;
    __int64 field_278;
    __int64 field_280;
    __int64 field_288;
    __int64 field_290;
    __int64 field_298;
    __int64 field_2A0;
    __int64 field_2A8;
    __int64 field_2B0;
    __int64 field_2B8;
    __int64 field_2C0;
    __int64 field_2C8;
    __int64 field_2D0;
    __int64 field_2D8;
    __int64 field_2E0;
    __int64 field_2E8;
    __int64 field_2F0;
    __int64 field_2F8;
    __int64 field_300;
    __int64 field_308;
    __int64 field_310;
    __int64 field_318;
    __int64 field_320;
    __int64 field_328;
    __int64 field_330;
    __int64 field_338;
    __int64 field_340;
    __int64 field_348;
    __int64 field_350;
    __int64 field_358;
    __int64 PhysXPrimitive;
    __int64 field_368;
    __int64 field_370;
    __int64 field_378;
    __int64 field_380;
    __int64 field_388;
    __int64 field_390;
    __int64 field_398;
};

struct ActorsListEntry
{
    Actor* actor;
    int id;
    int gap;
};

struct PhysListEntry
{
    uint64_t id;
    physx::PxActor* actor;
};

struct PhysListArray
{
    PhysListEntry* entry;
    uint64_t id;
};

struct ActorsList
{
    ActorsListEntry* array;
    size_t size;
};

struct ActorsListManager
{
    char gap0[120];
    ActorsList* actorsList;
};

struct SceneInstanceManager
{
    char gap0[64];
    ActorsListManager* actorsListManager;
};

struct SceneRoot_vt
{
    __int64 field_0;
    __int64 field_8;
    __int64 field_10;
    __int64 field_18;
    __int64 field_20;
    __int64 field_28;
    __int64 field_30;
    __int64 field_38;
    __int64 field_40;
    __int64 field_48;
    __int64 field_50;
    __int64 field_58;
    __int64 field_60;
    __int64 field_68;
    __int64 field_70;
    __int64 field_78;
    __int64 field_80;
    __int64 field_88;
    __int64 field_90;
    __int64 field_98;
    __int64 field_A0;
    __int64 field_A8;
    __int64 field_B0;
    __int64 field_B8;
    __int64 field_C0;
    __int64 field_C8;
    __int64 field_D0;
    __int64 field_D8;
    __int64 field_E0;
    __int64 field_E8;
    __int64 field_F0;
    __int64 field_F8;
    __int64 field_100;
    __int64 field_108;
    __int64 field_110;
    __int64 field_118;
    __int64 field_120;
    __int64 field_128;
    __int64 field_130;
    __int64 field_138;
    __int64 field_140;
    __int64 field_148;
    __int64 field_150;
    __int64 field_158;
    __int64 field_160;
    __int64 field_168;
    __int64 field_170;
    __int64 field_178;
    __int64 field_180;
    __int64 field_188;
    __int64 field_190;
    __int64 field_198;
    __int64 field_1A0;
    __int64 field_1A8;
    __int64 field_1B0;
    __int64 field_1B8;
    __int64 field_1C0;
    __int64 field_1C8;
    __int64 field_1D0;
    __int64 field_1D8;
    __int64 field_1E0;
    __int64 field_1E8;
    __int64 field_1F0;
    __int64 field_1F8;
    __int64 field_200;
    __int64 field_208;
    __int64 field_210;
    __int64 field_218;
    __int64 field_220;
    __int64 field_228;
    __int64 field_230;
    __int64 field_238;
    __int64 field_240;
    __int64 field_248;
    __int64 field_250;
    __int64 field_258;
    __int64 field_260;
    __int64 field_268;
};

struct SceneRoot
{
    SceneRoot_vt* vt;
    __int64 field_8;
    __int64 field_10;
    __int64 field_18;
    __int64 field_20;
    __int64 field_28;
    __int64 field_30;
    __int64 field_38;
    __int64 field_40;
    __int64 field_48;
    __int64 field_50;
    __int64 field_58;
    __int64 field_60;
    __int64 field_68;
    __int64 field_70;
    __int64 field_78;
    __int64 field_80;
    __int64 field_88;
    __int64 field_90;
    __int64 field_98;
    __int64 field_A0;
    __int64 field_A8;
    __int64 field_B0;
    __int64 field_B8;
    __int64 field_C0;
    __int64 field_C8;
};

struct asteroidStruct
{
    __int64 ptr0;       //0x00
    int ptr_data_0_0;   //0x08
    int ptr_data_0_1;   //0x0C
    __int64 ptr1;       //0x10
    int ptr_data_1_0;   //0x18
    int ptr_data_1_1;   //0x1C
    __int64 ptr2;	    //0x20
    int ptr_data_2_0;   //0x28
    int ptr_data_2_1;   //0x2C
    __int64 ptr3;	    //0x30
    int ptr_data_3_0;   //0x38
    int ptr_data_3_1;   //0x3C
    __int64 ptr4;	    //0x40
    int ptr_data_4_0;   //0x48
    int ptr_data_4_1;   //0x4C
    __int64 ptr5;	    //0x50
    int ptr_data_5_0;   //0x58
    int ptr_data_5_1;   //0x5C
    __int64 ptr6;	    //0x60
    int ptr_data_6_0;   //0x68
    int ptr_data_6_1;   //0x6C
    __int64 ptr7;	    //0x70
    int ptr_data_7_0;   //0x78
    int ptr_data_7_1;   //0x7C
    __int64 ptr8;	    //0x80
    int ptr_data_8_0;   //0x88
    int ptr_data_8_1;   //0x8C
    __int64 ptr9;	    //0x90
    int ptr_data_9_0;   //0x98
    int ptr_data_9_1;   //0x9C
    __int64 ptr10;	    //0xA0
    int ptr_data_10_0;  //0xA8
    int ptr_data_10_1;  //0xAC
    float x;            //0xB0
    float y;            //0xB4
    float z;            //0xB8
    float rotation1;    //0xBC
    float rotation2;    //0xC0
    int field_C4;       //0xC4
    __int64 field_C8;   //0xC8
    int field_D0;       //0xD0
    unsigned int field_D4;//0xD4
    __int64 field_D8;   //0xD8
    __int64 field_E0;   //0xE0
    __int64 field_E8;   //0xE8
    __int64 field_F0;   //0xF0
    __int64 field_F8;   //0xF8
    __int64 field_100;  //0x100
    __int64 field_108;  //0x108
    __int64 field_110;  //0x110
    __int64 field_118;  //0x118
    __int64 field_120;  //0x120
    __int64 field_128;  //0x128
    char* type;         //0x130
    __int64 field_138;  //0x138
    int field_140;	    //0x140
    int bitmaskFlag;    //0x144
    __int64 field_148;  //0x148
};

class playerEnt
{
public: 
    char pad_0000[19]; //0x0000
    int8_t rotationVelocity2; //0x0013
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
    int8_t rotationVelocity; //0x016B
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
    char pad_01E0[40]; //0x01E0
    class playerKinematicContext* kinematicContext; //0x0208
    float spawnedDuration; //0x0210
    char pad_0214[620]; //0x0214
}; //Size: 0x0480
static_assert(sizeof(playerEnt) == 0x480);

class playerKinematicContext
{
public:
    char pad_0000[8]; //0x0000
    class playerKinematicContextMember* kinematicContextMember; //0x0008
    char pad_0010[80]; //0x0010
    class N0000166C* N0000165C; //0x0060
    int32_t actorCount; //0x0068
    char pad_006C[340]; //0x006C
}; //Size: 0x01C0
static_assert(sizeof(playerKinematicContext) == 0x1C0);

class playerKinematicContextMember
{
public:
    char pad_0000[92]; //0x0000
    float FPS; //0x005C
    char pad_0060[296]; //0x0060
}; //Size: 0x0188
static_assert(sizeof(playerKinematicContextMember) == 0x188);

typedef void(*setDevConsoleState_type)(__int64 a1, unsigned __int8 a2);
typedef __int64(*addFuncToLuaClass_type)(__int64 L, const char* name, void* func, unsigned int type, void* callHandler, void* luaClass);
typedef char*(*addFuncToLuaClass_typeSTU)(__int64 L, const char** name, __int64 a3, char** a4, void* func, size_t type, __int64 callHandler, __int64 luaClass);
typedef char(*GetOptionFloat_type)(DWORD* a1, float* a2, char* a3);
typedef char(*SetOptionFloat_type)(DWORD* a1, float* a2, char* a3);
typedef char(*GetOptionBool_type)(DWORD* a1, bool* a2, char* a3);
typedef char(*SetOptionBool_type)(DWORD* a1, bool* a2, char* a3);
typedef SceneInstanceManager*(*getSceneInstanceManagerFromInstanceRootBySceneUH_type)(SceneRoot *root, int uh);
typedef Actor* (*SceneInstanceManager__getActor_type)(SceneInstanceManager* a1, int id);
typedef void* (*someGlobalGetterSetter_type)(char* name, unsigned int strLen, char a3, void* a4, char* a5);
typedef void* (*getPhysClass_type)();
typedef __int64(*PxControllerRelated_type)(__int64 a1, __int64 a2, __int64 a3, __int64 a4);
typedef void(*updatePositionDeltas_type)(__int64 context, float* posDeltas);
typedef void* (*somePxStuff_type)(__int64 a1);
typedef __int64 (*getPxActorFromList_type)(__int64 list, int id);
typedef __int64 (*setupGameConfig_type)(__int64 a1, unsigned int a2, __int64* preHashName, __int64 a4, __int64 a5, float value, int a7, __int64* a8, __int64* a9);
typedef __int64 (*createClassInstance_type)(__int64 a1, unsigned int a2, char** a3, __int64 a4, char a5);
typedef asteroidStruct* (*someGetObjectOrAsteroid_type)(__int64 a1, __int64 id);

extern std::unordered_map<int, SceneInstanceManager*> sceneInstances;
extern std::vector<SceneRoot*> sceneRoots;
extern std::unordered_map<int, Actor*> actors;
extern PhysListArray* physList;
extern std::atomic<uint32_t> maxObjects;
extern uint64_t objectManager;
extern uint64_t PxControllerObject;
extern uint64_t camObject;
//extern uintptr_t camObjectOffset;

extern someGlobalGetterSetter_type or_someGlobalGetterSetter;
extern getPhysClass_type getPhysClass;
extern getPhysClass_type maybeOpenDebug;;

extern uintptr_t baseAddress;

void initGameHooks();
void removeGameHooks();