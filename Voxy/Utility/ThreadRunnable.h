#pragma once
#include "CoreTypes.h"

#include <future>
#include <atomic>
#include <mutex>

struct IThreadRunnable
{
	IThreadRunnable();
	virtual ~IThreadRunnable();

	virtual void Thread_Run();
	virtual void Thread_Execute() = 0;
	
private:
	void Thread_InternalRun();
	bool isThreadRunning = false;
	std::future<void> thread;
};
