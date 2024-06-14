#include <iostream>
#include "ThreadPool.h"
#include "Banker.h"
#include <memory>
#include <vector>

void processTask(int id, std::shared_ptr<Banker> banker, std::vector<int> request, std::vector<int> release) {
    if (banker->RequestResources(id, request)) {
        std::cout << "Process " << id << " granted resources.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 模拟资源使用
        banker->ReleaseResources(id, release);
        std::cout << "Process " << id << " released resources.\n";
    } else {
        std::cout << "Process " << id << " denied resources.\n";
    }
}

int main() {
    int numProcesses = 5;
    int numResources = 3;

    std::vector<int> available = {10, 5, 7};
    std::vector<std::vector<int>> max = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };
    std::vector<std::vector<int>> allocation = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    auto banker = std::make_shared<Banker>(numProcesses, numResources, available, max, allocation);
    ThreadPool pool(numProcesses);

    std::vector<std::vector<int>> requests = {
        {1, 0, 2},
        {0, 2, 0},
        {3, 0, 0},
        {2, 1, 1},
        {0, 0, 2}
    };

    std::vector<std::vector<int>> releases = {
        {1, 0, 2},
        {0, 2, 0},
        {3, 0, 0},
        {2, 1, 1},
        {0, 0, 2}
    };

    for (int i = 0; i < numProcesses; ++i) {
        pool.enqueue([=]() {
            processTask(i, banker, requests[i], releases[i]);
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));  // 让任务执行完毕
    return 0;
}