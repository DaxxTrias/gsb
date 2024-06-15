#pragma once
#include <string>
#include <Windows.h>
#include <stdint.h>
#include "console.h"

using std::string;

void placeHook(std::string name, void* original, void* hook);
void removeHook(const string& name);

void* getTramp(std::string name);

template<typename T>
T FnCast(std::string name, T pFnCastTo) {
	return (T)getTramp(name);
}

void* findSignatureImpl(unsigned char* data, const char* combopattern);
void* findSignatureImpl(unsigned char* data, unsigned char* sig, char* pattern);

template<typename T>
T findSignature(unsigned char* data, const char* combopattern) {
	return (T)findSignatureImpl(data, combopattern);
}

template<typename T>
T findSignatureFull(unsigned char* data, unsigned char* sig, char* pattern) {
	return (T)findSignatureImpl(data, sig, pattern);
}

unsigned char* getStarbaseExe();
unsigned char* getLuaDll();
unsigned char* getPlayerKinematicsDll();