#include "pch.h"
#include "Lock.h"

#include <thread>

void Lock::WriteLock(const char* thread)
{
	const uint32 lockThreadId = (_mLockFlag.load() & WRITE_THREAD_MASK) >> 16;
	// 이미 write lock을 가진 스레드일 걍우 바로 통과
	if (LThreadId == lockThreadId)
	{
		_mWriterCount++;
		return;
	}

	const int64 beginTick = ::GetTickCount64();
	const uint32 desiredFlag = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount)
		{
			// Write 및 Read Lock이 모두 해제될 때까지
			uint32 expectedFlag = EMPTY_FLAG;
			if (_mLockFlag.compare_exchange_strong(OUT expectedFlag, desiredFlag))
			{
				_mWriterCount++;
				return;
			}
		}

		// 이건 너무 경쟁이 심해!
		ASSERT_MSG(::GetTickCount64() - beginTick < ACQUIRE_TIMEOUT_TICK, "Lock Time over");

		// 잠깐 타 프로세스에 넘겨 쉬고가자
		std::this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* thread)
{
	ASSERT_MSG(_mLockFlag.load() & READ_COUNT_MASK, "Invalid write unlock. Writer thread is null");

	const int32 lockCount = --_mWriterCount;
	if (lockCount == 0)
	{
		_mLockFlag.store(EMPTY_FLAG);
	}
}

void Lock::ReadLock(const char* thread)
{
	const uint32 lockThreadId = (_mLockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_mLockFlag.fetch_add(1);
		return;
	}

	const int64 beginTick = GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expectedFlag = (_mLockFlag.load() & READ_COUNT_MASK);
			// 사이에 경쟁 상태를 위해 확인
			if (_mLockFlag.compare_exchange_strong(OUT expectedFlag, expectedFlag + 1))
			{
				return;
			}
		}

		// 이건 너무 경쟁이 심해!
		ASSERT_MSG(::GetTickCount64() - beginTick < ACQUIRE_TIMEOUT_TICK, "Lock Time over");

		// 잠깐 타 프로세스에 넘겨 쉬고가자
		std::this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* thread)
{
	ASSERT_MSG((_mLockFlag.fetch_sub(1) & READ_COUNT_MASK) != 0, "Unlock was called redundantly");
}
