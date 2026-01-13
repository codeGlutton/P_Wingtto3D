#include "pch.h"
#include "TimeManager.h"

#include "ThreadManager.h"
#include "Utils/Thread/Job.h"

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::AddTarget(std::shared_ptr<UpdateTargetContext> data)
{
	if (data == nullptr || data->mTarget == nullptr || _mTargets.insert(data).second == false)
	{
		return;
	}

	uint8 phaseIndex = static_cast<uint8>(data->mPhase);
	if (data->mIsAsync == true)
	{
		_mPhases[static_cast<uint8>(data->mPhase)].mAsyncTargets.insert(data);
	}
	else
	{
		_mPhases[static_cast<uint8>(data->mPhase)].mAsyncTargets.insert(data);
	}
}

void TimeManager::RemoveTarget(std::shared_ptr<UpdateTargetContext> data)
{
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
	// 주파수 기록
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_mFrequency));
	// CPU 클럭 카운트 기록
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_mPrevCount));
}

void TimeManager::Update()
{
	UpdateTime();
	UpdateTargets();
}

void TimeManager::UpdateTime()
{
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
			THREAD_MANAGER->PushGlobalConcurrentJob(std::make_shared<Job>([target, deltaTime = _mDeltaTime, counter = phase.mCounter]()
				{
					target->mTarget->Update(deltaTime);
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
			target->mTarget->Update(_mDeltaTime);
		}

		for (auto& target : endTarget)
		{
			RemoveTarget(target);
		}

		phase.mCounter->wait();
	}
}
