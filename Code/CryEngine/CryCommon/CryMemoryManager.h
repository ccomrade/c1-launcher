////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   CryMemoryManager.h
//  Version:     v1.00
//  Created:     27/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Defines functions for CryEngine custom memory manager.
//               See also CryMemoryManager_impl.h, it must be included only once per module.
//               CryMemoryManager_impl.h is included by platform_impl.h
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
 
#ifndef __CryMemoryManager_h__
#define __CryMemoryManager_h__
#pragma once

//////////////////////////////////////////////////////////////////////////
// Define this if you want to use slow debug memory manager in any config.
//////////////////////////////////////////////////////////////////////////
//#define DEBUG_MEMORY_MANAGER
//////////////////////////////////////////////////////////////////////////

// THIS DEFINE ESPECIALLY FOR SOLVING TROUBLES WITH MEMORY ALLOCATOR. SHOULD BE DISABLED IN OTHER CASES
//#define _MEMORY_VALIDATOR_workaround

// That mean we use node_allocator for all small allocations

#define NOT_STANDARD_CRT

#ifdef DEBUG_MEMORY_MANAGER
	#define NOT_USE_CRY_MEMORY_MANAGER
#endif

#include "platform.h"


#ifndef STLALLOCATOR_CLEANUP
#define STLALLOCATOR_CLEANUP
#endif

//#ifdef NOT_USE_CRY_MEMORY_MANAGER
//#define CryModuleMalloc CRYMEMORYMANAGER_API malloc
//#define CryModuleFree CRYMEMORYMANAGER_API free
//#endif

#if !defined(PS3)
	#include <malloc.h>






#endif

#if defined(__cplusplus)
#if defined(PS3) || defined(LINUX)
#define NOT_STANDARD_CRT
	#include <new>
#else
	#include <new.h>
#endif
#endif

#ifndef _LIB
	#ifdef CRYSYSTEM_EXPORTS
		#define CRYMEMORYMANAGER_API DLL_EXPORT
	#else
		#define CRYMEMORYMANAGER_API DLL_IMPORT
	#endif
#else //_LIB
	#define CRYMEMORYMANAGER_API
#endif //_LIB



#ifdef __cplusplus

//////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_MEMORY_MANAGER
	#ifdef _DEBUG
		#define _DEBUG_MODE
	#endif
	// define _DEBUG even if in release mode.
	#define _DEBUG
#endif //DEBUG_MEMORY_MANAGER

#if defined(_DEBUG) && !defined(PS3) && !defined(LINUX)
	#include <crtdbg.h>
#endif //_DEBUG

// checks if the heap is valid in debug; in release, this function shouldn't be called
// returns non-0 if it's valid and 0 if not valid
ILINE int IsHeapValid()
{
#if (defined(_DEBUG) && !defined(RELEASE_RUNTIME) && !defined(PS3)) || (defined(DEBUG_MEMORY_MANAGER))
	return _CrtCheckMemory();
#else
	return true;
#endif
}

#ifdef DEBUG_MEMORY_MANAGER
// Restore debug mode define
	#ifndef _DEBUG_MODE
		#undef _DEBUG
	#endif //_DEBUG_MODE
#endif //DEBUG_MEMORY_MANAGER
//////////////////////////////////////////////////////////////////////////

#endif //__cplusplus

// Structure filled by call to CryModuleGetMemoryInfo()
struct CryModuleMemoryInfo
{
	uint64 requested;
	// Total amount of memory allocated
	uint64 allocated;
	// Total amount of memory freed
	uint64 freed;
	// Total number of memory allocations
	int num_allocations;
	// Allocated in CryString
	uint64 CryString_allocated;
	// Allocated in STL
	uint64 STL_allocated;
	// Amount of memory wasted in pools in stl (not usefully allocations)
	uint64 STL_wasted;
};

#if defined(LINUX) && 0 // XXX ?!
#define CryModuleMalloc malloc
#define CryModuleCalloc calloc
#define CryModuleRealloc realloc
#define CryModuleFree free
#ifdef __cplusplus
	inline void * __cdecl operator new   (size_t  size) throw(std::bad_alloc) { return CryModuleMalloc(size); }
	inline void * __cdecl operator new (size_t size, const std::nothrow_t &nothrow) { CryModuleMalloc(size); };
	inline void * __cdecl operator new[](size_t size) throw(std::bad_alloc) { return CryModuleMalloc(size); }; 
	inline void * __cdecl operator new[](size_t size, const std::nothrow_t &nothrow) { return CryModuleMalloc(size); }
	inline void __cdecl operator delete  (void *p) { CryModuleFree(p); };
	inline void __cdecl operator delete[](void *p) { CryModuleFree(p); };
