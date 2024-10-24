#include <Windows.h>
#include "hookUtils.h"
#include "console.h"
#include "gameHooks.h"
#include <unordered_map>
#include <dxgi.h>
#include <PxActor.h>
#include <PxRigidActor.h>
#include <PxScene.h>
#include <PxAggregate.h>
#include <PxRigidBody.h>

// most of the previous patterns seemed (mostly) accurate on v582, but some of the functions were rewritten
const char* setDevConsoleState_pattern = "4C 8B DC 55 41 54 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 80 79 ? ? 44 0F B6 E2";
const char* addFuncToLuaClass_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B EA 41 8B D9 41 8B D1 49 8B F8 48 8B F1 FF 15 ? ? ? ? 44 8B C3 48 8B D7 48 8B C8";
const char* GetOptionFloat_pattern = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 31 4C 8B F2 48 8B F9 33 D2 49 8B C8 49 8B D8 E8 ? ? ? ? 4C 8B C3 0F";
const char* SetOptionFloat_pattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 39 48 8B DA 48 8B F1 33 D2 49 8B C8 E8 ? ? ? ? 44 8B C0 48 8B CE E8";
const char* GetOptionBool_pattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 39 48 8B DA 48 8B F1 BA ? ? ? ? 49 8B C8 E8 ? ? ? ? 44 8B C0 48 8B CE E8 ? ? ? ? 8B 47 ? 8B 1B"; // maybe (original pattern had other possibilities)
const char* SetOptionBool_pattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 39 48 8B DA 48 8B F1 33 D2"; // maybe (original pattern had several other possibilities)
const char* getSceneInstanceManagerFromInstanceRootBySceneUH_pattern = "48 89 5C 24 ? 89 54 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05";
const char* SceneInstanceManager__getActor_pattern = "40 53 48 83 EC 20 48 8B 41 40 4C 8B 40 78 8B C2 25 ? ? ? ? 41 3B 40 08 0F 83 ? ? ? ? 8B C8 49 8B 00 48 03 C9 48 83 E0 FC 39 54 C8 08 0F 85 ? ? ? ? 48 8B 1C C8 48 85 DB 0F 84 ? ? ? ? 48 83 7B";
const char* someGlobalGetterSetter_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC 40 49 8B F1 8B EA 45 0F B6 F0 48 8B F9 E8 ? ? ? ?";
const char* iterOver_patter = "E8 ? ? ? ? 48 8B D0 48 8B CB E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 8F ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 49 08 48 8B 9E ? ? ? ? E8 ? ? ? ? 48 8B F8 48 85 C0 74 7C 48 8B 4B 60 8B 73 68 48 83 E1 FC 4C 8D 04 F5 ? ? ? ? 49 8D 14 08 48 3B CA 74 0E";
const char* somePxStuff_pattern = "40 53 48 83 EC ? 48 8B 01 48 8D 15 ? ? ? ? 48 8B D9 FF 50 ? 33 D2 84 C0 48 0F 45 D3 48 8B C2 48 83 C4 ? 5B C3 CC CC CC CC CC CC CC CC CC 48 8D 05"; // original pattern was showing multiple, this should be better
const char* maybeOpenDebug_pattern = "40 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? E8 ? ? ? ? 84 C0 0F 85 ? ? ? ?";
const char* getPxActorFromList_pattern = "48 89 5C 24 ? 57 48 83 EC ? 8B DA 48 8B F9 83 FA ? 75 ? 41 B8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 ? CC 8B C3 25 ? ? ? ? 3B 87 ? ? ? ? 73 ? 8B C8 48 8B 87 ? ? ? ? 48 03 C9 48 83 E0 ? 39 5C C8 ? 75 ? 48 8B 04 C8 48 85 C0 74 ? 8B 40"; // maybe (but theres tons of other possibilities from original pattern)
const char* setupGameConfig_pattern = "48 8B C4 55 53 48 8D 68 A1 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 48 8B D9 0F 85 ? ? ? ? 48 89 70 08 48 89 78 E8 4C 89 60 E0 4C 89 68 D8 4C 89 70 D0 4C 89 78 C8 0F 29 78 B8 44 0F 29 48 ?";
const char* createClassInstance_patter = "40 53 56 41 55 41 56 48 83 EC ? 8B DA"; // maybe (function appears to have been rewritten. only about 70% confidence)
const char* someGetObjectOrAsteroid_pattern = "48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 8B DA 3B 91 ? ? ? ? 72 ? 41 B8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 ? CC 3B 9F ? ? ? ? 72 ? 4C 8D 0D ? ? ? ? 41 B8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 ? CC 48 8B 8F ? ? ? ? 48 69 C3 ? ? ? ? 48 8B 5C 24 ? 48 83 E1 ? 48 03 C1 48 83 C4 ? 5F C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 ? 57 48 83 EC ? 41 8B D8"; // 50% chance this is right (there were 2 possibilities)

