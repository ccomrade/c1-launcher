////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CryName.h
//  Version:     v1.00
//  Created:     6/10/2004 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CryName_h__
#define __CryName_h__
#pragma once

#include <ISystem.h>
#include <StlUtils.h>








class CNameTable;

struct INameTable
{
#if defined USE_WRONLY_NAMETABLE
	struct SNameEntry
	{
    char *GetStr() { return reinterpret_cast<char *>(this); }
    void AddRef() { }
    int Release() { return 1; }
    int GetMemoryUsage() { return strlen(GetStr()); }
  };
#else
	// Name entry header, immediately after this header in memory starts actual string data.
	struct SNameEntry
	{
		// Reference count of this string.
		int nRefCount;
		// Current length of string.
		int nLength;
		// Size of memory allocated at the end of this class.
		int nAllocSize;
		// Here in memory starts character buffer of size nAllocSize.
		//char data[nAllocSize]

		char* GetStr() { return (char*)(this+1); }
		void  AddRef() { nRefCount++; /*InterlockedIncrement(&_header()->nRefCount);*/};
		int   Release() { return --nRefCount; };
    int   GetMemoryUsage() { return sizeof(SNameEntry)+strlen(GetStr()); }
	};
#endif

	// Finds an existing name table entry, or creates a new one if not found.
  virtual SNameEntry* GetEntry( const char *str ) = 0;
	// Only finds an existing name table entry, return 0 if not found.
	virtual SNameEntry* FindEntry( const char *str ) = 0;
	// Release existing name table entry.
	virtual void Release( SNameEntry *pEntry ) = 0;
  virtual int GetMemoryUsage() = 0;
};

#if defined USE_WRONLY_NAMETABLE

#define CNAMETABLE_BLOCKSIZE (4096)

// If a block has less than CLOSE_THRESHOLD bytes left, then we'll close it
// (move it to the closed block list).
#define CNAMETABLE_CLOSE_THRESHOLD (64)

class CNameTable : public INameTable
{
#if defined CNAMETABLE_CASE_SENSITIVE
  typedef std::set<char *, stl::less_strcmp<char *> > TableT;
	static inline int compare(const char *s1, const char *s2)
	{ return strcmp(s1, s2); }
#else
  typedef std::set<char *, stl::less_stricmp<char *> > TableT;
	static inline int compare(const char *s1, const char *s2)
	{ return stricmp(s1, s2); }
#endif

  TableT m_Table;

  struct SNameTableBlock
  {
    SNameTableBlock *m_pNext;
    size_t m_Size;
    char *Data() { return reinterpret_cast<char *>(this + 1); }
  };

  SNameTableBlock *m_pBlocks;
  SNameTableBlock *m_pClosedBlocks;

  char *Alloc(const char *str)
  {
    SNameTableBlock *pBlock = m_pBlocks, *pPrevBlock = NULL;
    size_t len = strlen(str) + 1;

    if (len >= CNAMETABLE_BLOCKSIZE)
    {
      pBlock = reinterpret_cast<SNameTableBlock *>(
          malloc(sizeof(SNameTableBlock) + len));
      pBlock->m_pNext = m_pClosedBlocks;
      m_pClosedBlocks = pBlock;
      pBlock->m_Size = len;
      char *data = pBlock->Data();
      memcpy(data, str, len);
      return data;
    }
    while (pBlock != NULL)
    {
      if (len <= CNAMETABLE_BLOCKSIZE - pBlock->m_Size)
        break;
      pPrevBlock = pBlock;
      pBlock = pBlock->m_pNext;
    }
    if (pBlock == NULL)
    {
      pBlock = reinterpret_cast<SNameTableBlock *>(
          malloc(sizeof(SNameTableBlock) + CNAMETABLE_BLOCKSIZE));
      pBlock->m_pNext = m_pBlocks;
      m_pBlocks = pBlock;
      pBlock->m_Size = len;
      char *data = pBlock->Data();
      memcpy(data, str, len);
      return data;
    }
    size_t size = pBlock->m_Size;
    assert(pBlock != NULL && len <= CNAMETABLE_BLOCKSIZE - size);
    char *data = pBlock->Data() + size;
    size += len;
    pBlock->m_Size = size;
    if (CNAMETABLE_BLOCKSIZE - size < CNAMETABLE_CLOSE_THRESHOLD)
    {
      if (pPrevBlock != NULL)
			{
				assert(pPrevBlock->m_pNext == pBlock);
        pPrevBlock->m_pNext = pBlock->m_pNext;
			}
      else
			{
				assert(m_pBlocks == pBlock);
        m_pBlocks = pBlock->m_pNext;
			}
      pBlock->m_pNext = m_pClosedBlocks;
      m_pClosedBlocks = pBlock;
    }
    memcpy(data, str, len);
    return data;
  }

public:
  CNameTable() : m_pBlocks(NULL), m_pClosedBlocks(NULL) { }

  virtual INameTable::SNameEntry *FindEntry(const char *str)
  {
    TableT &table = m_Table;

    if (table.empty())
      return NULL;
    TableT::const_iterator it = table.find(const_cast<char *>(str));
    if (it == table.end())
      return NULL;
    return reinterpret_cast<INameTable::SNameEntry *>(*it);
  }

