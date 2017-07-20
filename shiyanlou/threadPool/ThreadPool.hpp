#ifndef SHIYANLOU_THREADPOOL_HPP
#define SHIYANLOU_THREADPOOL_HPP

#include <future>
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
  public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    // 向线程池中增加线程
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
            -> std::future<std::result_of_t<F(Args...)>>;

  private:
    std::vector<std::thread>          workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex                        queue_mutex_;
    std::condition_variable           condition_;
    bool                              stop_;
};

inline ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this] {
            // running forever
            for (;;) {
                // 将任务从队列中取出后再执行，可以减小临界区停留时间。
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this, &task] {
                        return stop_ || !tasks_.empty();
                    });

                    // 如果线程池已经结束且任务队列为空，直接返回。
                    if (stop_ && tasks_.empty()) { return; }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;    // 不加锁貌似也可以
    }
    condition_.notify_all();
    for (std::thread& worker : workers_) { worker.join(); }
}

// 添加一个新的任务到线程池中
// 参数的完美转发只能通过 && + forward 来进行。
// result_of_t 可以获得
template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
        -> std::future<std::result_of_t<F(Args...)>> {
    using return_type = std::result_of_t<F(Args...)>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.emplace([task] { (*task)(); });
    }
    condition_.notify_one();
    return res;
}

#endif