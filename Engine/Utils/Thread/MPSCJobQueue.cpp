#include "pch.h"
#include "MPSCJobQueue.h"

#include "Manager/ThreadManager.h"

void MPSCJobQueue::Execute()
{
	while (true)
	{
		// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
		std::vector<std::shared_ptr<Job>> jobs;
		_mJobs.PopAll(OUT jobs);

		// 처리가 확정된 job 갯수
		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; ++i)
		{
			jobs[i]->Execute();
		}

		// 남은 일감이 없는 경우 (중도에 들어오지 않았거나, 들어올 예정이 없는 경우)
		if (_mJobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}
	}
}

void MPSCJobQueue::ExecuteOnce()
{
	// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
	std::vector<std::shared_ptr<Job>> jobs;
	_mJobs.PopAll(OUT jobs);

	// 처리가 확정된 job 갯수
	const int32 jobCount = static_cast<int32>(jobs.size());
	for (int32 i = 0; i < jobCount; ++i)
	{
		jobs[i]->Execute();
	}

	_mJobCount.fetch_sub(jobCount);
}

void MPSCJobQueue::ExecuteOnlyLastOne()
{
	// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
	std::vector<std::shared_ptr<Job>> jobs;
	_mJobs.PopAll(OUT jobs);

	// 처리가 확정된 job 갯수
	const int32 jobCount = static_cast<int32>(jobs.size());
	if (jobs.empty() == false)
	{
		jobs.back()->Execute();
	}

	_mJobCount.fetch_sub(jobCount);
}

void MPSCJobQueue::Push(const std::shared_ptr<Job>& job)
{
	ASSERT_MSG(job != nullptr, "nullptr job isn't allowed");

	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);
}

void MPSCJobQueue::Push(std::shared_ptr<Job>&& job)
{
	ASSERT_MSG(job != nullptr, "nullptr job isn't allowed");

	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);
}

