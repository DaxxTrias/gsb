#include "luaHooks.h"
#include <Windows.h>
#include "hookUtils.h"
#include "console.h"
#include "luaExecutor.h"

const char* f_parser_bytecode_pattern = "48 89 5C 24 ? 48 89 6C 24 ? 57 48 83 EC 30 4C 8B 02 48 8B DA 48 8B F9 4D 8B 08 49 8D 41 FF 49 89 00 4D 85 C9 74 13 4C 8B 02 49 8B 40 08 0F B6 28 48 FF C0 49 89 40 08";

f_parser_type or_f_parser;
lua_pushinteger_type or_lua_pushinteger_type;

lua_setglobal_type lua_setglobal_sb;
lua_pcallk_type lua_pcallk_sb;
luaL_loadstring_type luaL_loadstring_sb;
lua_pushstring_type lua_pushstring_sb;
lua_pushcclosure_type lua_pushcclosure_sb;
lua_tolstring_type lua_tolstring_sb;
lua_rawgeti_type lua_rawgeti_sb;

int testDumps = 0;

static int writer(lua_State* L, const void* b, size_t size, void* B) {
	(void)L;
	fwrite(b, 1, size, (FILE*)B);
	return 0;
}

void f_parser_hook(lua_State *L, void* ud) {
	Zio* z = *(Zio**)ud;

	FnCast("f_parser", or_f_parser)(L, (long long**)ud);

	char fileName[256];
	sprintf(fileName, "E:\\starbase\\dump\\dump_%d", testDumps);
	testDumps++;
	FILE* file = fopen(fileName, "wb");
	lua_dump(L, writer, file, 0);
	fflush(file);
	fclose(file);
}

int errorHandler(lua_State* L) {
	const char* err = lua_tostring(L, 1);

	fprintf(Con::fpout, "Error:%s\n", err);
	fflush(Con::fpout);

	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");

	if (lua_pcall(L, 0, 1, 0)) {
		const char* err = lua_tostring(L, -1);

		fprintf(Con::fpout, "Error in debug.traceback() call: %s\n", err);
		fflush(Con::fpout);
	} else {
		const char* stackTrace = lua_tostring(L, -1);

		fprintf(Con::fpout, "C++ stack traceback: %s\n", stackTrace);
		fflush(Con::fpout);
	}

	return 1;
}

int gsb_print(lua_State *L) {
    const char *str = lua_tolstring_sb(L, 1, 0);
	fprintf(Con::fpout, "lua: %s\n", str);
	fflush(Con::fpout);
    return 1i64;
}

int lua_rawgeti_hook(lua_State* L, int idx, lua_Integer n) {
	if (isRunString()) {
		Con::enableStdout(true);

		lua_pushcclosure_sb(L, luaB_print, 0i64);
		lua_setglobal_sb(L, "gprint");

		lua_pushcclosure_sb(L, errorHandler, 0i64);
		int status = luaL_loadstring_sb(L, getRunString().c_str());
		if (status != 0) {
			fprintf(Con::fpout, "error on load: %s\n", lua_tostring(L, -1));
			fflush(Con::fpout);
		} else {
			status = lua_pcallk_sb(L, 0, 0, -2, 0, 0);
			if (status) {
				fprintf(Con::fpout, "pcall error: %d\n", status);
				fprintf(Con::fpout, "txt: %s\n", lua_tostring(L, 0));
				
				const char* traceback = lua_tostring(L, -1);
				fprintf(Con::fpout, "traceback: %s\n", traceback);
				fflush(Con::fpout);
				lua_pop(L, 1);
			}
		}
	}

	return FnCast("lua_rawgeti", lua_rawgeti_sb)(L, idx, n);
}

void initLuaHook() {
	or_f_parser = findSignature<f_parser_type>(getLuaDll(), f_parser_bytecode_pattern);
	placeHook("f_parser", or_f_parser, f_parser_hook);

	HINSTANCE luaDll = GetModuleHandleA("lua_x64.dll");
	or_lua_pushinteger_type = (lua_pushinteger_type)GetProcAddress(luaDll, "lua_pushinteger");
	lua_setglobal_sb = (lua_setglobal_type)GetProcAddress(luaDll, "lua_setglobal");
	lua_pcallk_sb = (lua_pcallk_type)GetProcAddress(luaDll, "lua_pcallk");
	luaL_loadstring_sb = (luaL_loadstring_type)GetProcAddress(luaDll, "luaL_loadstring");
	lua_pushstring_sb = (lua_pushstring_type)GetProcAddress(luaDll, "lua_pushstring");
	lua_pushcclosure_sb = (lua_pushcclosure_type)GetProcAddress(luaDll, "lua_pushcclosure");
	lua_tolstring_sb = (lua_tolstring_type)GetProcAddress(luaDll, "lua_tolstring");
	lua_rawgeti_sb = (lua_rawgeti_type)GetProcAddress(luaDll, "lua_rawgeti");
	
	placeHook("lua_rawgeti", lua_rawgeti_sb, lua_rawgeti_hook);
}