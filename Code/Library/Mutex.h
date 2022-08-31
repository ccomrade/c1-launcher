#pragma once

// WinAPI headers must not be included in a header
#ifndef _INC_WINDOWS

#pragma pack(push, 8)
#ifdef BUILD_64BIT
struct CRITICAL_SECTION { unsigned char reserved[40]; };
#else
struct CRITICAL_SECTION { unsigned char reserved[24]; };
#endif
#pragma pack(pop)

extern "C" void __stdcall InitializeCriticalSection(CRITICAL_SECTION* cs);
extern "C" void __stdcall DeleteCriticalSection(CRITICAL_SECTION* cs);
extern "C" void __stdcall EnterCriticalSection(CRITICAL_SECTION* cs);
extern "C" void __stdcall LeaveCriticalSection(CRITICAL_SECTION* cs);

#endif

class Mutex
{
	CRITICAL_SECTION m_cs;

	// no copies
	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

public:
	Mutex()
	{
		InitializeCriticalSection(&m_cs);
	}

	~Mutex()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
};

template<class T>
class LockGuard
{
	T& m_lock;

public:
	explicit LockGuard(T& lock) : m_lock(lock)
	{
		m_lock.Lock();
	}

	~LockGuard()
	{
		m_lock.Unlock();
	}
};
