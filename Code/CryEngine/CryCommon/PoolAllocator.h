//---------------------------------------------------------------------------
// Copyright 2006 Crytek GmbH
// Created by: Michael Smith
//---------------------------------------------------------------------------
#ifndef __POOLALLOCATOR_H__
#define __POOLALLOCATOR_H__

//---------------------------------------------------------------------------
// Memory allocation class. Allocates fixed-size blocks of memory, a scheme
// sometimes known as Simple Segregated Memory.
//
// Allocation is amortized constant time. The normal case is very fast -
// basically just a couple of dereferences. If many blocks are allocated,
// the system may occasionally need to allocate a further bucket of blocks
// for itself. Deallocation is strictly fast constant time.
//
// There is no per-block space overhead. The free list mechanism uses the
// memory of the block itself when it is deallocated. Each block immediately
// follows the preceding one in memory. Individual blocks are aligned to 4
// byte boundaries - buckets are aligned to 16 byte boundaries. This means
// that if 16 byte alignment is required for each block, this can be achieved
// by ensuring that blocks are multiples of 16 bytes in size.
//
// In this implementation memory claimed by the system is never deallocated,
// until the entire allocator is deallocated. This is to ensure fast
// allocation/deallocation - reference counting the bucket quickly would
// require a pointer to the bucket be stored, whereas now no memory is used
// while the block is allocated.
//
// This allocator is suitable for use with STL lists - see STLPoolAllocator
// for an STL-compatible interface.
//
// The class can optionally support multi-threading, using the second
// template parameter. By default no synchronization is performed. For
// alternative values for the second argument, see
// PoolAllocatorSynchronization.h.
//---------------------------------------------------------------------------

#include "PoolAllocatorSynchronization.h"

// define to have additional checks against pointers being valid for this pool
// (like, ensuring deallocation to the correct pool)
//#define CHECK_POOL_ALLOCATED_POINTERS

#ifdef CHECK_POOL_ALLOCATED_POINTERS
#include "CryArray.h"
#endif

namespace stl
{
	template <int S, typename L = PoolAllocatorSynchronizationMultithreaded>
	class PoolAllocator
	{
	public:
		enum {ObjectSize = S};
		typedef L Synchronization;

		// Objects should be aligned to a 4-byte boundary - therefore make sure that objects are
		// rounded up to a multiple of 4.
		enum {ObjectAlignment = sizeof(void*)};
		enum {ObjectAllocationSize = (ObjectSize + ObjectAlignment - 1) & ~(ObjectAlignment - 1)};
		enum {DefaultBucketSize = 128};
		enum {BucketAlignment = 16};

		// Declare a type that is the same size as an allocated node in a bucket.
		template <int S1, int DUMMY> class TObjectNode
		{
		public:
			TObjectNode* pNext;

			// Have this padding to make sure the size of the object node is the same as the
			// allocation size of the objects.
			unsigned char padding[ObjectAllocationSize - sizeof(TObjectNode*)];
		};

		// Specialized type that is exactly 4 bytes - needed to avoid compiler warnings by
		// not allocating arrays of 0 length.
		template <int DUMMY> class TObjectNode<sizeof(void*), DUMMY>
		{
		public:
			TObjectNode* pNext;
		};

		typedef TObjectNode<S, 42> ObjectNode;

		class BucketNode
		{
		public:
			BucketNode* pNext;

			// We want the start of the objects to be aligned to 16 bytes. It would
			// be possible not to have this padding, and instead have the pNext pointer
			// after all the objects, but this would be more fiddly to have, and would
			// probably not save any memory, since the underlying allocator will round the
			// block off to 16 bytes anyway.
			unsigned char padding[BucketAlignment - sizeof(BucketNode*)];

			// The start of the objects in the bucket.
			ObjectNode objectNodes[1];
		};

		PoolAllocator(int nBucketSize = DefaultBucketSize);
		~PoolAllocator();

		void* Allocate();
		void Deallocate(void* pObject);

		// Free memory used by pool.
		// Use with care, all allocated and free objects of this pool are getting released.
		void FreeMemory();

