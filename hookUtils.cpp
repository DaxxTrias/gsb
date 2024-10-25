#include <Windows.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <MinHook.h>
#include "console.h"
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <deque>
#include <functional>
#include <type_traits>
#include <typeinfo>

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

unsigned char* getPlayerKinematicsDll() {
	return (unsigned char*)GetModuleHandle("PhysX3CharacterKinematic_x64.dll");
}

unsigned char* getLuaDll() {
	return (unsigned char*)GetModuleHandle("lua_x64.dll");
}

// Thread pool implementation
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

inline ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
	for (size_t i = 0; i < numThreads; ++i) {
		workers.emplace_back([this] {
			for (;;) {
				std::function<void()> task;

				{
					std::unique_lock<std::mutex> lock(this->queueMutex);
					this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
					if (this->stop && this->tasks.empty()) {
						return;
					}
					task = std::move(this->tasks.front());
					this->tasks.pop_front();
				}

				task();
			}
		});
	}
}

inline ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& worker : workers) {
		worker.join();
	}
}

template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<typename std::invoke_result<F>::type> {
	using returnType = typename std::invoke_result<F>::type;

	auto task = std::make_shared<std::packaged_task<returnType()>>(std::forward<F>(f));

	std::future<returnType> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queueMutex);

		if (stop) {
			throw std::runtime_error("enqueue on stopped ThreadPool");
		}

		tasks.emplace_back([task]() { (*task)(); });
	}
	condition.notify_one();
	return res;
}

// Global thread pool instance
ThreadPool threadPool(std::thread::hardware_concurrency());
