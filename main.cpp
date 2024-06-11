#include <thread>
#include <iostream>
#include <vector>

void do_some_work(int num) {
    std::cout << "thread: " << num << std::endl;
}

int main () {
    int threadNum = 3;
    std::vector<std::thread> threadList;
    threadList.reserve(threadNum);
    for (int idx = 0; idx < threadNum; ++idx) {
        threadList.emplace_back(std::thread{do_some_work, idx});
    }
    std::cout << "work in main thread" << std::endl;

    for (int idx = 0; idx < threadNum; ++idx) {
        threadList[idx].join();
    }
    std::cout << "main thread end" << std::endl;
    
    return 0;
}