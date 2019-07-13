//---------------------------------------------------------------------------
// Copyright 2006 Crytek GmbH
// Created by: Michael Smith
//---------------------------------------------------------------------------
#ifndef __STLPOOLALLOCATOR_H__
#define __STLPOOLALLOCATOR_H__

//---------------------------------------------------------------------------
// STL-compatible interface for the pool allocator (see PoolAllocator.h).
//
// This class is suitable for use as an allocator for STL lists. Note it will
// not work with vectors, since it allocates fixed-size blocks, while vectors
// allocate elements in variable-sized contiguous chunks.
//
// To create a list of type UserDataType using this allocator, use the
// following syntax:
//
// std::list<UserDataType, STLPoolAllocator<UserDataType> > myList;
//---------------------------------------------------------------------------

#include "PoolAllocator.h"
#include <stddef.h>
#include <climits>

namespace stl
{
	namespace STLPoolAllocatorHelper{
		inline void destruct(char *){}
		inline void destruct(wchar_t*){}
		template <typename T> 
			inline void destruct(T *t){t->~T();}
	}

	template <size_t SZ, class L> struct STLPoolAllocatorStatic
	{
		static PoolAllocator<SZ, L> * allocator;
	};

	template <class T, class L> struct STLPoolAllocatorKungFu : public STLPoolAllocatorStatic<sizeof(T),L>
	{
	};

	template <class T, class L = PoolAllocatorSynchronizationMultithreaded> class STLPoolAllocator
	{
	public:
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
		typedef T*        pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef T         value_type;
		template <class U> struct rebind
		{
			typedef STLPoolAllocator<U, L> other;
		};

		STLPoolAllocator() throw()
		{
		}

		STLPoolAllocator(const STLPoolAllocator&) throw()
		{
		}

		template <class U, class M> STLPoolAllocator(const STLPoolAllocator<U,M>&) throw()
		{
		}

		~STLPoolAllocator() throw()
		{
		}

		pointer address(reference x) const
		{
			return &x;
		}

		const_pointer address(const_reference x) const
		{
			return &x;
		}

		pointer allocate(size_type n, const void* hint = 0)
		{
			if (!STLPoolAllocatorKungFu<T,L>::allocator)
				STLPoolAllocatorKungFu<T,L>::allocator = new PoolAllocator<sizeof(T),L>;
			assert(n == 1);
			return static_cast<T*>(STLPoolAllocatorKungFu<T,L>::allocator->Allocate());
		}

		void deallocate(pointer p, size_type n)
		{
			assert(n == 1);
			STLPoolAllocatorKungFu<T,L>::allocator->Deallocate(p);
		}

		size_type max_size() const throw()
		{
			return INT_MAX;
		}

		void construct(pointer p, const T& val)
		{
			new(static_cast<void*>(p)) T(val);
		}

		void construct(pointer p)
		{
			new(static_cast<void*>(p)) T();
		}

		void destroy(pointer p)
		{
			STLPoolAllocatorHelper::destruct(p);
		}

		bool operator==(const STLPoolAllocator&) {return true;}
	};

	template <> class STLPoolAllocator<void>
	{
	public:
		typedef void* pointer;
		typedef const void* const_pointer;
		typedef void value_type;
		template <class U, class L> 
		struct rebind { typedef STLPoolAllocator<U> other; };
	};    


	template <size_t SZ, typename L> PoolAllocator<SZ,L> * STLPoolAllocatorStatic<SZ,L>::allocator;
}

#endif //__STLPOOLALLOCATOR_H__
