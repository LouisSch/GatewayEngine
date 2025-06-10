#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
public:
    ThreadPool(const size_t nWorkers);
    ~ThreadPool();

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
        using return_type = typename std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> resFuture = task->get_future();

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (stop.load(std::memory_order_acquire)) throw std::runtime_error("Cannot enqueue task on stopped thread pool.");
            tasks.emplace([task]() { (*task)(); });
        }

        condition.notify_one();

        return resFuture;
    }
};