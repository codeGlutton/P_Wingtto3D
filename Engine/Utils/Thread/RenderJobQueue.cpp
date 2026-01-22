#include "pch.h"
#include "RenderJobQueue.h"
#include "Manager/ThreadManager.h"

void RenderJobQueue::Execute()
{
	const uint64 renderFrameNumber = TIME_MANAGER->GetRenderFrameNumber();

	while (true)
	{
		// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
		std::vector<std::shared_ptr<JobContext<uint64>>> jobs;
		const int32 restJobCount = _mJobs.PopUntil(OUT jobs, [renderFrameNumber](const std::shared_ptr<JobContext<uint64>>& job) {
			return job->GetMetaData() <= renderFrameNumber;
		});

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

		// 해당 프레임 일감을 마친 경우
		if (restJobCount != 0 && jobCount == 0)
		{
			return;
		}
	}
}

void RenderJobQueue::Push(const std::shared_ptr<JobContext<uint64>>& job)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);
}

void RenderJobQueue::Push(std::shared_ptr<JobContext<uint64>>&& job)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);
}