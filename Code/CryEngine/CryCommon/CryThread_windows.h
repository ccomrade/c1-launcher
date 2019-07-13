// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __CRYTHREAD_WINDOWS_H__
#define __CRYTHREAD_WINDOWS_H__

#pragma once

#ifndef XENON
#include <windows.h>
#endif
#include <process.h>

// kernel mutex - don't use... use CryLock<> or CryCond<CryLock<> > instead
class CryLock_WinMutex
{
public:
	CryLock_WinMutex() : m_hdl(CreateMutex(NULL, FALSE, NULL)) {}
	~CryLock_WinMutex() { CloseHandle(m_hdl); }

	void Lock() { WaitForSingleObject(m_hdl, INFINITE); }
	void Unlock() { ReleaseMutex(m_hdl); }
	bool TryLock() { return WaitForSingleObject(m_hdl, 0) != WAIT_TIMEOUT; }
#ifndef NDEBUG
	bool IsLocked() { return true; }
#endif

	HANDLE _get_win32_handle() { return m_hdl; }

private:
	HANDLE m_hdl;
};

// critical section... don't use... use CryLock instead
class CryLock_CritSection
{
public:
	CryLock_CritSection() { InitializeCriticalSection(&m_cs); }
	~CryLock_CritSection() { DeleteCriticalSection(&m_cs); }

	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }
	bool TryLock() { return TryEnterCriticalSection(&m_cs) != FALSE; }

#ifndef NDEBUG
	bool IsLocked() { return m_cs.RecursionCount > 0 && (DWORD)m_cs.OwningThread == GetCurrentThreadId(); }
#endif

private:
	CRITICAL_SECTION m_cs;
};

template <> class CryLock<CRYLOCK_RECURSIVE> : public CryLock_CritSection {};
template <> class CryLock<CRYLOCK_FAST> : public CryLock_CritSection {};
template <> class CryCondLock<CRYLOCK_RECURSIVE> : public CryLock_WinMutex {};
template <> class CryCondLock<CRYLOCK_FAST> : public CryLock_WinMutex {};
#define _CRYTHREAD_CONDLOCK_GLITCH 1

// most of this is taken from http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
template <CryLockType typ>
class CryCond<CryCondLock<typ>>
{
public:
	typedef CryCondLock<typ> LockType;

