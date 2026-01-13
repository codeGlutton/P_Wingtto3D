#pragma once

#include "Job.h"
#include "Utils/Memory/ObjectPool.h"
#include "Utils/Thread/LockQueue.h"

/**
 * 콜백 LockQueue<JobRef>를 활용 용이하도록 wrapping한 클래스
 */
class JobQueue : public std::enable_shared_from_this<JobQueue>
{
	using CallBackType = std::function<void()>;

public:
	// 이미 생성된 콜백 클로저를 활용해 콜백할 경우 생성자
	void DoAsync(const CallBackType& callback, bool pushOnly = false)
	{
		Push(ObjectPool<Job>::MakeShared(callback), pushOnly);
	}
	void DoAsync(CallBackType&& callback, bool pushOnly = false)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)), pushOnly);
	}

	// 해당 객체의 메소드를 콜백할 경우 생성자
	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(*staticFunc)(Args...), Args&&... args)
	{
		DoAsync(false, staticFunc, std::forward<Args>(args)...);
	}
	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...) const, Args&&... args)
	{
		DoAsync(false, memFunc, std::forward<Args>(args)...);
	}
	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args&&... args)
	{
		DoAsync(false, memFunc, std::forward<Args>(args)...);
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(bool pushOnly, Ret(*staticFunc)(Args...), Args&&... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, staticFunc, std::forward<Args>(args)...), pushOnly);
	}
	template<typename T, typename Ret, typename... Args>
	void DoAsync(bool pushOnly, Ret(T::* memFunc)(Args...) const, Args&&... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...), pushOnly);
	}
	template<typename T, typename Ret, typename... Args>
	void DoAsync(bool pushOnly, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...), pushOnly);
	}

public:
	// JobQueue가 더 이상 필요하지 않을 경우, 순환 참조 끊어주기 위함
	void ClearJob()
	{
		_mJobs.Clear();
	}

	virtual void Execute() = 0;

private:
	virtual void Push(const std::shared_ptr<Job>& job, bool pushOnly = false) = 0;
	virtual void Push(std::shared_ptr<Job>&& job, bool pushOnly = false) = 0;

protected:
	LockQueue<std::shared_ptr<Job>> _mJobs;
	std::atomic<int32> _mJobCount = 0;
};

/**
 * 내부 Job이 항상 순서에 맞게 처리될 Queue
 */
class SequentialJobQueue : public JobQueue
{
public:
	virtual void Execute() override;

private:
	virtual void Push(const std::shared_ptr<Job>& job, bool pushOnly = false) override;
	virtual void Push(std::shared_ptr<Job>&& job, bool pushOnly = false) override;
};

/**
 * 내부 Job이 항상 순서에 관계없이 처리될 Queue
 */
class ConcurrentJobQueue : public JobQueue
{
public:
	void Append(std::shared_ptr<ConcurrentJobQueue> jobQueue);
	virtual void Execute() override;

private:
	virtual void Push(const std::shared_ptr<Job>& job, bool pushOnly = false) override;
	virtual void Push(std::shared_ptr<Job>&& job, bool pushOnly = false) override;

private:
	int32 _mBatchSize = 10;
};
