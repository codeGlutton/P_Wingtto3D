#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 allocSize) : _mAllocSize(allocSize)
{
	::InitializeSListHead(&_mHeader);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_mHeader)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->mAllocSize = 0;

	int32 count = _mReserveCount.load();

	::InterlockedPushEntrySList(&_mHeader, static_cast<PSLIST_ENTRY>(ptr));

	_mUseCount.fetch_sub(1);
	_mReserveCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_mHeader));

	if (memory == nullptr)
	{
		// 부족한 경우
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_mAllocSize, Memory::SLIST_ALIGN));
	}
	else
	{
		// 여분이 있는 경우
		ASSERT_MSG(memory->mAllocSize == 0, "Invalid memory pool reserve");
		_mReserveCount.fetch_sub(1);
	}
	_mUseCount.fetch_add(1);

	return memory;
}