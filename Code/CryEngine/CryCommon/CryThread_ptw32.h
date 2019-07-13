/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2007.
//
// Description: Implementation of the CryThread API based on the method used
// by the pthread_win32 implementation (no code copied from pthread_win32).
//
// History:
// Jun 22, 2007: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CRYTHREAD_PTW32_H__
#define __CRYTHREAD_PTW32_H__ 1

#pragma once

// This implementation of the CryThread API is based on the work of Ben
// Elliston and John Bossom, aka the pthread_win32 approach.  The
// implementation does _not_ copy the code from pthread_win32, but borrows
// some of the ideas.  See http://sources.redhat.com/pthreads-win32/ for
// details on the pthread_win32 implementation.

#ifdef _WIN32_WINNT
# if _WIN32_WINNT < 0x400
#  undef _WIN32_WINNT
#  define _WIN32_WINNT 0x400
# endif
#else
# define _WIN32_WINNT 0x400
#endif

#include <windows.h>
#include <intrin.h>

// Define this to enable lazy initialization of locks.
#define CRYLOCK_LAZYINIT 1
//#undef CRYLOCK_LAZYINIT

template <>
class CryLock<CRYLOCK_FAST>
{
	// Atomic state variable (modified via 'Interlocked*()' calls).
	//  0: free / not locked.
	//  1: locked, no waiters.
	// -1: locked, waiters are possible.
	LONG m_nLockState;

	// Event handle for mutex release notifications.  INVALID_HANDLE_VALUE if
	// the event has not been initialized yet (lazy initialization).
	HANDLE m_hEvent;

	// Get the event handle.  The method handles the synchronization required
	// for lazy initialization of the handle.
	HANDLE Event()
	{
#if defined CRYLOCK_LAZYINIT
		if (m_hEvent != INVALID_HANDLE_VALUE)
			return m_hEvent;
		HANDLE hEvent = CreateEvent(NULL, false, false, NULL);
		if (InterlockedCompareExchangePointer(
					(void **)&m_hEvent,
					hEvent,
					INVALID_HANDLE_VALUE) != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hEvent);
			return m_hEvent;
		}
		else
			return hEvent;
#else
		return m_hEvent;
#endif
	}

	CryLock(const CryLock<CRYLOCK_FAST> &);
	CryLock<CRYLOCK_FAST> &operator= (const CryLock<CRYLOCK_FAST> &);

public:
	CryLock()
		: m_nLockState(0)
	{
#if defined CRYLOCK_LAZYINIT
		m_hEvent = INVALID_HANDLE_VALUE;
#else
		m_hEvent = CreateEvent(NULL, false, false, NULL);
#endif
	}

	~CryLock()
	{
#if defined CRYLOCK_LAZYINIT
		if (m_hEvent != INVALID_HANDLE_VALUE)
			CloseHandle(m_hEvent);
#else
		CloseHandle(m_hEvent);
#endif
	}

	void Lock()
	{
		if (InterlockedExchange(&m_nLockState, 1) != 0)
		{
			HANDLE hEvent = Event();
			while (InterlockedExchange(&m_nLockState, -1) != 0)
				WaitForSingleObject(hEvent, INFINITE);
		}
	}

	bool TryLock()
	{
		if (InterlockedCompareExchange(&m_nLockState, 1, 0) != 0)
			return false;
		else
			return true;
	}

	void Unlock()
	{
		if (InterlockedExchange(&m_nLockState, 0) < 0)
			SetEvent(m_hEvent);
	}

#if !defined NDEBUG
	bool IsLocked() { return m_nLockState != 0; }
#endif
};

template <>
class CryLock<CRYLOCK_RECURSIVE>
{
	// Atomic state variable (modified via 'Interlocked*()' calls).
	//  0: free / not locked.
	//  1: locked, no waiters.
	// -1: locked, waiters are possible.
	LONG m_nLockState;

