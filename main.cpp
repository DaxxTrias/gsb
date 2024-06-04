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

int main()
{
	Con::init();
	Con::enableStdout(true);
	pauseAllThreads(true);

	MH_Initialize();
	initGameHooks();
	initDxHooks2();

	pauseAllThreads(false);

	fprintf(Con::fpout, "load cfg\n");

	//loadConfig("gsb.cfg");

	while (true) {
		char buff[4096];
		fgets(buff, 4000, Con::fpin);
		setRunString(buff);
	}

	return 0;
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