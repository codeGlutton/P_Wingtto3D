#include "pch.h"
#include "Thread.h"

#include "Manager/ThreadManager.h"

void WorkerThread::Run()
{
	while (IsRunnable() == true && THREAD_MANAGER->IsAlive() == true)
	{
		LEndTickCount = GetTickCount64() + mThreadMaxTick;
		Work();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

CustomThread::CustomThread(std::function<void()> work, std::function<bool()> condition) :
	_mWork(std::move(work)),
	_mCondition(std::move(condition))
{
}

bool CustomThread::IsRunnable()
{
	return _mCondition == nullptr || _mCondition();
}

void CustomThread::Work()
{
	_mWork();
}

