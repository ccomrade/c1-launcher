/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2006.
//
// History:
// Jun 20, 2006: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CryThreadImpl_h_
#define _CryThreadImpl_h_ 1

#include <CryThread.h>

// Include architecture specific code.
#if defined(LINUX)
#include <CryThreadImpl_pthreads.h>
#elif defined(WIN32) || defined(WIN64)
#include <CryThreadImpl_windows.h>




#else
// Put other platform specific includes here!
#endif

#include <IThreadTask.h>

void CryThreadSetName( unsigned int dwThreadId,const char *sThreadName )
{
	if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIThreadTaskManager())
		gEnv->pSystem->GetIThreadTaskManager()->SetThreadName( dwThreadId,sThreadName );
}

const char* CryThreadGetName( unsigned int dwThreadId )
{
	if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIThreadTaskManager())
		return gEnv->pSystem->GetIThreadTaskManager()->GetThreadName( dwThreadId );
	return "";
}

#endif

// vim:ts=2

