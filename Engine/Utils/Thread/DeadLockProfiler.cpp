#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const void* owner, const char* name)
{
	std::lock_guard guard(_mLock);

	int32 lockId = 0;

	auto iter = _mPtrToId.find(owner);
	if (iter == _mPtrToId.end())
	{
		lockId = static_cast<int32>(_mPtrToId.size());
		_mPtrToId[owner] = lockId;
		_mIdToPtr[lockId] = { owner, name };
	}
	else
	{
		lockId = iter->second;
	}

	if (LLockStack.empty() == false)
	{
		const int32 prevId = LLockStack.top();
		// 동일 객체의 락 중첩은 무조건 허용
		if (lockId != prevId)
		{
			// 이전 락 사용 클래스가 처음 샤로운 락 사용 클래스을 접근할 경우, 기록판 생성. 아니면 불러오기
			std::set<int32>& history = _mLockHistory[prevId];
			// 들어온 락 사용 클래스를 이전에 건 락 사용 클래스 다음으로 걸어본 것은
			// 처음으로 시도해보 것인가?
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const void* owner)
{
	std::lock_guard guard(_mLock);

	if (LLockStack.empty() == true)
	{
		FAIL_MSG("Multiple unlock");
	}

	int32 lockId = _mPtrToId[owner];
	if (LLockStack.top() != lockId)
	{
		FAIL_MSG("Invalid unlock");
	}

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_mPtrToId.size());
	_mDiscoveredOrder = std::vector<int32>(lockCount, -1);
	_mDiscoveredCount = 0;
	_mFinished = std::vector<bool>(lockCount, false);
	_mParent = std::vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; ++lockId)
	{
		Dfs(lockId);
	}

	_mDiscoveredOrder.clear();
	_mFinished.clear();
	_mParent.clear();
}

void DeadLockProfiler::Dfs(int32 id)
{
	if (_mDiscoveredOrder[id] != -1)
	{
		return;
	}

	_mDiscoveredOrder[id] = _mDiscoveredCount++;

	auto iter = _mLockHistory.find(id);
	// 락 걸고 바로 해제했거나, 방금 새로 생긴 락 사용 클래스인 경우
	if (iter == _mLockHistory.end())
	{
		_mFinished[id] = true;
		return;
	}

	std::set<int32>& nextSet = iter->second;
	for (int32 nextId : nextSet)
	{
		// 첫 방문
		if (_mDiscoveredOrder[nextId] == -1)
		{
			_mParent[nextId] = id;
			Dfs(nextId);
			continue;
		}

		// 순방향 (접근이 처음)
		if (_mDiscoveredOrder[id] < _mDiscoveredOrder[nextId])
		{
			continue;
		}

		// 인근 노드 탐색이 끝나기도 전에 다시 접근했으므로, 순환인 역방향 접근
		if (_mFinished[nextId] == false)
		{
			DEBUG_LOG("%s -> %s", _mIdToPtr[id].second, _mIdToPtr[nextId].second);

			int32 nowId = id;
			while (true)
			{
				DEBUG_LOG("%s -> %s", _mIdToPtr[_mParent[nowId]].second, _mIdToPtr[nowId].second);
				nowId = _mParent[nowId];
				if (nowId == nextId)
				{
					break;
				}
			}

			FAIL_MSG("Deadlock detected");
		}
	}

	_mFinished[id] = true;
}
