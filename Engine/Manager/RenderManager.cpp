#include "pch.h"
#include "RenderManager.h"

#include "Manager/ThreadManager.h"

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
}

void RenderManager::Init()
{
}

void RenderManager::Destroy()
{
}

void RenderManager::DrawWindow()
{
	ASSERT_THREAD(MainThreadType::Game);

	// 랜더 스레드 일정 틱 이상 이면 대기

	//FSlateDrawWindowPassInputs
}
