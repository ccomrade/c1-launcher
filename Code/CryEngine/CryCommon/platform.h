////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   platform.h
//  Version:     v1.00
//  Created:     11/12/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: Platform dependend stuff.
//               Include this file instead of windows.h
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_H_
#define _PLATFORM_H_
#pragma once

// Temporary here
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

// Debug STL turned off so we can use intermixed debug/release versions of DLL.
#undef _HAS_ITERATOR_DEBUGGING
#define _HAS_ITERATOR_DEBUGGING 0
#undef _SECURE_SCL
#define _SECURE_SCL 0

#if defined(_DEBUG) && !defined(PS3) && !defined(LINUX)
	#include <crtdbg.h>
#endif

//////////////////////////////////////////////////////////////////////////
// Available predefined compiler macros for Visual C++.
//		_MSC_VER										// Indicates MS Visual C compiler version
//		_WIN32, _WIN64, _XBOX_VER		// Indicates target OS
//		_M_IX86, _M_PPC							// Indicates target processor
//		_DEBUG											// Building in Debug mode
//		_DLL												// Linking with DLL runtime libs
//		_MT													// Linking with multi-threaded runtime libs
//////////////////////////////////////////////////////////////////////////

//
// Translate some predefined macros.
//

// NDEBUG disables std asserts, etc.
// Define it automatically if not compiling with Debug libs, or with ADEBUG flag.
#if !defined(_DEBUG) && !defined(ADEBUG) && !defined(NDEBUG)
	#define NDEBUG
#endif




	#define MATH_H <math.h>


// Xenon target. (We generally use _XBOX but should really use XENON).






// We use WIN macros without _.
#if defined(_WIN32) && !defined(XENON) && !defined(LINUX32) && !defined(LINUX64) && !defined(WIN32)
	#define WIN32
#endif
#if defined(_WIN64) && !defined(WIN64)
	#define WIN64
#endif

// All windows targets built as DLLs.
#if defined(WIN32)
	#ifndef _USRDLL
		#define _USRDLL
	#endif
#else
	//#define _LIB
#endif

#include "ProjectDefines.h"							// to get some defines available in every CryEngine project 

#include <stdlib.h>

// Function attribute for printf/scanf-style parameters.
// This enables extended argument checking by GCC.
//
// Usage:
// Put this after the function or method declaration (not the definition!),
// between the final closing parenthesis and the semicolon.
// The first parameter indicates the 1-based index of the format string
// parameter, the second parameter indicates the 1-based index of the first
// variable parameter.  Example:
//   void foobar(int a, const char *fmt, ...) PRINTF_PARAMS(2, 3);
//
// For va_list based printf style functions, specfy 0 as the second parameter.
// Example:
//   void foobarv(int a, const char *fmt, va_list ap) PRINTF_PARAMS(2, 0);
//
// Note that 'this' is counted as a method argument. For non-static methods,
// add 1 to the indices.
#if defined(__GNUC__)
  #define PRINTF_PARAMS(...) __attribute__ ((format (printf, __VA_ARGS__)))
  #define SCANF_PARAMS(...) __attribute__ ((format (scanf, __VA_ARGS__)))
#else
  #define PRINTF_PARAMS(...)
	#define SCANF_PARAMS(...)
#endif

// Storage class modifier for thread local storage.
#if defined(__GNUC__)
	#define THREADLOCAL __thread
#else
	#define THREADLOCAL __declspec(thread)
#endif

// DLL import / export






#if defined(__GNUC__)






		#define DLL_EXPORT __attribute__ ((visibility("default")))
		#define DLL_IMPORT __attribute__ ((visibility("default")))

#else
	#define DLL_EXPORT __declspec(dllexport)
	#define DLL_IMPORT __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
// Define BIT macro for use in enums and bit masks.
#define BIT(x) (1<<(x))
//////////////////////////////////////////////////////////////////////////

//will be defined for SPUs and PS3 therefore only
#define SPU_DEBUG_BREAK

//////////////////////////////////////////////////////////////////////////
// Globally Used Defines.
//////////////////////////////////////////////////////////////////////////
// CPU Types: _CPU_X86,_CPU_AMD64,_CPU_G5
// Platform: WIN23,WIN64,LINUX32,LINUX64,_XBOX
// CPU supported functionality: _CPU_SSE
//////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#include "MSVCspecific.h"
#endif

#if defined(WIN32) && !defined(WIN64)
#include "Win32specific.h"
#endif

#if defined(WIN64)
#include "Win64specific.h"
#endif

#if defined(LINUX64)
#include "Linux64Specific.h"
#endif

#if defined(LINUX32)
#include "Linux32Specific.h"
#endif









#include <stdio.h>

