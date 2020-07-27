#pragma warning( push )
#pragma warning( disable : 4551 )
#include "ThreadPool.h"

CThreadPool::CThreadPool(unsigned int _NumThreads)
{
	Initialize(_NumThreads);
}

CThreadPool::~CThreadPool()
{
	End();
}

void CThreadPool::Initialize(uint32_t _NumThreads)
{
	const static auto lambdaCondition = [&]() { return bStop || ExitThread || !m_tasks.empty(); };

	for (std::size_t perThread = 0; perThread < _NumThreads; ++perThread)
	{
		m_threads.emplace_back([=] {
			while (true)
			{
				Task task;

				{
					std::unique_lock<std::mutex> lock{ m_eventMutex };
					m_eventVar.wait(lock, lambdaCondition);

					if (m_tasks.empty())
						break;

					task = std::move(m_tasks.front());
					m_tasks.pop();
				}

				task();
			}
			});
	}
}

void CThreadPool::End() noexcept
{
	{
		std::unique_lock<std::mutex> lock{ m_eventMutex };
		bStop = true;
	}

	m_eventVar.notify_all();

	for (auto& thread : m_threads)
		thread.join();
}

CThreadPool*& CThreadPool::GetInstance()
{
	static uint32 ThreadCount = std::thread::hardware_concurrency();
	static CThreadPool* Instance = nullptr;
	bool bInitialized = false;

	if (!bInitialized)
	{
		Instance = new CThreadPool(ThreadCount);
		bInitialized = true;
	}
	return Instance;
}

void CThreadPool::ClearInstancePtr()
{
}

#pragma warning( pop )