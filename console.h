#pragma once

#include <stdio.h>

namespace Con {
	extern FILE* fpout;
	extern FILE* fpin;

	void init();
	void enableStdout(bool state);
	void cleanup();
};