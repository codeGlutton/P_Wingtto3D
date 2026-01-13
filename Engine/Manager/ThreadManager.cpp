#include "pch.h"
#include "ThreadManager.h"

#include "Utils/Thread/JobQueue.h"

#include "Utils/Thread/Thread.h"

ThreadManager::ThreadManager() :
	_mIsThreadAlive(true)
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	_mIsThreadAlive.store(false);
	Join();
}

void ThreadManager::Launch(std::function<void()> work, std::function<bool()> condition)
{
	Launch(std::make_shared<CustomThread>(work, condition));
}

void ThreadManager::Launch(std::shared_ptr<Thread> thread)
{
	std::lock_guard<std::mutex> guard(_mLock);

	_mThreads.push_back(std::thread([this, thread]()
		{
			InitTLS();
			thread->Run();
			DestroyTLS();
		}
	));
}

void ThreadManager::Join()
{
	for (std::thread& t : _mThreads)
	{
		if (t.joinable() == true)
		{
			t.join();
		}
	}
	_mThreads.clear();
}

void ThreadManager::PushGlobalSequentialJobQ(std::shared_ptr<SequentialJobQueue> jobQueue)
{
	_mGlobalSequentialJobQueues.Push(jobQueue);
}

void ThreadManager::PushGlobalConcurrentJobQ(std::shared_ptr<ConcurrentJobQueue> jobQueue)
{
	_mGlobalConcurrentJobQueue->Append(jobQueue);
}

void ThreadManager::PushGlobalConcurrentJob(std::shared_ptr<Job> job, bool pushOnly)
{
	if (job != nullptr)
	{
		return;
	}
	_mGlobalConcurrentJobQueue->DoAsync(*job, pushOnly);
}

void ThreadManager::DoGlobalJob()
{
	uint64 now;
	while (true)
	{
		{
			now = GetTickCount64();
			if (now > LEndTickCount)
			{
				break;
			}

			_mGlobalConcurrentJobQueue->Execute();
		}
		{
			now = GetTickCount64();
			if (now > LEndTickCount)
			{
				break;
			}

			std::shared_ptr<SequentialJobQueue> globalSequentialJobQueue = PopFromGlobalJobQ();
			if (globalSequentialJobQueue == nullptr)
			{
				break;
			}
			globalSequentialJobQueue->Execute();
		}
	}
}

std::shared_ptr<SequentialJobQueue> ThreadManager::PopFromGlobalJobQ()
{
	return _mGlobalSequentialJobQueues.Pop();
}

void ThreadManager::InitTLS()
{
	static std::atomic<uint32> threadId = 1;
	LThreadId = threadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}

void GlobalWorkerThread::Work()
{
	THREAD_MANAGER->DoGlobalJob();
}