		size_t GetTotalAllocatedNodeSize();
		size_t GetTotalAllocatedMemory();

		void CheckPtr(void * p, bool allowNull = true);

	private:
		ObjectNode* GrowBucket();

		Synchronization synchronization;
		const int nBucketSize;

		typedef typename L::template VolatilePtr<BucketNode>::type BucketList;
		typedef typename L::template VolatilePtr<ObjectNode>::type ObjectList;
		typedef typename L::template VolatilePtr<void>::type VoidList;

		BucketList pBucketList;
		ObjectList pFreeList;

#ifdef CHECK_POOL_ALLOCATED_POINTERS
		typedef std::pair<void*,void*> PointerRange;
		typedef DynArray<PointerRange> PointerRanges;
		PointerRanges legalPointers;
#endif
	};

	template <int S, typename L> ILINE void PoolAllocator<S, L>::CheckPtr(void * p, bool allowNull)
	{
		// check for 4 byte alignment
		assert((((UINT_PTR)p)&3)==0);
		// check that we're not in page 0 (unless we can be NULL)
		if (allowNull)
			assert(((UINT_PTR)p)>4095 || !p);
		else
			assert(((UINT_PTR)p)>4095);
#ifdef CHECK_POOL_ALLOCATED_POINTERS
		if (p)
		{
			Synchronization::CLock lk(synchronization);
			for (PointerRanges::iterator it = legalPointers.begin(); it != legalPointers.end(); ++it)
			{
				// allow up to the end of the block+1
				if (p >= it->first && p <= it->second)
					return;
			}
			assert(!"pointer not allocated by this pool allocator");
		}
#endif
	}

	template <int S, typename L> inline PoolAllocator<S, L>::PoolAllocator(int nBucketSize)
	:	nBucketSize(nBucketSize),
		pBucketList(0),
		pFreeList(0)
	{
	}

	template <int S, typename L> inline PoolAllocator<S, L>::~PoolAllocator()
	{
		this->FreeMemory();
	}

