#include "Utility/ThreadRunnable.h"

IThreadRunnable::IThreadRunnable()
{
}

IThreadRunnable::~IThreadRunnable()
{
	isThreadRunning = false;
	thread.get();
}

void IThreadRunnable::Thread_Run()
{
	if (isThreadRunning)
	{
		return;
	}

	thread = std::async(&IThreadRunnable::Thread_InternalRun, this);
}

void IThreadRunnable::Thread_InternalRun()
{
	isThreadRunning = true;
	while (isThreadRunning)
		Thread_Execute();
}