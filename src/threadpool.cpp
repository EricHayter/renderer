#include "threadpool.h"
#include <algorithm>
#include <mutex>

void ThreadPool::add_task(const std::function<void ()> &task)
{
	{
		std::lock_guard<std::mutex> lk(m_mut);
		m_tasks.push(task);
	}
	m_cv.notify_one();
}

void ThreadPool::join()
{
	std::unique_lock<std::mutex> lk(m_mut);
	m_cv.wait(lk, [this](){ return m_tasks.empty(); });
}

ThreadPool::ThreadPool() : ThreadPool(std::max(static_cast<int>(std::thread::hardware_concurrency()) - 1, 1)) {}

ThreadPool::ThreadPool(int thread_count) : m_thread_count(thread_count)
{
	m_workers.reserve(m_thread_count);
	auto worker_function = [this](){
		while (true)
		{
			std::unique_lock<std::mutex> lk(m_mut);
			m_cv.wait(lk, [this](){ return m_terminate || not m_tasks.empty(); });
			// ready for cleanup with no more tasks
			if (m_tasks.empty() && m_terminate)
				return;

			// get task
			auto task = m_tasks.front();
			m_tasks.pop();
			lk.unlock();

			task();
		};
	};

	for (int i = 0; i < m_thread_count; i++)
	{
		m_workers.push_back(std::thread(worker_function));
	}
}


ThreadPool::~ThreadPool()
{
	{
		// notify the worker threads to stop
		std::lock_guard<std::mutex> lk(m_mut);
		m_terminate = true;
	}
	m_cv.notify_all();

	for (std::thread& worker: m_workers)
	{
		if (worker.joinable())
			worker.join();
	}
}

