////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   platform_impl.h
//  Version:     v1.00
//  Created:     23/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: This file should only be included Once in DLL module.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __platform_impl_h__
#define __platform_impl_h__
#pragma once

#include <platform.h>
#include <ISystem.h>

#if defined(_LIB) && !defined(_LAUNCHER)
	extern CMTRand_int32 g_random_generator;
	extern SSystemGlobalEnvironment* gEnv;
#else //_LIB

//////////////////////////////////////////////////////////////////////////
// If not in static library.

#include <CryThreadImpl.h>
#include <CryCommon.cpp>

// this global environment variable must be initialized in each module!
SSystemGlobalEnvironment* gEnv = NULL;

/*
#ifdef CRY_STRING
int sEmptyStringBuffer[] = { -1, 0, 0, 0 };
template <>
string::StrHeader* string::m_emptyStringData = (string::StrHeader*)&sEmptyStringBuffer;
template <>
wstring::StrHeader* wstring::m_emptyStringData = (wstring::StrHeader*)&sEmptyStringBuffer;
#endif //CRY_STRING
*/

#if defined(WIN32) || defined(WIN64)
void CryPureCallHandler()
{
	CryError("Pure function call");
}

void InitPureCallHandler()
{
	_set_purecall_handler(CryPureCallHandler);
}
#else
void InitPureCallHandler() {}
#endif

void ModuleInitISystem( ISystem *pSystem )
{
	InitPureCallHandler();
	gEnv = 0;
	if (pSystem)
		gEnv = pSystem->GetGlobalEnvironment();
}

bool g_bProfilerEnabled = false;
bool g_bTraceAllocations = false;

//////////////////////////////////////////////////////////////////////////
extern "C" {
	CRYSYSTEM_API unsigned int CryRandom(); // Exported by CrySystem
}

CMTRand_int32 g_random_generator;
uint32 cry_rand32()
{
	return g_random_generator.Generate();
}
unsigned int cry_rand()
{
	//return CryRandom(); // Return in range from 0 to RAND_MAX
	return g_random_generator.Generate() & RAND_MAX;
}
float cry_frand()
{
	return g_random_generator.GenerateFloat();
}
//////////////////////////////////////////////////////////////////////////

// when using STL Port _STLP_DEBUG and _STLP_DEBUG_TERMINATE - avoid actually 
// crashing (default terminator seems to kill the thread, which isn't nice).
#ifdef _STLP_DEBUG_TERMINATE
void __stl_debug_terminate(void)
{
	CryWarning(VALIDATOR_MODULE_UNKNOWN, VALIDATOR_ERROR, "STL Debug Error" );
}
#endif


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#endif

#if defined(WIN32) || defined(WIN64)
#include <intrin.h>
#endif

// If we use cry memory manager this should be also included in every module.
#if defined(USING_CRY_MEMORY_MANAGER) && !defined(__SPU__)
	#include <CryMemoryManager_impl.h>
#endif

#if defined (_WIN32) || defined (XENON)

#include "CryAssert_impl.h"

//////////////////////////////////////////////////////////////////////////
void CryDebugBreak()
{
	DebugBreak();
}

//////////////////////////////////////////////////////////////////////////
void CrySleep( unsigned int dwMilliseconds )
{
	Sleep( dwMilliseconds );
}

