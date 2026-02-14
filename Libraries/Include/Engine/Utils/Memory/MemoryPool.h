#pragma once

namespace Memory
{
	enum
	{
		SLIST_ALIGN = 16,
		MAX_POOL_SIZE = 200,
		MAX_SPIN_COUNT = 5,
	};
}

struct alignas(Memory::SLIST_ALIGN) MemoryHeader : public SLIST_ENTRY
{
public:
	MemoryHeader(int32 size) : 
		mAllocSize(size) 
	{
	}

public:
	// 앞에 헤더 위치 지정 생성 후, 데이터 주소 반환
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}

	// 데이터 주소로 해당 헤더 부분을 리턴
	static MemoryHeader* DetachHeader(void* dataPtr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(dataPtr) - 1;
		return header;
	}

public:
	int32 mAllocSize;
};

class alignas(Memory::SLIST_ALIGN) MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

public:
	// 동적 변수 반납시
	void Push(MemoryHeader* ptr);
	// 동적 변수 재시용시
	MemoryHeader* Pop();

private:
	SLIST_HEADER _mHeader;
	// 관리 데이터 최대 사이즈
	int32 _mAllocSize = 0;

private:
	// 사용중인 갯수
	std::atomic<int32> _mUseCount = 0;
	// 반납된 갯수
	std::atomic<int32> _mReserveCount = 0;
};