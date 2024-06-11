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
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};


#endif