	// Event handle for mutex release notifications.  INVALID_HANDLE_VALUE if
	// the event has not been initialized yet (lazy initialization).
	HANDLE m_hEvent;

	// Recursion counter.
	unsigned m_nRecursionCount;

	// The thread ID of the thread owning the lock.  The value is
	// undefined/uninitialized if the lock is free.
	DWORD m_dwOwner;

	// Get the event handle.  The method handles the synchronization required
	// for lazy initialization of the handle.
	HANDLE Event()
	{
#if defined CRYLOCK_LAZYINIT
		if (m_hEvent != INVALID_HANDLE_VALUE)
			return m_hEvent;
		HANDLE hEvent = CreateEvent(NULL, false, false, NULL);
		if (InterlockedCompareExchangePointer(
					(void **)&m_hEvent,
					hEvent,
					INVALID_HANDLE_VALUE) != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hEvent);
			return m_hEvent;
		}
		else
			return hEvent;
#else
		return m_hEvent;
#endif
	}

	CryLock(const CryLock<CRYLOCK_RECURSIVE> &);
	CryLock<CRYLOCK_RECURSIVE> &operator= (const CryLock<CRYLOCK_RECURSIVE> &);

public:
	CryLock()
		: m_nLockState(0),
			m_nRecursionCount(0)
	{
#if defined CRYLOCK_LAZYINIT
		m_hEvent = INVALID_HANDLE_VALUE;
#else
		m_hEvent = CreateEvent(NULL, false, false, NULL);
#endif
	}

	~CryLock()
	{
#if defined CRYLOCK_LAZYINIT
		if (m_hEvent != INVALID_HANDLE_VALUE)
			CloseHandle(m_hEvent);
#else
		CloseHandle(m_hEvent);
#endif
	}

	void Lock()
	{
		const DWORD dwSelf = GetCurrentThreadId();

		if (InterlockedCompareExchange(&m_nLockState, 1, 0) == 0)
		{
			m_nRecursionCount = 1;
			m_dwOwner = dwSelf;
		}
		else
		{
			if (dwSelf == m_dwOwner)
				++m_nRecursionCount;
			else
			{
				HANDLE hEvent = Event();
				while (InterlockedExchange(&m_nLockState, -1) != 0)
					WaitForSingleObject(hEvent, INFINITE);
				m_nRecursionCount = 1;
				m_dwOwner = dwSelf;
			}
		}
	}

	bool TryLock()
	{
		if (InterlockedCompareExchange(&m_nLockState, 1, 0) == 0)
		{
			m_nRecursionCount = 1;
			m_dwOwner = GetCurrentThreadId();
			return true;
		}
		else
		{
			if (m_dwOwner == GetCurrentThreadId())
			{
				++m_nRecursionCount;
				return true;
			}
		}
		return false;
	}

	void Unlock()
	{
		assert(m_dwOwner == GetCurrentThreadId());
		if (--m_nRecursionCount == 0)
		{
			if (InterlockedExchange(&m_nLockState, 0) < 0)
				SetEvent(m_hEvent);
		}
	}
};

// The condition implementation uses semaphores internally.  The following
// semaphore class is currently _not_ part of the public CryThread API.
// However, if this implementation of CryThread is going to be _the_
// implementation on Win32/Win64, then the semaphore class may become part of
// the public API.

// Hide the CrySem class.  There's a matching #undef at the end of this file.
#define CrySem _CrySem

class CrySem
{
	// The semaphore value.
	int m_nValue;

	// Lock protecting the semaphore counter.
	CryLock<CRYLOCK_FAST> m_Lock;

	// The semaphore handle.  INVALID_HANDLE_VALUE if not initialized (lazy
	// initialization).
	HANDLE m_hSemaphore;

