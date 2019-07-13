/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2007.
//
// Description: Implementation of the CrySimpleThread class for Windows.
//
// History:
// Jun 22, 2007: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CRYTHREAD_WINDTHREAD_H__
#define __CRYTHREAD_WINDTHREAD_H__ 1

#pragma once

// This file contains the implementation of the CrySimpleThread class, which
// is common to all CryThread implementations on Windows - the CryThread
// implementations only differ in the way locks and conditions are
// implemented.
//
// Note: This file assumes that <windows.h> has already been included!

#include <process.h>

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

// vim:ts=2:sw=2

