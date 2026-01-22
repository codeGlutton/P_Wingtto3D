#pragma once

#include "Job.h"
#include "Utils/Memory/ObjectPool.h"
#include "Utils/Thread/LockQueue.h"

/**
 * 단일 소비, 다중 공급
 * 콜백 LockQueue<JobRef>를 활용 용이하도록 wrapping한 클래스
 */
class MPSCJobQueue : public std::enable_shared_from_this<MPSCJobQueue>
{
public:
	using CallBackType = std::function<void()>;

public:
	// 이미 생성된 콜백 클로저를 활용해 콜백할 경우 생성자
	void DoAsync(const CallBackType& callback)
	{
		Push(ObjectPool<Job>::MakeShared(callback));
	}
	void DoAsync(CallBackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	// 해당 객체의 메소드를 콜백할 경우 생성자
	template<typename T, typename Ret, typename... Args, typename... Params>
	void DoAsync(Ret(*staticFunc)(Args...), Params&&... params)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, staticFunc, std::forward<Params>(params)...));
	}
	template<typename T, typename Ret, typename... Args, typename... Params>
	void DoAsync(Ret(T::* memFunc)(Args...) const, Params&&... params)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Params>(params)...));
	}
	template<typename T, typename Ret, typename... Args, typename... Params>
	void DoAsync(Ret(T::* memFunc)(Args...), Params&&... params)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Params>(params)...));
	}

public:
	// JobQueue가 더 이상 필요하지 않을 경우, 순환 참조 끊어주기 위함
	void ClearJob()
	{
		_mJobs.Clear();
	}

	virtual void Execute();

private:
	virtual void Push(const std::shared_ptr<Job>& job);
	virtual void Push(std::shared_ptr<Job>&& job);

protected:
	LockQueue<std::shared_ptr<Job>> _mJobs;
	std::atomic<int32> _mJobCount = 0;
};


