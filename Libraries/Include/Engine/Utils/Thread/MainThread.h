#pragma once

#include "Utils/Thread/Thread.h"

class DXSharedResource;

namespace MainThreadType
{
	enum Type : uint32
	{
		Game = 0xFFFFFFFF,
		Render = 0,

		// 여기에 반드시 필요한 스레드 타입 추가

		Count
	};
}

class MainThread : public Thread
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
	virtual void RegisterDefaultResources();
	virtual void UnregisterDefaultResources();

protected:
	std::vector<std::shared_ptr<DXSharedResource>> _mDefaultResources;
};

