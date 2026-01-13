#pragma once

#include <thread>
#include <mutex>

#include "Utils/Thread/Thread.h"
#include "Utils/Thread/LockQueue.h"

#define THREAD_MANAGER ThreadManager::GetInst()

class Job;
class SequentialJobQueue;
class ConcurrentJobQueue;
class Thread;

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
	void Launch(std::function<void()> work, std::function<bool()> condition = nullptr);
	void Launch(std::shared_ptr<Thread> thread);
	void Join();

public:
	bool IsThreadAlive() const
	{
		return _mIsThreadAlive.load();
	}

public:
	void PushGlobalSequentialJobQ(std::shared_ptr<SequentialJobQueue> jobQueue);

	void PushGlobalConcurrentJob(std::shared_ptr<Job> job, bool pushOnly = false);
	void PushGlobalConcurrentJobQ(std::shared_ptr<ConcurrentJobQueue> jobQueue);

public:
	void DoGlobalJob();

private:
	std::shared_ptr<SequentialJobQueue> PopFromGlobalJobQ();

private:
	/**
	 * TLS 초기화
	 */
	void InitTLS();
	void DestroyTLS();

private:
	LockQueue<std::shared_ptr<SequentialJobQueue>> _mGlobalSequentialJobQueues;
	std::shared_ptr<ConcurrentJobQueue> _mGlobalConcurrentJobQueue;

private:
	std::mutex _mLock;
	std::vector<std::thread> _mThreads;

private:
	std::atomic<bool> _mIsThreadAlive;
};

/**
 * Thread Manager의 Global Queue를 처리하는 스레드
 */
class GlobalWorkerThread : public Thread
{
private:
	virtual void Work() override;
};