//////////////////////////////////////////////////////////////////////////
int CryMessageBox( const char *lpText,const char *lpCaption,unsigned int uType)
{
#ifdef WIN32
	return MessageBox( NULL,lpText,lpCaption,uType );
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
int CryCreateDirectory( const char *lpPathName,void *lpSecurityAttributes )
{
	return CreateDirectory( lpPathName,(LPSECURITY_ATTRIBUTES)lpSecurityAttributes );
}

//////////////////////////////////////////////////////////////////////////
int CryGetCurrentDirectory( unsigned int nBufferLength,char *lpBuffer )
{
#ifdef WIN32
	return GetCurrentDirectory(nBufferLength,lpBuffer);
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
short CryGetAsyncKeyState( int vKey )
{
#ifdef WIN32
	return GetAsyncKeyState(vKey);
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
long  CryInterlockedIncrement( int volatile *lpAddend )
{
	return InterlockedIncrement((long*)lpAddend);
}

//////////////////////////////////////////////////////////////////////////
long  CryInterlockedDecrement( int volatile *lpAddend )
{
	return InterlockedDecrement((long*)lpAddend);
}

//////////////////////////////////////////////////////////////////////////
long	CryInterlockedExchangeAdd(long volatile * lpAddend, long Value)
{
	return InterlockedExchangeAdd(lpAddend, Value);
}

long	CryInterlockedCompareExchange(long volatile * dst, long exchange, long comperand)
{
	return InterlockedCompareExchange(dst, exchange, comperand);
}

void*	CryInterlockedCompareExchangePointer(void* volatile * dst, void* exchange, void* comperand)
{
	return InterlockedCompareExchangePointer(dst, exchange, comperand);
}


//////////////////////////////////////////////////////////////////////////
void* CryCreateCriticalSection()
{
	CRITICAL_SECTION *pCS = new CRITICAL_SECTION;
	InitializeCriticalSection(pCS);
	return pCS;
}

//////////////////////////////////////////////////////////////////////////
void  CryDeleteCriticalSection( void *cs )
{
	CRITICAL_SECTION *pCS = (CRITICAL_SECTION*)cs;
	if (pCS->LockCount >= 0)
		CryError("Critical Section hanging lock" );
	DeleteCriticalSection(pCS);
	delete pCS;
}

//////////////////////////////////////////////////////////////////////////
void  CryEnterCriticalSection( void *cs )
{
	EnterCriticalSection((CRITICAL_SECTION*)cs);
}

//////////////////////////////////////////////////////////////////////////
bool  CryTryCriticalSection( void *cs )
{
	return TryEnterCriticalSection((CRITICAL_SECTION*)cs) != 0;
}

//////////////////////////////////////////////////////////////////////////
void  CryLeaveCriticalSection( void *cs )
{
	LeaveCriticalSection((CRITICAL_SECTION*)cs);
}

//////////////////////////////////////////////////////////////////////////
uint32 CryGetFileAttributes( const char *lpFileName )
{
	return GetFileAttributes( lpFileName );
}

//////////////////////////////////////////////////////////////////////////
bool CrySetFileAttributes( const char *lpFileName,uint32 dwFileAttributes )
{
	return SetFileAttributes( lpFileName,dwFileAttributes ) != 0;
}

#else // WIN32

// These are implemented in WinBase.cpp
#if 0
void  CrySleep( unsigned int dwMilliseconds ) {}
int   CryMessageBox( const char *lpText, const char *lpCaption, unsigned int uType) { return 0; }
int   CryCreateDirectory( const char *lpPathName, void *lpSecurityAttributes ) { return 0; }
int   CryGetCurrentDirectory( unsigned int nBufferLength, char *lpBuffer ) { *lpBuffer = 0; return 0; }
short CryGetAsyncKeyState( int vKey ) { return 0; };
long  CryInterlockedIncrement( long volatile *lpAddend ) { (*lpAddend)++; return 0; }
long  CryInterlockedDecrement( long volatile *lpAddend ) { (*lpAddend)--; return 0; }
void* CryCreateCriticalSection() { return 0; }
void  CryDeleteCriticalSection( void *cs ) {}
void  CryEnterCriticalSection( void *cs ) {}
bool  CryTryCriticalSection( void *cs ) { return true; }
void  CryLeaveCriticalSection( void *cs ) {}
void  CryInitializeCriticalSection( void *cs ) {}
void  CryDeleteCriticalSection( void *cs ) {}
uint32 CryGetFileAttributes( const char *lpFileName ) { return -1; }
bool  CrySetFileAttributes( const char *lpFileName, uint32 dwFileAttributes ) { return false; }
#endif

#endif // _WIN32

#endif //_LIB

#ifndef _LIB















#endif

#if defined(WIN32) || defined(WIN64) || defined(XENON)
#if !defined(_LIB) || defined(_LAUNCHER)
int64 CryQueryPerformanceCounter()
{
#if !defined (USE_MULTICORE_SAVE_TIMING) && !defined(XENON)
	return __rdtsc();




#endif
}
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Threads implementation. For static linking it must be declared inline otherwise creating multiple symbols
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) && !defined(_LAUNCHER)
	#define THR_INLINE inline
#else
	#define THR_INLINE 
#endif

#if !defined __CRYCG__
THR_INLINE CCryThread::CCryThread( void (*func)(void *), void * p )
{
	//we should really use the platform code here rather than hardcode windows functionality












#if defined(LINUX)
	int err = pthread_create(&m_handle, NULL, (void *(*)(void *))func, p);
	assert(err == 0);
#else
	m_handle = (THREAD_HANDLE) _beginthread(func, 0, p);
#endif
}
#endif // __CRYCG__

//////////////////////////////////////////////////////////////////////////
#if !defined __CRYCG__
THR_INLINE CCryThread::~CCryThread()
{




#if defined(LINUX)
	int err = pthread_join(m_handle, NULL);
	assert(err == 0);
#else
	WaitForSingleObject( m_handle, INFINITE );
#endif
}
#endif // __CRYCG__

//////////////////////////////////////////////////////////////////////////
#if !defined __CRYCG__
THR_INLINE void CCryThread::SetName( const char * name )
{
#if !defined(PS3) && !defined (WIN64) && !defined(LINUX)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		const char *szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	} THREADNAME_INFO;

	THREADNAME_INFO info;
	{
		info.dwType = 0x1000;
		info.szName = name;
		info.dwThreadID = DWORD(-1);
		info.dwFlags = 0;
	}
	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////
inline void CryDebugStr( const char *format,... )
{
	/*
#ifdef CRYSYSTEM_EXPORTS
	va_list	ArgList;
	char		szBuffer[65535];
	va_start(ArgList, format);
	_vsnprintf_c(szBuffer,sizeof(szBuffer)-1, format, ArgList);
	va_end(ArgList);
	strcat(szBuffer,"\n");
	OutputDebugString(szBuffer);
#endif
	*/
}

#endif // __platform_impl_h__
