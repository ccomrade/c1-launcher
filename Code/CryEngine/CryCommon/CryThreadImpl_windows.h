// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __CRYTHREADIMPL_WINDOWS_H__
#define __CRYTHREADIMPL_WINDOWS_H__
#pragma once

//#include <IThreadTask.h>

#define WIN32_LEAN_AND_MEAN
#ifndef XENON
#include <windows.h>
#endif

struct SThreadNameDesc
{
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;
};

THREADLOCAL CrySimpleThreadSelf
  *CrySimpleThreadSelf::m_Self = NULL;

#endif
