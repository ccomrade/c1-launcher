/////////////////////////////////////////////////////////////////////////////
//
// Crytek Source File
// Copyright (C), Crytek Studios, 2001-2006.
//
// History:
// Jun 20, 2006: Created by Sascha Demetrio
//
/////////////////////////////////////////////////////////////////////////////

#include "CryThread_pthreads.h"

#if !defined(PS3)
	template<>
	_PthreadLockAttr<PTHREAD_MUTEX_FAST_NP>
		_PthreadLockBase<PTHREAD_MUTEX_FAST_NP>::m_Attr = 0;
#endif

template<>
_PthreadLockAttr<PTHREAD_MUTEX_RECURSIVE>
	_PthreadLockBase<PTHREAD_MUTEX_RECURSIVE>::m_Attr = 0;

THREADLOCAL CrySimpleThreadSelf
	*CrySimpleThreadSelf::m_Self = NULL;

// vim:ts=2

