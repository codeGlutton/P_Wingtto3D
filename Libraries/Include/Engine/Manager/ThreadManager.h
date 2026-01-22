#pragma once

#include <thread>
#include <mutex>

#include "Utils/Thread/MainThread.h"
#include "Utils/Thread/LockQueue.h"

#define THREAD_MANAGER ThreadManager::GetInst()

class Job;
class MPSCJobQueue;
class SequentialJobQueue;
class ConcurrentJobQueue;

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
	void LaunchMainThreads(std::array<std::shared_ptr<MainThread>, MainThreadType::Count> threads);
	void Join();

public:
	bool IsAlive() const
	{
		return _mIsAlive.load();
	}

public:
	void PushGameThreadJob(std::shared_ptr<Job> job);

public:
	void PushGlobalSequentialJobQ(std::shared_ptr<SequentialJobQueue> jobQueue);

	void PushGlobalConcurrentJob(std::shared_ptr<Job> job, bool pushOnly = false);
	void PushGlobalConcurrentJobQ(std::shared_ptr<ConcurrentJobQueue> jobQueue);

public:
	void DoGameJob();
	void DoGlobalJob();

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

private:
	std::mutex _mLock;
	std::list<std::thread> _mThreads;

private:
	std::atomic<bool> _mIsAlive;
};

