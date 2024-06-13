#include <thread>
#include <iostream>
#include <vector>
#include "ThreadPool.h"
#include "Philosopher.h"

int main() {
    // 线程池基础应用。
    // ThreadPool pool(4);
    // std::vector<std::future<int>> results;
    // for (int i = 0; i < 8; ++i) {
    //     results.emplace_back(pool.enqueue([i]()->int {
    //         std::cout << "thread: " << std::this_thread::get_id() << " is working on task " << i << std::endl;
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //         std::cout << "thread: " << std::this_thread::get_id() << " finished task " << i << std::endl;
    //         return i*i;
    //     }));//执行两件事，把任务入队池，并且把结果存到vector里。
    // }
    // std::cout << "\nend emplace\n";
    // for (auto &&result: results) {
    //     std::cout << result.get() << " \n";//使用get，保证线程执行完成
    // }
    // std::cout << std::endl;


    // // 哲学家问题。
    // int num = 5;
    // auto pool2 = std::make_shared<ThreadPool>(num);
    // std::vector<std::shared_ptr<std::mutex>> forks;
    // for (int i = 0; i < num; ++i) {
    //     forks.push_back(std::make_shared<std::mutex>());
    // }
    // // std::vector<std::mutex> forks(num);
    // std::vector<Philosopher> philos;
    // for (int i = 0; i < num; ++i) {
    //     philos.push_back(Philosopher(i, forks[i], forks[(i+1)%num], pool2));
    // }
    // for (auto &phi : philos) {
    //     phi.dinner();
    // }

    // std::this_thread::sleep_for(std::chrono::seconds(30));
    // // 主线程等待一段时间，让哲学家执行完。
    // return 0;

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