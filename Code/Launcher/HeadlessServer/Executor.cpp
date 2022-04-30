#include "Executor.h"

Executor::Executor() : m_mainThreadID(WinAPI::GetCurrentThreadID())
{
}

Executor::~Executor()
{
}

bool Executor::IsMainThread() const
{
	return WinAPI::GetCurrentThreadID() == m_mainThreadID;
}

void Executor::SubmitTask(IExecutorMainThreadTask* task)
{
	m_mainThreadQueue.Push(task);
}

void Executor::ExecuteMainThreadTasks()
{
	IExecutorMainThreadTask* task = NULL;

	while (m_mainThreadQueue.Pop(task))
	{
		if (task)
		{
			task->RunOnMainThread();
			delete task;
		}
	}
}
