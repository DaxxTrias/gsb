#include "threadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
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

ThreadPool::~ThreadPool() {
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

ThreadPool threadPool(std::thread::hardware_concurrency());
