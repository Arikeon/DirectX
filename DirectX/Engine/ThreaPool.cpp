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
	const static auto lambdaCondition = [&]() { return bStop || ExitThread || !tasks.empty(); };

	for (std::size_t perThread = 0; perThread < _NumThreads; ++perThread)
	{
		threads.emplace_back([=] {
			while (true)
			{
				Task task;

				{
					std::unique_lock<std::mutex> lock{ eventMutex };
					eventVar.wait(lock, lambdaCondition);

					if (tasks.empty())
						break;

					task = std::move(tasks.front());
					tasks.pop();
				}

				task();
			}
			});
	}
}

void CThreadPool::End() noexcept
{
	{
		std::unique_lock<std::mutex> lock{ eventMutex };
		bStop = true;
	}

	eventVar.notify_all();

	for (auto& thread : threads)
		thread.join();
}

CThreadPool*& CThreadPool::GetInstance()
{
	static uint32_t ThreadCount = std::thread::hardware_concurrency();
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