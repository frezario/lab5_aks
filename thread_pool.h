#include <deque>
#include <future>
#include <memory>
#include <functional>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include "thread_safe_queue.h"

struct join_threads {
    explicit join_threads(std::vector<std::thread> &) {}
};

class function_wrapper {
    struct impl_base {
        virtual void call() = 0;

        virtual ~impl_base() = default;
    };

    std::unique_ptr<impl_base> impl;

    template<typename F>
    struct impl_type : impl_base {
        F f;

        explicit impl_type(F &&f_) : f(std::move(f_)) {}

        void call() override { f(); }
    };

public:
    template<typename F>
    explicit function_wrapper(F &&f):
            impl(new impl_type<F>(std::forward<F>(f))) {}

    void call() { impl->call(); }

    function_wrapper(function_wrapper &&other) noexcept:
            impl(std::move(other.impl)) {}

    function_wrapper &operator=(function_wrapper &&other)
    noexcept {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper &) = delete;

    function_wrapper(function_wrapper &) = delete;

    function_wrapper &operator=(const function_wrapper &) = delete;
};

class thread_pool {
    std::atomic_bool done;
    thread_safe_queue<std::function<void()> > work_queue;
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
    explicit thread_pool(size_t thread_count) :
            done(false), joiner(threads) {
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
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;

        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }
    // rest as before
};