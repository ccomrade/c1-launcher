/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2007.
//
// Description: Generation counter based implementation of the CryThread API.
//
// History:
// Jun 22, 2007: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CRYTHREAD_WINGC_H__
#define __CRYTHREAD_WINGC_H__ 1

#pragma once

// This implementation of the CryThread API is based on the "Generation Count
// Solution" by Douglas C. Schmidt and Irfan Pyarali. See
// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html for details.
// 
// Features of this implementation:
// - CryLock is implemented using a Win32 CriticalSection. CryLock can be used
//   together with condition variables (no need for a special CryCondLock
//   class).
// - Reasonably simple.
//
// Drawbacks:
// - Posibility of busy-waiting if the waiter is running on the highest
//   priority thread.
// - May cause unfair scheduling in some situations.
// - Includes <windows.h>, which may cause some problems.

#ifdef _WIN32_WINNT
# if _WIN32_WINNT < 0x400
#  undef _WIN32_WINNT
#  define _WIN32_WINNT 0x400
# endif
#else
# define _WIN32_WINNT 0x400
#endif

#include <windows.h>

template <typename CryLockT> class CryCond;

template <>
class CryLock<CRYLOCK_RECURSIVE>
{
	friend class CryCond< CryLock<CRYLOCK_RECURSIVE> >;

protected:
	CRITICAL_SECTION m_cs;

public:
	CryLock()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CryLock()
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

	bool TryLock()
	{
		if (TryEnterCriticalSection(&m_cs))
			return true;
		else
			return false;
	}

#ifndef NDEBUG
	bool IsLocked() { return true; }
#endif
};

template <>
class CryLock<CRYLOCK_FAST> : private CryLock<CRYLOCK_RECURSIVE>
{
	friend class CryCond< CryLock<CRYLOCK_FAST> >;

#ifndef NDEBUG
	// On Win32, critical sections are recursive. However, some platforms
	// support _real_ fast locks, so we'll add debug code to make sure a fast
	// lock is not locked recursively.
	bool m_bLocked;
#endif

public:
	CryLock()
	{
#ifndef NDEBUG
		m_bLocked = false;
#endif
	}

	~CryLock() { }

	void Lock()
	{
		EnterCriticalSection(&m_cs);
#ifndef NDEBUG
		assert(!m_bLocked);
		m_bLocked = true;
#endif
	}

	void Unlock()
	{
#ifndef NDEBUG
		m_bLocked = false;
#endif
		LeaveCriticalSection(&m_cs);
	}

	bool TryLock()
	{
		if (TryEnterCriticalSection(&m_cs))
		{
#ifndef NDEBUG
			assert(!m_bLocked);
			m_bLocked = true;
#endif
			return true;
		}
		else
			return false;
	}

#ifndef NDEBUG
	bool IsLocked() { return m_bLocked; }
#endif
};

class CryCondBase
{
	// Number of waiters.
	int m_nWaitersCount;

	// CS protecting the number of waiters 'm_nWaitersCount'.
	CRITICAL_SECTION m_csWaitersCountLock;

	// Number of waiting threads to be released on a notification.
	int m_nReleaseCount;

	// The current generation counter.  The generation counter is used to
	// prevent one thread from 'stealing' all releases from a notification.
	int m_nWaitGeneration;

	// Manual-reset event for blocking and releasing waiting threads.
	HANDLE m_hEvent;

	CryCondBase(const CryCondBase &);
	CryCondBase &operator= (const CryCondBase &);

protected:
	CryCondBase()
		: m_nWaitersCount(0),
			m_nReleaseCount(0),
			m_nWaitGeneration(0)
	{
		InitializeCriticalSection(&m_csWaitersCountLock);
		m_hEvent = CreateEvent(NULL, true, false, NULL);
	}

	~CryCondBase()
	{
		DeleteCriticalSection(&m_csWaitersCountLock);
		CloseHandle(m_hEvent);
	}

	void Wait(CRITICAL_SECTION *const cs)
	{
		const HANDLE hEvent = m_hEvent;

		EnterCriticalSection(&m_csWaitersCountLock);
		++m_nWaitersCount;
		const int nGeneration = m_nWaitGeneration;
		LeaveCriticalSection(&m_csWaitersCountLock);
		LeaveCriticalSection(cs);

		while (true)
		{
			WaitForSingleObject(hEvent, INFINITE);
			EnterCriticalSection(&m_csWaitersCountLock);
			bool bDone
				= m_nReleaseCount > 0 && m_nWaitGeneration != nGeneration;
			LeaveCriticalSection(&m_csWaitersCountLock);
			if (bDone)
				break;
		}

		EnterCriticalSection(cs);
		EnterCriticalSection(&m_csWaitersCountLock);
		--m_nWaitersCount;
		--m_nReleaseCount;
		const bool bLast = m_nReleaseCount == 0;
		LeaveCriticalSection(&m_csWaitersCountLock);

		if (bLast)
			ResetEvent(hEvent);
	}

