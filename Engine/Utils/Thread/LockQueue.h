#pragma once

#include "Utils/Thread/Lock.h"

template<typename T>
class LockQueue
{
public:
	void Push(T item);

	T Pop();
	void PopAll(OUT std::vector<T>& items);
	int32 PopUntil(OUT std::vector<T>& items, std::function<bool(const T&)> condition, bool isAllowedToPopAll = true);
	int32 PopBatch(OUT std::vector<T>& items, int32 batchSize);

public:
	int32 Clear();
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

	T item = std::move(_mItems.front());
	_mItems.pop();

	return item;
}

template<typename T>
inline void LockQueue<T>::PopAll(OUT std::vector<T>& items)
{
	WRITE_LOCK(_mLock);

	while (_mItems.empty() == false)
	{
		items.push_back(std::move(_mItems.front()));
		_mItems.pop();
	}
}

template<typename T>
inline int32 LockQueue<T>::PopUntil(OUT std::vector<T>& items, std::function<bool(const T&)> condition, bool isAllowedToPopAll)
{
	WRITE_LOCK(_mLock);

	while (_mItems.empty() == false)
	{
		T item = _mItems.front();
		if (condition(item) == false)
		{
			return static_cast<int32>(_mItems.size());
		}
		items.push_back(std::move(item));
		_mItems.pop();
	}

	if (isAllowedToPopAll == true)
	{
		return static_cast<int32>(_mItems.size());
	}
	else
	{
		for (auto& item : items)
		{
			_mItems.push(std::move(item));
		}
		items.clear();
		return 0;
	}
}

template<typename T>
inline int32 LockQueue<T>::PopBatch(OUT std::vector<T>& items, int32 batchSize)
{
	WRITE_LOCK(_mLock);

	for (int32 i = 0; i < batchSize; ++i)
	{
		if (_mItems.empty() == true)
		{
			break;
		}

		items.push_back(std::move(_mItems.front()));
		_mItems.pop();
	}
	return static_cast<int32>(_mItems.size());
}

template<typename T>
inline int32 LockQueue<T>::Clear()
{
	WRITE_LOCK(_mLock);

	int32 size = static_cast<int32>(_mItems.size());
	_mItems = std::queue<T>();

	return size;
}

template<typename T>
inline bool LockQueue<T>::IsEmpty() const
{
	READ_LOCK(_mLock);

	return _mItems.empty();
}


