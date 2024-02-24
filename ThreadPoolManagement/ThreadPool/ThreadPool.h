// ThreadPool.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once


#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>


// TODO: 在此处引用程序需要的其他标头。

class ThreadPool {
public:
    // 构造函数
    ThreadPool();    
    void add_task(const std::function<void()>& task);
    void show_threads();
    ~ThreadPool(); // 析构函数
private:
    std::queue<std::function<void()>> tasks;
    const size_t MaxThreadNum = 5;
    const size_t InitThreadNum = 3;
    std::vector<std::thread> threads;
    int tasksNum = 0;
    bool stop = false; // 线程池是否正在运行
    std::mutex queueMutex;
    std::condition_variable cv;
    void execute_tasks(); // 执行任务的主循环
};

