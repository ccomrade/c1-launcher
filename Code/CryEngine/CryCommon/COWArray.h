// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _COW_ARRAY_H_
#define _COW_ARRAY_H_

#include "CryArray.h"

//---------------------------------------------------------------------------
// Dynamically growable copy-on-write array container.
// Size and capacity are stored at front of allocated memory, 
// so size of empty array is just a pointer.
// Assumes T is movable.

template<class T, class A = FModuleAlloc>
struct COWArray
{
	// Base storage.
	typedef T						value_type;
	typedef T*					iterator;
	typedef const T*		const_iterator;
	typedef int					size_type;

	// Construction.
	inline COWArray()
		: m_aElems(null())
	{
		add_ref();
	}
	inline COWArray(COWArray<T,A> const& a)
		: m_aElems(a.m_aElems)
	{
		add_ref();
	}
	
	COWArray<T>& operator=(COWArray<T,A> const& a)
	{
		COWArray<T,A> temp(a);
		swap(temp);
		return *this;
	}
	inline ~COWArray()
	{
		release();
	}

	// Accessors.
	inline bool empty() const								{ return size() == 0; }
	inline size_type size() const						{ return header()->nSize; }
	inline size_type capacity() const				{ return header()->nCapacity; }

	inline T* begin()												
	{ 
		update(); 
		return m_aElems; 
	}
	inline const T* begin() const						{ return m_aElems; }

	inline value_type front()								
	{ 
		update(); 
		return *m_aElems; 
	}
	inline const value_type front() const		{ return *m_aElems; }

	inline T* end()													
	{ 
		update(); 
		return begin()+size(); 
	}
	inline const T* end() const							
	{ 
		return begin()+size(); 
	}

	inline value_type back()								
	{ 
		update(); 
		return *(begin()+size()-1); 
	}
	inline const value_type back() const		{ return *(begin()+size()-1); }

	inline T const& operator [] (int i) const
	{
		assert(i >= 0 && i < (int)size());
		return begin()[i];
	}
	inline T& operator [] (int i)
	{
		update(); 
		assert(i >= 0 && i < (int) size());
		return begin()[i];
	}

	const T& get(int i) const
	{
		return (*this)[i];
	}

	// Mutators.

	void resize(size_type nCount)
	{
		update_size(); 
		int nOld = size();
		destroy(begin()+nCount, end());
		resize_raw(nCount);
		init(begin()+nOld, end());
	}

	void resize(size_type nCount, const T& value)
	{
		update_size(); 
		int nOld = size();
		destroy(begin()+nCount, end());
		resize_raw(nCount);
		init(begin()+nOld, end(), value);
	}

	void reserve(size_type nCount)
	{
		update_size(); 
		if (nCount > capacity())
			resize_raw(size(), nCount);
	}

	iterator insert(iterator at)
	{
		update_size(); 
		iterator it = insert_raw(at);
		return new(it) T;
	}
	iterator insert(iterator at, T const& val)
	{
		update_size(); 
		iterator it = insert_raw(at);
		return new(it) T(val);
	}

	inline iterator push_back()									
	{ 
		update_size(); 
		resize_raw(size()+1);
		return new(end()-1) T;
	}
	inline iterator push_back(T const& val)
	{ 
		update_size(); 
		resize_raw(size()+1);
		return new(end()-1) T(val);
	}
	inline void pop_back()
	{
		update_size(); 
		if (!empty())
		{
			erase( size()-1 );
		}
	}

	iterator erase(iterator first, iterator final)
	{
		update_size(); 
		int pos = first-begin();
		int count = final-first;

		// Destroy, then delete elems.
		destroy(first, final);
		move(first, end() - count, final);

		// Resize mem.
		resize_raw(size() - count);
		return begin()+pos;
	}

	iterator erase(iterator it)
	{
		update_size(); 
		return erase(it, it+1);
	}

	int erase(int i)
	{
		update_size(); 
		assert(i >= 0 && i < size());
		erase(begin() + i);
		return i-1;
	}

	void clear()
	{
		COWArray temp;
		swap(temp);
	}

