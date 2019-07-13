////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   CryMemoryManager_impl.h
//  Version:     v1.00
//  Created:     27/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Provides implementation for CryMemoryManager globally defined functions.
//               This file included only by platform_impl.h, do not include it directly in code!
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
 
#ifndef __CryMemoryManager_impl_h__
#define __CryMemoryManager_impl_h__
#pragma once

#ifdef _LIB
	#include "ISystem.h"
#endif


























const int CM_MaxMemSize[eCryM_Num] = 
{
  0
};


// In debug builds, pass the standard allocator routine in to override alloc behavior.
#if defined(_DEBUG) && !defined(PS3)
	// Standard lib alloc routine (used in debug builds).
	#undef malloc
	#undef calloc
	#undef realloc
	#undef free

	void* StdAlloc(void* pOrig, size_t nNew)
	{
		if (!pOrig)
			return malloc(nNew);
		else if (nNew)
			return realloc(pOrig, nNew);
		else
		{
			free(pOrig);
			return NULL;
		}
	}
	static TPFAlloc CustomAlloc = StdAlloc;


#endif

#define DEBUG_ALLOC_FLAG	0x80000000

#ifndef XENON
#define DLL_ENTRY_CRYMALLOC "CryMalloc"
#define DLL_ENTRY_CRYFREE "CryFree"
#define DLL_ENTRY_CRYREALLOC "CryRealloc"
#define DLL_ENTRY_CRYGETMEMSIZE "CryGetMemSize"
#define DLL_ENTRY_CRYCRTMALLOC "CrySystemCrtMalloc"
#define DLL_ENTRY_CRYCRTFREE "CrySystemCrtFree"








#endif

#ifdef _LIB
  const char *CM_Name[eCryM_Num] = 
  {
    "Cry3DEngine",
    "CryAction",
    "CryAISystem",
    "CryAnimation",
    "CryEntitySystem",
    "CryFont",
    "CryInput",
    "CryMovie",
    "CryNetwork",
    "CryPhysics",
    "CryScriptSystem",
    "CrySoundSystem",
    "CrySystem",
    "CryGame",
    "CryRender",
    "Launcher",
  };
#endif









































//////////////////////////////////////////////////////////////////////////
// _PoolHelper definition.
//////////////////////////////////////////////////////////////////////////
struct _CryMemoryManagerPoolHelper
{

  typedef void *(*FNC_CryMalloc)(size_t size, size_t& allocated);
	typedef void *(*FNC_CryCrtMalloc)(size_t size);
  typedef void *(*FNC_CryRealloc)(void *memblock, size_t size, size_t& allocated);
  typedef size_t (*FNC_CryFree)(void *p);

#ifndef _LIB
  static uint64 allocatedMemory;
  static uint64 freedMemory;
  static uint64 requestedMemory;
  static int numAllocations;
#else
  static uint64 allocatedMemory[eCryM_Num];
  static uint64 freedMemory[eCryM_Num];
  static uint64 requestedMemory[eCryM_Num];
  static int numAllocations[eCryM_Num];
#endif

//	typedef size_t (*FNC_CryFree)(void);
	typedef size_t (*FNC_CryGetMemSize)(void *p, size_t);
	typedef int (*FNC_CryStats)(char *buf);
	
	static FNC_CryMalloc _CryMalloc;
	static FNC_CryRealloc _CryRealloc;
	static FNC_CryFree _CryFree;
	static FNC_CryGetMemSize _CryGetMemSize;
	static FNC_CryCrtMalloc _CryCrtMalloc;
	static FNC_CryFree _CryCrtFree;


	static int m_bInitialized;







