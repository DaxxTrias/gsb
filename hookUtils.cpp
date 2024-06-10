#include <Windows.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <MinHook.h>
#include "console.h"

using std::string;
using std::unordered_map;

struct HookData {
	void* hook;
	void* original;
	void* tramp;

};

unordered_map<string, HookData*> hooks;

void placeHook(string name, void* original, void* hook) {
	HookData* hookData = new HookData;
	hookData->hook = hook;
	hookData->original = original;
	hookData->tramp = 0;
	if (MH_CreateHook(original, hook, reinterpret_cast<void**>(&(hookData->tramp))) != MH_OK) {
		fprintf(Con::fpout, "error on placing hook %s\n", name.c_str());
		fflush(Con::fpout);
	}
	if (MH_EnableHook(original) != MH_OK) {
		fprintf(Con::fpout, "error on enabling hook%s\n", name.c_str());
		fflush(Con::fpout);
	}

	hooks[name] = hookData;

	fprintf(Con::fpout, "(MH) placed hook %s\n", name.c_str());
	fflush(Con::fpout);
}

void removeHook(const string& name) {
	auto it = hooks.find(name);
	if (it != hooks.end()) {
		HookData* hookData = it->second;

		// Disable the hook
		if (MH_DisableHook(hookData->original) != MH_OK) {
			fprintf(Con::fpout, "error on disabling hook %s\n", name.c_str());
			fflush(Con::fpout);
		}

		// Remove the hook
		if (MH_RemoveHook(hookData->original) != MH_OK) {
			fprintf(Con::fpout, "error on removing hook %s\n", name.c_str());
			fflush(Con::fpout);
		}

		// Clean up
		delete hookData;
		hooks.erase(it);

		fprintf(Con::fpout, "(MH) removed hook %s\n", name.c_str());
		fflush(Con::fpout);
	}
	else {
		fprintf(Con::fpout, "hook %s not found\n", name.c_str());
		fflush(Con::fpout);
	}
}

void* getTramp(string name) {
	auto data = hooks.find(name);
	if (data == hooks.end()) {
		return 0;
	}
	return (*data).second->tramp;
}

static void parsePattern(char* combo, char* pattern, char* mask) {
	char lastChar = ' ';
	unsigned int j = 0;
	const size_t len = strlen(combo);

	for (unsigned int i = 0; i < len; i++) {
		if ((combo[i] == '?' || combo[i] == '*') && (lastChar != '?' && lastChar != '*')) {
			pattern[j] = mask[j] = '?';
			j++;
		}
		else if (isspace(lastChar)) {
			pattern[j] = lastChar = (char)strtol(&combo[i], 0, 16);
			mask[j] = 'x';
			j++;
		}
		lastChar = combo[i];
	}
	pattern[j] = mask[j] = '\0';
}

void* findSignatureImpl(unsigned char* data, unsigned char* sig, char* pattern) {
	while (true) {
		data++;
		size_t offset = 0;
		while (data[offset] == sig[offset] || pattern[offset] == '?') {
			offset++;
			if (pattern[offset] == 0) {
				return data;
			}
		}
	}
}

void* findSignatureImpl(unsigned char* data, const char* combopattern) {
	unsigned char pattern[0x100] = { 0x0 };
	char mask[0x100] = { 0x0 };
	parsePattern((char*)combopattern, (char*)pattern, mask);
	return findSignatureImpl(data, pattern, mask);
}

unsigned char* getStarbaseExe() {
	return (unsigned char*)GetModuleHandle("Starbase.exe");
}

unsigned char* getLuaDll() {
	return (unsigned char*)GetModuleHandle("lua_x64.dll");
}