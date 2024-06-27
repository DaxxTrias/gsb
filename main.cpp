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
#include "killSwitch.h"
#include <atomic>

static bool minHookInitialized = false;
HMODULE g_hModule = nullptr;
std::atomic<bool> isShuttingDown(false); // Shutdown flag

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

	loadConfig("gsb.cfg");

	fprintf(Con::fpout, "cfg loaded\n");

	while (true) {
		char buff[4096];
		fgets(buff, 4000, Con::fpin);
		setRunString(buff);
	}

	return 0;
}

void Shutdown(HMODULE hModule) {
	// Check if shutdown is already in progress
	if (isShuttingDown.exchange(true)) {
		return; // If already shutting down, do nothing
	}

	// Signal threads to stop
	keepRunning = false;
	killSwitch = true;

	fprintf(Con::fpout, "Shutting down\n");
	fprintf(Con::fpout, "Dismantling ImGui\n");
	RemoveImGuiD3D11();
	stopUpdatePhysicsThread();
	fprintf(Con::fpout, "Halted Physics thread\n");
	removeGameHooks();
	fprintf(Con::fpout, "Removed GameHooks\n");
	removeDXHooks();
	fprintf(Con::fpout, "Removed DirectX hooks\n");

	// Uninitialize MinHook
	if (MH_Uninitialize() != MH_OK) {
		fprintf(Con::fpout, "MinHook uninitialization failed\n");
	}
	Con::cleanup();

	// Clean up the module and exit the thread
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL); // Prevent DllMain from being called on thread creation/destruction
		g_hModule = hinstDLL; // Store the module handle
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)updatePhysicsThread, NULL, 0, NULL);
		break;
	case DLL_PROCESS_DETACH:
		//todo: verify the shutdown process so we can cleanly detach.
		if (lpvReserved == NULL && !isShuttingDown.load()) { // Indicates a call to FreeLibrary or a DLL unload request
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Shutdown, g_hModule, 0, NULL);
		}
		break;
	}

	return TRUE;
}