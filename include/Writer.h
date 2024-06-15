#ifndef WRITER_H
#define WRITER_H

#include "ThreadPool.h"
#include <memory>
#include <queue>
#include <string>
// 读写问题和生产者消费者问题是比较接近的。
// 区别在于，写者之间与读者是互斥的，这个用普通的互斥锁就能实现。
// 而多个读者可以同时读取数据，他们之间是不冲突的。
// 都使用互斥锁，可以解决读写问题，但不能实现读者同时读数据。
// 专门用一个类实现读写者之间的锁管理
// 从实际使用上，感觉用LOCK类封装的效果更好。
class RWLock {
private:
    using ptr_mutex = std::shared_ptr<std::mutex>;
    ptr_mutex mtx;
    std::condition_variable writerCond;
    std::condition_variable readerCond;
    int readers;
    int waitingWriter; //通过统计等待中的写者数，实现写者优先功能
    bool writers;
public:
    RWLock(ptr_mutex mutex): mtx(mutex), readers(0), waitingWriter(0), writers(false) {}
    void ReadLock() {
        {
            std::unique_lock<std::mutex> lock(*mtx);
            readerCond.wait(lock, [this]() {
                return !writers && waitingWriter == 0;
            });
            ++readers;
        }
    }

    void ReadUnlock() {
        {
            std::lock_guard<std::mutex> lock(*mtx);
            --readers;
            if (readers == 0) {
                writerCond.notify_one();
            }
        }
    }

    void WriteLock() {
        {
            {//lock_guard与unique_lock的区别？在scope内，lock_guard不会被其他线程抢走锁
            // 在作用域结束后自动释放
            // 不需要判断条件，因此使用比较简单。
                std::lock_guard<std::mutex> lock(*mtx);
                ++waitingWriter;
            }
            // unique_lock还是需要配合condition.wait()使用。
            std::unique_lock<std::mutex> lock(*mtx);
            writerCond.wait(lock, [this]() {
                return readers == 0 && !writers;
            });
            --waitingWriter;
            writers = true;
        }
    }

    void WriteUnlock() {
        {
            std::lock_guard<std::mutex> lock(*mtx);
            writers = false;
            if (waitingWriter > 0) {
                writerCond.notify_one();
            } else {
                readerCond.notify_all();
            }
        }
    }
};

class Writer {
private:
    int id;
    using ptr_label = std::shared_ptr<char>;
    ptr_label lab;
    RWLock &lock;

public:
    Writer(int _id, ptr_label l, RWLock &lo):
    id(_id), lab(l), lock(lo) {}
    void Write() {
        for (int i = 0; i < 15; ++i) {
            {
                lock.WriteLock();
                *lab = char('a' + i);
                std::cout << "Writer " << id << " is writing " << *lab << '\n';
                std::this_thread::sleep_for(std::chrono::seconds(1));
                lock.WriteUnlock();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

class Reader {
private:
    int id;
    using ptr_label = std::shared_ptr<char>;
    ptr_label lab;
    RWLock &lock;

public:
    Reader(int _id, ptr_label l, RWLock &lo):
    id(_id), lab(l), lock(lo) {}
    void Read() {
        for (int i = 0; i < 10; ++i) {
            {
                lock.ReadLock();
                std::cout << "Reader " << id << " is reading " << *lab << '\n';
                // std::this_thread::sleep_for(std::chrono::seconds(1));
                lock.ReadUnlock();
            }
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

#endif