  virtual INameTable::SNameEntry *GetEntry(const char *str)
  {
    TableT &table = m_Table;

    if (table.empty())
    {
      char *entry = Alloc(str);
      table.insert(entry);
      return reinterpret_cast<INameTable::SNameEntry *>(entry);
    }
    TableT::iterator it = table.lower_bound(const_cast<char *>(str));
    if (it == table.end() || compare(str, *it))
    {
      char *entry = Alloc(str);
      table.insert(it, entry);
      return reinterpret_cast<INameTable::SNameEntry *>(entry);
    }
    return reinterpret_cast<INameTable::SNameEntry *>(*it);
  }

  virtual void Release(INameTable::SNameEntry *) { }

  virtual int GetMemoryUsage()
  {
    const TableT &table = m_Table;
    int size = sizeof(TableT) + table.size() * sizeof(char *);

    for (SNameTableBlock *pBlock = m_pBlocks;
        pBlock != NULL; pBlock = pBlock->m_pNext)
    {
      size += sizeof *pBlock
        + std::max(
            pBlock->m_Size,
            static_cast<size_t>(CNAMETABLE_BLOCKSIZE));
    }
    for (SNameTableBlock *pBlock = m_pClosedBlocks;
        pBlock != NULL; pBlock = pBlock->m_pNext)
    {
      size += sizeof *pBlock
        + std::max(
            pBlock->m_Size,
            static_cast<size_t>(CNAMETABLE_BLOCKSIZE));
    }
    return size;
  }
};
#else // else if defined USE_WRONLY_NAMETABLE
#define CRY_NAME_HASHTABLE_SIZE 1024*8

//////////////////////////////////////////////////////////////////////////
class CNameTable : public INameTable
{
public:
	CNameTable()
#ifdef USE_HASH_MAP
		: m_nameMap(CRY_NAME_HASHTABLE_SIZE)
#endif //USE_HASH_MAP
	{}

	// Only finds an existing name table entry, return 0 if not found.
	virtual SNameEntry* FindEntry( const char *str )
	{
		SNameEntry *pEntry = stl::find_in_map( m_nameMap,str,0 );
		return pEntry;
	}

	// Finds an existing name table entry, or creates a new one if not found.
	virtual SNameEntry* GetEntry( const char *str )
	{
		SNameEntry *pEntry = stl::find_in_map( m_nameMap,str,0 );
		if (!pEntry)
		{
			// Create a new entry.
			unsigned int nLen = strlen(str);
			unsigned int allocLen = sizeof(SNameEntry) + (nLen+1)*sizeof(char);
			pEntry = (SNameEntry*)malloc( allocLen );
			pEntry->nRefCount = 0;
			pEntry->nLength = nLen;
			pEntry->nAllocSize = allocLen;
			// Copy string to the end of name entry.
			char *pEntryStr = pEntry->GetStr();
			memcpy( pEntryStr,str,nLen+1 );
			// put in map.
			//m_nameMap.insert( NameMap::value_type(pEntry->GetStr(),pEntry) );
			m_nameMap[pEntry->GetStr()] = pEntry;
		}
		return pEntry;
	}

	// Release existing name table entry.
	virtual void Release( SNameEntry *pEntry )
	{
		assert(pEntry);
		m_nameMap.erase( pEntry->GetStr() );
		free(pEntry);
	}
  virtual int GetMemoryUsage()
  {
    int nSize = 0;
    NameMap::iterator it;
    int n = 0;
    for (it=m_nameMap.begin(); it!=m_nameMap.end(); it++)
    {
      nSize += strlen(it->first);
      nSize += it->second->GetMemoryUsage();
      n++;
    }
    nSize += n*8;
#ifdef USE_HASH_MAP
    nSize += CRY_NAME_HASHTABLE_SIZE;
#endif //USE_HASH_MAP
    return nSize;
  }

private:
#ifdef CNAMETABLE_CASE_SENSITIVE
  #ifdef USE_HASH_MAP
    typedef stl::hash_map<const char*,SNameEntry*,stl::hash_strcmp<const char*> > NameMap;
  #else
    typedef std::map<const char*,SNameEntry*,stl::less_strcmp<const char*> > NameMap;
  #endif
#else
  #ifdef USE_HASH_MAP
    typedef stl::hash_map<const char*,SNameEntry*,stl::hash_stricmp<const char*> > NameMap;
  #else
    typedef std::map<const char*,SNameEntry*,stl::less_stricmp<const char*> > NameMap;
  #endif
#endif

	NameMap m_nameMap;
};
#endif // end else if defined USE_WRONLY_NAMETABLE

///////////////////////////////////////////////////////////////////////////////
// Class CCryName.
//////////////////////////////////////////////////////////////////////////
class	CCryName
{
public:
	CCryName();
	CCryName( const CCryName& n );
	CCryName( const char *s );
	CCryName( const char *s,bool bOnlyFind );
	~CCryName();

	CCryName& operator=( const CCryName& n );
	CCryName& operator=( const char *s );