	bool TimedWait(CRITICAL_SECTION *const cs, uint32 nMilliseconds)
	{
		const HANDLE hEvent = m_hEvent;

		EnterCriticalSection(&m_csWaitersCountLock);
		++m_nWaitersCount;
		const int nGeneration = m_nWaitGeneration;
		LeaveCriticalSection(&m_csWaitersCountLock);
		LeaveCriticalSection(cs);
		bool csLocked = false;
		bool bTimeout = false;
		bool bLast = false;

		// The 'TimedWait(...)' implementation is a bit tricky, because the case
		// where a notification arrives between the 'WaitForSingleObject()' and
		// the following 'EnterCriticalSection(cs)' (in case of a timeout) must be
		// handled correctly, i.e. it must be treated as if the notification
		// arrived just in time -> no timeout.

		while (true)
		{
			bool bDone = false;
			if (WaitForSingleObject(hEvent, nMilliseconds) == WAIT_TIMEOUT)
			{
				EnterCriticalSection(cs);
				csLocked = true;
				// Note that any code signalling the condition should be holding the
				// lock.  Although this is not enforced, it is OK to miss the
				// notification if the lock is clear while the notification was sent
				// (because in that case the calling code is in a race condition
				// anyway and this can not be fixed here).
				EnterCriticalSection(&m_csWaitersCountLock);
				bDone = m_nReleaseCount > 0 && m_nWaitGeneration != nGeneration;
				if (!bDone)
					bTimeout = true;
				LeaveCriticalSection(&m_csWaitersCountLock);
				break;
			}
			else
			{
				EnterCriticalSection(&m_csWaitersCountLock);
				bDone = m_nReleaseCount > 0 && m_nWaitGeneration != nGeneration;
				LeaveCriticalSection(&m_csWaitersCountLock);
				if (bDone)
					break;
			}
		}

		if (!csLocked)
		{
			EnterCriticalSection(cs);
			csLocked = true;
		}
		EnterCriticalSection(&m_csWaitersCountLock);
		--m_nWaitersCount;
		if (!bTimeout)
		{
			--m_nReleaseCount;
			bLast = m_nReleaseCount == 0;
		}
		LeaveCriticalSection(&m_csWaitersCountLock);

		if (bLast)
			ResetEvent(hEvent);

		return !bTimeout;
	}

	void NotifySingle()
	{
		EnterCriticalSection(&m_csWaitersCountLock);
		if (m_nWaitersCount > m_nReleaseCount)
		{
			SetEvent(m_hEvent);
			++m_nReleaseCount;
			++m_nWaitGeneration;
		}
		LeaveCriticalSection(&m_csWaitersCountLock);
	}

	void Notify()
	{
		EnterCriticalSection(&m_csWaitersCountLock);
		if (m_nWaitersCount > 0)
		{
			SetEvent(m_hEvent);
			m_nReleaseCount = m_nWaitersCount;
			++m_nWaitGeneration;
		}
		LeaveCriticalSection(&m_csWaitersCountLock);
	}
};

template <>
class CryCond< CryLock<CRYLOCK_FAST> > : private CryCondBase
{
public:
	typedef CryLock<CRYLOCK_FAST> LockT;

	void Wait(LockT &lock)
	{
		CryCondBase::Wait(&lock.m_cs);
	}

	bool TimedWait(LockT &lock, uint32 nMilliseconds)
	{
		return CryCondBase::TimedWait(&lock.m_cs, nMilliseconds);
	}

	void NotifySingle() { CryCondBase::NotifySingle(); }

	void Notify() { CryCondBase::Notify(); }
};

template <>
class CryCond< CryLock<CRYLOCK_RECURSIVE> > : private CryCondBase
{
public:
	typedef CryLock<CRYLOCK_RECURSIVE> LockT;

	void Wait(LockT &lock)
	{
		CryCondBase::Wait(&lock.m_cs);
	}

	bool TimedWait(LockT &lock, uint32 nMilliseconds)
	{
		return CryCondBase::TimedWait(&lock.m_cs, nMilliseconds);
	}

	void NotifySingle() { CryCondBase::NotifySingle(); }

	void Notify() { CryCondBase::Notify(); }
};

#include <CryThread_winthread.h>

#endif

// vim:ts=2:sw=2