	static void Init()
	{
		m_bInitialized = 1;
		
#ifndef _LIB
    allocatedMemory = 0;
    freedMemory = 0;
    requestedMemory = 0;
    numAllocations = 0;

#if defined(LINUX)
		_CryMalloc=CryMalloc;
		_CryRealloc=CryRealloc;
		_CryFree=CryFree;
		_CryGetMemSize=CryGetMemSize;
		_CryCrtMalloc=CrySystemCrtMalloc;
		_CryCrtFree=(FNC_CryFree)CrySystemCrtFree;







#else
		HMODULE hMod;
		int iter;
		for(iter=0,hMod=GetModuleHandle(0); hMod; iter++)
		{
			_CryMalloc=(FNC_CryMalloc)GetProcAddress((HINSTANCE)hMod,DLL_ENTRY_CRYMALLOC);
			_CryRealloc=(FNC_CryRealloc)GetProcAddress((HINSTANCE)hMod,DLL_ENTRY_CRYREALLOC);
			_CryFree=(FNC_CryFree)GetProcAddress((HINSTANCE)hMod,DLL_ENTRY_CRYFREE);
			_CryGetMemSize=(FNC_CryGetMemSize)GetProcAddress((HINSTANCE)hMod,DLL_ENTRY_CRYGETMEMSIZE);
			_CryCrtMalloc=(FNC_CryCrtMalloc)GetProcAddress((HINSTANCE)hMod,DLL_ENTRY_CRYCRTMALLOC);
			_CryCrtFree=(FNC_CryFree)GetProcAddress((HINSTANCE)hMod,DLL_ENTRY_CRYCRTFREE);
			if ((_CryMalloc && _CryRealloc && _CryFree && _CryGetMemSize && _CryCrtMalloc && _CryCrtFree) || iter==1)
				break;
      hMod = ::LoadLibrary("CrySystem.dll"); 
		}
		if (!hMod || !_CryMalloc || !_CryRealloc || !_CryFree || !_CryGetMemSize ||! _CryCrtMalloc || !_CryCrtFree)
		{
#ifndef AVOID_MEMORY_ERROR
			#ifdef WIN32
				MessageBox(NULL, "Could not access CrySystem.dll (check working directory)", "Memory Manager", MB_OK);
      #else
      if (!hMod)
        OutputDebugString("Could not access CrySystem.dll (check working directory)");
      else
        OutputDebugString("Could not get Memory Functions in CrySystem.dll");
			#endif
			exit(1);
#endif
		};
#endif // LINUX || PS3
#else
    for (int i=0; i<eCryM_Num; i++)
    {
      allocatedMemory[i] = 0;
      freedMemory[i] = 0;
      requestedMemory[i] = 0;
      numAllocations[i] = 0;
    }








		_CryMalloc=CryMalloc;
		_CryRealloc=CryRealloc;
		_CryFree=CryFree;
		_CryGetMemSize=CryGetMemSize;
		_CryCrtMalloc=CrySystemCrtMalloc;
		_CryCrtFree=(FNC_CryFree)CrySystemCrtFree;

#endif
	}
 
#ifndef _LIB
	static void GetMemoryInfo( CryModuleMemoryInfo *pMmemInfo )
	{
		pMmemInfo->allocated = allocatedMemory;
		pMmemInfo->freed = freedMemory;
		pMmemInfo->requested = requestedMemory;
		pMmemInfo->num_allocations = numAllocations;
#ifdef CRY_STRING
		pMmemInfo->CryString_allocated = string::_usedMemory(0) + wstring::_usedMemory(0);
#endif
		pMmemInfo->STL_allocated = 0;
		pMmemInfo->STL_wasted = 0;
#if defined(CRY_STL_ALLOC) && defined(STLPORT)
		std::vector<int> dummy;
		pMmemInfo->STL_allocated = dummy.get_allocator().get_heap_size();
		pMmemInfo->STL_wasted = dummy.get_allocator().get_wasted_in_allocation() + dummy.get_allocator().get_wasted_in_blocks();
#endif
	}
#else
  static void GetMemoryInfo( CryModuleMemoryInfo *pMmemInfo, ECryModule eCM )
  {
    pMmemInfo->allocated = allocatedMemory[eCM];
    pMmemInfo->freed = freedMemory[eCM];
    pMmemInfo->requested = requestedMemory[eCM];
    pMmemInfo->num_allocations = numAllocations[eCM];
  }
#endif