#endif __cplusplus //__cplusplus
#endif //LNIUX


//////////////////////////////////////////////////////////////////////////
// Extern declarations of globals inside CrySystem.
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus 
extern "C" {
#endif //__cplusplus
#ifndef __MEMORY_VALIDATOR_workaround
CRYMEMORYMANAGER_API void *CryMalloc(size_t size, size_t& allocated);
CRYMEMORYMANAGER_API void *CryRealloc(void *memblock,size_t size, size_t& allocated);
CRYMEMORYMANAGER_API size_t CryFree(void *p);
CRYMEMORYMANAGER_API size_t CryGetMemSize(void *p, size_t size);
CRYMEMORYMANAGER_API int  CryStats(char *buf);
CRYMEMORYMANAGER_API void CryFlushAll();
CRYMEMORYMANAGER_API void CryCleanup();
CRYMEMORYMANAGER_API int  CryGetUsedHeapSize();
CRYMEMORYMANAGER_API int  CryGetWastedHeapSize();
CRYMEMORYMANAGER_API void *CrySystemCrtMalloc(size_t size);
CRYMEMORYMANAGER_API void CrySystemCrtFree(void *p);

// This function is local in every module
/*CRYMEMORYMANAGER_API*/ void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo);
#else
CRYMEMORYMANAGER_API void *CryMalloc(size_t size);
CRYMEMORYMANAGER_API void *CryRealloc(void *memblock,size_t size);
CRYMEMORYMANAGER_API size_t CryFree(void *p);
CRYMEMORYMANAGER_API size_t CryGetMemSize(void *p, size_t size);
CRYMEMORYMANAGER_API int  CryStats(char *buf);
CRYMEMORYMANAGER_API void CryFlushAll();
CRYMEMORYMANAGER_API void CryCleanup();
CRYMEMORYMANAGER_API int  CryGetUsedHeapSize();
CRYMEMORYMANAGER_API int  CryGetWastedHeapSize();
CRYMEMORYMANAGER_API void *CrySystemCrtMalloc(size_t size);
CRYMEMORYMANAGER_API void CrySystemCrtFree(void *p);

// This function is local in every module
/*CRYMEMORYMANAGER_API*/ void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo);
#endif
#ifdef __cplusplus
}
#endif //__cplusplus
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Cry Memory Manager accessible in all build modes.
//////////////////////////////////////////////////////////////////////////
#if !defined(USING_CRY_MEMORY_MANAGER)
	#define USING_CRY_MEMORY_MANAGER
#endif

#ifndef _LIB
#define CRY_MEM_USAGE_API extern "C" DLL_EXPORT
#else //_USRDLL
#define CRY_MEM_USAGE_API
#endif //_USRDLL

/////////////////////////////////////////////////////////////////////////
// Extern declarations,used by overrided new and delete operators.
//////////////////////////////////////////////////////////////////////////
extern "C"
{
#ifndef _LIB
	void* CryModuleMalloc(size_t size) throw();
	void* CryModuleCalloc(size_t num,size_t size) throw();
	void* CryModuleRealloc(void *memblock,size_t size) throw();
	void  CryModuleFree(void *ptr) throw();
#else
  void* CryModuleMalloc(size_t size, ECryModule eCM=eCryModule) throw();
  void* CryModuleCalloc(size_t num,size_t size, ECryModule eCM=eCryModule) throw();
  void* CryModuleRealloc(void *memblock,size_t size, ECryModule eCM=eCryModule) throw();
  void  CryModuleFree(void *ptr, ECryModule eCM=eCryModule) throw();

  ILINE void* _LibCryModuleMalloc(size_t size) throw() { return CryModuleMalloc(size); }
  ILINE void* _LibCryModuleCalloc(size_t num,size_t size) throw() { return CryModuleCalloc(num, size); }
  ILINE void* _LibCryModuleRealloc(void *memblock,size_t size) throw() { return CryModuleRealloc(memblock, size); }
  ILINE void  _LibCryModuleFree(void *ptr) throw() { CryModuleFree(ptr); }
#endif
}

