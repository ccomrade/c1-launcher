//---------------------------------------------------------------------------
// Copyright 2006 Crytek GmbH
// Created by: Michael Smith
//---------------------------------------------------------------------------
#ifndef __POOLALLOCATORSYNCHRONIZATION_H__
#define __POOLALLOCATORSYNCHRONIZATION_H__

//---------------------------------------------------------------------------
// Synchronization policies for PoolAllocator (see PoolAllocator.h).
//
// These can be used to make the pool allocator thread-safe. They specify
// the method of synchronization used. The default is to use no
// synchronization.
//
// To use one of these, pass the class as the second template argument when
// instantiating PoolAllocator. For instance:
//
// stl::PoolAllocator<sizeof(ObjectToAllocate), stl::PoolAllocatorSynchronizationMultithreaded> allocator;
//---------------------------------------------------------------------------

#include "MultiThread.h"

namespace stl
{
	// Multi-threaded synchronization policy class for pool allocator - uses CCryMutex.
	class PoolAllocatorSynchronizationMultithreaded
	{
	public:
#if /*defined(_WIN64) || defined(WIN64) || */defined(_WIN32) || defined(WIN32) || defined(XENON) || defined(PS3)
		// In windows the pool allocator can be lock-free.
		class CLock
		{
		public:
			CLock(PoolAllocatorSynchronizationMultithreaded& synchronizer)
			{
			}

		private:
			CLock( const CLock& );
			CLock& operator=( const CLock& );
		};
#else
			// Since there is currently no CryInterlockedCompareExchangePtr() we will have to use locks for non-windows
			// environments.
		PoolAllocatorSynchronizationMultithreaded() : m_lockCounter(0) {}

		class CLock
		{
		public:
			CLock(PoolAllocatorSynchronizationMultithreaded& synchronizer) : lock(synchronizer.m_lockCounter) {}

		private:
			CLock( const CLock& );
			CLock& operator=( const CLock& );

			WriteLock lock;
		};
		volatile int m_lockCounter;
#endif

		static void* AtomicCompareAndSwapPtr(void* volatile* dst, void* val, void* comp)
		{
			return CryInterlockedCompareExchangePointer(dst, val, comp);
		}

		// In multi-threaded situations list pointers need to be volatile.
		template <typename T> struct VolatilePtr {typedef T* volatile type;};
	};

	// Single-threaded synchronization policy class for pool allocator - performs no synchronization.
	class PoolAllocatorSynchronizationSinglethreaded
	{
	public:
		class CLock
		{
		public:
			CLock(PoolAllocatorSynchronizationSinglethreaded& synchronizer)
			{
			}

		private:
			CLock( const CLock& );
			CLock& operator=( const CLock& );
		};

		static void* AtomicCompareAndSwapPtr(void** dst, void* val, void* comp)
		{
			void* res = *dst;
			if (comp == res)
				*dst = val;
			return res;
		}

		// In single-threaded situations there is no need for volatile keyword.
		template <typename T> struct VolatilePtr {typedef T* type;};
	};
}

#endif //__POOLALLOCATORSYNCHRONIZATION_H__
