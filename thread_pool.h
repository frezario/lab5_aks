//
// Created by Naz on 3/15/2023.
//

#ifndef LAB5_THREAD_POOL_H
#define LAB5_THREAD_POOL_H

#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include "thread_safe_queue.h"

struct join_threads {
    join_threads(std::vector<std::thread> &) {}
};

class thread_pool {
    std::atomic_bool done;
    thread_safe_queue <std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread() {
        while (!done) {
            std::function<void()> task;
            if (work_queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool() :
            done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                threads.emplace_back(&thread_pool::worker_thread, this);
            }
        }
        catch (...) {
            done = true;
            throw;
        }
    }

    ~thread_pool() {
        done = true;
    }

    template<typename FunctionType>
    void submit(FunctionType f) {
        work_queue.push(std::function<void()>(f));
    }
};

#endif //LAB5_THREAD_POOL_H