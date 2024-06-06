#pragma once

#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <PxActor.h>

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
    __int64 ptr0;
    int ptr_data_0_0;
    int ptr_data_0_1;
    __int64 ptr1;
    int ptr_data_1_0;
    int ptr_data_1_1;
    __int64 ptr2;
    int ptr_data_2_0;
    int ptr_data_2_1;
    __int64 ptr3;
    int ptr_data_3_0;
    int ptr_data_3_1;
    __int64 ptr4;
    int ptr_data_4_0;
    int ptr_data_4_1;
    __int64 ptr5;
    int ptr_data_5_0;
    int ptr_data_5_1;
    __int64 ptr6;
    int ptr_data_6_0;
    int ptr_data_6_1;
    __int64 ptr7;
    int ptr_data_7_0;
    int ptr_data_7_1;
    __int64 ptr8;
    int ptr_data_8_0;
    int ptr_data_8_1;
    __int64 ptr9;
    int ptr_data_9_0;
    int ptr_data_9_1;
    __int64 ptr10;
    int ptr_data_10_0;
    int ptr_data_10_1;
    float x;
    float y;
    float z;
    int gapBC;
    __int64 field_C0;
    __int64 field_C8;
    int field_D0;
    unsigned int field_D4;
    __int64 field_D8;
    __int64 field_E0;
    __int64 field_E8;
    __int64 field_F0;
    __int64 field_F8;
    char* type;
    __int64 field_108;
    int field_110;
    int someFlag;
    __int64 field_118;
};

typedef void(*setDevConsoleState_type)(__int64 a1, unsigned __int8 a2);
typedef __int64(*addFuncToLuaClass_type)(__int64 L, const char* name, void* func, unsigned int type, void* callHandler, void* luaClass);
typedef char(*GetOptionFloat_type)(DWORD* a1, float* a2, char* a3);
typedef char(*SetOptionFloat_type)(DWORD* a1, float* a2, char* a3);
typedef char(*GetOptionBool_type)(DWORD* a1, bool* a2, char* a3);
typedef char(*SetOptionBool_type)(DWORD* a1, bool* a2, char* a3);
typedef SceneInstanceManager*(*getSceneInstanceManagerFromInstanceRootBySceneUH_type)(SceneRoot *root, int uh);
typedef Actor* (*SceneInstanceManager__getActor_type)(SceneInstanceManager* a1, int id);
typedef void* (*someGlobalGetterSetter_type)(char* name, unsigned int strLen, char a3, void* a4, char* a5);
typedef void* (*getPhysClass_type)();
typedef void* (*somePxStuff_type)(__int64 a1);
typedef __int64 (*getPxActorFromList_type)(__int64 list, int id);
typedef __int64 (*setupGameConfig_type)(__int64 a1, unsigned int a2, __int64* preHashName, __int64 a4, __int64 a5, float value, int a7, __int64* a8, __int64* a9);
typedef __int64 (*createClassInstance_type)(__int64 a1, unsigned int a2, char** a3, __int64 a4, char a5);
typedef asteroidStruct* (*someGetObjectOrAsteroid_type)(__int64 a1, __int64 id);

extern std::unordered_map<int, SceneInstanceManager*> sceneInstances;
extern std::vector<SceneRoot*> sceneRoots;
extern std::unordered_map<int, Actor*> actors;
extern PhysListArray* physList;
extern uint64_t objectManager;

extern someGlobalGetterSetter_type or_someGlobalGetterSetter;
extern getPhysClass_type getPhysClass;
extern getPhysClass_type maybeOpenDebug;;

void initGameHooks();