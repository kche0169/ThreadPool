// ThreadPool.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "ThreadPool.h"

//using namespace std;

ThreadPool::ThreadPool() {
	for (size_t i = 0; i < InitThreadNum; i++) {
		// threads.emplace_back(std::bind(&ThreadPool::execute_tasks, this));
		threads.emplace_back([this] { this->execute_tasks(); });

	}
}

ThreadPool::~ThreadPool() {
	{
		//锁住
		//设置CV,通知
		std::unique_lock<std::mutex> lock(queueMutex);
		stop = true;
	}
	cv.notify_all();

	for (std::thread& thread : threads) {
		thread.join();  
	}
}

void ThreadPool::add_task(const std::function<void()>& task) {
	
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		tasks.push(task);
	}
	cv.notify_one();
	// 告知下一个等待操作任务队列的进程可以行动
};



void ThreadPool::execute_tasks() {
	while (true) {
		std::function<void()> task;
		// 生成任务
		
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			// 锁住队列
			// 在满足条件之前释放
			cv.wait(lock, [this] { return stop || !tasks.empty(); });

			if (stop && tasks.empty()) {
				return;
			}

			task = tasks.front();
			tasks.pop();
			
		}

		task();
		// 执行任务
	}
}; 

void ThreadPool::show_threads() {
	for (std::thread & t : threads) {
		std::cout << t.get_id() << std::endl;
	}
}
void workThread(const int id) {
	std::cout << "线程 "<< id << " 正在处理" << std::endl;
}

int main() {
	ThreadPool threadPool;
	threadPool.show_threads();
	for (int i = 0; i < 10; ++i) {
		if (i % 2 == 0) {
			threadPool.add_task([i] {
						std::cout << "Task "<< i << " executed by thread " << std::this_thread::get_id() << std::endl;
						});
		}
		else {
			threadPool.add_task([i] {
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				std::cout << "Task " << i << " executed by thread " << std::this_thread::get_id() << std::endl;
				});
		}
		
	}


	// 确保所有任务执行完毕再退出
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
