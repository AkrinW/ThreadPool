#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include "ThreadPool.h"
#include <memory>

class Philosopher {
private:
    int id;
    using ptr_mutex = std::shared_ptr<std::mutex>;
    using ptr_pool = std::shared_ptr<ThreadPool>;
    ptr_mutex left, right;
    ptr_pool pool;
    int eatcount;
    void dining();
    void thinking();
public:
    Philosopher(int i, ptr_mutex l, ptr_mutex r, ptr_pool p):
    id(i), eatcount(0), left(l), right(r), pool(p) {}
    void dinner() {
        pool.get()->enqueue([this]() {
            this->dining();
        });
    }
};

void Philosopher::thinking() {
    std::cout << "Philosopher" << id << "is thinking\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Philosopher" << id << "end thinking\n";
}

void Philosopher::dining() {
    while (eatcount < 5) {
        thinking();
        // left.get()->lock();
        // right.get()->lock();
        // 获取锁
        {
            // 两种方法都可以，但是关键问题在于作用域上，没有作用域会死锁。
            std::cout << "getLock" << id << '\n';
            // lockway2
            std::lock(*left.get(), *right.get());
            std::unique_lock<std::mutex> leftLock(*left.get(), std::adopt_lock);
            std::unique_lock<std::mutex> rightLock(*right.get(), std::adopt_lock);
            // lockway1
            // std::scoped_lock lock(*left, *right);
            std::cout << "Philosopher" << id << "is eating\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ++eatcount;
            std::cout << "Philosopher" << id << "end eating\n";
        }//作用域结束后会释放锁，比较关键。
    }
    std::cout << "Philospher" << id << "eat5times\n";
}

#endif