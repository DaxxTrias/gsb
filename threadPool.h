#pragma once

#include <future>
#include <deque>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <type_traits>

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

extern ThreadPool threadPool;
