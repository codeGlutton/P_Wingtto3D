#pragma once

#include <thread>
#include <mutex>

#include "Utils/Thread/MainThread.h"
#include "Utils/Thread/LockQueue.h"
#include "Utils/Thread/DeadLockProfiler.h"

#define THREAD_MANAGER ThreadManager::GetInst()

class Job;
template<typename MetaData>
class JobContext;
class MPSCJobQueue;
class SequentialJobQueue;
class ConcurrentJobQueue;
class SceneJobQueue;

/**
 * Thread Manager의 Global Queue를 처리하는 스레드
 */
class GlobalWorkerThread : public WorkerThread
{
private:
	virtual void Work() override;
};

class ThreadManager
{
private:
	ThreadManager();
	~ThreadManager();

public:
	static ThreadManager* GetInst()
	{
		static ThreadManager inst;
		return &inst;
	}

public:
	void Init();
	void Destroy();

public:
	void Launch(std::function<void()> work, std::function<bool()> condition = nullptr);
	void Launch(std::shared_ptr<WorkerThread> thread);
	void LaunchMainThreads(std::array<std::shared_ptr<MainThread>, MainThreadType::Count - 1> threads);
	void Join(bool isForced = true);

public:
	bool IsAlive() const
	{
		return _mIsAlive.load();
	}

	/* 게임 Job Q */
public:
	void PushGameThreadJob(std::shared_ptr<Job> job);

public:
	void DoGameJob();
	void ClearGameJob();

	/* 랜더 Job Q */
public:
	void PushRenderThreadLogicUpdateJob(std::shared_ptr<Job> job);
	void PushRenderThreadSceneUpdateJob(std::shared_ptr<JobContext<float>> job);
	void PushRenderThreadRenderJob(std::shared_ptr<Job> job);

public:
	void DoRenderJob();
	void ClearRenderJob();

	/* 글로벌 Job Q */
public:
	void PushGlobalSequentialJobQ(std::shared_ptr<SequentialJobQueue> jobQueue);

	void PushGlobalConcurrentJob(std::shared_ptr<Job> job, bool pushOnly = false);
	void PushGlobalConcurrentJobQ(std::shared_ptr<ConcurrentJobQueue> jobQueue);

public:
	void DoGlobalJob();
	void ClearGlobalJob();

	/* 디버깅 */
#ifdef _DEBUG
public:
	DeadLockProfiler& GetDeadLockProfiler()
	{
		return _mDeadLockProfiler;
	}
#endif

private:
	/**
	 * TLS 초기화
	 */
	void InitTLS();
	void InitTLS_Internal(uint32 threadId);
	void DestroyTLS();

	/* MPMC로 처리되는 글로벌 Job Q들 */
private:
	// 순차적 글로벌 Job Q
	LockQueue<std::shared_ptr<SequentialJobQueue>> _mGlobalSequentialJobQueues;
	// 비순차적 글로벌 Job Q
	std::shared_ptr<ConcurrentJobQueue> _mGlobalConcurrentJobQueue;

	/* 게임 Job Q */
private:
	std::shared_ptr<MPSCJobQueue> _mGameThreadJobQueue;

	/* 렌더 Job Q */
private:
	// 랜더 틱마다 렌더 스레드의 DeltaTime보다 크거나 같은 첫번째 씬까지 처리 
	std::shared_ptr<SceneJobQueue> _mRenderThreadSceneUpdateJobQueue;
	// 렌더 틱마다 가장 최신 상태만 드로잉
	std::shared_ptr<MPSCJobQueue> _mRenderThreadRenderJobQueue;
	// 랜더 명령은 제외. 매 랜더 틱마다 모든 작업 처리
	std::shared_ptr<MPSCJobQueue> _mRenderThreadLogicUpdateJobQueue;

private:
	std::mutex _mLock;
	std::list<std::thread> _mThreads;

private:
	std::atomic<bool> _mIsAlive;

	/* 디버깅 */
private:
	DeadLockProfiler _mDeadLockProfiler;
};

