#pragma once

class JobQueue;

extern thread_local uint32 LThreadId;
extern thread_local uint64 LEndTickCount;

extern thread_local JobQueue* LCurrentJobQueue;
extern thread_local std::stack<int32> LLockStack;

