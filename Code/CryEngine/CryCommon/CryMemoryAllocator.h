// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _CRY_MEMORY_ALLOCATOR
#define _CRY_MEMORY_ALLOCATOR
 
#pragma once
#include <algorithm>

//#if ( defined(WIN32) || defined(WIN64) ) && ( defined(_DEBUG) || defined(DEBUG) )
//#undef CRY_MEMORY_ALLOCATOR
//#define _CRTDBG_MAP_ALLOC
//#include "crtdbg.h"
//#else

#define CRY_STL_ALLOC

#if defined (__OS400__) || defined (_WIN64) || defined (XENON) || (defined(PS3) && !defined(NOT_USE_CRY_MEMORY_MANAGER))
enum {_ALIGN = 16, _ALIGN_SHIFT = 4, _MAX_BYTES = 512, NFREELISTS=32};
#else
//enum {_ALIGN = 8, _ALIGN_SHIFT = 3, _MAX_BYTES = 128};
///enum {_ALIGN = 8, _ALIGN_SHIFT = 3, _MAX_BYTES = 256, NFREELISTS = 32};
enum {_ALIGN = 8, _ALIGN_SHIFT = 3, _MAX_BYTES = 512, NFREELISTS = 64};
#endif /* __OS400__ */

#define CRY_MEMORY_ALLOCATOR

#define S_FREELIST_INDEX(__bytes) ((__bytes - size_t(1)) >> (int)_ALIGN_SHIFT)

class  _Node_alloc_obj {
public:
	_Node_alloc_obj * _M_next;
};

struct _Node_alloc_Mem_block_Huge {
	//Pointer to the end of the memory block
	char *_M_end;
	// number 
	int _M_count;
	//Pointer to the next memory block
	_Node_alloc_Mem_block_Huge *_M_next;
};

struct _Node_alloc_Mem_block {
	//Pointer to the end of the memory block
	char *_M_end;
	//Pointer to the next memory block
	_Node_alloc_Mem_block_Huge *_M_huge_block;
	_Node_alloc_Mem_block *_M_next;
};


// Allocators!
enum EAllocFreeType 
{
	eCryDefaultMalloc,
	eCryMallocCryFreeAll,
	eCryMallocCryFreeCRTCleanup
};

template <EAllocFreeType type>
struct Node_Allocator 
{
	inline void * pool_alloc(size_t size) 
	{
		return CryModuleMalloc(size);
	};
	inline void * cleanup_alloc(size_t size) 
	{
		return CryCrtMalloc(size);
	};
	inline void pool_free(void * ptr) 
	{
		CryModuleFree(ptr);
	};
	inline void cleanup_free(void * ptr)
	{
		CryCrtFree(ptr);
	};
};

// partial 
template <>
struct Node_Allocator<eCryMallocCryFreeCRTCleanup>
{
	inline void * pool_alloc(size_t size) 
	{
		return CryCrtMalloc(size);
	};
	inline void * cleanup_alloc(size_t size) 
	{
		return CryCrtMalloc(size);
	};
	inline void pool_free(void * ptr) 
	{
		CryCrtFree(ptr);
	};
	inline void cleanup_free(void * ptr)
	{
		CryCrtFree(ptr);
	};
};

template <>
struct Node_Allocator<eCryDefaultMalloc>
{
	inline void * pool_alloc(size_t size) 
	{
		return malloc(size);
	};
	inline void * cleanup_alloc(size_t size) 
	{
		return malloc(size);
	};
	inline void pool_free(void * ptr) 
	{
		free(ptr);
	};
	inline void cleanup_free(void * ptr)
	{
		free(ptr);
	};
};

#include "MultiThread.h"

template <bool __threads, int _Size>
class Node_Alloc_Lock {
public:
	Node_Alloc_Lock() : lock(g_lockMemMan) 
	{ 
	}

	~Node_Alloc_Lock() 
	{
	}

	inline static bool ATOMIC_CAS(long volatile *dst, long val, long old_val) 
	{
		return (*dst = val) == val;
	}

	inline static long ATOMIC_INCREMENT(long volatile * x) 
	{
		return ++(*x);
	}

	inline static long ATOMIC_DECREMENT(long volatile * x) 
	{
		return --(*x);
	}


	inline static long	ATOMIC_EXCHANGE_ADD(long volatile * x, long y) 
	{
		return *x += y;
	}

//#define ATOMIC_CAS(__dst, __val, __old_val) (CryInterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)/*(InterlockedCompareExchange64((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)*/
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           CryInterlockedIncrement((int volatile*)__x)
//#    define ATOMIC_DECREMENT(__x)           CryInterlockedDecrement((int volatile*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#endif/*_WIN64*/


	WriteLock lock;
	static volatile int g_lockMemMan;
};

template <bool __threads, int _Size>
volatile int Node_Alloc_Lock<__threads, _Size>::g_lockMemMan = 0;

template <int _Size>
class Node_Alloc_Lock<false,_Size> 
{
public:
	Node_Alloc_Lock() { }  
	~Node_Alloc_Lock() { }
	
	inline static bool ATOMIC_CAS(long volatile *dst, long val, long old_val) 
	{
		return CryInterlockedCompareExchange(dst, val, old_val) == old_val;
	}

	inline static long ATOMIC_INCREMENT(long volatile * x) 
	{
#  if defined(_WIN64) || defined(WIN64)
		return _InterlockedIncrement((long*)x);
#else
		return CryInterlockedIncrement((int volatile *)x);
#endif
	}

	inline static long ATOMIC_DECREMENT(long volatile * x) 
	{
#  if defined(_WIN64) || defined(WIN64)
		return _InterlockedDecrement((long*)x);
#else
		return CryInterlockedDecrement((int volatile *)x);
#endif
	}

	inline static long	ATOMIC_EXCHANGE_ADD(long volatile * x, long y) 
	{
		return CryInterlockedExchangeAdd(x, y);
	}

//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           (++(*__x))
//#    define ATOMIC_DECREMENT(__x)           (--(*__x))
//#    define ATOMIC_EXCHANGE(__x, __y)       (*__x = *__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   (*__x += __y)
//#	 endif
//#else /*_WIN64*/
};


#define _malloc ::malloc
#define _free ::free


template <EAllocFreeType _alloc, bool __threads, int _Size>
class node_alloc {
private:
	static inline size_t  _S_round_up(size_t __bytes) { return (((__bytes) + (size_t)_ALIGN-1) & ~((size_t)_ALIGN - 1)); }
	typedef _Node_alloc_obj _Obj;

private:
	typedef _Node_alloc_Mem_block _Mem_block;

	// Returns an object of size __n, and optionally adds to size __n free list.
	static _Obj*   _S_refill(size_t __n);
	// Allocates a chunk for nobjs of size __p_size.  nobjs may be reduced
	// if it is inconvenient to allocate the requested number.
	static char*   _S_chunk_alloc(size_t __p_size, int& __nobjs);
	// Chunk allocation state.


	static void *  _M_allocate(size_t __n);
	/* __p may not be 0 */
	static void  _M_deallocate(void *__p, size_t __n);

	static size_t _S_freelist_count(int num);
	static size_t _S_freelist_fill(int num, void ** _ptr);
	static size_t _S_freelist_count_inside(int num, _Obj* first, _Obj* last);
	static size_t _S_fill_from_freelist(int num, _Obj* first, _Obj* last, void **);
	static void _S_freelist_delete_inside(int num, _Obj* first, _Obj* last);
	static void _S_freememblocks_delete_inside(int num, void* first, void* last);
	static size_t _S_freememblocks_count_inside(int num, void* first, void* last);



#if defined (DO_CLEAN_NODE_ALLOC)
	//A helper class to guaranty the memory pool management:
	//friend struct _Node_alloc_helper;
	//Some compilers (MSVC) seems to have trouble with friend declarations:
public:
	// Methods to report alloc/dealloc calls to the counter system.
	static size_t&  _S_alloc_call(size_t incr = 1);
	static void  _S_dealloc_call();


private:
	// Free all the allocated chuncks of memory
	static void  _S_chunk_dealloc();
	// Beginning of the linked list of allocated chunks of memory
#endif /* DO_CLEAN_NODE_ALLOC */

	//static _Obj *_S_chunks;
	static _Node_alloc_Mem_block_Huge * volatile _S_chunks;
	static _Obj * volatile _S_free_list[NFREELISTS]; 
	static long volatile _S_freelist_counter[NFREELISTS]; 
	// Amount of total allocated memory
	static long volatile _S_heap_size;
	static long volatile _S_wasted_in_allocation;
	static long volatile _S_wasted_in_blocks;

	static _Mem_block* volatile _S_free_mem_blocks;
	static _Node_alloc_Mem_block_Huge * volatile __pCurrentHugeBlock;


public:
	// this one is needed for proper simple_alloc wrapping
	static long volatile _S_allocations;
	typedef char value_type;
	/* __n must be > 0      */
	static void *  allocate(size_t __n) 
	{ 
		if (__n > (size_t)_MAX_BYTES) 
		{
			Node_Allocator<_alloc> all;
			return all.pool_alloc(__n);
		}
		else
			return _M_allocate(__n); 
	}

	static void *  alloc(size_t __n) 
	{ 
		return allocate(__n);
	}