	template <int S, typename L> ILINE void* PoolAllocator<S, L>::Allocate()
	{
		// Lock the allocator to manipulate state atomically (linux only).
		typename Synchronization::CLock lock(this->synchronization);

		CheckPtr(this->pFreeList);
		// Remove the first object from the free list and return it - needs to be atomic.
		ObjectNode* pOldFreeList = 0;
		ObjectNode* pNext = 0;
		do
		{
			pOldFreeList = this->pFreeList;

			// If we have allocated all the nodes in all the buckets, we should
			// allocate another bucket of nodes now.
			if (pOldFreeList == 0)
				pOldFreeList = this->GrowBucket();

			pNext = pOldFreeList->pNext;
		}
		while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pFreeList, pNext, pOldFreeList) != pOldFreeList);

		CheckPtr(pOldFreeList);
		return pOldFreeList;
	}

	template <int S, typename L> ILINE void PoolAllocator<S, L>::Deallocate(void* pObject)
	{
		CheckPtr(pObject);
		CheckPtr(this->pFreeList);
		// Lock the allocator to manipulate state atomically (linux only).
		typename Synchronization::CLock lock(this->synchronization);

		// Add the object to the front of the free list - needs to be atomic.
		ObjectNode* pOldFreeList = 0;
		do
		{
			pOldFreeList = this->pFreeList;
			static_cast<ObjectNode*>(pObject)->pNext = pOldFreeList;
		}
		while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pFreeList, static_cast<ObjectNode*>(pObject), pOldFreeList) != pOldFreeList);
		CheckPtr(this->pFreeList);
	}

	// NB: This function may return incorrect values if called concurrently from different threads.
	template <int S, typename L> size_t PoolAllocator<S, L>::GetTotalAllocatedNodeSize()
	{
		// Since we don't have pointers to the currently allocated nodes, we cannot calculate
		// the size directly. Instead, we have to find the total amount of memory we have
		// allocated, and subtract the free blocks and bucket overhead.

		// Find the total allocated memory.
		size_t uTotalAllocatedMemory = this->GetTotalAllocatedMemory();

		size_t uTotalFreeSize;
		size_t uBucketOverhead;
		{
			// Lock the allocator to manipulate state atomically (linux only).
			typename Synchronization::CLock lock(this->synchronization);

			// Temporarily remove the free list from the object so we can safely iterate through it - needs to be atomic.
			ObjectNode* pFreeList = 0;
			do
			{
				pFreeList = this->pFreeList;
			}
			while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pFreeList, 0, pFreeList) != pFreeList);

			// Count the total size of free nodes.
			uTotalFreeSize = 0;
			ObjectNode* pFreeListTail = 0;
			for (ObjectNode* pNode = pFreeList; pNode != 0; pNode = pNode->pNext)
				uTotalFreeSize += sizeof(ObjectNode), pFreeListTail = pNode;

			// Restore the free list to the object - needs to be atomic.
			if (pFreeListTail)
			{
				ObjectNode* pOldFreeList = 0;
				do
				{
					pOldFreeList = this->pFreeList;
					pFreeListTail->pNext = pOldFreeList;
				}
				while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pFreeList, pFreeList, pOldFreeList) != pOldFreeList);
			}

			// Temporarily remove the bucket list from the object so we can safely iterate through it - needs to be atomic.

			BucketNode* pBucketList = 0;
			do
			{
				pBucketList = this->pBucketList;
			}
			while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pBucketList, 0, pBucketList) != pBucketList);

			// Count the total amount of bucket overhead
			uBucketOverhead = 0;
			BucketNode* pBucketListTail = 0;
			for (BucketNode* pNode = pBucketList; pNode != 0; pNode = pNode->pNext)
			{
				uBucketOverhead += sizeof(BucketNode) - sizeof(ObjectNode);
				pBucketListTail = pNode;
			}

			// Restore the free list to the object - needs to be atomic.
			if (pBucketListTail)
			{
				BucketNode* pOldBucketList = 0;
				do
				{
					pOldBucketList = this->pBucketList;
					pBucketListTail->pNext = pOldBucketList;
				}
				while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pBucketList, pBucketList, pOldBucketList) != pOldBucketList);
			}
		}

		// The total node memory is the total amount of memory allocated minus the free nodes and
		// the bucket overhead
		return uTotalAllocatedMemory - uTotalFreeSize - uBucketOverhead;
	}

	template <int S, typename L> size_t PoolAllocator<S, L>::GetTotalAllocatedMemory()
	{
		// Lock the allocator to read state atomically (linux only).
		typename Synchronization::CLock lock(this->synchronization);

		// Calculate the total amount of allocated memory, including bucket headers.
		int nBucketSizeInBytes = sizeof(BucketNode) - sizeof(ObjectNode) + (this->nBucketSize * sizeof(ObjectNode));

		// Temporarily remove the bucket list from the object so we can safely iterate through it - needs to be atomic.
		BucketNode* pBucketList = 0;
		do 
		{
			pBucketList = this->pBucketList;
		}
		while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pBucketList, 0, pBucketList) != pBucketList);

		// Count the number of buckets.
		int nNumBuckets = 0;
		BucketNode* pBucketListTail = 0;
		for (BucketNode* pNode = pBucketList; pNode != 0; pNode = pNode->pNext)
			++nNumBuckets, pBucketListTail = pNode;

		// Restore the free list to the object - needs to be atomic.
		if (pBucketListTail)
		{
			BucketNode* pOldBucketList = 0;
			do
			{
				pOldBucketList = this->pBucketList;
				pBucketListTail->pNext = pOldBucketList;
			}
			while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pBucketList, pBucketList, pOldBucketList) != pOldBucketList);
		}

		return nNumBuckets * nBucketSizeInBytes;
	}

	template <int S, typename L> inline typename PoolAllocator<S, L>::ObjectNode* PoolAllocator<S, L>::GrowBucket()
	{
		// We will rely on the underlying allocator's behaviour to align this memory
		// to the strictest alignment requirements for data types on this platform.
		int nBucketSizeInBytes = sizeof(BucketNode) - sizeof(ObjectNode) + (this->nBucketSize * sizeof(ObjectNode));

		// Allocate the new bucket of the required size.
		BucketNode* pBucketNode = reinterpret_cast<BucketNode*>(new unsigned char [nBucketSizeInBytes]);

#ifdef CHECK_POOL_ALLOCATED_POINTERS
		{
			Synchronization::CLock lk(synchronization);
			legalPointers.push_back( PointerRange( pBucketNode, ((unsigned char *)pBucketNode) + nBucketSizeInBytes ) );
		}
#endif

		// Loop through all the nodes in the bucket, adding them to the free list.
		ObjectNode* pNewFreeList = 0;
		ObjectNode* pNewFreeListTail = &pBucketNode->objectNodes[0];
		for (int nObject = 0; nObject < this->nBucketSize; ++nObject)
		{
			ObjectNode* pObjectNode = &pBucketNode->objectNodes[nObject];
			pObjectNode->pNext = pNewFreeList;
			pNewFreeList = pObjectNode;
		}

		// Add the bucket to the front of the list of buckets - needs to be atomic.
		BucketNode* pOldBucketList = 0;
		do
		{
			pOldBucketList = this->pBucketList;
			pBucketNode->pNext = pOldBucketList;
		}
		while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pBucketList, pBucketNode, pOldBucketList) != pOldBucketList);

		// Prepend the new nodes to the front of the free list - needs to be atomic.
		ObjectNode* pOldFreeList = 0;
		do
		{
			pOldFreeList = this->pFreeList;
			pNewFreeListTail->pNext = pOldFreeList;
		}
		while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pFreeList, pNewFreeList, pOldFreeList) != pOldFreeList);

		return pNewFreeList;
	}

	// NB: It's really a bad idea to call this when other threads might still be using it.
	template <int S, typename L> inline void PoolAllocator<S, L>::FreeMemory()
	{
		// Lock the allocator to manipulate state atomically (linux only).
		typename Synchronization::CLock lock(this->synchronization);

		// Remove the buckets from the object - needs to be atomic.
		BucketNode* pBucketNode = 0;
		do 
		{
			pBucketNode = this->pBucketList;
			this->pFreeList = 0; // Potentially dodgy if other threads are still using this object - ie DONT CALL THIS FUNC if other threads are still using us.
		}
		while (Synchronization::AtomicCompareAndSwapPtr((VoidList*)&this->pBucketList, 0, pBucketNode) != pBucketNode);

		// Loop through all the buckets, freeing the memory.
		while (pBucketNode != 0)
		{
			// We must be careful to read the next bucket pointer before we free
			// the bucket - otherwise we will be reading from deleted and possibly
			// cleared memory.
			BucketNode* pNextBucketNode = pBucketNode->pNext;

			// Delete the current bucket.
			delete [] reinterpret_cast<unsigned char*>(pBucketNode);

			// Move to the next bucket in the list.
			pBucketNode = pNextBucketNode;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	template <int S>
	class PoolAllocatorNoMT : public PoolAllocator<S,PoolAllocatorSynchronizationSinglethreaded>
	{
	public:
		PoolAllocatorNoMT(int nBucketSize = PoolAllocator<S>::DefaultBucketSize)
			: PoolAllocator<S,PoolAllocatorSynchronizationSinglethreaded>(nBucketSize) {}
	};

	//////////////////////////////////////////////////////////////////////////
	template<class T>
	class TPoolAllocator: public PoolAllocator<sizeof(T)>
	{
	public:
		TPoolAllocator(int nBucketSize = PoolAllocator<sizeof(T)>::DefaultBucketSize)
			: PoolAllocator<sizeof(T)>(nBucketSize)
		{}

		void Delete(T* ptr)
		{
			if (ptr)
			{
				ptr->~T();
				Deallocate(ptr);
			}
		}
	};

}

template<class T>
void* operator new(size_t size, stl::TPoolAllocator<T>& pool)
{
	assert(size == sizeof(T));
	return pool.Allocate();
}

// This delete op is only called if the T() constructor throws an exception (which in our code is never).
// It's here to avoid a compiler warning.
template<class T>
void operator delete(void* ptr, stl::TPoolAllocator<T>& pool)
{
	pool.Deallocate(ptr);
}


#endif //__POOLALLOCATOR_H__