	// Get the semaphore handle.  The method handles the synchronization
	// required for lazy initialization of the handle.
	HANDLE Semaphore()
	{
#if defined CRYLOCK_LAZYINIT
		if (m_hSemaphore != INVALID_HANDLE_VALUE)
			return m_hSemaphore;
		HANDLE hSemaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);
		if (InterlockedCompareExchangePointer(
					(void **)&m_hSemaphore,
					hSemaphore,
					INVALID_HANDLE_VALUE) != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hSemaphore);
			return m_hSemaphore;
		}
		else
			return hSemaphore;
#else
		return m_hSemaphore;
#endif
	}

	CrySem();
	CrySem(const CrySem &);
	CrySem &operator= (const CrySem &);

public:
	CrySem(unsigned int value)
		: m_nValue(static_cast<int>(value))
	{
#if defined CRYLOCK_LAZYINIT
		m_hSemaphore = INVALID_HANDLE_VALUE;
#else
		m_hSemaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);
#endif
	}

	~CrySem()
	{
#if defined CRYLOCK_LAZYINIT
		if (m_hSemaphore != INVALID_HANDLE_VALUE)
			CloseHandle(m_hSemaphore);
#else
		CloseHandle(m_hSemaphore);
#endif
	}

	void Wait()
	{
		m_Lock.Lock();
		const int nValue = --m_nValue;
		if (nValue < 0)
		{
			HANDLE hSemaphore = Semaphore();
			m_Lock.Unlock();
			WaitForSingleObject(hSemaphore, INFINITE);
		}
		else
			m_Lock.Unlock();
	}

	bool TimedWait(uint32 nMilliseconds)
	{
		m_Lock.Lock();
		const int nValue = --m_nValue;
		if (nValue < 0)
		{
			HANDLE hSemaphore = Semaphore();
			m_Lock.Unlock();
			if (WaitForSingleObject(hSemaphore, nMilliseconds) == WAIT_TIMEOUT)
			{
				m_Lock.Lock();
				// The semaphore may have been posted between the timeout and now, so
				// we try again with a timeout value of 0 (this time with 'm_Lock'
				// being locked).
				if (WaitForSingleObject(hSemaphore, 0) == WAIT_OBJECT_0)
				{
					m_Lock.Unlock();
					return true;
				}
				++m_nValue;
				m_Lock.Unlock();
				return false;
			}
		}
		else
			m_Lock.Unlock();
		return true;
	}

	bool TryWait()
	{
		m_Lock.Lock();
		if (m_nValue > 0)
		{
			--m_nValue;
			m_Lock.Unlock();
			return true;
		}
		else
		{
			m_Lock.Unlock();
			return false;
		}
	}

	void Post()
	{
		m_Lock.Lock();
		const int nValue = ++m_nValue;
		if (nValue <= 0)
			ReleaseSemaphore(m_hSemaphore, 1, NULL);
		m_Lock.Unlock();
	}

	void Post(unsigned nCount)
	{
		m_Lock.Lock();
		int nWaiters = -m_nValue;
		m_nValue += nCount;
		if (nWaiters > 0)
			ReleaseSemaphore(
					m_hSemaphore,
					min(nWaiters, static_cast<int>(nCount)),
					NULL);
		m_Lock.Unlock();
	}

	int Value() const { return m_nValue; }
};

template <typename LockT>
class CryCondBase
{
	// Number of blocked waiters.
	int m_nWaitersBlocked;

	// Number of timed-out waiters.
	int m_nWaitersGone;

	// Number of waiting threads to unblock.
	int m_nWaitersToUnblock;

	// Semaphore queuing up threads waiting for the condition to be signalled.
	CrySem m_semBlockQueue;

	// Semaphore guarding access to the blocked thread queue and counter.
	CrySem m_semBlockLock;

	// Lock guarding the unbock counter.
	CryLock<CRYLOCK_FAST> m_lockUnblock;

