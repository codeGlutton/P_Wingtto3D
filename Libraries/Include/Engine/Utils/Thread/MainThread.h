#pragma once

#include "Utils/Thread/Thread.h"

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
	virtual void Work() override;
};

