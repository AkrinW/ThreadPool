#ifndef BANKER_H
#define BANKER_H

#include "ThreadPool.h"
#include <memory>

// 银行家问题考虑资源分配顺序的问题。
// 当有多个线程同时请求资源时，为了避免资源不足而死锁，需要在每次分配时检查是否进入死锁。
// 对于已经完成的线程，释放它已占据的资源。
class Banker {
private:
int numProcess; //请求的进程个数
int numResources; // 需要分配的资源类型
std::vector<int> available; //各资源初始的可用数量
std::vector<std::vector<int>> max; // 二维矩阵，表示每个进程每个资源的需求最大值
std::vector<std::vector<int>> allocation; // 二维矩阵，表示每个进程每个资源已分配数量
std::vector<std::vector<int>> need; // 二维矩阵，表示每个进程还需要的资源数量。
std::mutex mtx;
std::condition_variable condition;
bool isSafeState();

public:
    Banker(int np, int nr, std::vector<int> ava, std::vector<std::vector<int>> m,
    std::vector<std::vector<int>> alloc);// 初始化最大值和已分配数量
    bool RequestResources(int id, std::vector<int> request);
    void ReleaseResources(int id, std::vector<int> release);
};

Banker::Banker(int np, int nr, std::vector<int> ava, 
std::vector<std::vector<int>> m, std::vector<std::vector<int>> alloc)
: numProcess(np), numResources(nr), available(ava), max(m),
allocation(alloc), need(numProcess, std::vector<int>(numResources)) {
    for (int i = 0; i < numProcess; ++i) {
        for (int j = 0; j < numResources; ++j) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
}

void Banker::ReleaseResources(int id, std::vector<int> release) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        for (int i = 0; i < numResources; ++i) {
            available[i] += release[i];
            allocation[id][i] -= release[i];
            need[id][i] += release[i];
        }//把某一个进程的资源全部释放了。
        condition.notify_all();
    }
}

bool Banker::RequestResources(int id, std::vector<int> request) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        for (int i = 0; i < numResources; ++i) {
            if (request[i] > need[id][i] || request[i] > available[i]) {
                return false;
            }
        }//检查能否分配资源，请求不能超过已有资源和进程还需要的资源数。

        for (int i = 0; i < numResources; ++i) {
            available[i] -= request[i];
            allocation[id][i] += request[i];
            need[id][i] -= request[i];
        }// 分配资源。

        if (!isSafeState()) {//检查目前是否线程安全。
            for (int i = 0; i < numResources; ++i) {// 不安全，把资源返回。
                available[i] += request[i];
                allocation[id][i] -= request[i];
                need[id][i] += request[i];
            }
            return false;
        }
    }
    return true;
}

bool Banker::isSafeState() {
    std::vector<int> work = available;
    std::vector<bool> finish(numProcess, false);//finish表示的是这个进程是否可以完成
    // all_of 判断范围内是否所有元素都满足条件。
    for (int i = 0; i < numProcess; ++i) {
        // 检查每个进程，是否剩余的资源数可以完成全部未进程。
        if (!finish[i]) {
            bool flag = true;
            for (int j = 0; j < numResources; ++j) {
                if (need[i][j] > work[j]) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                for (int j = 0; j < numResources; ++j) {
                    work[j] += allocation[i][j];
                }
                finish[i] = true;
                i = -1;
            }
        }
        // 执行错误的原因出现在这里，work[i]会数组越界，发生未定义问题，gpt的代码不可以全信。
        // if (!finish[i] && std::all_of(need[i].begin(), need[i].end(), [&work, i](int n) { 
        //     return n <= work[i];//对于一个未完成的进程，看它是否能用剩余资源完成。
        // })) {//true, 可以完成，就把他的资源回收到work里
        //     for (int j = 0; j < numProcess; ++j) {
        //         work[j] += allocation[i][j];
        //     }
        //     finish[i] = true;// 回收说明完成这个进程了。
        //     i = -1; // 从头开始，检查其他的进程
        // }
    }

    return std::all_of(finish.begin(), finish.end(), [](bool f) { 
        return f;
    });// 都完成了，说明线程安全，否则不安全。
}

#endif