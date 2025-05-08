#pragma once

#include <queue>
#include <mutex>
#include <functional>
#include <thread>
#include <condition_variable>

class ThreadPool
{
	private:
	std::queue<std::function<void ()>> pool;
	std::mutex mut;
	std::condition_variable cv;
	
	public:
	void await_add_task(const std::function<void ()> &task);
	void join();
}
