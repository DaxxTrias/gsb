#include "console.h"
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

FILE* Con::fpout;
FILE* Con::fpin;

void Con::init() {
	int hConHandle;
	intptr_t lStdHandle;

	AllocConsole();

	lStdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_INPUT_HANDLE));
	hConHandle = _open_osfhandle(static_cast<long>(lStdHandle), _O_TEXT);
	fpin = _fdopen(hConHandle, "r");

	lStdHandle = reinterpret_cast<intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
	hConHandle = _open_osfhandle(static_cast<long>(lStdHandle), _O_TEXT);
	fpout = _fdopen(hConHandle, "w");
}

void Con::cleanup() {
	if (fpout) {
		fclose(fpout);
		fpout = nullptr;
	}
	if (fpin) {
		fclose(fpin);
		fpin = nullptr;
	}
	FreeConsole();
}

void Con::enableStdout(bool state) {
	const char* pipe = state ? "CONOUT$" : "NUL:";
	FILE* newStdout = freopen(pipe, "w", stdout);
	if (newStdout == nullptr) {
		// handle error
	}
}