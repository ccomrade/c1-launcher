/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2006.
//
// Description: Public include file for the multi-threading API.
//
// History:
// Jun 20, 2006: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CryThread_h_
#define _CryThread_h_ 1

#define _GNU_SOURCE 1

// Lock types:
//
// CRYLOCK_NONE
//   An empty dummy lock where the lock/unlock operations do nothing.
//
// CRYLOCK_FAST
//   A fast (non-recursive) mutex.
//
// CRYLOCK_RECURSIVE
//   A recursive mutex.
enum CryLockType
{
	CRYLOCK_NONE = 0,
#if !defined(PS3) || defined(__CRYCG__)
	CRYLOCK_FAST = 1,
#endif
	CRYLOCK_RECURSIVE = 2,
};





  #define CRYLOCK_HAVE_FASTLOCK 1


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void CryThreadSetName( unsigned int nThreadId,const char *sThreadName );
const char* CryThreadGetName( unsigned int nThreadId );

/////////////////////////////////////////////////////////////////////////////
//
// Primitive locks and conditions.
//
// Primitive locks are represented by instances of class CryLock<Type> and
// CryRWLock.
//
// Conditions are represented by instances of class CryCond<LockClass> where
// LockClass is the class of lock to be associated with the condition.
//
// CryCondLock<> acts like CryLock<>, but is always appropriate for CryCond.

template<CryLockType Type> class CryLock
{
	/* Unsupported lock type. */
};

template<CryLockType Type> class CryCondLock
{
	/* Unsupported lock type. */
};

class CryRWLock;

template<class LockClass> class CryCond
{
	/* Unsupported lock class. */
};

template<> class CryLock<CRYLOCK_NONE>
{
	CryLock(const CryLock<CRYLOCK_NONE>&);
	void operator = (const CryLock<CRYLOCK_NONE>&);

public:
	CryLock() { }
  void Lock() { }
	bool TryLock() { return true; }
  void Unlock() { }
#ifndef NDEBUG
	bool IsLocked() { return true; }
#endif
};

typedef CryLock<CRYLOCK_NONE> CryNoLock;



	typedef CryLock<CRYLOCK_FAST> CryFastLock;

typedef CryLock<CRYLOCK_RECURSIVE> CryRecursiveLock;

template<> class CryCond<CryNoLock>
{
	CryCond(const CryCond<CryNoLock>&);
	CryCond<CryNoLock>& operator = (const CryCond<CryNoLock>&);

public:
  CryCond() { }

  void Notify() { }
  void NotifySingle() { }
  void Wait() { }
	bool TimedWait(uint32 milliseconds) { return true; }
};

template<class LockClass> class CryAutoLock
{
private:
	LockClass &m_Lock;

	CryAutoLock();
	CryAutoLock(const CryAutoLock<LockClass>&);
	CryAutoLock<LockClass>& operator = (const CryAutoLock<LockClass>&);

public:
	CryAutoLock(LockClass &Lock) : m_Lock(Lock) { m_Lock.Lock(); }
	~CryAutoLock() { m_Lock.Unlock(); }
};

/////////////////////////////////////////////////////////////////////////////
//
// Threads.

// Base class for runnable objects.
//
// A runnable is an object with a Run() and a Cancel() method.  The Run()
// method should perform the runnable's job.  The Cancel() method may be
// called by another thread requesting early termination of the Run() method.
// The runnable may ignore the Cancel() call, the default implementation of
// Cancel() does nothing.
class CryRunnable
{
public:
	virtual ~CryRunnable() { }
	virtual void Run() = 0;
	virtual void Cancel() { }
};

// Class holding information about a thread.
//
// A reference to the thread information can be obtained by calling GetInfo()
// on the CrySimpleThread (or derived class) instance.
//
// NOTE:
// If the code is compiled with NO_THREADINFO defined, then the GetInfo()
// method will return a reference to a static dummy instance of this
// structure.  It is currently undecided if NO_THREADINFO will be defined for
// release builds!
struct CryThreadInfo
{
	// The symbolic name of the thread.
	//
	// You may set this name directly or through the SetName() method of
	// CrySimpleThread (or derived class).
	string m_Name;

	// A thread identification number.
	// The number is unique but architecture specific.  Do not assume anything
	// about that number except for being unique.
	//
	// This field is filled when the thread is started (i.e. before the Run()
	// method or thread routine is called).  It is advised that you do not
	// change this number manually.
	uint32 m_ID;
};

// Simple thread class.
//
// CrySimpleThread is a simple wrapper around a system thread providing
// nothing but system-level functionality of a thread.  There are two typical
// ways to use a simple thread:
//
// 1. Derive from the CrySimpleThread class and provide an implementation of
//    the Run() (and optionally Cancel()) methods.
// 2. Specify a runnable object when the thread is started.  The default
//    runnable type is CryRunnable.
//
// The Runnable class specfied as the template argument must provide Run()
// and Cancel() methods compatible with the following signatures:
//
//   void Runnable::Run();
//   void Runnable::Cancel();
//
// If the Runnable does not support cancellation, then the Cancel() method
// should do nothing.
//
// The same instance of CrySimpleThread may be used for multiple thread
// executions /in sequence/, i.e. it is valid to re-start the thread by
// calling Start() after the thread has been joined by calling Join().
template<class Runnable = CryRunnable> class CrySimpleThread;

// Standard thread class.
//
// The class provides a lock (mutex) and an associated condition variable.  If
// you don't need the lock, then you should used CrySimpleThread instead of
// CryThread.
template<class Runnable = CryRunnable> class CryThread;

// Include architecture specific code.


