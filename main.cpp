#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <tlhelp32.h>
#include <MinHook.h>
#include <stdlib.h>
#include <dxgi.h>
#include <PxActor.h>
#include <PxRigidActor.h>
#include <PxScene.h>
#include <PxAggregate.h>
#include <PxRigidBody.h>
#include <PxRigidStatic.h>
#include "threads.h"
#include "console.h"
#include "luaHooks.h"
#include "gameHooks.h"
#include "luaExecutor.h"
#include "hookUtils.h"
#include "menu.h"
#include "physics.h"
#include "dxHooks2.h"

static bool minHookInitialized = false;

int main()
{
	//todo: we need to create our own launcher tool, or find one with the right specs
	// ie: auto inject super early, or suspend process so we can do it ourselves
	//todo: if we inject to soon we may try to hook functions not yet available due to not loading in yet. should sleep it a bit in this case
	Con::init();
	Con::enableStdout(true);
	pauseAllThreads(true);

	//todo: we should probably do a check for other overlays. for example nvidia's OpenAutomate wrapper. 
	// When it fails to hook/detour it doesnt detach gracefully

	if (!minHookInitialized) {
		if (MH_Initialize() != MH_OK) {
			MessageBoxA(nullptr, "(MH) Failed to init", "Error", MB_OK);
			return E_FAIL;
		}
		minHookInitialized = true;
		fprintf(Con::fpout, "(MH) Initialized\n");
	}

	initGameHooks();
	fprintf(Con::fpout, "(MH) game funcs hooked\n");

	//todo: we should check if device is already created (does present or draw have a device?) if so detach gracefully.
	// since we dont yet know a way to force a new DXGI::Create
	initDxHooks2();
	fprintf(Con::fpout, "(MH) D3D11.1 hooked\n");

	pauseAllThreads(false);

	fprintf(Con::fpout, "cfg loaded\n");

	loadConfig("gsb.cfg");

	while (true) {
		char buff[4096];
		fgets(buff, 4000, Con::fpin);
		setRunString(buff);
	}

	return 0;
}

void Shutdown() {
	fprintf(Con::fpout, "Shutting down\n");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL,
	DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)updatePhysicsThread, NULL, 0, NULL);
	}

	return TRUE;
}