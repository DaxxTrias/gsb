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
    auto enqueue(F&& f) -> std::future<typename std::invoke_result<F>::type> {
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

private:
    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

extern ThreadPool threadPool;

// Explicit instantiation declarations
template auto ThreadPool::enqueue<std::function<void()>>(std::function<void()>&&) -> std::future<void>;
template auto ThreadPool::enqueue<std::function<int()>>(std::function<int()>&&) -> std::future<int>;
