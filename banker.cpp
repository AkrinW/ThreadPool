#include <iostream>
#include "ThreadPool.h"
#include "Banker.h"
#include <memory>
#include <vector>

// 除了死锁，还会有活锁问题。
// 双方互相请求锁失败后，又同时将锁释放，导致还是没有线程成功获取锁。
// 银行家问题让人感觉更像是单线程问题。
void processTask(int id, Banker* banker, std::vector<int> request, std::vector<int> release) {
    bool flag = true;
    while (flag) {
        std::cout << id << ' ';
        {
            if (banker->RequestResources(id, request)) {
                std::cout << "Process " << id << " granted resources.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));  // 模拟资源使用
                banker->ReleaseResources(id, release);
                std::cout << "Process " << id << " released resources.\n";
                flag = false;
                // std::this_thread::sleep_for(std::chrono::seconds(id));
            } else {
                std::cout << "Process " << id << " denied resources.\n";
                std::this_thread::sleep_for(std::chrono::seconds(id+1));
                // 请求失败需要等待一段时间，不然会一直占用请求。
                // id有可能为0，所以需要加常数
            }
        }
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

    Banker *bank = new Banker(numProcesses, numResources, available, max, allocation);
    ThreadPool pool(numProcesses);

    std::vector<std::vector<int>> requests = {
        {7, 4, 3},
        {1, 2, 2},
        {6, 0, 0},
        {0, 1, 1},
        {4, 3, 1}

        // {1, 0, 2},
        // {0, 2, 0},
        // {3, 0, 0},
        // {2, 1, 1},// 
        // {0, 0, 2} // 这两组请求超过need了，所以永远不会成功。

        // {0, 0, 0},// 分配大量资源，并且请求0资源，但是依然执行失败，说明是线程分配出现问题
        // {0, 0, 0},
        // {0, 0, 0},
        // {0, 0, 0},
        // {0, 0, 0}
    };

    std::vector<std::vector<int>> releases = {
        {7, 4, 3},
        {1, 2, 2},
        {6, 0, 0},
        {0, 1, 1},
        {4, 3, 1}
        // {1, 0, 2},
        // {0, 2, 0},
        // {3, 0, 0},
        // {2, 1, 1},
        // {0, 0, 2}
        // {0, 0, 0},
        // {0, 0, 0},
        // {0, 0, 0},
        // {0, 0, 0},
        // {0, 0, 0}
    };

    for (int i = 0; i < numProcesses; ++i) {
        pool.enqueue([=]() {
            processTask(i, bank, requests[i], releases[i]);
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));  // 让任务执行完毕
    return 0;
}
