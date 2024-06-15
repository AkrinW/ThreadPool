#include "ThreadPool.h"
#include "Writer.h"
#include <iostream>
#include <thread>
#include <memory>

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