	void Notify(bool bNotifyAll)
	{
		m_lockUnblock.Lock();
		int nSignalsToIssue;
		if (m_nWaitersToUnblock != 0)
		{
			if (m_nWaitersBlocked == 0)
			{
				m_lockUnblock.Unlock();
				return;
			}
			if (bNotifyAll)
			{
				nSignalsToIssue = m_nWaitersBlocked;
				m_nWaitersToUnblock += nSignalsToIssue;
				m_nWaitersBlocked = 0;
			}
			else
			{
				nSignalsToIssue = 1;
				++m_nWaitersToUnblock;
				--m_nWaitersBlocked;
			}
		}
		else if (m_nWaitersBlocked > m_nWaitersGone)
		{
			m_semBlockLock.Wait();
			if (m_nWaitersGone != 0)
			{
				m_nWaitersBlocked -= m_nWaitersGone;
				m_nWaitersGone = 0;
			}
			if (bNotifyAll)
			{
				nSignalsToIssue = m_nWaitersBlocked;
				m_nWaitersToUnblock = nSignalsToIssue;
				m_nWaitersBlocked = 0;
			}
			else
			{
				nSignalsToIssue = 1;
				m_nWaitersToUnblock = 1;
				--m_nWaitersBlocked;
			}
		}
		else
		{
			m_lockUnblock.Unlock();
			return;
		}
		m_lockUnblock.Unlock();
		m_semBlockQueue.Post(nSignalsToIssue);
	}

	CryCondBase(const CryCondBase<LockT> &);
	CryCondBase<LockT> &operator= (const CryCondBase<LockT> &);

protected:
	CryCondBase()
		: m_nWaitersBlocked(0),
			m_nWaitersGone(0),
			m_nWaitersToUnblock(0),
			m_semBlockQueue(0),
			m_semBlockLock(1)
	{ }

public:
	void Wait(LockT &lock)
	{
		m_semBlockLock.Wait();
		++m_nWaitersBlocked;
		m_semBlockLock.Post();
		lock.Unlock();
		m_semBlockQueue.Wait();
		m_lockUnblock.Lock();
		const int nSignalsLeft = m_nWaitersToUnblock;
		assert(nSignalsLeft > 0);
		--m_nWaitersToUnblock;
		m_lockUnblock.Unlock();
		if (nSignalsLeft == 1)
			m_semBlockLock.Post();
		lock.Lock();
	}

	bool TimedWait(LockT &lock, uint32 nMilliseconds)
	{
		m_semBlockLock.Wait();
		++m_nWaitersBlocked;
		m_semBlockLock.Post();
		lock.Unlock();
		const bool bResult = m_semBlockQueue.TimedWait(nMilliseconds);
		m_lockUnblock.Lock();
		const int nSignalsLeft = m_nWaitersToUnblock;
		if (nSignalsLeft != 0)
			--m_nWaitersToUnblock;
		else if (++m_nWaitersGone == INT_MAX / 2)
		{
			m_semBlockLock.Wait();
			m_nWaitersBlocked -= m_nWaitersGone;
			m_semBlockLock.Post();
			m_nWaitersGone = 0;
		}
		m_lockUnblock.Unlock();
		if (nSignalsLeft == 1)
			m_semBlockLock.Post();
		lock.Lock();
		return bResult;
	}

	void NotifySingle() { Notify(false); }

	void Notify() { Notify(true); }
};

#undef CrySem

template <>
class CryCond< CryLock<CRYLOCK_FAST> >
	: public CryCondBase< CryLock<CRYLOCK_FAST> >
{
	typedef CryCond< CryLock<CRYLOCK_FAST> > CondT;
	CryCond(const CondT &);
	CondT &operator= (const CondT &);

public:
	CryCond() { }
};

template <>
class CryCond< CryLock<CRYLOCK_RECURSIVE> >
	: public CryCondBase< CryLock<CRYLOCK_RECURSIVE> >
{
	typedef CryCond< CryLock<CRYLOCK_RECURSIVE> > CondT;
	CryCond(const CondT &);
	CondT &operator= (const CondT &);

public:
	CryCond() { }
};

#include <CryThread_winthread.h>

#endif

// vim:ts=2:sw=2