#if defined(LINUX)
#include <CryThread_pthreads.h>
#elif defined(WIN32) || defined(WIN64)

// Select one of the following implementations:

// Old production implementation.
#include <CryThread_windows.h>

// Implementation based on generation counters.
//#include <CryThread_wingc.h>

// Implementation based on the PTW32 concept.
//#include <CryThread_ptw32.h>





#else
// Put other platform specific includes here!
#include <CryThread_dummy.h>
#endif

#if !defined _CRYTHREAD_CONDLOCK_GLITCH
#define CryCondLock CryLock
#endif // !_CRYTHREAD_CONDLOCK_GLITCH

// The the architecture specific code does not define a class CryRWLock, then
// a default implementation is provided here.
#if !defined _CRYTHREAD_HAVE_RWLOCK && !defined _CRYTHREAD_CONDLOCK_GLITCH
class CryRWLock
{
	CryFastLock m_lockExclusiveAccess;
	CryFastLock m_lockSharedAccessComplete;
	CryCond<CryFastLock> m_condSharedAccessComplete;

	int m_nSharedAccessCount;
	int m_nCompletedSharedAccessCount;
	bool m_bExclusiveAccess;

	CryRWLock(const CryRWLock &);
	CryRWLock &operator= (const CryRWLock &);

	void AdjustSharedAccessCount()
	{
		m_nSharedAccessCount -= m_nCompletedSharedAccessCount;
		m_nCompletedSharedAccessCount = 0;
	}

public:
	CryRWLock()
		: m_nSharedAccessCount(0),
			m_nCompletedSharedAccessCount(0),
			m_bExclusiveAccess(false)
	{ }

	void RLock()
	{
		m_lockExclusiveAccess.Lock();
		if (++m_nSharedAccessCount == INT_MAX)
		{
			m_lockSharedAccessComplete.Lock();
			AdjustSharedAccessCount();
			m_lockSharedAccessComplete.Unlock();
		}
		m_lockExclusiveAccess.Unlock();
	}

	bool TryRLock()
	{
		if (!m_lockExclusiveAccess.TryLock())
			return false;
		if (++m_nSharedAccessCount == INT_MAX)
		{
			m_lockSharedAccessComplete.Lock();
			AdjustSharedAccessCount();
			m_lockSharedAccessComplete.Unlock();
		}
		m_lockExclusiveAccess.Unlock();
		return true;
	}

	void WLock()
	{
		m_lockExclusiveAccess.Lock();
		m_lockSharedAccessComplete.Lock();
		assert(!m_bExclusiveAccess);
		AdjustSharedAccessCount();
		if (m_nSharedAccessCount > 0)
		{
			m_nCompletedSharedAccessCount -= m_nSharedAccessCount;
			do
			{
				m_condSharedAccessComplete.Wait(m_lockSharedAccessComplete);
			}
			while (m_nCompletedSharedAccessCount < 0);
			m_nSharedAccessCount = 0;
		}
		m_bExclusiveAccess = true;
	}

	bool TryWLock()
	{
		if (!m_lockExclusiveAccess.TryLock())
			return false;
		if (!m_lockSharedAccessComplete.TryLock())
		{
			m_lockExclusiveAccess.Unlock();
			return false;
		}
		assert(!m_bExclusiveAccess);
		AdjustSharedAccessCount();
		if (m_nSharedAccessCount > 0)
		{
			m_lockSharedAccessComplete.Unlock();
			m_lockExclusiveAccess.Unlock();
			return false;
		}
		else
			m_bExclusiveAccess = true;
		return true;
	}

	void Unlock()
	{
		if (!m_bExclusiveAccess)
		{
			m_lockSharedAccessComplete.Lock();
			if (++m_nCompletedSharedAccessCount == 0)
				m_condSharedAccessComplete.NotifySingle();
			m_lockSharedAccessComplete.Unlock();
		}
		else
		{
			m_bExclusiveAccess = false;
			m_lockSharedAccessComplete.Unlock();
			m_lockExclusiveAccess.Unlock();
		}
	}
};
#endif // !defined _CRYTHREAD_HAVE_RWLOCK

// Thread class.
//
// CryThread is an extension of CrySimpleThread providing a lock (mutex) and a
// condition variable per instance.
template<class Runnable> class CryThread
	: public CrySimpleThread<Runnable>
{
	CryCondLock<CRYLOCK_RECURSIVE> m_Lock;
	CryCond< CryCondLock<CRYLOCK_RECURSIVE> > m_Cond;

	CryThread(const CryThread<Runnable>&);
	void operator = (const CryThread<Runnable>&);

public:
	CryThread() { }
	void Lock() { m_Lock.Lock(); }
	bool TryLock() { return m_Lock.TryLock(); }
	void Unlock() { m_Lock.Unlock(); }
	void Wait() { m_Cond.Wait(m_Lock); }
	// Timed wait on the associated condition.
	//
	// The 'milliseconds' parameter specifies the relative timeout in
	// milliseconds.  The method returns true if a notification was received and
	// false if the specified timeout expired without receiving a notification.
	//
	// UNIX note: the method will _not_ return if the calling thread receives a
	// signal.  Instead the call is re-started with the _original_ timeout
	// value.  This misfeature may be fixed in the future.
	bool TimedWait(uint32 milliseconds)
	{
		return m_Cond.TimedWait(m_Lock, milliseconds);
	}
	void Notify() { m_Cond.Notify(); }
	void NotifySingle() { m_Cond.NotifySingle(); }
	CryFastLock &GetLock() { return m_Lock; }
};

#endif

// vim:ts=2

