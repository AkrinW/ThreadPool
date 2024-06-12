#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
class ThreadPool {
private:
    std::vector<std::thread> workers;//工作的线程
    std::queue<std::function<void()>> tasks;//工作队列
    std::mutex queueMutex;//互斥锁
    bool stop;
    std::condition_variable condition;
public:
    ThreadPool(int num);
    ~ThreadPool();
    //submit a task to the pool
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

ThreadPool::ThreadPool(int num): stop(false) {
    for (int i = 0; i < num; ++i) {
        //存一个lambda表达式。
        workers.emplace_back([this]() {//this指针,能够改变类内数据。
            for (;;) {
                std::function<void()> task;
                //最外花括号定义一段作用域scope
                //可以保证作用域内的代码执行完就销毁
                {
                    //创建一个互斥锁，锁定类内的队列锁。
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    //类内的condition
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();                    
                    });
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
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
    for (std::thread &worker: workers) {
        worker.join();
    }
}

//Args-arguments。...任意多个参数
//F &&f, 右值引用，可以绑定左右值。
//尾返回类型，可以把auto放前面，就能够返回多种类型，在这个情形下把返回写前面也可以。
//Args &&...args表示0个或多个参数。
// template<class F, class ...Args>
// std::future<typename std::result_of<F(Args...)>::type> 
// ThreadPool::enqueue(F &&f, Args &&...args){};

template<class F, class ...Args> 
auto ThreadPool::enqueue(F &&f, Args &&...args)
->std::future<typename std::result_of<F(Args...)>::type> {
    //将绑定的函数和参数包装成 std::packaged_task，以便异步执行。
    auto task = std::make_shared<std::packaged_task<returnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<returnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        //将任务添加到任务队列中，并通知一个等待中的线程去执行该任务。
        tasks.emplace([task]() {
            (*task)();
        });
    }
    condition.notify_one();
    return res;
}
//chatgpt的代码无法通过编译。
//先学习lambda表达式。连代码都看不明白。

#endif