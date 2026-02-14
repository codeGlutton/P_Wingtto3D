#pragma once

#define READ_LOCK(lock) ReadLockGuard readLockGuard_##lock(lock, this, typeid(this).name())
#define WRITE_LOCK(lock) WriteLockGuard writeLockGuard_##lock(lock, this, typeid(this).name())

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
	void WriteLock(const void* owner, const char* name);
	void WriteUnlock(const void* owner);
	void ReadLock(const void* owner, const char* name);
	void ReadUnlock(const void* owner);

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
	ReadLockGuard(Lock& lock, const void* owner, const char* name) : 
		_mLock(lock),
		_mOwner(owner),
		_mName(name)
	{ 
		_mLock.ReadLock(_mOwner, _mName);
	}
	~ReadLockGuard() 
	{ 
		_mLock.ReadUnlock(_mOwner);
	}

private:
	Lock& _mLock;
	const void* _mOwner;
	const char* _mName;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const void* owner, const char* name) :
		_mLock(lock), 
		_mOwner(owner),
		_mName(name)
	{
		_mLock.WriteLock(_mOwner, _mName);
	}
	~WriteLockGuard() 
	{
		_mLock.WriteUnlock(_mOwner);
	}

private:
	Lock& _mLock;
	const void* _mOwner;
	const char* _mName;
};