setDevConsoleState_type or_setDevConsoleState;
addFuncToLuaClass_type or_addFuncToLuaClass;
GetOptionFloat_type or_GetOptionFloat;
SetOptionFloat_type or_SetOptionFloat;
GetOptionBool_type or_GetOptionBool;
SetOptionBool_type or_SetOptionBool;
getSceneInstanceManagerFromInstanceRootBySceneUH_type or_getSceneInstanceManagerFromInstanceRootBySceneUH;
SceneInstanceManager__getActor_type or_SceneInstanceManager__getActor;
someGlobalGetterSetter_type or_someGlobalGetterSetter;
getPhysClass_type getPhysClass;
somePxStuff_type or_somePxStuff;
getPhysClass_type maybeOpenDebug;
getPxActorFromList_type or_getPxActorFromList;
setupGameConfig_type or_setupGameConfig;
createClassInstance_type or_createClassInstance;
someGetObjectOrAsteroid_type someGetObjectOrAsteroid_or;

std::unordered_map<int, SceneInstanceManager*> sceneInstances;
std::vector<SceneRoot*> sceneRoots;
std::unordered_map<int, Actor*> actors;
PhysListArray* physList = 0;
uint64_t objectManager;

void setDevConsoleState_hook(__int64 a1, unsigned __int8 a2) {
	FnCast("setDevConsoleState", or_setDevConsoleState)(a1, true);
}

__int64 addFuncToLuaClass_hook(__int64 L, const char* name, void* func, unsigned int type, void* callHandler, void* luaClass) {
	return FnCast("addFuncToLuaClass", or_addFuncToLuaClass)(L, name, func, type, callHandler, luaClass);
}

char GetOptionFloat_hook(DWORD* a1, float* a2, char* a3) {
	char result = FnCast("GetOptionFloat", or_GetOptionFloat)(a1, a2, a3);
	return result;
}

char SetOptionFloat_hook(DWORD* a1, float* a2, char* a3) {
	char result = FnCast("SetOptionFloat", or_SetOptionFloat)(a1, a2, a3);
	return result;
}

char GetOptionBool_hook(DWORD* a1, bool* a2, char* a3) {
	char result = FnCast("GetOptionBool", or_GetOptionBool)(a1, a2, a3);
	return result;
}

char SetOptionBool_hook(DWORD* a1, bool* a2, char* a3) {
	char result = FnCast("SetOptionBool", or_SetOptionBool)(a1, a2, a3);
	return result;
}

SceneInstanceManager* getSceneInstanceManagerFromInstanceRootBySceneUH_hook(SceneRoot *root, int uh) {
	//sceneRoots.push_back(root);
	SceneInstanceManager *result = FnCast("getSceneInstanceManagerFromInstanceRootBySceneUH", or_getSceneInstanceManagerFromInstanceRootBySceneUH)(root, uh);
	sceneInstances[uh] = result;
	return result;
}

Actor* SceneInstanceManager__getActor_hook(SceneInstanceManager* a1, int id) {
	Actor* result = FnCast("SceneInstanceManager__getActor", or_SceneInstanceManager__getActor)(a1, id);
	actors[id] = result;
	return result;
}

