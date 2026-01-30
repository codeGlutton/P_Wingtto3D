#pragma once

#include <thread>

class Thread
{
public:
	virtual void Init()
	{
		srand(static_cast<uint32>(time(NULL)));
		rand();
	}
	virtual void Run() = 0;
};

class WorkerThread : public Thread
{
public:
	virtual void Run() final;

private:
	virtual bool IsRunnable()
	{
		return true;
	}
	virtual void Work() = 0;

public:
	const uint64 mThreadMaxTick = 64;
};

class CustomThread : public WorkerThread
{
public:
	CustomThread(std::function<void()> work, std::function<bool()> condition = nullptr);

private:
	virtual bool IsRunnable() override;
	virtual void Work() override;

private:
	std::function<void()> _mWork = nullptr;
	std::function<bool()> _mCondition = nullptr;
};