	bool	operator==( const CCryName &n ) const;
	bool	operator!=( const CCryName &n ) const;

	bool	operator==( const char *s ) const;
	bool	operator!=( const char *s ) const;

	bool	operator<( const CCryName &n ) const;
	bool	operator>( const CCryName &n ) const;

	bool	empty() const { return length() == 0; }
	void	reset()	{	_release(m_str);	m_str = 0; }

	const	char*	c_str() const { return (m_str) ? m_str: ""; }
	int	length() const { return _length(); };

	static bool find( const char *str ) { return GetNameTable()->FindEntry(str) != 0; }
  static const char *create( const char *str )
  {
    CCryName name = CCryName(str);
    name._addref(name.c_str());
    return name.c_str();
  }
  static int GetMemoryUsage()
  {
#ifdef USE_STATIC_NAME_TABLE
    CNameTable *pTable = GetNameTable();
#else
    INameTable *pTable = GetNameTable();
#endif
    return pTable->GetMemoryUsage();
  }

private:
	typedef INameTable::SNameEntry SNameEntry;

#ifdef USE_STATIC_NAME_TABLE
	static CNameTable* GetNameTable()
	{
    // Note: can not use a 'static CNameTable sTable' here, because that
    // implies a static destruction order depenency - the name table is
    // accessed from static destructor calls.
		static CNameTable *table = NULL;

    if (table == NULL)
      table = new CNameTable();
    return table;
	}
#else
	//static INameTable* GetNameTable() { return GetISystem()->GetINameTable(); }
	static INameTable* GetNameTable() { return gEnv->pNameTable; }
#endif

#ifdef USE_WRONLY_NAMETABLE
  SNameEntry *_entry( const char *pBuffer ) const
  {
    assert(pBuffer);
    return reinterpret_cast<SNameEntry *>(const_cast<char *>(pBuffer));
  }
  void _release( const char *pBuffer) { }
	int  _length() const { return (m_str) ? strlen(m_str) + 1 : 0; };
	void _addref( const char *pBuffer ) { }
#else
	SNameEntry* _entry( const char *pBuffer ) const { assert(pBuffer); return ((SNameEntry*)pBuffer)-1; }
	void _release( const char *pBuffer ) {
		if (pBuffer && _entry(pBuffer)->Release() <= 0)
			GetNameTable()->Release(_entry(pBuffer));
	}
	int  _length() const { return (m_str) ? _entry(m_str)->nLength : 0; };
	void _addref( const char *pBuffer ) { if (pBuffer) _entry(pBuffer)->AddRef(); }
#endif

	const char *m_str;
};

//////////////////////////////////////////////////////////////////////////
inline CCryName::CCryName()
{
	m_str = 0;
}

//////////////////////////////////////////////////////////////////////////
inline CCryName::CCryName( const CCryName& n )
{
	_addref( n.m_str );
	m_str = n.m_str;
}

//////////////////////////////////////////////////////////////////////////
inline CCryName::CCryName( const char *s )
{
	m_str = 0;
	*this = s;
}

//////////////////////////////////////////////////////////////////////////
inline CCryName::CCryName( const char *s,bool bOnlyFind )
{
	assert(s);
	m_str = 0;
	const char *pBuf = 0;
	if (*s) // if not empty
	{
		SNameEntry *pNameEntry = GetNameTable()->FindEntry(s);
		if (pNameEntry)
		{
			m_str = pNameEntry->GetStr();
			_addref(m_str);
		}
	}
}

inline CCryName::~CCryName()
{
	_release(m_str);
}

//////////////////////////////////////////////////////////////////////////
inline CCryName&	CCryName::operator=( const CCryName &n )
{
	if (m_str != n.m_str)
	{
		_release(m_str);
		m_str = n.m_str;
		_addref(m_str);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
inline CCryName&	CCryName::operator=( const char *s )
{
	assert(s);
	const char *pBuf = 0;
	if (*s) // if not empty
	{
		pBuf = GetNameTable()->GetEntry(s)->GetStr();
	}
	if (m_str != pBuf)
	{
		_release(m_str);
		m_str = pBuf;
		_addref(m_str);
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////////
inline bool	CCryName::operator==( const CCryName &n ) const {
	return m_str == n.m_str;
}

inline bool	CCryName::operator!=( const CCryName &n ) const {
	return !(*this == n);
}

inline bool	CCryName::operator==( const char* str ) const {
	return m_str && stricmp(m_str,str) == 0;
}

inline bool	CCryName::operator!=( const char* str ) const {
	if (!m_str)
		return true;
	return stricmp(m_str,str) != 0;
}

inline bool	CCryName::operator<( const CCryName &n ) const {
	return m_str < n.m_str;
}

inline bool	CCryName::operator>( const CCryName &n ) const {
	return m_str > n.m_str;
}

inline bool	operator==( const string &s,const CCryName &n ) {
	return n == s;
}
inline bool	operator!=( const string &s,const CCryName &n ) {
	return n != s;
}

inline bool	operator==( const char *s,const CCryName &n ) {
	return n == s;
}
inline bool	operator!=( const char *s,const CCryName &n ) {
	return n != s;
}

#endif //__CryName_h__
