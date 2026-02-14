#include "pch.h"
#include "TLS.h"

#include "Utils/Thread/JobQueue.h"

#include "Utils/Thread/MainThread.h"

thread_local uint32	LThreadId = MainThreadType::Game;
thread_local uint64 LEndTickCount = 0;

thread_local JobQueue* LCurrentJobQueue = nullptr;
thread_local std::stack<int32> LLockStack;

