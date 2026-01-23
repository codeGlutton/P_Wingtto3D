#include "pch.h"
#include "TimeManager.h"

#include "Manager/ThreadManager.h"
#include "Utils/Thread/Job.h"

#include "Utils/Memory/ObjectPool.h"

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

uint64 TimeManager::GetGameFrameNumber() const
{
	ASSERT_THREAD(MainThreadType::Game);
	return _mGameFrameNumber;
}

uint32 TimeManager::GetFps() const
{
	ASSERT_THREAD(MainThreadType::Game);
	return _mFps;
}

float TimeManager::GetDeltaTime() const
{
	ASSERT_THREAD(MainThreadType::Game);
	return _mDeltaTime;
}

uint64 TimeManager::GetRenderFrameNumber() const
{
	return _mRenderFrameNumber.load();
}

void TimeManager::SetRenderFrameNumber(uint64 frameNumber)
{
	ASSERT_THREAD(MainThreadType::Render);
	_mRenderFrameNumber.store(frameNumber);
}

void TimeManager::NotifyToAddTarget(std::shared_ptr<UpdateTargetContext> data)
{
	ASSERT_THREAD(MainThreadType::Game);

	if (data == nullptr)
	{
		return;
	}
	std::shared_ptr<IUpdatable> target = data->mTarget.lock();
	if (target == nullptr || _mTargets.insert(data).second == false)
	{
		return;
	}

	uint8 phaseIndex = static_cast<uint8>(data->mPhase);
	if (data->mIsAsync == true)
	{
		_mPhases[static_cast<uint8>(data->mPhase)].mAsyncTargets.insert(std::move(data));
	}
	else
	{
		_mPhases[static_cast<uint8>(data->mPhase)].mAsyncTargets.insert(std::move(data));
	}
}

void TimeManager::NotifyToRemoveTarget(std::shared_ptr<UpdateTargetContext> data)
{
	ASSERT_THREAD(MainThreadType::Game);

	if (_mTargets.erase(data) == 0)
	{
		return;
	}

	uint8 phaseIndex = static_cast<uint8>(data->mPhase);
	if (data->mIsAsync == true)
	{
		_mPhases[static_cast<uint8>(data->mPhase)].mAsyncTargets.erase(data);
	}
	else
	{
		_mPhases[static_cast<uint8>(data->mPhase)].mAsyncTargets.erase(data);
	}
}

void TimeManager::Init()
{
	ASSERT_THREAD(MainThreadType::Game);

	// 주파수 기록
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_mFrequency));
	// CPU 클럭 카운트 기록
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_mPrevCount));
}

void TimeManager::Update()
{
	ASSERT_THREAD(MainThreadType::Game);

	UpdateTime();
	UpdateTargets();
}

void TimeManager::Destroy()
{
}

void TimeManager::UpdateTime()
{
	// 프레임 넘버 증가
	++_mGameFrameNumber;

	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	// 지난 시간 = 몇 클록이 지났는가 / 주파수
	_mDeltaTime = (currentCount - _mPrevCount) / static_cast<float>(_mFrequency);
	_mPrevCount = currentCount;

	// 게임 상에서 몇 프레임 지났는가
	_mFrameCount++;
	_mFrameTime += _mDeltaTime;

	// 초당 몇 프레임이 나왔는가 계산
	if (_mFrameTime > 1.f)
	{
		_mFps = static_cast<uint32>(_mFrameCount / _mFrameTime);

		_mFrameTime = 0.f;
		_mFrameCount = 0;
	}
}

void TimeManager::UpdateTargets()
{
	for (auto& phase : _mPhases)
	{
		phase.mCounter = std::make_shared<std::latch>(phase.mAsyncTargets.size());
		std::vector<std::shared_ptr<UpdateTargetContext>> endTarget;
		for (auto& target : phase.mAsyncTargets)
		{
			if (target->mState == UpdateState::Stop)
			{
				endTarget.push_back(target);
				phase.mCounter->count_down();
				continue;
			}
			target->mState = UpdateState::Run;
			THREAD_MANAGER->PushGlobalConcurrentJob(ObjectPool<Job>::MakeShared([target, deltaTime = _mDeltaTime, counter = phase.mCounter]()
				{
					std::shared_ptr<IUpdatable> targetSharedPtr = target->mTarget.lock();
					if (targetSharedPtr != nullptr)
					{
						targetSharedPtr->Update(deltaTime);
					}
					counter->count_down();
				}), false);
		}
		for (auto& target : phase.mSyncTargets)
		{
			if (target->mState == UpdateState::Stop)
			{
				endTarget.push_back(target);
				continue;
			}
			target->mState = UpdateState::Run;
			std::shared_ptr<IUpdatable> targetSharedPtr = target->mTarget.lock();
			if (targetSharedPtr != nullptr)
			{
				targetSharedPtr->Update(_mDeltaTime);
			}
		}

		for (auto& target : endTarget)
		{
			NotifyToRemoveTarget(target);
		}

		phase.mCounter->wait();
	}
}
