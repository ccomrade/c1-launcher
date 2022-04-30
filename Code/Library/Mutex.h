#pragma once

#include "LockGuard.h"
#include "WinAPI.h"

class Mutex
{
	void* m_handle;

	// no copies
	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

public:
	Mutex() : m_handle(WinAPI::CriticalSection::Create())
	{
	}

	~Mutex()
	{
		WinAPI::CriticalSection::Destroy(m_handle);
	}

	void Lock()
	{
		WinAPI::CriticalSection::Enter(m_handle);
	}

	void Unlock()
	{
		WinAPI::CriticalSection::Leave(m_handle);
	}
};
