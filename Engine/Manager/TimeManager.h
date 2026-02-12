#pragma once

#include <latch>
#include "Utils/Thread/LockQueue.h"

#define TIME_MANAGER TimeManager::GetInst()
#define FPS TIME_MANAGER->GetFps()
#define DELTA_TIME TIME_MANAGER->GetDeltaTime()

#define RENDER_TIME_MANAGER RenderTimeManager::GetInst()
#define RENDER_DELTA_TIME RENDER_TIME_MANAGER->GetDeltaTime()

enum class UpdatePhase : uint8
{
	None = 255,

	PrePhysics = 0,		// 입력 값 누적
	Physics,			// 물리 처리, 애님
	PostPhysics,		// 애님 후 처리
	View,				// 카메라 업데이트
	PreRender,			// 모든 처리 결정 후 
	Render,				// 렌더 스레드 넘겨주기
	
	Count
};
constexpr size_t UpdatePhaseCount = static_cast<size_t>(UpdatePhase::Count);

enum class UpdateState : uint8
{
	Start = 0,
	Run,
	Stop,
};

class IUpdatable abstract
{
	GEN_INTERFACE_REFLECTION(IUpdatable)

	friend class TimeManager;

protected:
	virtual void Update(float deltaTime) = 0;
	virtual void FixedUpdate() = 0;
};

struct UpdateTargetContext
{
public:
	UpdateTargetContext(std::shared_ptr<IUpdatable> target, UpdatePhase phase) :
		mTarget(target),
		mPhase(phase)
	{
	}

public:
	std::weak_ptr<IUpdatable> mTarget;

	UpdatePhase mPhase = UpdatePhase::None;
	UpdateState mState = UpdateState::Start;
	bool mIsAsync = false;
	bool mCanEverTick = true;
};

struct UpdatePhaseContext
{
public:
	std::unordered_set<std::shared_ptr<UpdateTargetContext>> mSyncTargets;
	std::unordered_set<std::shared_ptr<UpdateTargetContext>> mAsyncTargets;

public:
	std::shared_ptr<std::latch> mCounter;
};

/**
 * 스레드 틱 시간에 대한 데이터를 계산 반환 해주는 싱글톤 객체
 */
class TimeManager
{
private:
	TimeManager();
	~TimeManager();

public:
	static TimeManager* GetInst()
	{
		static TimeManager inst;
		return &inst;
	}

public:
	uint32 GetFps() const;
	float GetDeltaTime() const;
	float GetFixedDeltaTime() const;
	double GetPlayTime() const;

public:
	void NotifyToAddTarget(std::shared_ptr<UpdateTargetContext> data);
	void NotifyToRemoveTarget(std::shared_ptr<UpdateTargetContext> data);

public:
	void Init();
	void Update();
	void Destroy();

private:
	void UpdateTime();
	void UpdateTargets();

private:
	std::array<UpdatePhaseContext, UpdatePhaseCount> _mPhases;
	std::unordered_set<std::shared_ptr<UpdateTargetContext>> _mTargets;

private:
	// 클록 주파수
	uint64 _mFrequency = 0ull;
	// 이전 클록 카운트
	uint64 _mPrevCount = 0ull;
	// 업데이트 델타
	float _mDeltaTime = 0.f;

private:
	// 누적 프레임 카운트
	uint32 _mFrameCount = 0ul;
	// 초당 프레임을 연산하기 위한 누적 값
	float _mFrameTimeAcc = 0.f;
	uint32 _mFps = 0ul;

	const float _mFixedDeltaTime = 1.f / 60.f;
	// 고정 틱을 처리하기 위한 누적 값
	float _mFixedTimeAcc = _mFixedDeltaTime;
	bool _mUseFixedUpdate = false;

	double _mPlayTimeAcc = 0.;
};

class RenderTimeManager
{
private:
	RenderTimeManager();
	~RenderTimeManager();

public:
	static RenderTimeManager* GetInst()
	{
		static RenderTimeManager inst;
		return &inst;
	}

public:
	float GetDeltaTime() const;

public:
	void Init();
	void Update();
	void Destroy();

private:
	// 클록 주파수
	uint64 _mFrequency = 0ull;
	// 이전 클록 카운트
	uint64 _mPrevCount = 0ull;
	// 업데이트 델타
	float _mDeltaTime = 0.f;
};

