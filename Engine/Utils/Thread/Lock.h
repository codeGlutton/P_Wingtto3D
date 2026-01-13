#pragma once

#define READ_LOCK(lock) ReadLockGuard readLockGuard_##lock(lock, typeid(this).name())
#define WRITE_LOCK(lock) WriteLockGuard writeLockGuard_##lock(lock, typeid(this).name())

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,	// lock 최대 경쟁 상태로 인한 지연 틱
		MAX_SPIN_COUNT = 5000,			// spin lock 시도 종료 횟수

		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* thread);
	void WriteUnlock(const char* thread);
	void ReadLock(const char* thread);
	void ReadUnlock(const char* thread);

private:
	// 상위 16비트는 Thread id, 하위 16비트는 걸린 read lock 수
	std::atomic<uint32>	_mLockFlag = EMPTY_FLAG;
	// 얼마나 많은 write lock이 걸려있는지
	uint16 _mWriterCount = 0;
};


/* Wrapper 클래스들 */

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* thread) : 
		_mLock(lock),
		_mName(thread)
	{ 
		_mLock.ReadLock(thread); 
	}
	~ReadLockGuard() 
	{ 
		_mLock.ReadUnlock(_mName); 
	}

private:
	Lock& _mLock;
	const char* _mName;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* thread) :
		_mLock(lock), 
		_mName(thread)
	{
		_mLock.WriteLock(thread);
	}
	~WriteLockGuard() 
	{
		_mLock.WriteUnlock(_mName);
	}

private:
	Lock& _mLock;
	const char* _mName;
};