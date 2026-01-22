#pragma once

#include "Utils/Thread/Lock.h"

template<typename T>
class LockQueue
{
public:
	void Push(T item);

	T Pop();
	void PopAll(OUT std::vector<T>& items);
	int32 PopUntil(OUT std::vector<T>& items, std::function<bool(const T&)> condition);
	int32 PopBatch(OUT std::vector<T>& items, int32 batchSize);

public:
	void Clear();
	bool IsEmpty() const;

private:
	mutable Lock _mLock;
	std::queue<T> _mItems;
};

template<typename T>
inline void LockQueue<T>::Push(T item)
{
	WRITE_LOCK(_mLock);

	_mItems.push(std::move(item));
}

template<typename T>
inline T LockQueue<T>::Pop()
{
	WRITE_LOCK(_mLock);

	if (_mItems.empty() == true)
	{
		return T();
	}

	T item = _mItems.front();
	_mItems.pop();

	return item;
}

template<typename T>
inline void LockQueue<T>::PopAll(OUT std::vector<T>& items)
{
	WRITE_LOCK(_mLock);

	while (_mItems.empty() == false)
	{
		items.push_back(_mItems.front());
		_mItems.pop();
	}
}

template<typename T>
inline int32 LockQueue<T>::PopUntil(OUT std::vector<T>& items, std::function<bool(const T&)> condition)
{
	WRITE_LOCK(_mLock);

	while (items.empty() == false)
	{
		T item = _mItems.front();
		if (condition(item) == false)
		{
			break;
		}
		items.push_back(std::move(item));
		_mItems.pop();
	}
	return static_cast<int32>(_mItems.size());
}

template<typename T>
inline int32 LockQueue<T>::PopBatch(OUT std::vector<T>& items, int32 batchSize)
{
	WRITE_LOCK(_mLock);

	for (int32 i = 0; i < batchSize; ++i)
	{
		if (IsEmpty() == true)
		{
			break;
		}

		items.push_back(_mItems.front());
		_mItems.pop();
	}
	return static_cast<int32>(_mItems.size());
}

template<typename T>
inline void LockQueue<T>::Clear()
{
	WRITE_LOCK(_mLock);

	_mItems = std::queue<T>();
}

template<typename T>
inline bool LockQueue<T>::IsEmpty() const
{
	READ_LOCK(_mLock);

	return _mItems.empty();
}