	//////////////////////////////////////////////////////////////////////////
	// Local version of allocations, does memory counting per module.
	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static __forceinline void*	Malloc(size_t size)
#else
  static __forceinline void*	Malloc(size_t size, ECryModule eCM)
#endif
	{
		if (!m_bInitialized)
			Init();
		size_t allocated;
#ifndef __MEMORY_VALIDATOR_workaround
		void * p = _CryMalloc( size, allocated );
# ifndef _LIB
		allocatedMemory += allocated;
# else
    allocatedMemory[eCM] += allocated;
# endif
#else
		void * p = _CryMalloc( size);
		allocatedMemory += _CryGetMemSize(p);
#endif

#ifndef _LIB
    requestedMemory += size;
		numAllocations++;
#else
    requestedMemory[eCM] += size;
    numAllocations[eCM]++;
    if (CM_MaxMemSize[eCM]>0 && requestedMemory[eCM]/(1024*1024) > CM_MaxMemSize[eCM])
    {
      CryError("Memory usage for module '%s' exceed limit (%d Mb)", CM_Name[eCM], CM_MaxMemSize[eCM]);
    }
#endif

		/*
		if (CustomAlloc || !_CryMalloc)
		{
			// Use and remember custom allocation routine.
			// Flag memory block by setting size negative.
			size_t* p = (size_t*)CustomAlloc(NULL, size + 2*sizeof(size_t));
			*p++ = (size_t)CustomAlloc;
			*p++ = size | DEBUG_ALLOC_FLAG;
			return p;
		}
		*/
		return p;
	}
	//////////////////////////////////////////////////////////////////////////
	// Local version of allocations, does memory counting per module.
	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static __forceinline void*	Calloc(size_t num,size_t size)
	{
		void *p = (char*)Malloc(num*size);
		memset(p, 0, num*size );
		return p;
	}
#else
  static __forceinline void*	Calloc(size_t num,size_t size, ECryModule eCM)
  {
    void *p = (char*)Malloc(num*size, eCM);
    memset(p, 0, num*size );
    return p;
  }
#endif

	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static __forceinline void*	Realloc(void *memblock,size_t size)
#else
  static __forceinline void*	Realloc(void *memblock,size_t size, ECryModule eCM)
#endif
	{
		if (!m_bInitialized)
			Init();
		if (memblock == NULL)
		{
			//numAllocations++;
			//allocatedMemory += size;
#ifndef _LIB
			return Malloc(size);
#else
      return Malloc(size, eCM);
#endif
		}

//		int* t = (int*)memblock;
		size_t oldsize = _CryGetMemSize(memblock, size);//*--t;	

		/*
		size_t* p = NULL;
		if (oldsize & DEBUG_ALLOC_FLAG)
		{
			// Allocated with custom allocator.
			TPFAlloc opa = (TPFAlloc)*--t;
			if (opa == CustomAlloc)
			{
				// Same allocator, use its realloc.
				p = (size_t*)CustomAlloc(t, size + 2*sizeof(size_t));
				*p++ = (size_t)CustomAlloc;
				*p++ = size | DEBUG_ALLOC_FLAG;
				return p;
			}
			else
			{
				// Separate malloc and free.
				p = (size_t*)Malloc(size);
				memcpy(p, memblock, oldsize);
				Free(memblock);
			}
		}
*/
		size_t allocated;
#ifndef ___MEMORY_VALIDATOR_workaround
		void * p=  _CryRealloc( memblock, size, allocated );
# ifndef _LIB
		allocatedMemory += allocated;
# else
    allocatedMemory[eCM] += allocated;
# endif
#else
		void * p=  _CryRealloc( memblock, size);
		allocatedMemory += _CryGetMemSize(p);
#endif

#ifndef _LIB
    numAllocations++;
    requestedMemory += size;
    freedMemory += oldsize &= ~DEBUG_ALLOC_FLAG;
#else
    numAllocations[eCM]++;
    requestedMemory[eCM] += size;
    freedMemory[eCM] += oldsize &= ~DEBUG_ALLOC_FLAG;
    if (CM_MaxMemSize[eCM]>0 && requestedMemory[eCM]/(1024*1024) > CM_MaxMemSize[eCM])
    {
      CryError("Memory usage for module '%s' exceed limit (%d Mb)", CM_Name[eCM], CM_MaxMemSize[eCM]);
    }
#endif
/*
		if (p)
			return p;
		else
		*/
		return p;
			
	}
	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static __forceinline void Free( void *memblock )
#else
  static __forceinline void Free( void *memblock, ECryModule eCM )
#endif
	{
		if (!m_bInitialized)
			Init();
		if (memblock != 0)
		{
			/*
			if (size & DEBUG_ALLOC_FLAG)
			{
				// Free using custom allocator.
				size &= ~DEBUG_ALLOC_FLAG;
				TPFAlloc pa = (TPFAlloc)*--t;
				pa(t, 0);
			}
			else
			*/
#ifndef _LIB				
			freedMemory += _CryFree( memblock );
#else
      freedMemory[eCM] += _CryFree( memblock );
#endif
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// Static variables.
//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
uint64 _CryMemoryManagerPoolHelper::allocatedMemory = 0;
uint64 _CryMemoryManagerPoolHelper::freedMemory = 0;
uint64 _CryMemoryManagerPoolHelper::requestedMemory = 0;
int _CryMemoryManagerPoolHelper::numAllocations = 0;
#else
uint64 _CryMemoryManagerPoolHelper::allocatedMemory[eCryM_Num];
uint64 _CryMemoryManagerPoolHelper::freedMemory[eCryM_Num];
uint64 _CryMemoryManagerPoolHelper::requestedMemory[eCryM_Num];
int _CryMemoryManagerPoolHelper::numAllocations[eCryM_Num];
#endif
_CryMemoryManagerPoolHelper::FNC_CryMalloc _CryMemoryManagerPoolHelper::_CryMalloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryGetMemSize _CryMemoryManagerPoolHelper::_CryGetMemSize = NULL;
_CryMemoryManagerPoolHelper::FNC_CryRealloc _CryMemoryManagerPoolHelper::_CryRealloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryFree _CryMemoryManagerPoolHelper::_CryFree = NULL;
_CryMemoryManagerPoolHelper::FNC_CryCrtMalloc _CryMemoryManagerPoolHelper::_CryCrtMalloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryFree _CryMemoryManagerPoolHelper::_CryCrtFree = NULL;
int _CryMemoryManagerPoolHelper::m_bInitialized = 0;
//////////////////////////////////////////////////////////////////////////

#ifndef _LIB
//////////////////////////////////////////////////////////////////////////
void* CryModuleMalloc( size_t size ) throw()
{
	return _CryMemoryManagerPoolHelper::Malloc(size);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleCalloc( size_t num,size_t size ) throw()
{
	return _CryMemoryManagerPoolHelper::Calloc(num,size);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleRealloc( void *ptr,size_t size )  throw()
{
	return _CryMemoryManagerPoolHelper::Realloc(ptr,size);
};
//////////////////////////////////////////////////////////////////////////
void  CryModuleFree( void *ptr ) throw()
{
	_CryMemoryManagerPoolHelper::Free(ptr);
};
//////////////////////////////////////////////////////////////////////////
CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo )
{
	_CryMemoryManagerPoolHelper::GetMemoryInfo(pMemInfo);
};

void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo)
{
	_CryMemoryManagerPoolHelper::GetMemoryInfo(pInfo);
};



#else

//////////////////////////////////////////////////////////////////////////
void* CryModuleMalloc( size_t size, ECryModule eCM ) throw()
{
  return _CryMemoryManagerPoolHelper::Malloc(size, eCM);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleCalloc( size_t num,size_t size, ECryModule eCM ) throw()
{
  return _CryMemoryManagerPoolHelper::Calloc(num,size,eCM);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleRealloc( void *ptr,size_t size, ECryModule eCM )  throw()
{
  return _CryMemoryManagerPoolHelper::Realloc(ptr,size, eCM);
};
//////////////////////////////////////////////////////////////////////////
void  CryModuleFree( void *ptr, ECryModule eCM ) throw()
{
  _CryMemoryManagerPoolHelper::Free(ptr, eCM);
};
//////////////////////////////////////////////////////////////////////////
CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo, ECryModule eCM )
{
  _CryMemoryManagerPoolHelper::GetMemoryInfo(pMemInfo, eCM);
};
//////////////////////////////////////////////////////////////////////////
void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo)
{
	_CryMemoryManagerPoolHelper::GetMemoryInfo(pInfo, eCryModule);
};
#endif

void *CryCrtMalloc(size_t size)
{
#if defined(NOT_USE_CRY_MEMORY_MANAGER) || defined(_DEBUG)
	return malloc(size);
#else
	return _CryMemoryManagerPoolHelper::_CryCrtMalloc(size);
#endif
}

void CryCrtFree(void *p)
{
#if defined(NOT_USE_CRY_MEMORY_MANAGER) || defined(_DEBUG)
	free(p);
#else
	_CryMemoryManagerPoolHelper::_CryCrtFree(p);
#endif
};


// If using CryMemoryManager, redefine new & delete for entire module.
#if (!defined(_DEBUG) || defined(PS3)) && !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)
	#ifndef _LIB
		void * __cdecl operator new   (size_t size) { return CryModuleMalloc(size); } 
		void * __cdecl operator new[] (size_t size) { return CryModuleMalloc(size); }; 
		void __cdecl operator delete  (void *p) { CryModuleFree(p); };
		void __cdecl operator delete[](void *p) { CryModuleFree(p); };
	#endif//_LIB
#endif //!defined(_DEBUG) && !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)

//////////////////////////////////////////////////////////////////////////

void* SafeAlloc( TPFAlloc allocator, void *ptr, size_t size )
{
	if (ptr)
		// Use original allocator.
		allocator = *--((TPFAlloc*&)ptr);
	if (size)
		size += sizeof(TPFAlloc);
	ptr = (*allocator)(ptr, size);
	if (ptr)
		// Store allocator.
	 	*((TPFAlloc*&)ptr)++ = allocator;
	return ptr;
}

#endif // __CryMemoryManager_impl_h__

