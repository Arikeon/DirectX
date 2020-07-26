#pragma once
//https://www.youtube.com/watch?v=eWTGtp3HXiw Tutorial used

#include "Build.h"
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>

using Task = std::function<void()>;

class CThreadPool
{
public:
	explicit CThreadPool(uint32_t _NumThreads);

	~CThreadPool();

	template<class Type>
	INLINE auto EnqueTask(Type _Task)->std::future<decltype(_Task())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(_Task()) ()>>(std::move(_Task));
		{
			std::unique_lock<std::mutex> lock{ m_eventMutex };
			m_tasks.emplace([=] {
				(*wrapper)();
				});
		}

		m_eventVar.notify_one();
		return wrapper->get_future();
	}

	static CThreadPool*& GetInstance(void);
	static void ClearInstancePtr(void);
private:
	bool bStop = false;
	INLINE void Initialize(uint32_t _NumThreads);
	INLINE void End() noexcept;

	std::queue<Task> m_tasks;
	std::vector<std::thread> m_threads;
	std::condition_variable m_eventVar;
	std::mutex m_eventMutex;
};