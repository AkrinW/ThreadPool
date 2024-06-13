#include <thread>
#include <iostream>
#include <vector>
#include "ThreadPool.h"
#include "Philosopher.h"

int main() {
    // 哲学家问题。
    int num = 5;
    auto pool2 = std::make_shared<ThreadPool>(num);
    std::vector<std::shared_ptr<std::mutex>> forks;
    for (int i = 0; i < num; ++i) {
        forks.push_back(std::make_shared<std::mutex>());
    }
    // std::vector<std::mutex> forks(num);
    std::vector<Philosopher> philos;
    for (int i = 0; i < num; ++i) {
        philos.push_back(Philosopher(i, forks[i], forks[(i+1)%num], pool2));
    }
    for (auto &phi : philos) {
        phi.dinner();
    }

    std::this_thread::sleep_for(std::chrono::seconds(30));
    // 主线程等待一段时间，让哲学家执行完。
    return 0;
}