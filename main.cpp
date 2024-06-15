#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include "ThreadPool.h"
#include "Philosopher.h"
#include "Banker.h"
#include "Producer.h"
#include "Writer.h"

// int main() {
//     // 线程池基础应用。
//     // ThreadPool pool(4);
//     // std::vector<std::future<int>> results;
//     // for (int i = 0; i < 8; ++i) {
//     //     results.emplace_back(pool.enqueue([i]()->int {
//     //         std::cout << "thread: " << std::this_thread::get_id() << " is working on task " << i << std::endl;
//     //         std::this_thread::sleep_for(std::chrono::seconds(1));
//     //         std::cout << "thread: " << std::this_thread::get_id() << " finished task " << i << std::endl;
//     //         return i*i;
//     //     }));//执行两件事，把任务入队池，并且把结果存到vector里。
//     // }
//     // std::cout << "\nend emplace\n";
//     // for (auto &&result: results) {
//     //     std::cout << result.get() << " \n";//使用get，保证线程执行完成
//     // }
//     // std::cout << std::endl;


//     // // 哲学家问题。
//     int num = 5;
//     auto pool2 = std::make_shared<ThreadPool>(num);
//     std::vector<std::shared_ptr<std::mutex>> forks;
//     for (int i = 0; i < num; ++i) {
//         forks.push_back(std::make_shared<std::mutex>());
//     }
//     // std::vector<std::mutex> forks(num);
//     std::vector<Philosopher> philos;
//     for (int i = 0; i < num; ++i) {
//         philos.push_back(Philosopher(i, forks[i], forks[(i+1)%num], pool2));
//     }
//     for (auto &phi : philos) {
//         phi.dinner();
//     }

//     std::this_thread::sleep_for(std::chrono::seconds(30));
//     // 主线程等待一段时间，让哲学家执行完。
//     return 0;

// }

// 银行家问题
// void processTask(int id, Banker* banker, std::vector<int> request, std::vector<int> release) {
//     bool flag = true;
//     while (flag) {
//         std::cout << id << ' ';
//         {
//             if (banker->RequestResources(id, request)) {
//                 std::cout << "Process " << id << " granted resources.\n";
//                 std::this_thread::sleep_for(std::chrono::seconds(1));  // 模拟资源使用
//                 banker->ReleaseResources(id, release);
//                 std::cout << "Process " << id << " released resources.\n";
//                 flag = false;
//                 // std::this_thread::sleep_for(std::chrono::seconds(id));
//             } else {
//                 std::cout << "Process " << id << " denied resources.\n";
//                 std::this_thread::sleep_for(std::chrono::seconds(id+1));
//                 // 请求失败需要等待一段时间，不然会一直占用请求。
//                 // id有可能为0，所以需要加常数
//             }
//         }
//     }
// }

// int main() {
//     int numProcesses = 5;
//     int numResources = 3;

//     std::vector<int> available = {10, 5, 7};
//     std::vector<std::vector<int>> max = {
//         {7, 5, 3},
//         {3, 2, 2},
//         {9, 0, 2},
//         {2, 2, 2},
//         {4, 3, 3}
//     };
//     std::vector<std::vector<int>> allocation = {
//         {0, 1, 0},
//         {2, 0, 0},
//         {3, 0, 2},
//         {2, 1, 1},
//         {0, 0, 2}
//     };

//     Banker *bank = new Banker(numProcesses, numResources, available, max, allocation);
//     ThreadPool pool(numProcesses);

//     std::vector<std::vector<int>> requests = {
//         {7, 4, 3},
//         {1, 2, 2},
//         {6, 0, 0},
//         {0, 1, 1},
//         {4, 3, 1}

//         // {1, 0, 2},
//         // {0, 2, 0},
//         // {3, 0, 0},
//         // {2, 1, 1},// 
//         // {0, 0, 2} // 这两组请求超过need了，所以永远不会成功。

//         // {0, 0, 0},// 分配大量资源，并且请求0资源，但是依然执行失败，说明是线程分配出现问题
//         // {0, 0, 0},
//         // {0, 0, 0},
//         // {0, 0, 0},
//         // {0, 0, 0}
//     };

//     std::vector<std::vector<int>> releases = {
//         {7, 4, 3},
//         {1, 2, 2},
//         {6, 0, 0},
//         {0, 1, 1},
//         {4, 3, 1}
//         // {1, 0, 2},
//         // {0, 2, 0},
//         // {3, 0, 0},
//         // {2, 1, 1},
//         // {0, 0, 2}
//         // {0, 0, 0},
//         // {0, 0, 0},
//         // {0, 0, 0},
//         // {0, 0, 0},
//         // {0, 0, 0}
//     };

//     for (int i = 0; i < numProcesses; ++i) {
//         pool.enqueue([=]() {
//             processTask(i, bank, requests[i], releases[i]);
//         });
//     }

//     std::this_thread::sleep_for(std::chrono::seconds(10));  // 让任务执行完毕
//     return 0;
// }

// 生产者消费者问题
// int main() {
//     ThreadPool pool(5);
//     auto mtx = std::make_shared<std::mutex>();
//     std::condition_variable conditon;
//     int numProducer = 2;
//     int numConsumer = 3;
//     int numProitem = 15;
//     int numConitem = 10;
//     auto buffer = std::make_shared<std::queue<int>>();
//     int maxBufferSize = 10; 
//     // std::vector<Producer*> pro(numProducer);
//     // std::vector<Consumer*> con(numConsumer);
//     for (int i = 0; i < numProducer; ++i) {
//         // pro.push_back(new Producer(i, numProitem, buffer, maxBufferSize, mtx));
//         pool.enqueue([=, &conditon]() {// 混合值传递和引用传递。condition必须用引用。
//             Producer(i, numProitem, buffer, maxBufferSize, mtx, conditon).Produce();
//         });
//     }
//     // 为什么这里需要分开引用和值呢？原因出在i上，需要把i用值引用，每个生产者的id都一样了。
//     for (int j = 0; j < numConsumer; ++j) {
//         // con.push_back(new Consumer(i, numConitem, buffer, maxBufferSize, mtx));
//         pool.enqueue([=, &mtx, &conditon]() {//mtx是指针，所以用不用引用都对。
//             Consumer(j, numConitem, buffer, maxBufferSize, mtx, conditon).Consume();
//         });
//     }
//     std::this_thread::sleep_for(std::chrono::seconds(50));
//     return 0;
// }

// 读者写者问题
int main() {
    ThreadPool pool(4);
    auto mtx = std::make_shared<std::mutex>();
    // std::condition_variable conditon;
    RWLock rwLock(mtx);
    int numWriter = 2;
    int numReader = 4;
    auto label = std::make_shared<char>('\0');
    for (int i = 0; i < numWriter; ++i) {
        pool.enqueue([=, &rwLock]() {
            Writer(i, label, rwLock).Write();
        });
    }
    for (int j = 0; j < numReader; ++j) {
        pool.enqueue([=, &rwLock]() {
            Reader(j, label, rwLock).Read();
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(50));
    return 0;
}
// void do_some_work(int num) {
//     std::cout << "thread: " << num << std::endl;
// }

// int main () {
//     int threadNum = 3;
//     std::vector<std::thread> threadList;
//     threadList.reserve(threadNum);
//     for (int idx = 0; idx < threadNum; ++idx) {
//         threadList.emplace_back(std::thread{do_some_work, idx});
//     }
//     std::cout << "work in main thread" << std::endl;

//     for (int idx = 0; idx < threadNum; ++idx) {
//         threadList[idx].join();
//     }
//     std::cout << "main thread end" << std::endl;
    
//     return 0;
// }