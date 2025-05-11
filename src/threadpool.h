#pragma once

#include <queue>
#include <mutex>
#include <functional>
#include <thread>
#include <condition_variable>
#include <algorithm>

class ThreadPool
{
	public:
	void add_task(const std::function<void ()> &task);
	void join();
	int thread_count() { return m_thread_count; };

	ThreadPool();
	ThreadPool(int thread_count);

	// copy constructors are not allowed.
	ThreadPool(const ThreadPool &other) = delete;
	ThreadPool operator=(const ThreadPool &other) = delete;

	// we do allow moves if desired.
	ThreadPool(const ThreadPool &&other) noexcept;
	ThreadPool& operator=(const ThreadPool &&other) noexcept;

	~ThreadPool();

	private:
	std::vector<std::thread> m_workers;
	std::queue<std::function<void ()>> m_tasks;
	std::mutex m_mut;
	std::condition_variable m_cv;
	int m_thread_count;
	bool m_terminate = false;
};
