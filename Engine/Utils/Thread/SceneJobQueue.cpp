#include "pch.h"
#include "SceneJobQueue.h"
#include "Manager/ThreadManager.h"
#include "Manager/RenderManager.h"

void SceneJobQueue::ExecuteOnce()
{
	const float preRemainDeltaTime = SCENE->GetPreRemainTime();
	const float accRenderDeltaTime = SCENE->GetAccRenderDeltaTime() + RENDER_TIME_MANAGER->GetDeltaTime();
	float accGameDeltaTime = preRemainDeltaTime;

	// 가능하대로 빼내 처리 (도중에 새로운 Job 추가 가능성 있음)
	std::vector<std::shared_ptr<JobContext<float>>> jobs;
	const int32 restJobCount = _mJobs.PopUntil(OUT jobs, [&accGameDeltaTime, accRenderDeltaTime](const std::shared_ptr<JobContext<float>>& job) {
		accGameDeltaTime += job->GetMetaData();
		return accGameDeltaTime < accRenderDeltaTime;
		}, false);

	// 처리가 확정된 job 갯수
	const int32 jobCount = static_cast<int32>(jobs.size());
	if (jobCount == 0)
	{
		// Game Thread가 바쁘다면 랜더의 1틱 델타 만큼을 버린다
	}
	else
	{
		// 정상 처리
		SCENE->SetAccRenderDeltaTime(0.f);

		// 보간 없이 처리
		int32 i = 0;
		for (; i < jobCount - 1; ++i)
		{
			SCENE->SetPreRemainTime(0.f);
			jobs[i]->Execute();
		}
		// 마지막 작업만 처리 후 남은 보간 시간 저장
		SCENE->SetPreRemainTime(accGameDeltaTime - accRenderDeltaTime);
		jobs[i]->Execute();
	}

	_mJobCount.fetch_sub(jobCount);
}

void SceneJobQueue::Push(const std::shared_ptr<JobContext<float>>& job)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);
}

void SceneJobQueue::Push(std::shared_ptr<JobContext<float>>&& job)
{
	// 실제 job 추가 전에 Count 증가로 PushJobQ 예고
	const int32 preCount = _mJobCount.fetch_add(1);
	_mJobs.Push(job);
}