	void swap( COWArray& other )
	{
		std::swap(m_aElems, other.m_aElems);
	}

protected:

	T*				m_aElems;

	struct Header
	{
		size_type	nCapacity, nSize, nRefs;
	};

	static T* null()
	{
		static Header empty = {0,0,0};
		return (T*)(&empty+1);
	}

	inline Header* header()
	{
		assert(m_aElems);
		return ((Header*)m_aElems)-1;
	}
	inline Header const* header() const
	{
		assert(m_aElems);
		return ((Header*)m_aElems)-1;
	}

	void add_ref()
	{
		if (capacity()) 
			header()->nRefs ++;
	}

	void release()
	{
		if (capacity())
		{
			if (0 >= --header()->nRefs)
			{
				destroy(begin(), end());
				A::Alloc(header(), 0);
				m_aElems = null();
			}
		}
	}

	void update()
	{
		if (header()->nRefs <= 1 || !capacity())
			return;
		clone();
	}

	void update_size()
	{
		update();
	}

	void clone()
	{
		assert(header()->nRefs > 1);
		header()->nRefs --;

		size_t size = header()->nSize;
		const T * data = m_aElems;

		m_aElems = null();
		add_ref();
		resize_raw( size );
		init( m_aElems, m_aElems+size, data );
	}

	inline void* mem()
	{
		return capacity() ? header() : 0;
	}

	void resize_raw(size_type nCount)
	{
		// Set new size, and automatically determine allocation count.
		if (capacity() == 0)
			// If first allocation, allocate exact size.
			resize_raw(nCount, nCount);
		else if (nCount > capacity())
		{
			// Grow if needed, to next power-of-two.





			// Grow if needed, to next power-of-two.
			size_type nCap = 4;
			nCap = 4;
			while (nCap < nCount)
				nCap <<= 1;
			resize_raw(nCount, nCap);

		}
		else
		{
			header()->nSize = nCount;
		}
	}

	void resize_raw(size_type nCount, size_type nAlloc)
	{
		// Set new size and allocation count.
		assert(nCount <= nAlloc);
		assert(m_aElems == null() || header()->nRefs <= 1);
		if (nAlloc != capacity())
		{
			if (nAlloc == 0)
			{
				A::Alloc(mem(), 0);
				m_aElems = null();
			}
			else
			{
				size_t nSize = sizeof(Header) + nAlloc * sizeof(T);
				void* newMem = A::Alloc(mem(), nSize);
				Header* head = (Header*)newMem;
				head->nCapacity = nAlloc;
				m_aElems = (T*)(head+1);
			}
		}
		if (nAlloc > 0)
		{
			header()->nRefs = 1;
			header()->nSize = nCount;
		}
	}

	iterator insert_raw(iterator at)
	{
		int pos = at-begin();

		// Realloc, then move elements.
		resize_raw(size()+1);
		move(begin()+pos+1, end(), begin()+pos);
		return begin()+pos;
	}

	void set_size(size_type nCount)
	{
		assert(nCount <= capacity());
		header()->nSize = nCount;
	}

	// Init a range of raw elements to default or copied values.
	static inline void init(T* first, T* final)
	{
		for (; first < final; first++)
			new(first) T;
	}

	static inline void init(T* first, T* final, T const& val)
	{
		for (; first < final; first++)
			new(first) T(val);
	}

	static inline void init(T* first, T* final, T const* src)
	{
		for (; first < final; first++)
			new(first) T(*src++);
	}

	// Copy values to a range of existing elements.
	static inline void copy(T* first, T* final, T const& val)
	{
		for (; first < final; first++)
			*first = val;
	}

	static inline void copy(T* first, T* final, T const* src)
	{
		for (; first < final; first++)
			*first = *src++;
	}

	// Move a range of move-invariant elements.
	static inline void move(T* first, T* final, T const* src)
	{
		memmove(first, src, (size_t)final-(size_t)first);
	}

	// Destroy a range of existing elements.
	static inline void destroy(T* first, T* final)
	{
		for (; first < final; first++)
			first->~T();
	}
};

#endif
