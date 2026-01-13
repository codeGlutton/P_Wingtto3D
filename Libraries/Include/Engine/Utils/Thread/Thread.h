#pragma once

#include <thread>

class Thread
{
public:
	void Run();

private:
	virtual bool IsRunnable()
	{
		return true;
	}
	virtual void Work()
	{
	}

public:
	const uint64 mThreadTick = 64;
};

class CustomThread : public Thread
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

