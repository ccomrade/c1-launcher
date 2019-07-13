//---------------------------------------------------------------------------
// Copyright 2006 Crytek GmbH
// Created by: Craig Tiller
//---------------------------------------------------------------------------
#ifndef __STLPOOLALLOCATOR_MANYELEMS_H__
#define __STLPOOLALLOCATOR_MANYELEMS_H__

//---------------------------------------------------------------------------
// STL-compatible interface for the pool allocator (see PoolAllocator.h).
//
// this class acts like STLPoolAllocator, but it is also usable for vectors
// which means that it can be used as a more efficient allocator for many
// implementations of hash_map (typically this uses internally a vector and
// a list with the same allocator)
//---------------------------------------------------------------------------

#include "PoolAllocator.h"
#include "STLPoolAllocator.h"
#include <stddef.h>
#include <climits>

namespace stl
{
	template <class T, class L = PoolAllocatorSynchronizationMultithreaded> class STLPoolAllocator_ManyElems
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
			typedef STLPoolAllocator_ManyElems<U, L> other;
		};

		STLPoolAllocator_ManyElems() throw()
		{
		}

		STLPoolAllocator_ManyElems(const STLPoolAllocator_ManyElems&) throw()
		{
		}

		template <class U, class M> STLPoolAllocator_ManyElems(const STLPoolAllocator_ManyElems<U,M>&) throw()
		{
		}

		~STLPoolAllocator_ManyElems() throw()
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

		void deallocate(pointer p, size_type n)
		{
			if (n == 1)
				stl::STLPoolAllocatorKungFu<T,L>::allocator->Deallocate(p);
			else
				CryModuleFree( p );
		}

		pointer allocate(size_type n, const void* hint = 0)
		{
			if (n == 1)
			{
				if (!STLPoolAllocatorKungFu<T,L>::allocator)
					STLPoolAllocatorKungFu<T,L>::allocator = new PoolAllocator<sizeof(T),L>;
				return static_cast<T*>(STLPoolAllocatorKungFu<T,L>::allocator->Allocate());
			}
			else
				return static_cast<T*>(CryModuleMalloc( n * sizeof(T) ));
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

		bool operator==(const STLPoolAllocator_ManyElems&) {return true;}
	};

	template <> class STLPoolAllocator_ManyElems<void>
	{
	public:
		typedef void* pointer;
		typedef const void* const_pointer;
		typedef void value_type;
		template <class U, class L> 
		struct rebind { typedef STLPoolAllocator_ManyElems<U> other; };
	};    
}

#endif //__STLPOOLALLOCATOR_H__
