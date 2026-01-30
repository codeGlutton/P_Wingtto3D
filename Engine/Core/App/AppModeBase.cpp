#include "pch.h"
#include "AppModeBase.h"

#include "ManagerMaximum.h"

#include "Utils/Thread/Thread.h"
#include "Utils/Thread/MainThread.h"

void AppModeBase::Init()
{
	/* 싱글톤 초기화 */

	{
		// 순서 무관계

		PATH_MANAGER->Init();
		INPUT_MANAGER->Init();
	}

	{
		// 기반

		PACKAGE_MANAGER->Init();
		TIME_MANAGER->Init();
		OBJECT_MANAGER->Init();
	}

	{
		// 패키징 로드 요구

		APP_WIN_MANAGER->Init();
		RESOURCE_MANAGER->Init();
	}

	{
		// 최후 시작 요구

		RENDER_MANAGER->Init();
		THREAD_MANAGER->Init();
	}

	BeginThread();
}

void AppModeBase::Update()
{
	INPUT_MANAGER->Update();
	TIME_MANAGER->Update();
	THREAD_MANAGER->DoGameJob();
}

void AppModeBase::End()
{
	EndThread();

	{
		// 최후 시작 요구

		RENDER_MANAGER->Destroy();
	}

	{
		// 패키징 로드 요구

		RESOURCE_MANAGER->Destroy();
		APP_WIN_MANAGER->Destroy();
	}

	{
		// 기반

		OBJECT_MANAGER->Destroy();
		TIME_MANAGER->Destroy();
		PACKAGE_MANAGER->Destroy();
	}

	{
		// 순서 무관계

		INPUT_MANAGER->Destroy();
		PATH_MANAGER->Destroy();
	}
}

void AppModeBase::BeginThread()
{
	/* 스레드 생성 */

	const uint32 coreCount = std::thread::hardware_concurrency();
	const uint32 reservedCount = MainThreadType::Count + 1; // 자기 자신 (게임 스레드) 추가

	// 글로벌 워커 스레드 실행
	const uint32 workerThreadCount = (coreCount > reservedCount) ? (coreCount - reservedCount) : 1u;
	for (uint32 i = 0; i < workerThreadCount; ++i)
	{
		THREAD_MANAGER->Launch(std::make_shared<GlobalWorkerThread>());
	}

	// 메인 스레드 실행
	std::array<std::shared_ptr<MainThread>, MainThreadType::Count> mainThreads = {
		std::make_shared<RenderThread>()
	};
	THREAD_MANAGER->LaunchMainThreads(mainThreads);
}

void AppModeBase::EndThread()
{
	THREAD_MANAGER->Destroy();
}

