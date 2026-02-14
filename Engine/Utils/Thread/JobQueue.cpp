#include "pch.h"
#include "JobQueue.h"

#include "Manager/ThreadManager.h"

void SequentialJobQueue::Execute()
{
	LCurrentJobQueue = this;

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
			LCurrentJobQueue = nullptr;
			return;
		}

		// 일감이 너무 몰려서 특정 시간을 초과한 경우
		const uint64 now = GetTickCount64();
		if (now >= LEndTickCount)
		{
			// 타 스레드에 GlobalQueue로 명시
			LCurrentJobQueue = nullptr;
			THREAD_MANAGER->PushGlobalSequentialJobQ(std::static_pointer_cast<SequentialJobQueue>(shared_from_this()));
			return;
		}
	}
}

void SequentialJobQueue::Push(const std::shared_ptr<Job>& job, bool pushOnly)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);

	if (preCount == 0)
	{
		// Execute 내부에서 Execute 방지
		if (LCurrentJobQueue == nullptr && pushOnly == false && LThreadId >= MainThreadType::End)
		{
			Execute();
		}
		else
		{
			// 타 스레드에 GlobalQueue로 명시
			THREAD_MANAGER->PushGlobalSequentialJobQ(std::static_pointer_cast<SequentialJobQueue>(shared_from_this()));
		}
	}
}

void SequentialJobQueue::Push(std::shared_ptr<Job>&& job, bool pushOnly)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);

	if (preCount == 0)
	{
		// Execute 내부에서 Execute 방지
		if (LCurrentJobQueue == nullptr && pushOnly == false && LThreadId >= MainThreadType::End)
		{
			Execute();
		}
		else
		{
			// 타 스레드에 GlobalQueue로 명시
			THREAD_MANAGER->PushGlobalSequentialJobQ(std::static_pointer_cast<SequentialJobQueue>(shared_from_this()));
		}
	}
}

void ConcurrentJobQueue::Append(std::shared_ptr<ConcurrentJobQueue> jobQueue)
{
	if (this == jobQueue.get())
	{
		return;
	}

	// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
	std::vector<std::shared_ptr<Job>> jobs;
	jobQueue->_mJobs.PopAll(OUT jobs);

	for (auto& job : jobs)
	{
		Push(job, true);
	}
}

void ConcurrentJobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
		std::vector<std::shared_ptr<Job>> jobs;
		_mJobs.PopBatch(OUT jobs, _mBatchSize);

		// 처리가 확정된 job 갯수
		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; ++i)
		{
			jobs[i]->Execute();
		}

		// 남은 일감이 없는 경우
		if (_mJobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		// 일감이 너무 몰려서 특정 시간을 초과한 경우
		const uint64 now = GetTickCount64();
		if (now >= LEndTickCount)
		{
			// 타 스레드에 GlobalQueue로 명시
			LCurrentJobQueue = nullptr;
			THREAD_MANAGER->PushGlobalConcurrentJobQ(std::static_pointer_cast<ConcurrentJobQueue>(shared_from_this()));
			return;
		}
	}
}

void ConcurrentJobQueue::Push(const std::shared_ptr<Job>& job, bool pushOnly)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);

	if (preCount == 0)
	{
		// Execute 내부에서 Execute 방지
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 타 스레드에 GlobalQueue로 명시
			THREAD_MANAGER->PushGlobalConcurrentJobQ(std::static_pointer_cast<ConcurrentJobQueue>(shared_from_this()));
		}
	}
}

void ConcurrentJobQueue::Push(std::shared_ptr<Job>&& job, bool pushOnly)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);

	if (preCount == 0)
	{
		// Execute 내부에서 Execute 방지
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 타 스레드에 GlobalQueue로 명시
			THREAD_MANAGER->PushGlobalConcurrentJobQ(std::static_pointer_cast<ConcurrentJobQueue>(shared_from_this()));
		}
	}
}
