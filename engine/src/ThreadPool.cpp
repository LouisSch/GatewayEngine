#include "ThreadPool.hpp"

ThreadPool::ThreadPool(const size_t nWorkers) {
    for (size_t i = 0; i < nWorkers; i++) {
        workers.emplace_back([this]() {
            std::function<void()> task;
            
            while (true) {
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this]() {
                        return stop.load(std::memory_order_acquire) || !tasks.empty();
                    });

                    if (stop.load(std::memory_order_acquire) && tasks.empty()) return;
                    
                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop.store(true);
    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) worker.join();
    }
}