	CryCond()
	{
		m_waitersCount = 0;
		m_wasBroadcast = 0;
		m_sema = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
		InitializeCriticalSection(&m_waitersCountLock);
		m_waitersDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	~CryCond()
	{
		CloseHandle(m_sema);
		DeleteCriticalSection(&m_waitersCountLock);
		CloseHandle(m_waitersDone);
	}

	void Wait( LockType& lock )
	{
		EnterCriticalSection( &m_waitersCountLock );
		m_waitersCount ++;
		LeaveCriticalSection( &m_waitersCountLock );

		SignalObjectAndWait( lock._get_win32_handle(), m_sema, INFINITE, FALSE );

		EnterCriticalSection( &m_waitersCountLock );
		m_waitersCount --;
		bool lastWaiter = m_wasBroadcast && m_waitersCount == 0;
		LeaveCriticalSection( &m_waitersCountLock );

		if (lastWaiter)
			SignalObjectAndWait( m_waitersDone, lock._get_win32_handle(), INFINITE, FALSE );
		else
			WaitForSingleObject( lock._get_win32_handle(), INFINITE );
	}

	bool TimedWait( LockType& lock, uint32 millis )
	{
		EnterCriticalSection( &m_waitersCountLock );
		m_waitersCount ++;
		LeaveCriticalSection( &m_waitersCountLock );

		bool ok = true;
		if (WAIT_TIMEOUT == SignalObjectAndWait( lock._get_win32_handle(), m_sema, millis, FALSE ))
			ok = false;

		EnterCriticalSection( &m_waitersCountLock );
		m_waitersCount --;
		bool lastWaiter = m_wasBroadcast && m_waitersCount == 0;
		LeaveCriticalSection( &m_waitersCountLock );

		if (lastWaiter)
			SignalObjectAndWait( m_waitersDone, lock._get_win32_handle(), INFINITE, FALSE );
		else
			WaitForSingleObject( lock._get_win32_handle(), INFINITE );

		return ok;
	}

	void NotifySingle()
	{
		EnterCriticalSection(&m_waitersCountLock);
		bool haveWaiters = m_waitersCount > 0;
		LeaveCriticalSection(&m_waitersCountLock);
		if (haveWaiters)
			ReleaseSemaphore(m_sema, 1, 0);
	}

	void Notify()
	{
		EnterCriticalSection( &m_waitersCountLock );
		bool haveWaiters = false;
		if (m_waitersCount > 0)
		{
			m_wasBroadcast = 1;
			haveWaiters = true;
		}
		if (haveWaiters)
		{
			ReleaseSemaphore( m_sema, m_waitersCount, 0 );
			LeaveCriticalSection( &m_waitersCountLock );
			WaitForSingleObject( m_waitersDone, INFINITE );
			m_wasBroadcast = 0;
		}
		else
		{
			LeaveCriticalSection( &m_waitersCountLock );
		}
	}

private:
	int m_waitersCount;
	CRITICAL_SECTION m_waitersCountLock;
	HANDLE m_sema;
	HANDLE m_waitersDone;
	size_t m_wasBroadcast;
};

class CrySimpleThreadSelf
{
protected:
	static THREADLOCAL CrySimpleThreadSelf *m_Self;
};

template<class Runnable>
class CrySimpleThread
	: public CryRunnable,
		protected CrySimpleThreadSelf
{
public:
	typedef void (*ThreadFunction)(void *);

private:
	HANDLE m_thread;
	Runnable *m_Runnable;
	struct
	{
		ThreadFunction m_ThreadFunction;
		void *m_ThreadParameter;
	} m_ThreadFunction;
	volatile bool m_bIsStarted;
	volatile bool m_bIsRunning;

protected:
	virtual void Terminate()
	{
		// This method must be empty.
		// Derived classes overriding Terminate() are not required to call this
		// method.
	}

private:
	static void RunRunnable(void *thisPtr)
	{
		CrySimpleThread<Runnable> *const self = (CrySimpleThread<Runnable> *)thisPtr;
		self->m_bIsStarted = true;
		self->m_bIsRunning = true;
		self->m_Runnable->Run();
		self->m_bIsRunning = false;
		self->Terminate();
	}

	static void RunThis(void *thisPtr)
	{
		CrySimpleThread<Runnable> *const self = (CrySimpleThread<Runnable> *)thisPtr;
    self->m_bIsRunning = true;
		self->m_bIsStarted = true;
		self->Run();
		self->m_bIsRunning = false;
		self->Terminate();
	}

	CrySimpleThread(const CrySimpleThread<Runnable>&);
	void operator = (const CrySimpleThread<Runnable>&);

public:
	CrySimpleThread()
		: m_bIsStarted(false), m_bIsRunning(false)
	{
		m_thread = NULL;
		m_Runnable = NULL;
	}

	virtual ~CrySimpleThread()
	{
		if (IsStarted())
		{
			// Note: We don't want to cache a pointer to ISystem and/or ILog to
			// gain more freedom on when the threading classes are used (e.g.
			// threads may be started very early in the initialization).
			ISystem *pSystem = GetISystem();
			ILog *pLog = NULL;
			if (pSystem != NULL)
				pLog = pSystem->GetILog();
			if (pLog != NULL)
				pLog->LogError("Runaway thread");
			Cancel();
			Join();
		}
	}

	virtual void Run()
	{
		// This Run() implementation supports the void StartFunction() method.
		// However, code using this class (or derived classes) should eventually
		// be refactored to use one of the other Start() methods.  This code will
		// be removed some day and the default implementation of Run() will be
		// empty.
		if (m_ThreadFunction.m_ThreadFunction != NULL)
		{
			m_ThreadFunction.m_ThreadFunction(m_ThreadFunction.m_ThreadParameter);
		}
	}

	// Cancel the running thread.
	//
	// If the thread class is implemented as a derived class of CrySimpleThread,
	// then the derived class should provide an appropriate implementation for
	// this method.  Calling the base class implementation is _not_ required.
	//
	// If the thread was started by specifying a Runnable (template argument),
	// then the Cancel() call is passed on to the specified runnable.
	//
	// If the thread was started using the StartFunction() method, then the
	// caller must find other means to inform the thread about the cancellation
	// request.
	virtual void Cancel()
	{
		if (IsStarted() && m_Runnable != NULL)
			m_Runnable->Cancel();
	}

	virtual void Start(Runnable &runnable, unsigned cpuMask = 0)
	{
		m_Runnable = &runnable;
		m_thread = (HANDLE) _beginthread( RunRunnable, 0, this );
#ifndef XENON
		//SetThreadAffinityMask(m_thread, cpuMask);
#endif
		assert(m_thread);
	}

	virtual void Start(unsigned cpuMask = 0)
	{
		m_thread = (HANDLE) _beginthread( RunThis, 0, this );
#ifndef XENON
    //SetThreadAffinityMask(m_thread, cpuMask);
#endif
		assert(m_thread);
	}

	void StartFunction(
		ThreadFunction threadFunction,
		void *threadParameter = NULL
		)
	{
		m_ThreadFunction.m_ThreadFunction = threadFunction;
		m_ThreadFunction.m_ThreadParameter = threadParameter;
		Start();
	}

	static CrySimpleThread<Runnable> *Self()
	{
		return reinterpret_cast<CrySimpleThread<Runnable> *>(m_Self);
	}

	void Exit()
	{
		assert(!"implemented");
	}

	void Join()
	{
		WaitForSingleObject( m_thread, INFINITE );
	}

  void Stop()
  {
    m_bIsStarted = false;
  }

	bool IsStarted() { return m_bIsStarted; }
	bool IsRunning() { return m_bIsRunning; }
};

#endif
