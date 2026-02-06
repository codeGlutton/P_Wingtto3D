#pragma once

#include "Utils/Thread/Job.h"
#include "Utils/Memory/ObjectPool.h"
#include "Manager/ThreadManager.h"

template<typename T>
struct ThreadSafeRefCountData
{
public:
	ThreadSafeRefCountData()
	{
	}
	ThreadSafeRefCountData(std::function<void(const T&)> destructor)
	{
		_mDestructor = [this, destructor]() {
			destructor(mData);
			};
	}
	~ThreadSafeRefCountData()
	{
		if (_mDestructor != nullptr)
		{
			THREAD_MANAGER->PushGameThreadJob(ObjectPool<Job>::MakeShared(_mDestructor));
		}
	}

public:
	T mData;

private:
	std::function<void()> _mDestructor;
};

template<typename T>
using ThreadSafeRefCounting = std::shared_ptr<ThreadSafeRefCountData<T>>;

template<typename T>
ThreadSafeRefCounting<T> CreateRefCounting(std::function<void(const T&)> destructor)
{
	return std::make_shared<ThreadSafeRefCountData<T>>(destructor);
}
