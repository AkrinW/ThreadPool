#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

std::mutex mutex;//锁
std::condition_variable condition;//条件变量
bool ready = false;//判断条件
//用于实现多线程间的条件同步。允许多个线程等待条件成立，当成立时，线程被唤醒继续执行。
//std::mutex保证只有1个线程访问共享数据。
//std::condition_variable在特定条件下唤醒等待的线程。
//等待条件。线程进入临界区时，检查条件，不满足的话，线程会释放锁，并且等待。
//通知其他线程。条件变为true时，一个线程会通知其他线程。
//重新检查。被通知的线程重新获得锁，并且重新检查。如果仍然不满足，线程会继续等待。

//可用函数
    // 使当前进程等待，参数中的锁被释放，允许其他线程访问。
    // condition.wait(std::unique_lock<std::mutex> &lock);
    
    // 指定一个超时时间，如果到时间了还没成立条件，就会进入等待。
    // condition.wait_for(std::unique_lock<std::mutex> &__lock, const std::chrono::duration<_Rep, _Period> &__rtime)
    // condition.wait_until(std::unique_lock<std::mutex> &__lock, const std::chrono::time_point<_Clock, _Duration> &__atime)

    //唤醒在等待的线程
    // condition.notify_one();
    // condition.notify_all();

    // 为什么要用unique_lock?和unique_ptr一样，它保证只有一个指针指向锁。从而实现锁的独占机制，保证了线程的安全性。
    // condition中的函数要求对mutex独占。

void PrintID(int id) {
    std::unique_lock<std::mutex> lock(mutex);//构造指针
    while (!ready) {
        std::cout << "before wait" << id << '\n';
        condition.wait(lock);//不满足条件就等待。
        std::cout << "after wait" << id << '\n';
    }
    std::cout << "isready" << id << '\n';
}

void Signal() {
    std::unique_lock<std::mutex> lock(mutex);
    ready = true;
    condition.notify_all();//唤醒所有进程。
}

int main() {
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(PrintID, i);
        std::cout << "init thread" << i << '\n';//创建线程时，进程和线程的函数同时进行。输出结果是乱序抢占的。
    }
    std::cout << "10 threads ready to race\n";
    //信号唤醒前，所有线程卡在wait()函数上。
    Signal();//唤醒线程
    std::cout << "after signal()\n";
    for (int i = 0; i < 100; ++i) {
        std::cout << i << ' ';
    }//这一段for循环，体现出线程的执行，在唤醒后与主线程抢占。
    //执行完全部函数后，线程就停下了。没有后续输出。
    for (auto &th: threads) {//遍历已有的线程，必须用引用
        std::cout << "in join ready" << th.get_id() << '\n';
        th.join();
        std::cout << "join thread" << th.get_id() << '\n';//join后，无法获取th的id。
    }
    return 0;
}