	/* __p may not be 0 */
	static void  deallocate(void *__p, size_t __n) 
	{
		if (__n > (size_t)_MAX_BYTES) 
		{
			Node_Allocator<_alloc> all;
			all.pool_free(__p);
		}
		else 
		_M_deallocate(__p, __n); 
	}
	static void  dealloc(void *__p, size_t __n) 
	{
		deallocate(__p, __n);
	}

	/* __n must be > 0      */
//	static void *  alloc(size_t __n) { return (__n > (size_t)_MAX_BYTES) ?  Node_Allocator<_alloc>.pool_alloc(__n) : _M_allocate(__n); }
	/* __p may not be 0 */
	//static void  dealloc(void *__p, size_t __n) { if (__n > (size_t)_MAX_BYTES) Node_Allocator<_alloc>.pool_free(__p); else _M_deallocate(__p, __n); }

	static size_t get_heap_size() { return _S_heap_size; };
	static size_t get_wasted_in_allocation() { return _S_wasted_in_allocation; };
	static size_t get_wasted_in_blocks() { 	return _S_wasted_in_blocks; };

	static void cleanup();
};


//#  if defined(_WIN64) || defined(WIN64)
//#define ATOMIC_CAS(__dst, __val, __old_val) (_InterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           _InterlockedIncrement((long*)__x)
//#    define ATOMIC_DECREMENT(__x)           _InterlockedDecrement((long*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       _InterlockedExchange((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#else /*_WIN32*/
//#if defined(_WIN32) || defined(WIN32) || defined(XENON)
//#define ATOMIC_CAS(__dst, __val, __old_val) (InterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           InterlockedIncrement((long*)__x)
//#    define ATOMIC_DECREMENT(__x)           InterlockedDecrement((long*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       InterlockedExchange((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#define ATOMIC_CAS(__dst, __val, __old_val)  ((*__dst = __val) == __val)
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           (++(*__x))
//#    define ATOMIC_DECREMENT(__x)           (--(*__x))
//#    define ATOMIC_EXCHANGE(__x, __y)       (*__x = *__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   (*__x += __y)
//#	 endif
//#else /*_WIN64*/
//#define ATOMIC_CAS(__dst, __val, __old_val) (CryInterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)/*(InterlockedCompareExchange64((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)*/
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           CryInterlockedIncrement((int volatile*)__x)
//#    define ATOMIC_DECREMENT(__x)           CryInterlockedDecrement((int volatile*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#endif/*_WIN64*/
//#endif/*_WIN32*/


template <class _ListElem, bool __threads, int __size>
inline void __cas_new_head (_ListElem * volatile *__free_list, _ListElem *__new_head) {
	_ListElem *__old_head;
	do {
		__old_head = *__free_list;
		__new_head->_M_next = __old_head;
	} while (!Node_Alloc_Lock<__threads, __size>::ATOMIC_CAS((long volatile*)__free_list, (long)__new_head, (long)__old_head));
}


template <EAllocFreeType _alloc, bool __threads, int _Size>
void* node_alloc<_alloc, __threads, _Size>::_M_allocate(size_t __n) 
{
	Node_Alloc_Lock<__threads, _Size> lock;

	_Obj *__r;
	long allocated = S_FREELIST_INDEX(__n);
	_Obj * volatile * __my_free_list = _S_free_list + allocated;

	do {
		__r = *__my_free_list;
		if (__r  == 0) {
			__r = _S_refill(__n);
			break;
		}
	}
	while (!Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS((long volatile*)__my_free_list, (long)__r->_M_next, (long)__r));

	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD(&_S_wasted_in_allocation,((allocated + 1) << _ALIGN_SHIFT) - __n);
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD(&_S_wasted_in_blocks,-((allocated + 1) << _ALIGN_SHIFT));
	long volatile * counter = _S_freelist_counter + S_FREELIST_INDEX(__n);
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_DECREMENT(counter);

	//_S_wasted_in_allocation += allocated - __n;
	//_S_wasted_in_blocks -= allocated;
	//--_S_freelist_counter[S_FREELIST_INDEX(__n)];

	return __r;
}

#ifndef __STATIC_CAST
#define __STATIC_CAST(x,y) static_cast<x>(y)
#endif

#ifndef __REINTERPRET_CAST
#define __REINTERPRET_CAST(x,y) reinterpret_cast<x>(y)
#endif


template <EAllocFreeType _alloc, bool __threads, int _Size>
void node_alloc<_alloc, __threads, _Size>::_M_deallocate(void *__p, size_t __n) 
{
	Node_Alloc_Lock<__threads, _Size> lock;

	long allocated = S_FREELIST_INDEX(__n);
	_Obj * volatile * __my_free_list = _S_free_list + allocated;
	_Obj *__pobj = __STATIC_CAST(_Obj*, __p);
	__cas_new_head<_Obj, __threads, _Size>(__my_free_list, __pobj);

	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD(&_S_wasted_in_allocation,-((allocated + 1) << _ALIGN_SHIFT) + __n);

	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD(&_S_wasted_in_blocks,((allocated + 1) << _ALIGN_SHIFT));
	long volatile * counter = _S_freelist_counter + S_FREELIST_INDEX(__n);
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_INCREMENT(counter);

	//_S_wasted_in_allocation -= allocated - __n;
	//_S_wasted_in_blocks += allocated;
	//++_S_freelist_counter[S_FREELIST_INDEX(__n)];
}

/* Returns an object of size __n, and optionally adds to size __n free list.*/
/* We assume that __n is properly aligned.                                  */
/* We hold the allocation lock.                                             */
//template <bool __threads, int __inst>

//#define NUM_OBJ 20
#define NUM_OBJ 20

template <EAllocFreeType _alloc, bool __threads, int _Size>
_Node_alloc_obj* node_alloc<_alloc,__threads, _Size>::_S_refill(size_t __n) {
	int __nobjs = NUM_OBJ;
	__n = _S_round_up(__n);
	char* __chunk = _S_chunk_alloc(__n, __nobjs);

	if (1 == __nobjs) 
	{
		//++_S_freelist_counter[S_FREELIST_INDEX(__n)];
		long volatile* counter = _S_freelist_counter + S_FREELIST_INDEX(__n);
		Node_Alloc_Lock<__threads, _Size>::ATOMIC_INCREMENT(counter);
		__pCurrentHugeBlock->_M_count += 1;
		return (_Obj*)__chunk;
	}

	_Obj* volatile* __my_free_list = _S_free_list + S_FREELIST_INDEX(__n);
	_Obj* __result;
	_Obj* __current_obj;
	_Obj* __new_head;
	_Obj* __next_obj;

	/* Build free list in chunk */

//	_S_freelist_counter[S_FREELIST_INDEX(__n)] += __nobjs;
	long volatile * counter = _S_freelist_counter +S_FREELIST_INDEX(__n);
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD(counter,__nobjs);

	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD((long volatile*)&__pCurrentHugeBlock->_M_count, __nobjs);
	//__pCurrentHugeBlock->_M_count += __nobjs;
	__result = (_Obj*)__chunk;
	__new_head = __next_obj = (_Obj*)(__chunk + __n);
	for (--__nobjs; --__nobjs; ) {
		__current_obj = __next_obj;
		__next_obj = (_Obj*)((char*)__next_obj + __n);
		__current_obj->_M_next = __next_obj;

	}


	//Link the new free list to the global free list
	do {
		__next_obj->_M_next = *__my_free_list;
		// !!!!

	} while (!Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS((long volatile*)__my_free_list, (long)__new_head, (long)__next_obj->_M_next));

	return __result;
}

/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */

