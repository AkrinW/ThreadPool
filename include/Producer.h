#ifndef PRODUCER_H
#define PRODUCER_H

#include "ThreadPool.h"
#include <memory>
#include <queue>
// 生产者与消费者之间用缓冲区连接，生产者把数据存入缓冲区，消费者从缓冲区提取数据。
// 使用多线程主要用来解决两者之间数据处理速度不同的问题，避免一方过度等待。
class Producer {
private:
    int id;
    int itemnum;//要生产的数据数量
    using ptr_mutex = std::shared_ptr<std::mutex>;
    ptr_mutex mtx;
    std::condition_variable &condition;
    using ptr_buffer = std::shared_ptr<std::queue<int>>;
    ptr_buffer buffer;
    int maxBufferSize;
    bool ifProduce() {
        return buffer->size() < maxBufferSize;
    }
public:
    Producer(int _id, int item, ptr_buffer buf, int m, ptr_mutex mutex,
    std::condition_variable &cond): id(_id), itemnum(item), 
    buffer(buf), maxBufferSize(m), mtx(mutex), condition(cond) {}
    void Produce() {
        for (int i = 0; i < itemnum; ++i) {
            {
                std::unique_lock<std::mutex> lock(*mtx);
                condition.wait(lock, [this]() {
                    return ifProduce();
                });
                buffer->push(i);
                std::cout << "Producer" << this->id << "produce item" << i << '\n';
                condition.notify_all();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

class Consumer {
private:
    int id;
    int itemnum;// 要消费的数量
    using ptr_mutex = std::shared_ptr<std::mutex>;
    ptr_mutex mtx;
    std::condition_variable &condition;
    using ptr_buffer = std::shared_ptr<std::queue<int>>;
    ptr_buffer buffer;
    int maxBufferSize;
    bool ifConsume() {
        return !buffer->empty();
    }
public:
    Consumer(int id, int item, ptr_buffer buf, int m, ptr_mutex mutex,
    std::condition_variable &cond): id(id), itemnum(item), 
    buffer(buf), maxBufferSize(m), mtx(mutex), condition(cond) {}
    void Consume() {
        for (int i = 0; i < itemnum; ++i) {
            {
                std::unique_lock<std::mutex> lock(*mtx);
                condition.wait(lock, [this]() {
                    return ifConsume();
                });
                int item = buffer->front();
                buffer->pop();
                std::cout << "Consumer" << this->id << "consume item" << item << '\n';
                //用于模拟生产消费速度不同情况
                condition.notify_all();
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
};

#endif