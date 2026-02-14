#pragma once

#include <mutex>

/**
 * 데드락 탐지 프로파일 클래스
 */
class DeadLockProfiler
{
public:
	void PushLock(const void* owner, const char* name);
	void PopLock(const void* owner);
	void CheckCycle();

private:
	void Dfs(int32 id);

private:
	// 기록된 락 사용 클래스 주소 -> 아이디
	std::unordered_map<const void*, int32> _mPtrToId;
	// 기록된 락 사용 클래스 아이디 -> 주소, 이름
	std::unordered_map<int32, std::pair<const void*, const char*>> _mIdToPtr;
	// 특정 락 에서 다른 락 접근 기록
	std::map<int32, std::set<int32>> _mLockHistory;

private:
	// 발견 여부 및 순서 표시
	std::vector<int32> _mDiscoveredOrder;
	// 발견 순서 표시용 카운트
	int32 _mDiscoveredCount = 0;
	// 연결된 노드 검사를 모두 마친 노드인가
	std::vector<bool> _mFinished;
	// 첫 발견시 어떤 노드에서 접근했나
	std::vector<int32> _mParent;

private:
	std::mutex _mLock;
};