template <EAllocFreeType _alloc, bool __threads, int _Size>
char* node_alloc<_alloc,__threads, _Size>::_S_chunk_alloc(size_t _p_size,
																									 int& __nobjs) 
{
	char* __result = 0;
	size_t __total_bytes = _p_size * __nobjs;

	Node_Allocator<_alloc> allocator;
	//We get the first available free block:
	_Mem_block *__pblock;
	do {
		__pblock = _S_free_mem_blocks;
		if (__pblock == 0) break;
	}
	while (!Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS((long volatile*)&_S_free_mem_blocks, (long)__pblock->_M_next, (long)__pblock));

	if (__pblock != 0) 
	{
		size_t __bytes_left = __pblock->_M_end - (char*)__pblock;
		char *__new_buf_pos = (char*)__pblock;

		if ((__bytes_left < __total_bytes) && (__bytes_left >= _p_size)) 
		{
			//We won't get all the requested objs:
			__nobjs = (int)(__bytes_left/_p_size);
			__total_bytes = _p_size * __nobjs;
		}

		if (__bytes_left >= __total_bytes) 
		{
			__result = (char*)__pblock;
			__bytes_left -= __total_bytes;
			__new_buf_pos += __total_bytes;
		}

		//We get rid of the leaving part of the mem block:
		if (__bytes_left != 0) 
		{
			if ((__result != 0) && (__bytes_left > sizeof(_Mem_block))) 
			{
				//We use part of the block and there is enough mem left to put the block back to 
				//the free mem blocks:
				_Mem_block *__pnew_block = (_Mem_block*)(__new_buf_pos);
				__pnew_block->_M_end = __pblock->_M_end;
				__cas_new_head<_Node_alloc_Mem_block, __threads, _Size>(&_S_free_mem_blocks, __pnew_block);
				//__pCurrentHugeBlock->_M_count += __nobjs;
			}
			else 
			{
				//A too small block, we put it in the main free list elements:
				/*

				_Obj* volatile* __my_free_list = _S_free_list + S_FREELIST_INDEX(__bytes_left);
				_Obj* __pobj = __REINTERPRET_CAST(_Obj*, __new_buf_pos);
				++_S_freelist_counter[S_FREELIST_INDEX(__bytes_left)];
				__cas_new_head(__my_free_list, __pobj);
				if (__result == 0)
				__pCurrentHugeBlock->_M_count +=  1;

				*/
			}
		}

		if (__result != 0) 
		{
			return __result;
		}
	}

	//#define _ALLOCATION_SIZE 4 * 1024 * 1024
#define _ALLOCATION_SIZE 512 * 1024
	//We haven't found a free block or not enough place, we ask memory to the the system:
	size_t __bytes_to_get =  _Size;//_ALLOCATION_SIZE;////
	__result = (char*)allocator.pool_alloc(__bytes_to_get);//(char*)_malloc(__bytes_to_get);
	//++_S_allocations;
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_INCREMENT(&_S_allocations);
	//_S_wasted_in_blocks += __bytes_to_get;
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_EXCHANGE_ADD(&_S_wasted_in_blocks,__bytes_to_get);
	//this is gonna be optimized away in profile builds
	if (0 == __result) 
	{
		// BOOOOOOM!



		__result= 0;

	}

	{
		size_t newHeapSize, oldHeapSize;
		do {
			oldHeapSize = _S_heap_size;
			newHeapSize = oldHeapSize + __bytes_to_get;
		} while (!Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS(&_S_heap_size, newHeapSize, oldHeapSize));
	}

	//#  ifdef DO_CLEAN_NODE_ALLOC
	_Node_alloc_Mem_block_Huge *__pBlock = (_Node_alloc_Mem_block_Huge*)(__result);// + __total_bytes);
	__bytes_to_get -= sizeof(_Node_alloc_Mem_block_Huge);
	__pBlock->_M_end = __result + __bytes_to_get;
	__pBlock->_M_count = 0;
	__cas_new_head<_Node_alloc_Mem_block_Huge, __threads, _Size>(&_S_chunks, __pBlock);
	__result += sizeof(_Node_alloc_Mem_block_Huge);
	//__pCurrentHugeBlock = __pBlock;

	_Node_alloc_Mem_block_Huge * volatile  __old_block;
	do {
		__old_block = __pCurrentHugeBlock;
	} while (!Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS((long volatile *)&__pCurrentHugeBlock, (long)__pBlock, (long)__old_block));

	//__cas_new_head(__pCurrentHugeBlock)

	//#  endif
	_Mem_block *__pnewBlock = (_Mem_block*)(__result + __total_bytes);
	__pnewBlock->_M_end = __result + __bytes_to_get;
	__cas_new_head<_Node_alloc_Mem_block, __threads, _Size>(&_S_free_mem_blocks, __pnewBlock);
	return __result;
}

#  if defined (DO_CLEAN_NODE_ALLOC)
//template <bool __threads, int __inst>
template <EAllocFreeType _alloc, bool __threads, int _Size>
size_t& node_alloc<_alloc,__threads, _Size>::_S_alloc_call(size_t incr) {
	static size_t _S_counter = 0;
	if (incr != 0) {
		Node_Alloc_Lock<__threads, _Size>::ATOMIC_INCREMENT(&_S_counter);
	}
	return _S_counter;
}

template <EAllocFreeType _alloc, bool __threads, int _Size>
void node_alloc<_alloc,__threads, _Size>::_S_dealloc_call() 
{

	size_t *pcounter = &_S_alloc_call(0);
	Node_Alloc_Lock<__threads, _Size>::ATOMIC_DECREMENT(pcounter);
	//As we are only releasing memory on shared library unload, counter
	//can only reach 0 once threads has been stopped so we do not have to
	//check atomic_decrement result.
	if (*pcounter == 0) {
		_S_chunk_dealloc();
	}
}
#  endif /* DO_CLEAN_NODE_ALLOC */

/*#endif / * _STLP_USE_LOCK_FREE_IMPLEMENTATION * /*/
template <EAllocFreeType _alloc, bool __threads, int _Size>
size_t node_alloc<_alloc,__threads, _Size>::_S_freelist_count(int num)
{

	_Obj* volatile* __pList = _S_free_list + num;
	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
	size_t count(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		__pcur = __pnext;
		++count;
	}
	return count;
}

template <EAllocFreeType _alloc, bool __threads, int _Size>
size_t node_alloc<_alloc,__threads, _Size>::_S_freelist_fill(int num, void ** _ptr)
{

	_Obj* volatile* __pList = _S_free_list + num;
	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
	size_t count(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		_ptr[count] = __pcur;
		__pcur = __pnext;
		++count;
	}
	return count;
}

template <EAllocFreeType _alloc, bool __threads, int _Size>
size_t node_alloc<_alloc,__threads, _Size>::_S_freelist_count_inside(int num, _Obj* first, _Obj* last)
{

	_Obj* volatile* __pList = _S_free_list + num;
	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
	size_t count(0);
	size_t totalcount(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		if (__pcur >= first && __pcur <= last)
			++count;
		__pcur = __pnext;
		++totalcount;
	}
	return count;
}

template <EAllocFreeType _alloc, bool __threads, int _Size>
size_t node_alloc<_alloc,__threads, _Size>::_S_fill_from_freelist(int num, _Obj* first, _Obj* last, void ** _ptr)
{

	_Obj* volatile* __pList = _S_free_list + num;
	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
	size_t count(0);
	//size_t totalcount(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		if (__pcur >= first && __pcur <= last)
		{
			_ptr[count++]	= __pcur;
		}
		//			++count;
		__pcur = __pnext;
		//	++totalcount;
	}
	return count;
}

template <EAllocFreeType _alloc, bool __threads, int _Size>
void node_alloc<_alloc,__threads, _Size>::_S_freelist_delete_inside(int num, _Obj* first, _Obj* last)
{

	_Obj* volatile* __pList = _S_free_list + num;
	_Obj *__pnext, *__pfirst, *__plast,* __r;

	do {
		__r = *__pList;
		if (__r < first || __r > last)
		{
			break;
		}
	}
	while (Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS(__pList, __r->_M_next, __r));


	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList);
	__pfirst = __pcur;

	size_t count(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		if (__pcur >= first && __pcur <= last)
		{
			//if (__pnext)
			{
				__cas_new_head<_Obj, __threads, _Size>(&__pnext, __pfirst);
				++count;
			}
		}
		else
		{
			__pfirst = __pcur;
		}
		__pcur = __pnext;
	}

	__pcur = __REINTERPRET_CAST(_Obj*, *__pList);

	// for testing purposes
	/*
	#ifdef _DEBUG
	count = 0;
	while (__pcur != 0) {
	__pnext = __pcur->_M_next;
	if (__pcur >= first && __pcur < last)
	{
	++count;
	}
	__pcur = __pnext;
	}
	#endif
	*/

}

template <EAllocFreeType _alloc, bool __threads, int _Size>
size_t node_alloc<_alloc,__threads, _Size>::_S_freememblocks_count_inside(int num, void* first, void* last)
{

	_Mem_block* volatile* __pList = &_S_free_mem_blocks;
	_Mem_block* __pcur = __REINTERPRET_CAST(_Mem_block*, *__pList), *__pnext;
	size_t count(0);
	size_t totalcount(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		if ((void*)__pcur >= first && (void*)__pcur <= last)
			count += (char*)__pcur->_M_end - (char*)__pcur;
		__pcur = __pnext;
		++totalcount;
	}
	return count;
}

template <EAllocFreeType _alloc, bool __threads, int _Size>
void node_alloc<_alloc,__threads, _Size>::_S_freememblocks_delete_inside(int num, void* first, void* last)
{

	_Mem_block * volatile* __pList = &_S_free_mem_blocks;
	_Mem_block *__r;

	do {
		__r = *__pList;
		if (__r < first || __r > last)
		{
			break;
		}
	}
	while (Node_Alloc_Lock<__threads, _Size>::ATOMIC_CAS((long volatile *)__pList, (long)__r->_M_next, (long)__r));

	_Mem_block * __pcur = __REINTERPRET_CAST(_Mem_block *, *__pList), *__pnext, *__pfirst;
	__pfirst = __pcur;

	size_t count(0);
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		if ((void*)__pcur >= (void*)first && (void*)__pcur <= (void*)last)
		{
			//if (__pnext)
			{
				__cas_new_head<_Mem_block, __threads, _Size>(&__pnext, __pfirst);
				++count;
			}
		}
		else
		{ 
			__pfirst = __pcur;
		}
		__pcur = __pnext;
	}

	// For testing purposes
	/*
	#ifdef _DEBUG
	__pcur = __REINTERPRET_CAST(_Mem_block*, *__pList);

	count = 0;
	while (__pcur != 0) {
	__pnext = __pcur->_M_next;
	if ((void*)__pcur >= (void*)first && (void*)__pcur <= (void*)last)
	{
	++count;
	}
	__pcur = __pnext;
	}

	#endif // _DEBUG
	*/

}




