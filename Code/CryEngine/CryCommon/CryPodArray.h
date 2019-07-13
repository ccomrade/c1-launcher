////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CryPodArray.h
//  Created:     28/5/2002 by Vladimir Kajalin
//  Description: Simple POD types container
// -------------------------------------------------------------------------
//  History:
//      Refactored 07/03/2007 by Timur.
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CRY_POD_ARRAY_H__
#define __CRY_POD_ARRAY_H__
#pragma once

//////////////////////////////////////////////////////////////////////////
// POD Array
// vector like class (random access O(1)) without construction/destructor/copy constructor/assignment handling
//////////////////////////////////////////////////////////////////////////
template <class T>
class PodArray
{
  T * m_pElements;
  int m_nCount;
  int m_nAllocatedCount;

public:
	typedef T        value_type;
	typedef T*       iterator;
	typedef const T* const_iterator;

	//////////////////////////////////////////////////////////////////////////
	// STL compatible interface
	//////////////////////////////////////////////////////////////////////////
	void resize (size_t numElements)
	{
		int nOldCount = m_nCount;
		PreAllocate ((int)numElements, (int)numElements);
		assert (numElements == size());
		for (int nElement = nOldCount; nElement < m_nCount; ++nElement)
			(*this)[nElement] = T();
	}
	//////////////////////////////////////////////////////////////////////////
	ILINE void reserve (unsigned numElements) { PreAllocate ((int)numElements); }
	ILINE void push_back (const T& rElement) { Add (rElement); }
	ILINE size_t size() const	{ return (size_t)m_nCount; }
	ILINE size_t capacity() const { return (size_t)m_nAllocatedCount; }

	ILINE void clear() { Clear(); }
	ILINE T* begin() { return m_pElements; }
	ILINE T* end() { return m_pElements+m_nCount; }
	ILINE const T* begin()const { return m_pElements; }
	ILINE const T* end()const { return m_pElements+m_nCount; }
	ILINE bool empty() const { return m_nCount==0; }
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	PodArray() : m_nCount(0),m_pElements(0),m_nAllocatedCount(0) {}
	PodArray( const PodArray<T> &from ) : m_nCount(0),m_pElements(0),m_nAllocatedCount(0) { AddList( from ); }
  ~PodArray() { Free(); }

  void Reset() { Free(); }
	void Free()
  {    
    if(m_pElements)
      free (m_pElements); 
    m_pElements=0;
    m_nCount=0; 
    m_nAllocatedCount= 0; 
  }

  ILINE void Clear() { m_nCount=0; }

  int Find(const T & p)
  {
    for(int i=0; i<m_nCount; i++)
      if(p == (*this)[i])
        return i;

    return -1;;
  }

  inline void AddList(const PodArray<T> & lstAnother)
  {
    PreAllocate(m_nCount + lstAnother.Count());

    memcpy(&m_pElements[m_nCount],&lstAnother.m_pElements[0],sizeof(m_pElements[0])*lstAnother.Count());

    m_nCount += lstAnother.Count();
  }

  inline void AddList(T * pAnotherArray, int nAnotherCount)
  {
    PreAllocate(m_nCount + nAnotherCount);

    memcpy(&m_pElements[m_nCount],pAnotherArray,sizeof(m_pElements[0])*nAnotherCount);

    m_nCount += nAnotherCount;
  }

  ILINE void Add(const T & p)
  {
    if( m_nCount >= m_nAllocatedCount )
    {
      assert(&p<m_pElements || &p>=(m_pElements+m_nAllocatedCount));
      m_nAllocatedCount = m_nCount*2 + 8;
      m_pElements = (T*)realloc(m_pElements,m_nAllocatedCount*sizeof(T));
      assert(m_pElements);
    }

    memcpy(&m_pElements[m_nCount],&p,sizeof(m_pElements[m_nCount]));
    m_nCount++;
  }

  void InsertBefore(const T & p, const unsigned int nBefore)
  {
    assert( nBefore>=0 && nBefore<=(unsigned int)m_nCount );
		T tmp; Add(tmp); // add empty object to increase memory buffer
    memmove(&(m_pElements[nBefore+1]), &(m_pElements[nBefore]), sizeof(T)*(m_nCount-nBefore-1));
    m_pElements[nBefore] = p;
  }

  void PreAllocate(int elem_count, int nNewCount = 0)
  {
    if( elem_count > m_nAllocatedCount )
    {
      m_nAllocatedCount = elem_count;

      T * new_elements = (T*)malloc(m_nAllocatedCount*sizeof(T));
      assert(new_elements);
      memset(new_elements, 0, sizeof(T)*m_nAllocatedCount);
      memcpy(new_elements, m_pElements, sizeof(T)*m_nCount);
      if(m_pElements)
        free (m_pElements);
      m_pElements = new_elements;
    }
    
    if(nNewCount)
      m_nCount = nNewCount;
  }

  inline void Delete(const int nElemId, const int nElemCount = 1)
  {
    assert( nElemId >= 0 && nElemId+nElemCount <= m_nCount );
    memmove(&(m_pElements[nElemId]), &(m_pElements[nElemId+nElemCount]), sizeof(T)*(m_nCount-nElemId-nElemCount));
    m_nCount-=nElemCount;
  }

  inline void DeleteFastUnsorted(const int nElemId, const int nElemCount = 1)
  {
		assert( nElemId >= 0 && nElemId+nElemCount <= m_nCount );
		memmove(&(m_pElements[nElemId]), &(m_pElements[m_nCount-nElemCount]), sizeof(T)*nElemCount);
		m_nCount-=nElemCount;
  }

  inline bool Delete(const T & del)
  {
		bool bFound = false;    
		for( int i=0; i<Count(); i++ )
    if(m_pElements[i] == del)
    { 
      Delete(i); 
      i--; 
			bFound = true;
    }
		return bFound;
  }

  ILINE  int Count() const { return m_nCount; }
  ILINE  unsigned int Size() const { return m_nCount; }
  
  ILINE  int IsEmpty() const { return m_nCount==0; }

  ILINE  T & operator [] (int i) const { assert( i>=0 && i<m_nCount ); return m_pElements[i]; }
  ILINE  T & GetAt (int i) const { assert( i>=0 && i<m_nCount ); return  m_pElements[i]; }
  ILINE  T * Get   (int i) const { assert( i>=0 && i<m_nCount ); return &m_pElements[i]; }
  ILINE  T * GetElements() { return m_pElements; }
	ILINE  unsigned int GetDataSize() const { return m_nCount*sizeof(T); }

  T & Last() const { assert(m_nCount); return m_pElements[m_nCount-1]; }
  ILINE void DeleteLast() { assert(m_nCount); m_nCount--; }

  PodArray<T>& operator = ( const PodArray<T> & source_list)
  {
    Free();
		AddList( source_list );
    
    return *this;
  }
  
	//////////////////////////////////////////////////////////////////////////
	// Return true if arrays have the same data.
  bool Compare( const PodArray<T> &l ) const
  {
    if(Count() != l.Count())
      return 0;

    if(memcmp(m_pElements, l.m_pElements, m_nCount*sizeof(T))!=0)
      return 0;

    return 1;
  }
};

#endif // __CRY_POD_ARRAY_H__
