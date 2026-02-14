#pragma once

#include "Utils/Thread/Thread.h"

class DXSharedResource;

namespace MainThreadType
{
	enum Type : uint32
	{
		None = 0, // Lock 소유자 없음을 표기하기 위한 값
		Game,
		Render,

		// 여기에 반드시 필요한 스레드 타입 추가

		End,
		Count = End - 1
	};
}

class MainThread abstract : public Thread
{
public:
	virtual void Run() final;

private:
	virtual void Work() = 0;

public:
	const uint64 mThreadMaxTick = 16;
};

class RenderThread : public MainThread
{
private:
	virtual void Init() override;
	virtual void Work() override;
	virtual void Destroy() override;

protected:
	virtual void RegisterDefaultRuntimeResources();
	virtual void UnregisterDefaultRuntimeResources();

protected:
	std::vector<std::shared_ptr<DXSharedResource>> _mDefaultResources;
};

