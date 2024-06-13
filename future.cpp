#include <iostream>
#include <future>
#include <thread>
#include <chrono>//时间函数

// 异步调用，将耗时，阻塞的任务交给其他线程调用，保证当前线程快速响应。
// 异步调用可以阻塞当前的线程，直到异步操作完成。
// 确保执行其他操作前，已经获得了异步的结果。
// 提高多核性能。
// 使用场景分析:
// 1. 异步任务。文件读写，网络请求，计算密集型。std::future表示这些任务的结果。
// 实现任务与主线程分离。
// 2. 并发控制。需要等待某线程完成后执行后续，std::future实现同步。
// 3. 结果获取。用安全的方式获取异步结果。std::future::get()，这个函数会阻塞当前进程。
// 因此调用get时，我们能肯定得到异步的结果。

int LongRunTask() {
    std::cout << "before sleep\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "after sleep\n";
    return 42;
}

int main() {
    std::cout << "before future\n";
    std::future<int> future_result = std::async(LongRunTask);
    std::cout << "after future\n";
    std::cout << "before get\n";
    int result = future_result.get();//get后已经执行完了线程内容。
    std::cout << "after get\n";
    std::cout << result << '\n';
    return 0;
}