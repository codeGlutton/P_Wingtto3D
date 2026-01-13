#pragma once

#include "Utils/Memory/MemoryPool.h"

template<typename T>
class ObjectPool
{
public:
	template<typename...Args>
	static T* Pop(Args&&... args)
	{
		T* memory = static_cast<T*>(MemoryHeader::AttachHeader(_mPool.Pop(), _mAllocSize));
		new(memory)T(std::forward<Args>(args)...);
		return memory;
	}

	static void Push(T* obj)
	{
		obj->~T();
		_mPool.Push(MemoryHeader::DetachHeader(obj));
	}

	template<typename... Args>
	static std::shared_ptr<T> MakeShared(Args&&... args)
	{
		std::shared_ptr<T> ptr = { Pop(std::forward<Args>(args)...), Push };
		return ptr;
	}

private:
	static int32 _mAllocSize;
	static MemoryPool _mPool;
};

template<typename T>
int32 ObjectPool<T>::_mAllocSize = sizeof(T) + sizeof(MemoryHeader);
template<typename T>
MemoryPool ObjectPool<T>::_mPool{ _mAllocSize };