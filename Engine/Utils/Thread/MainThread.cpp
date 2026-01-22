#include "pch.h"
#include "MainThread.h"

#include "Manager/ThreadManager.h"
#include "Manager/RenderManager.h"

void MainThread::Run()
{
	while (THREAD_MANAGER->IsAlive() == true)
	{
		LEndTickCount = GetTickCount64() + mThreadMaxTick;
		Work();
	}
}

void RenderThread::Work()
{

}
