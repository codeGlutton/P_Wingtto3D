#include "pch.h"
#include "ThreadManager.h"

#include "Utils/Thread/JobQueue.h"
#include "Utils/Thread/MPSCJobQueue.h"
#include "Utils/Thread/SceneJobQueue.h"

void GlobalWorkerThread::Work()
{
	THREAD_MANAGER->DoGlobalJob();
}

ThreadManager::ThreadManager() :
	_mIsAlive(true),
	_mGlobalConcurrentJobQueue(std::make_shared<ConcurrentJobQueue>()),
	_mGameThreadJobQueue(std::make_shared<MPSCJobQueue>())
{
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::Init()
{
	InitTLS_Internal(MainThreadType::Game);
}

void ThreadManager::Destroy()
{
	_mIsAlive.store(false);
	Join();
	DestroyTLS();
}

void ThreadManager::Launch(std::function<void()> work, std::function<bool()> condition)
{
	ASSERT_THREAD(MainThreadType::Game);
	Launch(std::make_shared<CustomThread>(work, condition));
}

void ThreadManager::Launch(std::shared_ptr<WorkerThread> thread)
{
	ASSERT_THREAD(MainThreadType::Game);
	std::lock_guard<std::mutex> guard(_mLock);

	_mThreads.push_back(std::thread([this, thread]()
		{
			InitTLS();
			thread->Init();
			thread->Run();
			thread->Destroy();
			DestroyTLS();

			const std::thread::id id = std::this_thread::get_id();
			PushGameThreadJob(ObjectPool<Job>::MakeShared([this, id]() {
				for (auto iter = _mThreads.begin(); iter != _mThreads.end(); ++iter)
				{
					if (id == iter->get_id())
					{
						_mThreads.erase(iter);
						break;
					}
				}
			}));
		}
	));
}

void ThreadManager::LaunchMainThreads(std::array<std::shared_ptr<MainThread>, MainThreadType::Count> threads)
{
	ASSERT_THREAD(MainThreadType::Game);
	for (uint32 threadId = 0; threadId < MainThreadType::Count; ++threadId)
	{
		std::lock_guard<std::mutex> guard(_mLock);

		_mThreads.push_back(std::thread([this, threadId, thread = threads[threadId]]()
			{
				InitTLS_Internal(threadId);
				thread->Init();
				thread->Run();
				thread->Destroy();
				DestroyTLS();

				const std::thread::id id = std::this_thread::get_id();
				PushGameThreadJob(ObjectPool<Job>::MakeShared([this, id]() {
					for (auto iter = _mThreads.begin(); iter != _mThreads.end(); ++iter)
					{
						if (id == iter->get_id())
						{
							_mThreads.erase(iter);
							break;
						}
					}
				}));
			}
		));
	}

	// 게임 스레드 동작은 App의 Update에서 처리 중
}

void ThreadManager::Join()
{
	ASSERT_THREAD(MainThreadType::Game);
	for (std::thread& t : _mThreads)
	{
		if (t.joinable() == true)
		{
			t.join();
		}
	}
	_mThreads.clear();
}

void ThreadManager::PushGameThreadJob(std::shared_ptr<Job> job)
{
	if (job != nullptr)
	{
		return;
	}
	_mGameThreadJobQueue->DoAsync(*job);
}

void ThreadManager::DoGameJob()
{
	ASSERT_THREAD(MainThreadType::Game);
	_mGameThreadJobQueue->Execute();
}

void ThreadManager::PushRenderThreadLogicUpdateJob(std::shared_ptr<Job> job)
{
	if (job != nullptr)
	{
		return;
	}
	_mRenderThreadLogicUpdateJobQueue->DoAsync(*job);
}

void ThreadManager::PushRenderThreadSceneUpdateJob(std::shared_ptr<JobContext<float>> job)
{
	if (job != nullptr)
	{
		return;
	}
	_mRenderThreadSceneUpdateJobQueue->DoAsync(*job);
}

void ThreadManager::PushRenderThreadRenderJob(std::shared_ptr<Job> job)
{
	if (job != nullptr)
	{
		return;
	}
	_mRenderThreadRenderJobQueue->DoAsync(*job);
}

void ThreadManager::DoRenderJob()
{
	ASSERT_THREAD(MainThreadType::Render);

	// 콜백 주로 처리
	_mRenderThreadLogicUpdateJobQueue->ExecuteOnce();
	// 씬 내부 데이터 업데이트
	_mRenderThreadSceneUpdateJobQueue->ExecuteOnce();

	// 위젯 랜더 (SceneViewport 존재 시, 월드도 랜더)
	_mRenderThreadRenderJobQueue->ExecuteOnlyLastOne();
}

void ThreadManager::PushGlobalSequentialJobQ(std::shared_ptr<SequentialJobQueue> jobQueue)
{
	_mGlobalSequentialJobQueues.Push(jobQueue);
}

void ThreadManager::PushGlobalConcurrentJob(std::shared_ptr<Job> job, bool pushOnly)
{
	if (job != nullptr)
	{
		return;
	}
	_mGlobalConcurrentJobQueue->DoAsync(*job, pushOnly);
}

void ThreadManager::PushGlobalConcurrentJobQ(std::shared_ptr<ConcurrentJobQueue> jobQueue)
{
	_mGlobalConcurrentJobQueue->Append(jobQueue);
}

void ThreadManager::DoGlobalJob()
{
	uint64 now;
	{
		now = GetTickCount64();
		if (now > LEndTickCount)
		{
			return;
		}

		_mGlobalConcurrentJobQueue->Execute();
	}
	{
		now = GetTickCount64();
		if (now > LEndTickCount)
		{
			return;
		}

		std::shared_ptr<SequentialJobQueue> globalSequentialJobQueue = _mGlobalSequentialJobQueues.Pop();
		if (globalSequentialJobQueue == nullptr)
		{
			return;
		}
		globalSequentialJobQueue->Execute();
	}
}

void ThreadManager::InitTLS()
{
	static std::atomic<uint32> threadId = MainThreadType::Count;
	uint32 newId = threadId.fetch_add(1);
	InitTLS_Internal(newId);
}

void ThreadManager::InitTLS_Internal(uint32 threadId)
{
	LThreadId = threadId;
}

void ThreadManager::DestroyTLS()
{
}


