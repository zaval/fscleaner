//
// Created by Dmytrii Zavalnyi on 20.08.2025..
//

#ifndef FSCLEANER_THREADPOOL_H
#define FSCLEANER_THREADPOOL_H


#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <type_traits>
#include <filesystem>
#include <iostream>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {

        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task](){ (*task)(); });
        }

        condition.notify_one();
        return res;
    }

    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// Constructor




// Destructor



#endif //FSCLEANER_THREADPOOL_H