/////////////////////////////////////////////////////////////////////////
// CryModule memory manager routines must always be included.
// They are used by any module which doesn't define NOT_USE_CRY_MEMORY_MANAGER
// No Any STL includes must be before this line.
//////////////////////////////////////////////////////////////////////////
#if 1  //#ifndef NOT_USE_CRY_MEMORY_MANAGER
#define USE_NEWPOOL
#include "CryMemoryManager.h"
#else
inline int IsHeapValid()
{
#if defined(_DEBUG) && !defined(RELEASE_RUNTIME) && !defined(XENON) && !defined(PS3)
	return _CrtCheckMemory();


#endif
}
#endif // NOT_USE_CRY_MEMORY_MANAGER

// Memory manager breaks strdup
// Use something higher level, like CryString
// PS3 headers require this, does not compile otherwise
#if !defined(PS3)
	#define strdup dont_use_strdup
#endif







//////////////////////////////////////////////////////////////////////////
#ifndef DEPRICATED
#define DEPRICATED
#endif

//////////////////////////////////////////////////////////////////////////
// compile time error stuff
//////////////////////////////////////////////////////////////////////////
template<bool> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) \
	{ CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; } 

// Assert dialog box macros
#include "CryAssert.h"

// Replace standard assert calls by our custom one
// Works only ifdef USE_CRY_ASSERT && _DEBUG && WIN32
#ifndef assert
#define assert CRY_ASSERT
#endif

//////////////////////////////////////////////////////////////////////////
// Platform dependent functions that emulate Win32 API.
// Mostly used only for debugging!
//////////////////////////////////////////////////////////////////////////
void   CryDebugBreak();
void   CrySleep( unsigned int dwMilliseconds );
int    CryMessageBox( const char *lpText,const char *lpCaption,unsigned int uType);
int    CryCreateDirectory( const char *lpPathName,void *lpSecurityAttributes );
int    CryGetCurrentDirectory( unsigned int nBufferLength,char *lpBuffer );
short  CryGetAsyncKeyState( int vKey );
unsigned int CryGetFileAttributes( const char *lpFileName );
bool   CrySetFileAttributes( const char *lpFileName,uint32 dwFileAttributes );

#define CrySwprintf _snwprintf

_inline void CryHeapCheck()
{
#if !defined(LINUX) && !defined (PS3)
  int Result = _heapchk();
  assert(Result!=_HEAPBADBEGIN);
  assert(Result!=_HEAPBADNODE);
  assert(Result!=_HEAPBADPTR);
  assert(Result!=_HEAPEMPTY);
  assert(Result==_HEAPOK);
#endif
}

// Useful function to clean the structure.
template <class T>
inline void ZeroStruct( T &t ) { memset( &t,0,sizeof(t) ); }

#ifndef NOT_USE_CRY_STRING
	#include "CryString.h"
	typedef CryStringT<char> string;
	typedef CryStringT<wchar_t> wstring;

#else // NOT_USE_CRY_STRING
	#include <string>				// STL string
	typedef std::string string;
	typedef std::wstring wstring;
#endif // NOT_USE_CRY_STRING

// Include MultiThreading support.
#include "MultiThread.h"

// Include support for meta-type data.
#include "TypeInfo.h"

// Include Big/Small endianess conversion.
#include "Endian.h"

// Include array.
#include <CryArray.h>

// Wrapper code for non-windows builds.
#if defined(LINUX)
	#include "Linux_Win32Wrapper.h"
#endif




#ifndef NOT_USE_CRY_STRING
	// Fixed-Sized (stack based string)
	// put after the platform wrappers because of missing wcsicmp/wcsnicmp functions
	#include "CryFixedString.h"
#endif

// need this in a common header file and any other file would be too misleading
enum ETriState
{
	eTS_false,
	eTS_true,
	eTS_maybe
};

#define SAFE_DELETE_VOID_ARRAY(p) { if(p) { delete[] (unsigned char*)(p);   (p)=NULL; } }

#ifndef SPU_ENTRY
	#if defined __CRYCG__
		#define SPU_ENTRY(job_name) __attribute__ ((crycg_attr (entry, "job = " #job_name )))
	#else
		#define SPU_ENTRY(job_name)
	#endif
#endif

//only for PS3 this will take effect, win32 does not support alignment
#if !defined(_ALIGN)



		#define _ALIGN(num) 

#endif

#if !defined(PS3)
	//dummy definitions to avoid ifdef's
	ILINE void SPUAddCacheWriteRangeAsync(const unsigned int, const unsigned int){}
	#define __cache_range_write_async(a,b)
	#define __flush_cache_range(a,b)
	#define __flush_cache()
	#define DECLARE_SPU_JOB(func_name, typedef_name)
	#define DECLARE_SPU_CLASS_JOB(func_name, typedef_name, class_name)
#endif//__SPU__


#endif // _PLATFORM_H_
