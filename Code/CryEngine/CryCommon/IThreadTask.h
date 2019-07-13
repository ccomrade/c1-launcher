////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2006.
// -------------------------------------------------------------------------
//  File name:   IThreadTask.h
//  Version:     v1.00
//  Created:     19/09/2006 by Timur.
//  Compilers:   Visual Studio 2005
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __i_thread_task_h__
#define __i_thread_task_h__

#if _MSC_VER > 1000
#pragma once
#endif

enum EThreadTaskFlags
{

};

struct SThreadTaskParams
{
	int nFlags;           //< Task flags. @see ETaskFlags
	int nPreferedThread;  //< Prefered Thread index (0,1,2,3...)
};


//////////////////////////////////////////////////////////////////////////
// Tasks must implement this interface.
//////////////////////////////////////////////////////////////////////////
struct IThreadTask
{
	// The function to be called on every update.
	virtual void OnUpdate() = 0;
};

#define MAX_TASK_THREADS_COUNT 4

//////////////////////////////////////////////////////////////////////////
// Task manager.
//////////////////////////////////////////////////////////////////////////
struct IThreadTaskManager
{
	// Register new task to the manager.
	virtual void RegisterTask( IThreadTask *pTask,const SThreadTaskParams &options ) = 0;
	virtual void UnregisterTask( IThreadTask *pTask ) = 0;

	// Limit number of threads to this amount.
	virtual void SetMaxThreadCount( int nMaxThreads ) = 0;

	virtual void SetThreadName( unsigned int dwThreadId,const char *sThreadName ) = 0;
	virtual const char* GetThreadName( unsigned int dwThreadId ) = 0;
};

#endif //__i_thread_task_h__