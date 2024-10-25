#pragma once
#include <string>
#include <Windows.h>
#include <stdint.h>
#include "console.h"
#include <future>
#include <deque>

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

// Declaration of the ThreadPool class
class ThreadPool {
public:
	ThreadPool(size_t numThreads);
	~ThreadPool();

	template<class F>
	auto enqueue(F&& f) -> std::future<typename std::invoke_result<F>::type>;

private:
	std::vector<std::thread> workers;
	std::deque<std::function<void()>> tasks;

	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop;
};

// Declaration of the global threadPool instance
extern ThreadPool threadPool;