void* someGlobalGetterSetter_hook(char* name, unsigned int strLen, char a3, void* a4, char* a5) {
	void* result = FnCast("someGlobalGetterSetter", or_someGlobalGetterSetter)(name, strLen, a3, a4, a5);
	/*{
		char* temp = (char*)result;
		if (strstr(name, "ToggleWireframe")) {
			uint64_t data = *(uint64_t*)(temp + 7);
			fprintf(Con::fpout, "%s request %d %llX\n", name, *a5, data);
		}
	}*/
	return result;
}

void* somePxStuff_hook(uint64_t a1) {
	physx::PxActor* actor = (physx::PxActor*)a1;//(physx::PxActor*)(*(char**)(physicalComp + 8));
	if (actor == nullptr) {
		fprintf(Con::fpout, "actor - null\n");
		fflush(Con::fpout);
		return FnCast("somePxStuff", or_somePxStuff)((long long)a1);
	}

	physx::PxRigidActor* rigid = actor->is<physx::PxRigidActor>();
	physx::PxVec3 pos = rigid->getGlobalPose().p;
	physx::PxScene *scene = rigid->getScene();
	physx::PxActor** Actors = (physx::PxActor**)malloc(8 * 4096);
	fprintf(Con::fpout, "pos %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);
	fflush(Con::fpout);
	return FnCast("somePxStuff", or_somePxStuff)((long long)a1);
}

__int64 getPxActorFromList_hook(__int64 list, int id) {
	fprintf(Con::fpout, "getPxActorFromList %llx %d\n", list, id);
	physList = *(PhysListArray**)(list + 320);
    fprintf(Con::fpout, "physList %p\n", physList);
	return FnCast("getPxActorFromList", or_getPxActorFromList)(list, id);
}

__int64 __fastcall setupGameConfig_hook(__int64 a1, unsigned int a2, __int64* preHashName, __int64 a4, __int64 a5, float value, int a7, __int64* a8, __int64* a9) {
	fprintf(Con::fpout, "%s - %f\n", (char*)preHashName, value);
	fflush(Con::fpout);
	return FnCast("setupGameConfig", or_setupGameConfig)(a1, a2, preHashName, a4, a5, value, a7, a8, a9);
}

__int64 createClassInstance_hook(__int64 a1, unsigned int a2, char** a3, __int64 a4, char a5) {
	bool balance = false;
	if (strstr(*a3, "AsteroidLoadingAndLoddingRules")) {
		fprintf(Con::fpout, "%s\n", *a3);
		fflush(Con::fpout);
		//balance = true;
	}
	__int64 result = FnCast("createClassInstance", or_createClassInstance)(a1, a2, a3, a4, a5);

	if (balance) {
		fprintf(Con::fpout, "    %s %llx\n", *(char**)(result + 0x180), result);
		fflush(Con::fpout);

		for (int i = 0x1A0; i <= 0x1CC; i += 4) {
			fprintf(Con::fpout, "    %llx - %f\n", (uint64_t)i, *(float*)(result + i));
			fflush(Con::fpout);
		}
	}

	return result;
}

asteroidStruct* __fastcall someGetObjectOrAsteroid_hook(__int64 a1, __int64 id) {
	objectManager = a1;
	asteroidStruct *asteroid = FnCast("someGetObjectOrAsteroid", someGetObjectOrAsteroid_or)(a1, id);
	if (strstr(asteroid->type, "ore")) {
        fprintf(Con::fpout, "asteroid %lld - %p [%.2f %.2f %.2f] %s\n", id, (void*)asteroid, asteroid->x, asteroid->y, asteroid->z, asteroid->type);
		fflush(Con::fpout);
	}
	return asteroid;
}

void initGameHooks() {
	or_setDevConsoleState = findSignature<setDevConsoleState_type>(getStarbaseExe(), setDevConsoleState_pattern);
	//placeHook("setDevConsoleState", or_setDevConsoleState, setDevConsoleState_hook);

	or_addFuncToLuaClass = findSignature<addFuncToLuaClass_type>(getStarbaseExe(), addFuncToLuaClass_pattern);
	//placeHook("addFuncToLuaClass", or_addFuncToLuaClass, addFuncToLuaClass_hook);

	or_GetOptionFloat = findSignature<GetOptionFloat_type>(getStarbaseExe(), GetOptionFloat_pattern);
	//placeHook("GetOptionFloat", or_GetOptionFloat, GetOptionFloat_hook);

	or_SetOptionFloat = findSignature<SetOptionFloat_type>(getStarbaseExe(), SetOptionFloat_pattern);
	//placeHook("SetOptionFloat", or_SetOptionFloat, SetOptionFloat_hook);

	or_GetOptionBool = findSignature<GetOptionBool_type>(getStarbaseExe(), GetOptionBool_pattern);
	//placeHook("GetOptionBool", or_GetOptionBool, GetOptionBool_hook);

	or_SetOptionBool = findSignature<SetOptionBool_type>(getStarbaseExe(), SetOptionBool_pattern);
	//placeHook("SetOptionBool", or_SetOptionBool, SetOptionBool_hook);

	or_getSceneInstanceManagerFromInstanceRootBySceneUH = findSignature<getSceneInstanceManagerFromInstanceRootBySceneUH_type>(getStarbaseExe(), getSceneInstanceManagerFromInstanceRootBySceneUH_pattern);
	//placeHook("getSceneInstanceManagerFromInstanceRootBySceneUH", or_getSceneInstanceManagerFromInstanceRootBySceneUH, getSceneInstanceManagerFromInstanceRootBySceneUH_hook);

	or_SceneInstanceManager__getActor = findSignature<SceneInstanceManager__getActor_type>(getStarbaseExe(), SceneInstanceManager__getActor_pattern);
	//placeHook("SceneInstanceManager__getActor", or_SceneInstanceManager__getActor, SceneInstanceManager__getActor_hook);

	or_someGlobalGetterSetter = findSignature<someGlobalGetterSetter_type>(getStarbaseExe(), someGlobalGetterSetter_pattern);
	//placeHook("someGlobalGetterSetter", or_someGlobalGetterSetter, someGlobalGetterSetter_hook);

	char* ptr = findSignature<char*>(getStarbaseExe(), iterOver_patter);
	int32_t offset = *((int32_t*)(ptr + 1)) + 5;
	getPhysClass = (getPhysClass_type)(ptr + offset);

	or_somePxStuff = findSignature<somePxStuff_type>(getStarbaseExe(), somePxStuff_pattern);
	//placeHook("somePxStuff", or_somePxStuff, somePxStuff_hook);

	maybeOpenDebug = findSignature<getPhysClass_type>(getStarbaseExe(), maybeOpenDebug_pattern);

	or_getPxActorFromList = findSignature<getPxActorFromList_type>(getStarbaseExe(), getPxActorFromList_pattern);
	placeHook("getPxActorFromList", or_getPxActorFromList, getPxActorFromList_hook);

	//or_createClassInstance = findSignature<createClassInstance_type>(getStarbaseExe(), createClassInstance_patter);
	//placeHook("createClassInstance", or_createClassInstance, createClassInstance_hook);

	//MessageBoxA(nullptr, "test", "test", MB_OK);

	someGetObjectOrAsteroid_or = findSignature<someGetObjectOrAsteroid_type>(getStarbaseExe(), someGetObjectOrAsteroid_pattern);
	//placeHook("someGetObjectOrAsteroid", someGetObjectOrAsteroid_or, someGetObjectOrAsteroid_hook);

	//uint64_t preSetupC = findSignature<uint64_t>(getStarbaseExe(), setupGameConfig_pattern) + 0x144;
	//or_setupGameConfig = (setupGameConfig_type)(preSetupC + 5 + (*(int*)(preSetupC + 1)));
	//or_setupGameConfig = (uint32_t*)findSignature<uint64_t>(getStarbaseExe(), setupGameConfig_pattern) + 0x150);
	//MessageBoxA(NULL, "a", "a", MB_OK);
	//placeHook("setupGameConfig", or_setupGameConfig, setupGameConfig_hook);
}



