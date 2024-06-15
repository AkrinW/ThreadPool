#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include "ThreadPool.h"
#include "Producer.h"


int main() {
    ThreadPool pool(5);
    auto mtx = std::make_shared<std::mutex>();
    std::condition_variable conditon;
    int numProducer = 2;
    int numConsumer = 3;
    int numProitem = 15;
    int numConitem = 10;
    auto buffer = std::make_shared<std::queue<int>>();
    int maxBufferSize = 10; 
    // std::vector<Producer*> pro(numProducer);
    // std::vector<Consumer*> con(numConsumer);
    for (int i = 0; i < numProducer; ++i) {
        // pro.push_back(new Producer(i, numProitem, buffer, maxBufferSize, mtx));
        pool.enqueue([=, &conditon]() {// 混合值传递和引用传递。condition必须用引用。
            Producer(i, numProitem, buffer, maxBufferSize, mtx, conditon).Produce();
        });
    }
    // 为什么这里需要分开引用和值呢？原因出在i上，需要把i用值引用，每个生产者的id都一样了。
    for (int j = 0; j < numConsumer; ++j) {
        // con.push_back(new Consumer(i, numConitem, buffer, maxBufferSize, mtx));
        pool.enqueue([=, &mtx, &conditon]() {//mtx是指针，所以用不用引用都对。
            Consumer(j, numConitem, buffer, maxBufferSize, mtx, conditon).Consume();
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(50));
    return 0;
}
