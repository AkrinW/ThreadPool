#include <thread>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <functional>

void do_some_work(int num) {
    std::cout << "thread: " << num << std::endl;
}

bool thread_is_active(const std::thread::id thread_id) { 
        return thread_id != std::thread::id();
}

void print_num(int i) {
    std::cout << i << '\n';
}


int main () {
    int threadNum = 3;
    std::vector<std::thread> threadList;
    threadList.reserve(threadNum);
    //线程对主程序是抢占式的，因此会乱序输出。
    // for (int idx = 0; idx < threadNum; ++idx) {
    //     threadList.emplace_back(std::thread{do_some_work, idx});
    // }
    // std::cout << "work in main thread" << std::endl;
    //join用于阻塞当前执行的线程，直到执行的join的线程完成。
    // for (int idx = 0; idx < threadNum; ++idx) {
    //     std::cout << idx << threadList[idx].get_id() << std::endl;
    //     threadList[idx].join();
    //     std::cout << idx << threadList[idx].get_id() << std::endl;
    // }
    // std::cout << "main thread end" << std::endl;
    //未初始化时，get_id()提示信息
    // std::thread trd;
    // std::cout << thread_is_active(trd.get_id());
    //初始化后，get_id()显示信息
    std::thread trd2{[]{
        std::cout << "work in sub thread\n";
    }};
    std::cout << trd2.get_id() << std::endl;
    trd2.join();
    //绑定函数
    auto invoker = std::bind(print_num,-3);
    invoker();
    return 0;
}