#ifdef _LIB
	CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo, ECryModule eCM );
	#if (!defined(_DEBUG) || defined(PS3)) && !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)
		ILINE void * __cdecl operator new   (size_t size) throw (std::bad_alloc) { return CryModuleMalloc(size, eCryModule); }
		ILINE void * __cdecl operator new   (size_t size, const std::nothrow_t &nothrow) throw() { return CryModuleMalloc(size, eCryModule); }
		ILINE void * __cdecl operator new[] (size_t size) throw (std::bad_alloc) { return CryModuleMalloc(size, eCryModule); }
		ILINE void * __cdecl operator new[] (size_t size, const std::nothrow_t &nothrow) throw() { return CryModuleMalloc(size, eCryModule); }
		ILINE void __cdecl operator delete  (void *p) throw (){ CryModuleFree(p, eCryModule); };
		ILINE void __cdecl operator delete[](void *p) throw (){ CryModuleFree(p, eCryModule); };
		ILINE void __cdecl operator delete  (void *p, const std::nothrow_t &nothrow) throw (){ CryModuleFree(p, eCryModule); };
		ILINE void __cdecl operator delete[](void *p, const std::nothrow_t &nothrow) throw (){ CryModuleFree(p, eCryModule); };
	#endif
#else
	CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo );
#endif


//////////////////////////////////////////////////////////////////////////

// Redirect standard memory routines to CryModule functions in this module,
// unless _DEBUG or NOT_USE_CRY_MEMORY_MANAGER

#if defined(__SPU__) || ((!defined(_DEBUG) || defined(PS3)) && !defined(NOT_USE_CRY_MEMORY_MANAGER))
	#undef malloc
	#undef calloc
	#undef realloc
	#undef free
	#undef memalign
# ifndef __CRYCG__
# ifndef _LIB
	#define malloc        CryModuleMalloc
	#define calloc        CryModuleCalloc
	#define realloc       CryModuleRealloc
	#define free          CryModuleFree
# else
  #define malloc        _LibCryModuleMalloc
  #define calloc        _LibCryModuleCalloc
  #define realloc       _LibCryModuleRealloc
  #define free          _LibCryModuleFree
# endif
# endif
#endif

// useful debug macro
// we use memset otherwise windows will no allocate physical memory
// we free last one so biggest block left is n MB
#define MEMORY_DEBUG_POINT(pSystem,no)	\
	{ assert(no);	static ICVar *pVar = pSystem->GetIConsole()->GetCVar("sys_memory_debug");	assert(pVar);\
		if(pVar && pVar->GetIVal()==no)	{\
			void *pMem=0; uint32 dwSumMB=0,dwSizeMB=10;		\
			while(pMem=malloc(dwSizeMB*1024*1024))\
			{ dwSumMB+=dwSizeMB;memset(pMem,no,dwSizeMB*1024*1024);} \
			if(pMem)free(pMem);\
			pSystem->GetILog()->Log("MEMORY_DEBUG_POINT %d activated, %d MB",no,dwSumMB); } }




// Need for our allocator to avoid deadlock in cleanup
/*CRYMEMORYMANAGER_API */void *CryCrtMalloc(size_t size);
/*CRYMEMORYMANAGER_API */void CryCrtFree(void *p);

#if !defined( NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)// && !defined(_STLP_BEGIN_NAMESPACE) // Avoid non STLport version
#include "CryMemoryAllocator.h"
#endif

//////////////////////////////////////////////////////////////////////////

// Single function which handles all alloc/dealloc cases.
typedef void* (*TPFAlloc)( void* ptr, size_t size );

// Standard Alloc, using whichever malloc is defined in module.
ILINE void* ModuleAlloc( void* ptr, size_t size )
{
	if (size)
		return ptr ? realloc(ptr, size) : malloc(size);
	if (ptr)
		free(ptr);
	return 0;
}

// Cross-module safe allocation, stores pointer to allocating function in header.
void* SafeAlloc( TPFAlloc allocator, void *ptr, size_t size );

#endif // __CryMemoryManager_h__