#if defined (DO_CLEAN_NODE_ALLOC)
/* We deallocate all the memory chunks      */
template <EAllocFreeType _alloc, bool __threads, int _Size>
void node_alloc<_alloc,__threads, _Size>::_S_chunk_dealloc() 
{
	_Mem_block *__pcur = _S_chunks, *__pnext;
	while (__pcur != 0) {
		__pnext = __pcur->_M_next;
		_free(__pcur);
		__pcur = __pnext;
	}
	_S_chunks = 0;
	_S_free_mem_blocks = 0;
	_S_heap_size = 0;
	memset((char*)_S_free_list, 0, NFREELISTS * sizeof(_Obj*));
}
#endif /* DO_CLEAN_NODE_ALLOC */

template <EAllocFreeType _alloc, bool __threads, int _Size>
_Node_alloc_obj * volatile node_alloc<_alloc,__threads, _Size>::_S_free_list[NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
template <EAllocFreeType _alloc, bool __threads, int _Size>
long volatile node_alloc<_alloc,__threads, _Size>::_S_freelist_counter[NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
template <EAllocFreeType _alloc, bool __threads, int _Size>
_Node_alloc_Mem_block* volatile node_alloc<_alloc,__threads, _Size>::_S_free_mem_blocks = 0;
template <EAllocFreeType _alloc, bool __threads, int _Size>
long volatile node_alloc<_alloc,__threads, _Size>::_S_heap_size = 0;
template <EAllocFreeType _alloc, bool __threads, int _Size>
long volatile node_alloc<_alloc,__threads, _Size>::_S_allocations = 0;
template <EAllocFreeType _alloc, bool __threads, int _Size>
_Node_alloc_Mem_block_Huge* volatile node_alloc<_alloc,__threads, _Size>::__pCurrentHugeBlock = 0;
template <EAllocFreeType _alloc, bool __threads, int _Size>
_Node_alloc_Mem_block_Huge * volatile node_alloc<_alloc,__threads, _Size>::_S_chunks = 0;
template <EAllocFreeType _alloc, bool __threads, int _Size>
long volatile node_alloc<_alloc,__threads, _Size>::_S_wasted_in_allocation = 0;
template <EAllocFreeType _alloc, bool __threads, int _Size>
long volatile node_alloc<_alloc,__threads, _Size>::_S_wasted_in_blocks = 0;



template <EAllocFreeType _alloc, bool __threads, int _Size>
void node_alloc<_alloc, __threads, _Size>::cleanup()
{
#ifndef LINUX
	Node_Alloc_Lock<__threads, _Size> lock;
	Node_Allocator<_alloc> allocator;
	

	_Node_alloc_Mem_block_Huge *__pcur = _S_chunks, *__pnext, *__pfirst;
	__pfirst = __pcur;

	typedef _Obj* ptrtype;

	ptrtype * tmp_free_list[NFREELISTS]; 
	ptrtype * tmp_low[NFREELISTS]; 
	ptrtype * tmp_high[NFREELISTS]; 
	size_t tmp_free_list_size[NFREELISTS]; 
	size_t tmp_count[NFREELISTS]; 

	for (size_t i =0; i < NFREELISTS; ++i)
	{

		size_t count = _S_freelist_counter[i];

		// For testing purposes
		//#ifdef _DEBUG
		//		size_t count1 = _S_freelist_count(i);
		//		if (count != count1)
		//			count = count1;
		//#endif

		tmp_free_list_size[i] = count;
		tmp_free_list[i] = 0;
		if (count)
		{
			tmp_free_list[i] = (ptrtype*)allocator.cleanup_alloc((count + 1) * sizeof(ptrtype)) ;//::new ptrtype [count];
			_S_freelist_fill(i,(void**)tmp_free_list[i]);
			//qsort(tmp_free_list[i][0], count, sizeof(_Obj*), compare);

			//ptrtype _min = tmp_free_list[i][0];
			//size_t _minind = 0;
			//for (size_t k = 0; k < count; ++k)
			//{
			//	for (size_t j = k; j < count; ++j)
			//	{
			//		if ((size_t)tmp_free_list[i][j] < (size_t)_min)
			//		{
			//			_min = tmp_free_list[i][j];
			//			_minind = j;
			//		}
			//	}
			//	std::swap(tmp_free_list[i][k], tmp_free_list[i][_minind]);
			//}

			std::sort(&(tmp_free_list[i][0]), &(tmp_free_list[i][count]));
		}
	}


	while (__pcur != 0) 
	{
		bool bdelete(false);
		__pnext = __pcur->_M_next;

		size_t freelistsize = 0;
		size_t	cursize = __pcur->_M_count;//(size_t)(__pcur->_M_end - (char*)__pcur);// - sizeof(_Node_alloc_Mem_block_Huge));
		for (size_t i =0; i < NFREELISTS; ++i)
		{
			size_t count = tmp_free_list_size[i];//- 1;
			tmp_count[i] = 0;
			if (tmp_free_list_size[i] > 0)
			{
				_Obj** low = std::lower_bound(&(tmp_free_list[i][0]), &(tmp_free_list[i][count]), (_Obj*)__pcur);
				_Obj** high = std::upper_bound(&(tmp_free_list[i][0]), &(tmp_free_list[i][count]), (_Obj*)__pcur->_M_end);// + sizeof(_Node_alloc_Mem_block_Huge));
				tmp_low[i] = low;
				tmp_high[i] = high;

				if (low == high && ((void*)low < (void*)__pcur || (void*) high > (void*)__pcur->_M_end))
				{
					//For testing purposes
					int b = 0;
				}
				else 
				{
					tmp_count[i] = high - low;// + 1;
					freelistsize += tmp_count[i];//  * (i + 1) << _ALIGN_SHIFT;
					//For testing purposes
					//#ifdef _DEBUG
					//					size_t ttt = _S_freelist_count_inside(i, (_Obj*)__pcur, (_Obj*)__pcur->_M_end);// + sizeof(_Node_alloc_Mem_block_Huge));
					//					if (ttt != tmp_count[i])
					//						int a = 0;
					//#endif
				}
			}
		}

		if (freelistsize)
		{

			if (cursize == freelistsize)
			{
				bdelete = true;

				for (size_t i =0; i < NFREELISTS; ++i)
				{
					if (tmp_count[i])
					{
						//For testing purposes
						//_S_freelist_delete_inside(i, (_Obj*)__pcur, (_Obj*)__pcur->_M_end + sizeof(_Node_alloc_Mem_block_Huge)/* + sizeof(_Node_alloc_Mem_block)*/);

						if (tmp_count[i] >1 )
						{
							size_t copied = &(tmp_free_list[i][tmp_free_list_size[i]]) - tmp_high[i];// + 1;
							tmp_free_list_size[i] -=  tmp_count[i];
							//on PS3, memcpy is performed backwards (dunno why), so we have potential overlapping areas here (tested)
							memmove(tmp_low[i], tmp_high[i]/* + 1*/, copied * sizeof(_Obj*));
						}
						else
						{
							tmp_free_list_size[i] = 0;
						}
					}
				}

				_S_freememblocks_delete_inside(0, (_Obj*)__pcur, (_Obj*)__pcur->_M_end);

				// delete from S_chunks
				if (__pfirst == __pcur)
				{
					//ATOMIC_CAS(_S_chunks, __pcur->_M_next, __pcur);
					_S_chunks = __pcur->_M_next;
					__pfirst = _S_chunks;
				}
				else
				{
					__cas_new_head<_Node_alloc_Mem_block_Huge, __threads, _Size>(&__pnext, __pfirst);
				}
				_S_heap_size -= _Size/*cursize*/;
				_S_wasted_in_blocks -= _Size;


				// free memory
				//_free(__pcur);
				allocator.pool_free(__pcur);
				--_S_allocations;
			}

			if (!bdelete)
			{
				__pfirst = __pcur;
			}
		}
		__pcur = __pnext;
	}


	for (size_t i =0; i < NFREELISTS; ++i)
	{
		if (tmp_free_list[i])
		{
			// restore bew free_list
			_Obj * volatile * __my_free_list = _S_free_list + i;
			_S_freelist_counter[i] = tmp_free_list_size[i];

			if (tmp_free_list_size[i] > 0)
			{
				_Obj * _pcur = tmp_free_list[i][0];
				*__my_free_list = _pcur;
				for (size_t j = 1; j < tmp_free_list_size[i]; ++j)
				{
					_pcur->_M_next = tmp_free_list[i][j];
					_pcur = _pcur->_M_next;
				}
				_pcur->_M_next = 0;
			}
			else
			{
				*__my_free_list = 0;
			}

			allocator.cleanup_free(tmp_free_list[i]);
			//::delete [] tmp_free_list[i];
		}
	}
#endif
}


#undef S_FREELIST_INDEX

//#endif // WIN32|DEBUG

#endif













//#ifndef _CRY_MEMORY_ALLOCATOR
//#define _CRY_MEMORY_ALLOCATOR
//
//#pragma once
//#include <algorithm>
//
////#if ( defined(WIN32) || defined(WIN64) ) && ( defined(_DEBUG) || defined(DEBUG) )
////#undef CRY_MEMORY_ALLOCATOR
////#define _CRTDBG_MAP_ALLOC
////#include "crtdbg.h"
////#else
//
//#define CRY_STL_ALLOC
//
//
//#if defined (__OS400__) || defined (_WIN64) || defined (XENON) || defined(PS3)
//enum {_ALIGN = 16, _MAX_BYTES = 512};
//#else
////enum {_ALIGN = 8, _ALIGN_SHIFT = 3, _MAX_BYTES = 128};
//enum {_ALIGN = 8, _MAX_BYTES = 256};
//#endif /* __OS400__ */
//
//
//enum {NFREELISTS=32*8, NFREELISTSSHIFT = 5, MAX_SHIFT = 8};
//
//#define CRY_MEMORY_ALLOCATOR
//
////#define S_FREELIST_INDEX(__bytes) ((__bytes - size_t(1)) >> (int)_ALIGN_SHIFT)
////#define S_FREELIST_INDEX(__bytes, __align_shift) ((__bytes - size_t(1)) >> (int)__align_shift)
//
//class  _Node_alloc_obj {
//public:
//	_Node_alloc_obj * _M_next;
//};
//
//struct _Node_alloc_Mem_block_Huge {
//	//Pointer to the end of the memory block
//	char *_M_end;
//	// number 
//	int _M_count;
//	//Pointer to the next memory block
//	_Node_alloc_Mem_block_Huge *_M_next;
//};
//
//struct _Node_alloc_Mem_block {
//	//Pointer to the end of the memory block
//	char *_M_end;
//	//Pointer to the next memory block
//	_Node_alloc_Mem_block_Huge *_M_huge_block;
//	_Node_alloc_Mem_block *_M_next;
//};
//
//
//// Allocators!
//enum EAllocFreeType 
//{
//	eCryDefaultMalloc,
//	eCryMallocCryFreeAll,
//	eCryMallocCryFreeCRTCleanup
//};
//
//template <EAllocFreeType type>
//struct Node_Allocator 
//{
//	inline void * pool_alloc(size_t size) 
//	{
//		return CryModuleMalloc(size);
//	};
//	inline void * cleanup_alloc(size_t size) 
//	{
//		return CryModuleMalloc(size);
//	};
//	inline void pool_free(void * ptr) 
//	{
//		CryModuleFree(ptr);
//	};
//	inline void cleanup_free(void * ptr)
//	{
//		CryModuleFree(ptr);
//	};
//};
//
//// partial 
//template <>
//struct Node_Allocator<eCryMallocCryFreeCRTCleanup>
//{
//	inline void * pool_alloc(size_t size) 
//	{
//		return CryCrtMalloc(size);
//	};
//	inline void * cleanup_alloc(size_t size) 
//	{
//		return CryCrtMalloc(size);
//	};
//	inline void pool_free(void * ptr) 
//	{
//		CryCrtFree(ptr);
//	};
//	inline void cleanup_free(void * ptr)
//	{
//		CryCrtFree(ptr);
//	};
//};
//
//template <>
//struct Node_Allocator<eCryDefaultMalloc>
//{
//	inline void * pool_alloc(size_t size) 
//	{
//		return malloc(size);
//	};
//	inline void * cleanup_alloc(size_t size) 
//	{
//		return malloc(size);
//	};
//	inline void pool_free(void * ptr) 
//	{
//		free(ptr);
//	};
//	inline void cleanup_free(void * ptr)
//	{
//		free(ptr);
//	};
//};
//
//#ifndef ALLOCATOR_TEST
//#include "MultiThread.h"
//#else
//
//class WriteLock
//{
//public:
//	WriteLock(int){}
//};
//#endif
//
//template <bool __threads, int _Size>
//class Node_Alloc_Lock {
//public:
//	Node_Alloc_Lock() : lock(g_lockMemMan) 
//	{ 
//	}
//
//	~Node_Alloc_Lock() 
//	{
//	}
//
//	WriteLock lock;
//	static volatile int g_lockMemMan;
//};
//
//template <bool __threads, int _Size>
//volatile int Node_Alloc_Lock<__threads, _Size>::g_lockMemMan = 0;
///*
//template <int _Size>
//class Node_Alloc_Lock<true,_Size> 
//{
//public:
//Node_Alloc_Lock : lock(g_lockMemMan)
//{ 
//}
//
//~Node_Alloc_Lock() 
//{
//}
//WriteLock lock;
//};
//*/
//template <int _Size>
//class Node_Alloc_Lock<false,_Size> 
//{
//public:
//	Node_Alloc_Lock() { }  
//	~Node_Alloc_Lock() { }
//};
//
////#define _PARANOID
//// avoid lock here. Just use safe atomic operations
////#ifndef _PARANOID
////template <int _Size>
////class Node_Alloc_Lock<true,_Size> 
////{
////public:
////	Node_Alloc_Lock() { }  
////	~Node_Alloc_Lock() { }
////};
////#endif
//
//
////struct _Node_alloc_Mem_block {
////	//Pointer to the end of the memory block
////	char *_M_end;
////	//Pointer to the next memory block
////	_Node_alloc_Mem_block *_M_next;
////};
//
//#define _malloc ::malloc
//#define _free ::free
//
//#define MAX_ALIGN	128
//static char _ALIGN_SHIFT[MAX_ALIGN+1] = {
//	0, 0, 1, 2, 2, 2, 3, 3,	3, 3, 3, 3,	4, 4, 4, 4, 4,	4,	4,	4,	4,	4,	4,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,
//	5,	5,	5,	5,	5,	5,	5,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,  6,	6,	6,	6,	6,
//	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
//	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7
//};
//
////inline size_t S_FREELIST_INDEX( size_t __bytes, size_t __align_shift ) 
////{
////	return  /*__align_shift *  NFREELISTS*/ (__align_shift << NFREELISTSSHIFT) + ((__bytes - size_t(1)) >> (int)__align_shift);
////}
//
//inline size_t S_FREELIST_INDEX( size_t __bytes, size_t __align ) 
//{
//	size_t __align_shift = _ALIGN_SHIFT[__align];
//	return  /*__align_shift *  NFREELISTS*/ (__align_shift << NFREELISTSSHIFT) + ((__bytes - size_t(1)) >> (int)__align_shift);
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//class node_alloc {
//private:
//	//static inline size_t  _S_round_up(size_t __bytes) { return (((__bytes) + (size_t)_ALIGN-1) & ~((size_t)_ALIGN - 1)); }
//	static inline size_t  _S_round_up(size_t __bytes, size_t __align) { return (((__bytes) + (size_t)__align-1) & ~((size_t)__align - 1)); }
//	typedef _Node_alloc_obj _Obj;
//
//private:
//	typedef _Node_alloc_Mem_block _Mem_block;
//
//	// Returns an object of size __n, and optionally adds to size __n free list.
//	static _Obj*  _S_refill(size_t __n, size_t __align);
//	// Allocates a chunk for nobjs of size __p_size.  nobjs may be reduced
//	// if it is inconvenient to allocate the requested number.
//	static char*  _S_chunk_alloc(size_t __p_size, int& __nobjs);
//	// Chunk allocation state.
//
//
//	static void * _M_allocate(size_t __n, size_t __align);
//	/* __p may not be 0 */
//	static void		_M_deallocate(void *__p, size_t __n, size_t __align);
//
//	static size_t _S_freelist_count(int num);
//	static size_t _S_freelist_fill(int num, void ** _ptr);
//	static size_t _S_freelist_count_inside(int num, _Obj* first, _Obj* last);
//	static size_t _S_fill_from_freelist(int num, _Obj* first, _Obj* last, void **);
//	static void		_S_freelist_delete_inside(int num, _Obj* first, _Obj* last);
//	static void		_S_freememblocks_delete_inside(int num, void* first, void* last);
//	static size_t _S_freememblocks_count_inside(int num, void* first, void* last);
//
//
//
//#if defined (DO_CLEAN_NODE_ALLOC)
//	//A helper class to guaranty the memory pool management:
//	//friend struct _Node_alloc_helper;
//	//Some compilers (MSVC) seems to have trouble with friend declarations:
//public:
//	// Methods to report alloc/dealloc calls to the counter system.
//	static size_t&  _S_alloc_call(size_t incr = 1);
//	static void  _S_dealloc_call();
//
//
//private:
//	// Free all the allocated chuncks of memory
//	static void  _S_chunk_dealloc();
//	// Beginning of the linked list of allocated chunks of memory
//#endif /* DO_CLEAN_NODE_ALLOC */
//
//	//static _Obj *_S_chunks;
//	static _Node_alloc_Mem_block_Huge * volatile _S_chunks;
//	static _Obj * volatile _S_free_list[NFREELISTS]; 
//	static long volatile _S_freelist_counter[NFREELISTS]; 
//	// Amount of total allocated memory
//	static long volatile _S_heap_size;
//	static long volatile _S_wasted_in_allocation;
//	static long volatile _S_wasted_in_blocks;
//
//	static _Mem_block* volatile _S_free_mem_blocks;
//	static _Node_alloc_Mem_block_Huge * volatile __pCurrentHugeBlock;
//
//
//public:
//	// this one is needed for proper simple_alloc wrapping
//	static long volatile _S_allocations;
//	typedef char value_type;
//	/* __n must be > 0      */
//	static void *  allocate(size_t __n, size_t __align = _ALIGN) 
//	{ 
//		if (__n > (size_t)_MAX_BYTES) 
//		{
//			Node_Allocator<_alloc> all;
//			return all.pool_alloc(__n);
//		}
//		else
//			return _M_allocate(__n, __align); 
//	}
//
//	static void *  alloc(size_t __n, size_t __align = _ALIGN) 
//	{ 
//		return allocate(__n, __align);
//	}
//
//	/* __p may not be 0 */
//	static void  deallocate(void *__p, size_t __n, size_t __align = _ALIGN) 
//	{
//		if (__n > (size_t)_MAX_BYTES) 
//		{
//			Node_Allocator<_alloc> all;
//			all.pool_free(__p);
//		}
//		else 
//			_M_deallocate(__p, __n, __align); 
//	}
//	static void  dealloc(void *__p, size_t __n, size_t __align = _ALIGN) 
//	{
//		deallocate(__p, __n, __align);
//	}
//
//	/* __n must be > 0      */
//	//	static void *  alloc(size_t __n) { return (__n > (size_t)_MAX_BYTES) ?  Node_Allocator<_alloc>.pool_alloc(__n) : _M_allocate(__n); }
//	/* __p may not be 0 */
//	//static void  dealloc(void *__p, size_t __n) { if (__n > (size_t)_MAX_BYTES) Node_Allocator<_alloc>.pool_free(__p); else _M_deallocate(__p, __n); }
//
//	static size_t get_heap_size() { return _S_heap_size; };
//	static size_t get_wasted_in_allocation() { return _S_wasted_in_allocation; };
//	static size_t get_wasted_in_blocks() { 	return _S_wasted_in_blocks; };
//
//	static void cleanup();
//};
//
//
//#  if defined(_WIN64) || defined(WIN64)
//#define ATOMIC_CAS(__dst, __val, __old_val) (_InterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           _InterlockedIncrement((long*)__x)
//#    define ATOMIC_DECREMENT(__x)           _InterlockedDecrement((long*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       _InterlockedExchange((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#else /*_WIN32*/
//#if defined(_WIN32) || defined(WIN32) || defined(XENON)
//#define ATOMIC_CAS(__dst, __val, __old_val) (InterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           InterlockedIncrement((long*)__x)
//#    define ATOMIC_DECREMENT(__x)           InterlockedDecrement((long*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       InterlockedExchange((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)      InterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#else /*_WIN64*/
//#define ATOMIC_CAS(__dst, __val, __old_val) (CryInterlockedCompareExchange((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)/*(InterlockedCompareExchange64((long volatile*)__dst, (long)__val, (long)__old_val) == (long)__old_val)*/
//#  ifndef ATOMIC_INCREMENT
//#    define ATOMIC_INCREMENT(__x)           CryInterlockedIncrement((int volatile*)__x)
//#    define ATOMIC_DECREMENT(__x)           CryInterlockedDecrement((int volatile*)__x)
//#    define ATOMIC_EXCHANGE(__x, __y)       CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#    define ATOMIC_EXCHANGE_ADD(__x, __y)   CryInterlockedExchangeAdd((long*)__x, (long)__y)
//#	 endif
//#endif/*_WIN64*/
//#endif/*_WIN32*/
//
//
//template <class _ListElem>
//inline void __cas_new_head (_ListElem * volatile *__free_list, _ListElem *__new_head) {
//	_ListElem *__old_head;
//	do {
//		__old_head = *__free_list;
//		__new_head->_M_next = __old_head;
//	} while (!ATOMIC_CAS(__free_list, __new_head, __old_head));
//}
//
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void* node_alloc<_alloc, __threads, _Size>::_M_allocate(size_t __n, size_t __align) 
//{
//	Node_Alloc_Lock<__threads, _Size> lock;
//
//	_Obj *__r;
//	long allocated = S_FREELIST_INDEX(__n, __align);
//	_Obj * volatile * __my_free_list = _S_free_list + allocated;
//
//	do {
//		__r = *__my_free_list;
//		if (__r  == 0) {
//			__r = _S_refill(__n, __align);
//			break;
//		}
//	}
//	while (!ATOMIC_CAS(__my_free_list, __r->_M_next, __r));
//
//	ATOMIC_EXCHANGE_ADD(&_S_wasted_in_allocation,allocated - __n);
//	ATOMIC_EXCHANGE_ADD(&_S_wasted_in_blocks,-allocated);
//	long volatile * counter = _S_freelist_counter + allocated;//S_FREELIST_INDEX(__n, __align);
//	ATOMIC_DECREMENT(counter);
//
//	//_S_wasted_in_allocation += allocated - __n;
//	//_S_wasted_in_blocks -= allocated;
//	//--_S_freelist_counter[S_FREELIST_INDEX(__n)];
//
//	return __r;
//}
//
//#ifndef __STATIC_CAST
//#define __STATIC_CAST(x,y) static_cast<x>(y)
//#endif
//
//#ifndef __REINTERPRET_CAST
//#define __REINTERPRET_CAST(x,y) reinterpret_cast<x>(y)
//#endif
//
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void node_alloc<_alloc, __threads, _Size>::_M_deallocate(void *__p, size_t __n, size_t __align) 
//{
//	Node_Alloc_Lock<__threads, _Size> lock;
//
//	long allocated = S_FREELIST_INDEX(__n, __align);
//	_Obj * volatile * __my_free_list = _S_free_list + allocated;
//	_Obj *__pobj = __STATIC_CAST(_Obj*, __p);
//	__cas_new_head(__my_free_list, __pobj);
//
//	ATOMIC_EXCHANGE_ADD(&_S_wasted_in_allocation,-allocated + __n);
//
//	ATOMIC_EXCHANGE_ADD(&_S_wasted_in_blocks,+allocated);
//	long volatile * counter = _S_freelist_counter + allocated;//S_FREELIST_INDEX(__n);
//	ATOMIC_INCREMENT(counter);
//
//	//_S_wasted_in_allocation -= allocated - __n;
//	//_S_wasted_in_blocks += allocated;
//	//++_S_freelist_counter[S_FREELIST_INDEX(__n)];
//}
//
///* Returns an object of size __n, and optionally adds to size __n free list.*/
///* We assume that __n is properly aligned.                                  */
///* We hold the allocation lock.                                             */
////template <bool __threads, int __inst>
//
////#define NUM_OBJ 20
//#define NUM_OBJ 20
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//_Node_alloc_obj* node_alloc<_alloc,__threads, _Size>::_S_refill(size_t __n, size_t __align) {
//	int __nobjs = NUM_OBJ;
//	__n = _S_round_up(__n, __align);
//	char* __chunk = _S_chunk_alloc(__n, __nobjs);
//
//	if (1 == __nobjs) 
//	{
//		//++_S_freelist_counter[S_FREELIST_INDEX(__n)];
//		long volatile* counter = _S_freelist_counter + S_FREELIST_INDEX(__n, __align);
//		ATOMIC_INCREMENT(counter);
//		__pCurrentHugeBlock->_M_count += 1;
//		return (_Obj*)__chunk;
//	}
//
//	_Obj* volatile* __my_free_list = _S_free_list + S_FREELIST_INDEX(__n, __align);
//	_Obj* __result;
//	_Obj* __current_obj;
//	_Obj* __new_head;
//	_Obj* __next_obj;
//
//	/* Build free list in chunk */
//
//	//	_S_freelist_counter[S_FREELIST_INDEX(__n)] += __nobjs;
//	long volatile * counter = _S_freelist_counter +S_FREELIST_INDEX(__n, __align);
//	ATOMIC_EXCHANGE_ADD(counter,__nobjs);
//
//	ATOMIC_EXCHANGE_ADD(&__pCurrentHugeBlock->_M_count, __nobjs);
//	//__pCurrentHugeBlock->_M_count += __nobjs;
//	__result = (_Obj*)__chunk;
//	__new_head = __next_obj = (_Obj*)(__chunk + __n);
//	for (--__nobjs; --__nobjs; ) {
//		__current_obj = __next_obj;
//		__next_obj = (_Obj*)((char*)__next_obj + __n);
//		__current_obj->_M_next = __next_obj;
//
//	}
//
//
//	//Link the new free list to the global free list
//	do {
//		__next_obj->_M_next = *__my_free_list;
//		// !!!!
//
//	} while (!ATOMIC_CAS(__my_free_list, __new_head, __next_obj->_M_next));
//
//	return __result;
//}
//
///* We allocate memory in large chunks in order to avoid fragmenting     */
///* the malloc heap too much.                                            */
///* We assume that size is properly aligned.                             */
///* We hold the allocation lock.                                         */
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//char* node_alloc<_alloc,__threads, _Size>::_S_chunk_alloc(size_t _p_size,
//																													int& __nobjs) 
//{
//	char* __result = 0;
//	size_t __total_bytes = _p_size * __nobjs;
//
//	Node_Allocator<_alloc> allocator;
//	//We get the first available free block:
//	_Mem_block *__pblock;
//	do {
//		__pblock = _S_free_mem_blocks;
//		if (__pblock == 0) break;
//	}
//	while (!ATOMIC_CAS(&_S_free_mem_blocks, __pblock->_M_next, __pblock));
//
//	if (__pblock != 0) 
//	{
//		size_t __bytes_left = __pblock->_M_end - (char*)__pblock;
//		char *__new_buf_pos = (char*)__pblock;
//
//		if ((__bytes_left < __total_bytes) && (__bytes_left >= _p_size)) 
//		{
//			//We won't get all the requested objs:
//			__nobjs = (int)(__bytes_left/_p_size);
//			__total_bytes = _p_size * __nobjs;
//		}
//
//		if (__bytes_left >= __total_bytes) 
//		{
//			__result = (char*)__pblock;
//			__bytes_left -= __total_bytes;
//			__new_buf_pos += __total_bytes;
//		}
//
//		//We get rid of the leaving part of the mem block:
//		if (__bytes_left != 0) 
//		{
//			if ((__result != 0) && (__bytes_left > sizeof(_Mem_block))) 
//			{
//				//We use part of the block and there is enough mem left to put the block back to 
//				//the free mem blocks:
//				_Mem_block *__pnew_block = (_Mem_block*)(__new_buf_pos);
//				__pnew_block->_M_end = __pblock->_M_end;
//				__cas_new_head(&_S_free_mem_blocks, __pnew_block);
//				//__pCurrentHugeBlock->_M_count += __nobjs;
//			}
//			else 
//			{
//				//A too small block, we put it in the main free list elements:
//				/*
//
//				_Obj* volatile* __my_free_list = _S_free_list + S_FREELIST_INDEX(__bytes_left);
//				_Obj* __pobj = __REINTERPRET_CAST(_Obj*, __new_buf_pos);
//				++_S_freelist_counter[S_FREELIST_INDEX(__bytes_left)];
//				__cas_new_head(__my_free_list, __pobj);
//				if (__result == 0)
//				__pCurrentHugeBlock->_M_count +=  1;
//
//				*/
//			}
//		}
//
//		if (__result != 0) 
//		{
//			return __result;
//		}
//	}
//
//	//#define _ALLOCATION_SIZE 4 * 1024 * 1024
//#define _ALLOCATION_SIZE 512 * 1024
//	//We haven't found a free block or not enough place, we ask memory to the the system:
//	size_t __bytes_to_get =  _Size;//_ALLOCATION_SIZE;////
//	__result = (char*)allocator.pool_alloc(__bytes_to_get);//(char*)_malloc(__bytes_to_get);
//	//++_S_allocations;
//	ATOMIC_INCREMENT(&_S_allocations);
//	//_S_wasted_in_blocks += __bytes_to_get;
//	ATOMIC_EXCHANGE_ADD(&_S_wasted_in_blocks,__bytes_to_get);
//	if (0 == __result) {
//
//		// BOOOOOOM!
//		__result= 0;
//	}
//
//	{
//		size_t newHeapSize, oldHeapSize;
//		do {
//			oldHeapSize = _S_heap_size;
//			newHeapSize = oldHeapSize + __bytes_to_get;
//		} while (!ATOMIC_CAS(&_S_heap_size, newHeapSize, oldHeapSize));
//	}
//
//	//#  ifdef DO_CLEAN_NODE_ALLOC
//	_Node_alloc_Mem_block_Huge *__pBlock = (_Node_alloc_Mem_block_Huge*)(__result);// + __total_bytes);
//	__bytes_to_get -= sizeof(_Node_alloc_Mem_block_Huge);
//	__pBlock->_M_end = __result + __bytes_to_get;
//	__pBlock->_M_count = 0;
//	__cas_new_head(&_S_chunks, __pBlock);
//	__result += sizeof(_Node_alloc_Mem_block_Huge);
//	//__pCurrentHugeBlock = __pBlock;
//
//	_Node_alloc_Mem_block_Huge * volatile  __old_block;
//	do {
//		__old_block = __pCurrentHugeBlock;
//	} while (!ATOMIC_CAS(&__pCurrentHugeBlock, __pBlock, __old_block));
//
//	//__cas_new_head(__pCurrentHugeBlock)
//
//	//#  endif
//	_Mem_block *__pnewBlock = (_Mem_block*)(__result + __total_bytes);
//	__pnewBlock->_M_end = __result + __bytes_to_get;
//	__cas_new_head(&_S_free_mem_blocks, __pnewBlock);
//	return __result;
//}
//
//#  if defined (DO_CLEAN_NODE_ALLOC)
////template <bool __threads, int __inst>
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//size_t& node_alloc<_alloc,__threads, _Size>::_S_alloc_call(size_t incr) {
//	static size_t _S_counter = 0;
//	if (incr != 0) {
//		ATOMIC_INCREMENT(&_S_counter);
//	}
//	return _S_counter;
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void node_alloc<_alloc,__threads, _Size>::_S_dealloc_call() 
//{
//
//	size_t *pcounter = &_S_alloc_call(0);
//	ATOMIC_DECREMENT(pcounter);
//	//As we are only releasing memory on shared library unload, counter
//	//can only reach 0 once threads has been stopped so we do not have to
//	//check atomic_decrement result.
//	if (*pcounter == 0) {
//		_S_chunk_dealloc();
//	}
//}
//#  endif /* DO_CLEAN_NODE_ALLOC */
//
///*#endif / * _STLP_USE_LOCK_FREE_IMPLEMENTATION * /*/
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//size_t node_alloc<_alloc,__threads, _Size>::_S_freelist_count(int num)
//{
//
//	_Obj* volatile* __pList = _S_free_list + num;
//	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
//	size_t count(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		__pcur = __pnext;
//		++count;
//	}
//	return count;
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//size_t node_alloc<_alloc,__threads, _Size>::_S_freelist_fill(int num, void ** _ptr)
//{
//
//	_Obj* volatile* __pList = _S_free_list + num;
//	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
//	size_t count(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		_ptr[count] = __pcur;
//		__pcur = __pnext;
//		++count;
//	}
//	return count;
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//size_t node_alloc<_alloc,__threads, _Size>::_S_freelist_count_inside(int num, _Obj* first, _Obj* last)
//{
//
//	_Obj* volatile* __pList = _S_free_list + num;
//	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
//	size_t count(0);
//	size_t totalcount(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		if (__pcur >= first && __pcur <= last)
//			++count;
//		__pcur = __pnext;
//		++totalcount;
//	}
//	return count;
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//size_t node_alloc<_alloc,__threads, _Size>::_S_fill_from_freelist(int num, _Obj* first, _Obj* last, void ** _ptr)
//{
//
//	_Obj* volatile* __pList = _S_free_list + num;
//	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList), *__pnext;
//	size_t count(0);
//	//size_t totalcount(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		if (__pcur >= first && __pcur <= last)
//		{
//			_ptr[count++]	= __pcur;
//		}
//		//			++count;
//		__pcur = __pnext;
//		//	++totalcount;
//	}
//	return count;
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void node_alloc<_alloc,__threads, _Size>::_S_freelist_delete_inside(int num, _Obj* first, _Obj* last)
//{
//
//	_Obj* volatile* __pList = _S_free_list + num;
//	_Obj *__pnext, *__pfirst, *__plast,* __r;
//
//	do {
//		__r = *__pList;
//		if (__r < first || __r > last)
//		{
//			break;
//		}
//	}
//	while (ATOMIC_CAS(__pList, __r->_M_next, __r));
//
//
//	_Obj* __pcur = __REINTERPRET_CAST(_Obj*, *__pList);
//	__pfirst = __pcur;
//
//	size_t count(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		if (__pcur >= first && __pcur <= last)
//		{
//			//if (__pnext)
//			{
//				__cas_new_head(&__pnext, __pfirst);
//				++count;
//			}
//		}
//		else
//		{
//			__pfirst = __pcur;
//		}
//		__pcur = __pnext;
//	}
//
//	__pcur = __REINTERPRET_CAST(_Obj*, *__pList);
//
//	// for testing purposes
//	/*
//	#ifdef _DEBUG
//	count = 0;
//	while (__pcur != 0) {
//	__pnext = __pcur->_M_next;
//	if (__pcur >= first && __pcur < last)
//	{
//	++count;
//	}
//	__pcur = __pnext;
//	}
//	#endif
//	*/
//
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//size_t node_alloc<_alloc,__threads, _Size>::_S_freememblocks_count_inside(int num, void* first, void* last)
//{
//
//	_Mem_block* volatile* __pList = &_S_free_mem_blocks;
//	_Mem_block* __pcur = __REINTERPRET_CAST(_Mem_block*, *__pList), *__pnext;
//	size_t count(0);
//	size_t totalcount(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		if ((void*)__pcur >= first && (void*)__pcur <= last)
//			count += (char*)__pcur->_M_end - (char*)__pcur;
//		__pcur = __pnext;
//		++totalcount;
//	}
//	return count;
//}
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void node_alloc<_alloc,__threads, _Size>::_S_freememblocks_delete_inside(int num, void* first, void* last)
//{
//
//	_Mem_block * volatile* __pList = &_S_free_mem_blocks;
//	_Mem_block *__r;
//
//	do {
//		__r = *__pList;
//		if (__r < first || __r > last)
//		{
//			break;
//		}
//	}
//	while (ATOMIC_CAS(__pList, __r->_M_next, __r));
//
//	_Mem_block * __pcur = __REINTERPRET_CAST(_Mem_block *, *__pList), *__pnext, *__pfirst;
//	__pfirst = __pcur;
//
//	size_t count(0);
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		if ((void*)__pcur >= (void*)first && (void*)__pcur <= (void*)last)
//		{
//			//if (__pnext)
//			{
//				__cas_new_head(&__pnext, __pfirst);
//				++count;
//			}
//		}
//		else
//		{ 
//			__pfirst = __pcur;
//		}
//		__pcur = __pnext;
//	}
//
//	// For testing purposes
//	/*
//	#ifdef _DEBUG
//	__pcur = __REINTERPRET_CAST(_Mem_block*, *__pList);
//
//	count = 0;
//	while (__pcur != 0) {
//	__pnext = __pcur->_M_next;
//	if ((void*)__pcur >= (void*)first && (void*)__pcur <= (void*)last)
//	{
//	++count;
//	}
//	__pcur = __pnext;
//	}
//
//	#endif // _DEBUG
//	*/
//
//}
//
//
//
//
//#if defined (DO_CLEAN_NODE_ALLOC)
///* We deallocate all the memory chunks      */
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void node_alloc<_alloc,__threads, _Size>::_S_chunk_dealloc() 
//{
//	_Mem_block *__pcur = _S_chunks, *__pnext;
//	while (__pcur != 0) {
//		__pnext = __pcur->_M_next;
//		_free(__pcur);
//		__pcur = __pnext;
//	}
//	_S_chunks = 0;
//	_S_free_mem_blocks = 0;
//	_S_heap_size = 0;
//	memset((char*)_S_free_list, 0, NFREELISTS * sizeof(_Obj*));
//}
//#endif /* DO_CLEAN_NODE_ALLOC */
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//_Node_alloc_obj * volatile node_alloc<_alloc,__threads, _Size>::_S_free_list[NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//long volatile node_alloc<_alloc,__threads, _Size>::_S_freelist_counter[NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//_Node_alloc_Mem_block* volatile node_alloc<_alloc,__threads, _Size>::_S_free_mem_blocks = 0;
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//long volatile node_alloc<_alloc,__threads, _Size>::_S_heap_size = 0;
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//long volatile node_alloc<_alloc,__threads, _Size>::_S_allocations = 0;
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//_Node_alloc_Mem_block_Huge* volatile node_alloc<_alloc,__threads, _Size>::__pCurrentHugeBlock = 0;
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//_Node_alloc_Mem_block_Huge * volatile node_alloc<_alloc,__threads, _Size>::_S_chunks = 0;
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//long volatile node_alloc<_alloc,__threads, _Size>::_S_wasted_in_allocation = 0;
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//long volatile node_alloc<_alloc,__threads, _Size>::_S_wasted_in_blocks = 0;
//
//
//
//template <EAllocFreeType _alloc, bool __threads, int _Size>
//void node_alloc<_alloc, __threads, _Size>::cleanup()
//{
//
//	Node_Alloc_Lock<__threads, _Size> lock;
//	Node_Allocator<_alloc> allocator;
//
//
//	_Node_alloc_Mem_block_Huge *__pcur = _S_chunks, *__pnext, *__pfirst;
//	__pfirst = __pcur;
//
//	typedef _Obj* ptrtype;
//
//	ptrtype * tmp_free_list[NFREELISTS]; 
//	ptrtype * tmp_low[NFREELISTS]; 
//	ptrtype * tmp_high[NFREELISTS]; 
//	size_t tmp_free_list_size[NFREELISTS]; 
//	size_t tmp_count[NFREELISTS]; 
//
//	for (size_t i =0; i < NFREELISTS; ++i)
//	{
//
//		size_t count = _S_freelist_counter[i];
//
//		// For testing purposes
//		//#ifdef _DEBUG
//		//		size_t count1 = _S_freelist_count(i);
//		//		if (count != count1)
//		//			count = count1;
//		//#endif
//
//		tmp_free_list_size[i] = count;
//		tmp_free_list[i] = 0;
//		if (count)
//		{
//			tmp_free_list[i] = (ptrtype*)allocator.cleanup_alloc((count + 1) * sizeof(ptrtype)) ;//::new ptrtype [count];
//			_S_freelist_fill(i,(void**)tmp_free_list[i]);
//			//qsort(tmp_free_list[i][0], count, sizeof(_Obj*), compare);
//
//			//ptrtype _min = tmp_free_list[i][0];
//			//size_t _minind = 0;
//			//for (size_t k = 0; k < count; ++k)
//			//{
//			//	for (size_t j = k; j < count; ++j)
//			//	{
//			//		if ((size_t)tmp_free_list[i][j] < (size_t)_min)
//			//		{
//			//			_min = tmp_free_list[i][j];
//			//			_minind = j;
//			//		}
//			//	}
//			//	std::swap(tmp_free_list[i][k], tmp_free_list[i][_minind]);
//			//}
//
//			std::sort(&(tmp_free_list[i][0]), &(tmp_free_list[i][count]));
//		}
//	}
//
//
//	while (__pcur != 0) 
//	{
//		bool bdelete(false);
//		__pnext = __pcur->_M_next;
//
//		size_t freelistsize = 0;
//		size_t	cursize = __pcur->_M_count;//(size_t)(__pcur->_M_end - (char*)__pcur);// - sizeof(_Node_alloc_Mem_block_Huge));
//		for (size_t i =0; i < NFREELISTS; ++i)
//		{
//			size_t count = tmp_free_list_size[i];//- 1;
//			tmp_count[i] = 0;
//			if (tmp_free_list_size[i] > 0)
//			{
//				_Obj** low = std::lower_bound(&(tmp_free_list[i][0]), &(tmp_free_list[i][count]), (_Obj*)__pcur);
//				_Obj** high = std::upper_bound(&(tmp_free_list[i][0]), &(tmp_free_list[i][count]), (_Obj*)__pcur->_M_end);// + sizeof(_Node_alloc_Mem_block_Huge));
//				tmp_low[i] = low;
//				tmp_high[i] = high;
//
//				if (low == high && ((void*)low < (void*)__pcur || (void*) high > (void*)__pcur->_M_end))
//				{
//					//For testing purposes
//					int b = 0;
//				}
//				else 
//				{
//					tmp_count[i] = high - low;// + 1;
//					freelistsize += tmp_count[i];//  * (i + 1) << _ALIGN_SHIFT;
//					//For testing purposes
//					//#ifdef _DEBUG
//					//					size_t ttt = _S_freelist_count_inside(i, (_Obj*)__pcur, (_Obj*)__pcur->_M_end);// + sizeof(_Node_alloc_Mem_block_Huge));
//					//					if (ttt != tmp_count[i])
//					//						int a = 0;
//					//#endif
//				}
//			}
//		}
//
//		if (freelistsize)
//		{
//
//			if (cursize == freelistsize)
//			{
//				bdelete = true;
//
//				for (size_t i =0; i < NFREELISTS; ++i)
//				{
//					if (tmp_count[i])
//					{
//						//For testing purposes
//						//_S_freelist_delete_inside(i, (_Obj*)__pcur, (_Obj*)__pcur->_M_end + sizeof(_Node_alloc_Mem_block_Huge)/* + sizeof(_Node_alloc_Mem_block)*/);
//
//						if (tmp_count[i] >1 )
//						{
//							size_t copied = &(tmp_free_list[i][tmp_free_list_size[i]]) - tmp_high[i];// + 1;
//							tmp_free_list_size[i] -=  tmp_count[i];
//							memcpy(tmp_low[i], tmp_high[i]/* + 1*/, copied * sizeof(_Obj*));
//						}
//						else
//						{
//							tmp_free_list_size[i] = 0;
//						}
//					}
//				}
//
//				_S_freememblocks_delete_inside(0, (_Obj*)__pcur, (_Obj*)__pcur->_M_end);
//
//				// delete from S_chunks
//				if (__pfirst == __pcur)
//				{
//					//ATOMIC_CAS(_S_chunks, __pcur->_M_next, __pcur);
//					_S_chunks = __pcur->_M_next;
//					__pfirst = _S_chunks;
//				}
//				else
//				{
//					__cas_new_head(&__pnext, __pfirst);
//				}
//				_S_heap_size -= _Size/*cursize*/;
//				_S_wasted_in_blocks -= _Size;
//
//
//				// free memory
//				//_free(__pcur);
//				allocator.pool_free(__pcur);
//				--_S_allocations;
//			}
//
//			if (!bdelete)
//			{
//				__pfirst = __pcur;
//			}
//		}
//		__pcur = __pnext;
//	}
//
//
//	for (size_t i =0; i < NFREELISTS; ++i)
//	{
//		if (tmp_free_list[i])
//		{
//			// restore bew free_list
//			_Obj * volatile * __my_free_list = _S_free_list + i;
//			_S_freelist_counter[i] = tmp_free_list_size[i];
//
//			if (tmp_free_list_size[i] > 0)
//			{
//				_Obj * _pcur = tmp_free_list[i][0];
//				*__my_free_list = _pcur;
//				for (size_t j = 1; j < tmp_free_list_size[i]; ++j)
//				{
//					_pcur->_M_next = tmp_free_list[i][j];
//					_pcur = _pcur->_M_next;
//				}
//				_pcur->_M_next = 0;
//			}
//			else
//			{
//				*__my_free_list = 0;
//			}
//
//			allocator.cleanup_free(tmp_free_list[i]);
//			//::delete [] tmp_free_list[i];
//		}
//	}
//
//}
//
//
//#undef S_FREELIST_INDEX
//
////#endif // WIN32|DEBUG
//
//#endif
