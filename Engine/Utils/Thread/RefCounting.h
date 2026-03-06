#pragma once

#include "Utils/Thread/Job.h"
#include "Utils/Memory/ObjectPool.h"
#include "Manager/ThreadManager.h"

template<typename T, MainThreadType::Type MainThreadT = MainThreadType::Game> requires (MainThreadT == MainThreadType::Game || MainThreadT == MainThreadType::Render)
struct ThreadSafeRefCountData
{
public:
	using DataType = T;

public:
	ThreadSafeRefCountData()
	{
		// 데이터를 복제해 전달만 해도, 안전하게 지정해둔 스레드에서 제거 가능
		_mDestructor = [](const T& data) {};
	}
	ThreadSafeRefCountData(std::function<void(const T&)> destructor)
	{
		// 데이터를 복제해 전달만 해도, 안전하게 지정해둔 스레드에서 제거 가능
		_mDestructor = destructor;
	}
	~ThreadSafeRefCountData()
	{
		if constexpr (MainThreadT == MainThreadType::Game)
		{
			if (_mDestructor != nullptr)
			{
				THREAD_MANAGER->PushGameThreadJob(ObjectPool<Job>::MakeShared(
					[data = this->mData, destructor = this->_mDestructor]() {
						destructor(data);
					}
				));
			}
		}
		else if constexpr (MainThreadT == MainThreadType::Render)
		{
			if (_mDestructor != nullptr)
			{
				THREAD_MANAGER->PushRenderThreadLogicUpdateJob(ObjectPool<Job>::MakeShared(
					[data = this->mData, destructor = this->_mDestructor]() {
						destructor(data);
					}
				));
			}
		}
	}

public:
	// 참조 데이터, 일반적으로 std::shared_ptr과 같은 객체
	T mData;

private:
	// 지정 스레드에서 처리할 람다
	std::function<void(const T&)> _mDestructor;
};

template<typename T, MainThreadType::Type MainThreadT = MainThreadType::Game>
using ThreadSafeRefCounting = std::shared_ptr<ThreadSafeRefCountData<T, MainThreadT>>;

template<typename T, MainThreadType::Type MainThreadT = MainThreadType::Game>
ThreadSafeRefCounting<T, MainThreadT> CreateRefCounting()
{
	return std::make_shared<ThreadSafeRefCountData<T, MainThreadT>>();
}

template<typename T, MainThreadType::Type MainThreadT = MainThreadType::Game>
ThreadSafeRefCounting<T, MainThreadT> CreateRefCounting(const T& initData)
{
	auto result = std::make_shared<ThreadSafeRefCountData<T, MainThreadT>>();
	result->mData = initData;

	return result;
}

template<typename T, MainThreadType::Type MainThreadT = MainThreadType::Game>
ThreadSafeRefCounting<T, MainThreadT> CreateRefCounting(std::function<void(const T&)> destructor)
{
	return std::make_shared<ThreadSafeRefCountData<T, MainThreadT>>(destructor);
}

template<typename T, MainThreadType::Type MainThreadT = MainThreadType::Game>
ThreadSafeRefCounting<T, MainThreadT> CreateRefCounting(std::function<void(const T&)> destructor, const T& initData)
{
	auto result = std::make_shared<ThreadSafeRefCountData<T, MainThreadT>>(destructor);
	result->mData = initData;

	return result;
}