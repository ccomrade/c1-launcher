#pragma once

#include <new>

#include "Library/ThreadSafeQueue.h"
#include "Library/WinAPI.h"

struct IExecutorMainThreadTask
{
	virtual ~IExecutorMainThreadTask()
	{
	}

	virtual void RunOnMainThread() = 0;
};

class Executor
{
	unsigned long m_mainThreadID;
	ThreadSafeQueue<IExecutorMainThreadTask*> m_mainThreadQueue;

public:
	Executor();
	~Executor();

	bool IsMainThread() const;

	void SubmitTask(IExecutorMainThreadTask* task);

	void ExecuteMainThreadTasks();
};
