#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> pool;
public:
    ThreadPool(int num);
    ~ThreadPool();
};


#endif