#pragma once

#include "Job.h"
#include "Utils/Memory/ObjectPool.h"
#include "Utils/Thread/LockQueue.h"

#include "Manager/TimeManager.h"

/**
 * 단일 소비, 다중 공급
 * 랜더링 스레드를 위한 JobQ
 */
class RenderJobQueue : public std::enable_shared_from_this<RenderJobQueue>
{
public:
	using CallBackType = std::function<void()>;

public:
	// 이미 생성된 콜백 클로저를 활용해 콜백할 경우 생성자
	void DoAsync(const CallBackType& callback)
	{
		Push(ObjectPool<JobContext<uint64>>::MakeShared(TIME_MANAGER->GetGameFrameNumber(), callback));
	}
	void DoAsync(CallBackType&& callback)
	{
		Push(ObjectPool<JobContext<uint64>>::MakeShared(TIME_MANAGER->GetGameFrameNumber(), std::move(callback)));
	}

	// 해당 객체의 메소드를 콜백할 경우 생성자
	template<typename T, typename Ret, typename... Args, typename... Params>
	void DoAsync(Ret(*staticFunc)(Args...), Params&&... params)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<JobContext<uint64>>::MakeShared(TIME_MANAGER->GetGameFrameNumber(), owner, staticFunc, std::forward<Params>(params)...));
	}
	template<typename T, typename Ret, typename... Args, typename... Params>
	void DoAsync(Ret(T::* memFunc)(Args...) const, Params&&... params)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<JobContext<uint64>>::MakeShared(TIME_MANAGER->GetGameFrameNumber(), owner, memFunc, std::forward<Params>(params)...));
	}
	template<typename T, typename Ret, typename... Args, typename... Params>
	void DoAsync(Ret(T::* memFunc)(Args...), Params&&... params)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<JobContext<uint64>>::MakeShared(TIME_MANAGER->GetGameFrameNumber(), owner, memFunc, std::forward<Params>(params)...));
	}
	
public:
	// JobQueue가 더 이상 필요하지 않을 경우, 순환 참조 끊어주기 위함
	void ClearJob()
	{
		_mJobs.Clear();
	}

	virtual void Execute();

private:
	virtual void Push(const std::shared_ptr<JobContext<uint64>>& job);
	virtual void Push(std::shared_ptr<JobContext<uint64>>&& job);

protected:
	LockQueue<std::shared_ptr<JobContext<uint64>>> _mJobs;
	std::atomic<int32> _mJobCount = 0;
};

