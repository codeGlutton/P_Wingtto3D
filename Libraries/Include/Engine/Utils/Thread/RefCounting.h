#pragma once

#include "Utils/Thread/Job.h"
#include "Utils/Memory/ObjectPool.h"
#include "Manager/ThreadManager.h"

template<typename T>
struct RefCountData
{
public:
	RefCountData()
	{
	}
	RefCountData(std::function<void(const T&)> destructor)
	{
		_mDestructor = [this, destructor]() {
			destructor(mData);
			};
	}
	~RefCountData()
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
using RefCounting = std::shared_ptr<RefCountData<T>>;

template<typename T>
RefCounting<T> CreateRefCounting(std::function<void(const T&)> destructor)
{
	return std::make_shared<RefCountData<T>>(destructor);
}
