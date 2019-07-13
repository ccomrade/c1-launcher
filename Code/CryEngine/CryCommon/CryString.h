////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   CryString.h
//  Version:     v1.00
//  Created:     2/8/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Custom reference counted string class.
//               Can easily be substituted instead of string.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CryString_h__
#define __CryString_h__
#pragma once

#include <string.h>
#include <wchar.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>

#define CRY_STRING

// forward declaration of CryStackString
template<class T,size_t S> class CryStackStringT;


class CConstCharWrapper;	//forward declaration for special const char * without memory allocations

//extern void CryDebugStr( const char *format,... );
//#define CRY_STRING_DEBUG(s) { if (*s) CryDebugStr( "[%6d] %s",_usedMemory(0),(s) );}
#define CRY_STRING_DEBUG(s)

//////////////////////////////////////////////////////////////////////////
// CryStringT class.
//////////////////////////////////////////////////////////////////////////
template <class T>
class CryStringT
{
public:
	//////////////////////////////////////////////////////////////////////////
	// Types compatible with STL string.
	//////////////////////////////////////////////////////////////////////////
	typedef CryStringT<T> _Self;
	typedef size_t size_type;
	typedef T value_type;
	typedef const value_type* const_str;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef pointer iterator;
	typedef const_pointer const_iterator;

	enum _npos_type {
		npos = (size_type)~0
	};
	
	//////////////////////////////////////////////////////////////////////////
	// Constructors
	//////////////////////////////////////////////////////////////////////////
	CryStringT();

private:
	CryStringT(const CConstCharWrapper& str);	//ctor for strings without memory allocations
	friend class CConstCharWrapper;
public:

	CryStringT( const _Self& str );
	CryStringT( const _Self& str,size_type nOff,size_type nCount );
	explicit CryStringT( value_type ch, size_type nRepeat = 1 );
	CryStringT( const_str str );
	CryStringT( const_str str, size_type nLength );
	CryStringT( const_iterator _First,const_iterator _Last );
	~CryStringT();

	//////////////////////////////////////////////////////////////////////////
	// STL string like interface.
	//////////////////////////////////////////////////////////////////////////
	//! Operators.
	size_type length() const;
	size_type size() const;
	bool empty() const;
	void clear();  // free up the data

	//! Returns the storage currently allocated to hold the string, a value at least as large as length().
	size_type capacity() const;

	// Sets the capacity of the string to a number at least as great as a specified number.
	// nCount = 0 is shrinking string to fit number of characters in it.
	void reserve( size_type nCount=0 );

	_Self& append( const value_type* _Ptr );
	_Self& append( const value_type* _Ptr,size_type nCount );
	_Self& append( const _Self& _Str,size_type nOff,size_type nCount );
	_Self& append( const _Self& _Str );
	_Self& append( size_type nCount,value_type _Ch );
	_Self& append( const_iterator _First,const_iterator _Last );

	_Self& assign( const_str _Ptr );
	_Self& assign( const_str _Ptr, size_type nCount );
	_Self& assign( const _Self& _Str, size_type off, size_type nCount );
	_Self& assign( const _Self& _Str );
	_Self& assign( size_type nCount, value_type _Ch );
	_Self& assign( const_iterator _First,const_iterator _Last );

	value_type  at( size_type index ) const;
	//value_type& at( size_type index );

	const_iterator begin() const { return m_str; };
	const_iterator end() const { return m_str+length(); };
	
	iterator begin() { return m_str; };
	iterator end() { return m_str+length(); };

	//! cast to C string operator.
	operator const_str() const { return m_str; }

	//! cast to C string.
	const value_type *c_str() const { return m_str; }
	const value_type *data() const { return m_str; };

	//////////////////////////////////////////////////////////////////////////
	// string comparasion.
	//////////////////////////////////////////////////////////////////////////
	int compare( const _Self& _Str ) const;
	int compare( size_type _Pos1,size_type _Num1,const _Self& _Str ) const;
	int compare( size_type _Pos1,size_type _Num1,const _Self& _Str,size_type nOff,size_type nCount ) const;
	int compare( const value_type* _Ptr	) const;
	int compare( size_type _Pos1,size_type _Num1,const value_type* _Ptr	) const;
	int compare( size_type _Pos1,size_type _Num1,const value_type* _Ptr,size_type _Num2 = npos	) const;

	// Case insensitive comparison
	int compareNoCase( const _Self& _Str ) const;
	int compareNoCase( size_type _Pos1,size_type _Num1,const _Self& _Str ) const;
	int compareNoCase( size_type _Pos1,size_type _Num1,const _Self& _Str,size_type nOff,size_type nCount ) const;
	int compareNoCase( const value_type* _Ptr	) const;
	int compareNoCase( size_type _Pos1,size_type _Num1,const value_type* _Ptr	) const;
	int compareNoCase( size_type _Pos1,size_type _Num1,const value_type* _Ptr,size_type _Num2 = npos	) const;
	
	// Copies at most a specified number of characters from an indexed position in a source string to a target character array.
	size_type copy( value_type* _Ptr, size_type nCount,	size_type nOff = 0 ) const;

	void push_back( value_type _Ch ) { _ConcatenateInPlace( &_Ch, 1 ); }
	void resize( size_type nCount,value_type _Ch=' ');
	
	//! simple sub-string extraction
	_Self substr( size_type pos, size_type count=npos ) const;

	// replace part of string.
	_Self& replace( value_type chOld, value_type chNew );
	_Self& replace( const_str strOld, const_str strNew );
	_Self& replace( size_type pos,size_type count,const_str strNew );
	_Self& replace( size_type pos,size_type count,const_str strNew,size_type count2 );
	_Self& replace( size_type pos,size_type count,size_type nNumChars,value_type chNew);

	// insert new elements to string.
	_Self& insert(size_type nIndex,value_type ch);
	_Self& insert(size_type nIndex,size_type nCount, value_type ch);
	_Self& insert(size_type nIndex,const_str pstr);
	_Self& insert(size_type nIndex,const_str pstr,size_type nCount );
	
	//! delete count characters starting at zero-based index
	_Self& erase( size_type nIndex,size_type count=npos );

	//! searching (return starting index, or -1 if not found)
	//! look for a single character match
	//! like "C" strchr
	size_type find( value_type ch,size_type pos = 0 ) const;
	//! look for a specific sub-string
	//! like "C" strstr
	size_type find( const_str subs,size_type pos = 0 ) const;
	size_type rfind( value_type ch,size_type pos = npos ) const; 

	size_type find_first_of( value_type _Ch,size_type nOff = 0 ) const;
	size_type find_first_of( const_str charSet,size_type nOff = 0 ) const;
	//size_type find_first_of( const value_type* _Ptr,size_type _Off,size_type _Count ) const;
	size_type find_first_of( const _Self& _Str, size_type _Off = 0 ) const;

	size_type find_first_not_of( value_type _Ch,size_type _Off=0 ) const;
	size_type find_first_not_of( const value_type* _Ptr,size_type _Off=0 ) const;
	size_type find_first_not_of( const value_type* _Ptr,size_type _Off,size_type _Count ) const;
	size_type find_first_not_of( const _Self& _Str,size_type _Off = 0 ) const;

	void swap( _Self& _Str );

	//////////////////////////////////////////////////////////////////////////
	// overloaded operators.
	//////////////////////////////////////////////////////////////////////////
	// overloaded indexing.
	//value_type operator[]( size_type index ) const; // same as at()
	//	value_type&	operator[]( size_type index ); // same as at()

	// overloaded assignment
	_Self& operator=( const _Self& str );
	_Self& operator=( value_type ch );
	_Self& operator=( const_str str );

	template<size_t AnySize> CryStringT(const CryStackStringT<T, AnySize> & str);

private:
	// we prohibit an implicit conversion from CryStackString to make user aware of allocation!
	// -> use string(stackedString) instead
	// as the private statement seems to be ignored (VS C++), we add a compile time error, see below
	template<size_t AnySize> _Self& operator=(const CryStackStringT<T, AnySize> & str)
	{
		// we add a compile-time error as the Visual C++ compiler seems to ignore the private statement?
		STATIC_CHECK(0, Use_Explicit_String_Assignment_When_Assigning_From_StackString);
		// not reached, as above will generate a compile time error
		_Assign(str.c_str(), str.length());
		return *this;
	}

public:
	// string concatenation
	_Self& operator+=( const _Self& str );
	_Self& operator+=( value_type ch );
	_Self& operator+=( const_str str );

	//template <class TT> friend CryStringT<TT> operator+( const CryStringT<TT>& str1, const CryStringT<TT>& str2 );
	//template <class TT> friend CryStringT<TT> operator+( const CryStringT<TT>& str, value_type ch );
	//template <class TT> friend CryStringT<TT> operator+( value_type ch, const CryStringT<TT>& str );
	//template <class TT> friend CryStringT<TT> operator+( const CryStringT<TT>& str1, const_str str2 );
	//template <class TT> friend CryStringT<TT> operator+( const_str str1, const CryStringT<TT>& str2 );

  // Workaround: X360 compiler crash (XDK: 5426)
#ifndef XENON
	size_t GetAllocatedMemory() const
	{
		StrHeader* header = _header();
		if (header == _emptyHeader())
			return 0;
		return sizeof(StrHeader) + (header->nAllocSize+1)*sizeof(value_type);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Extended functions.
	// This functions are not in the STL string.
	// They have an ATL CString interface.
	//////////////////////////////////////////////////////////////////////////
	//! Format string, use (sprintf)
	_Self& Format( const value_type* format,... );
	// This is _fast_ version
	_Self& MakeLower();
	// This is correct version
	_Self& MakeLowerLocale();
	_Self& MakeUpper();
	
	_Self& Trim();
	_Self& Trim( value_type ch );
	_Self& Trim( const value_type *sCharSet );

	_Self& TrimLeft();
	_Self& TrimLeft( value_type ch );
	_Self& TrimLeft( const value_type *sCharSet );

	_Self& TrimRight();
	_Self& TrimRight( value_type ch );
	_Self& TrimRight( const value_type *sCharSet );

	_Self SpanIncluding( const_str charSet ) const;
	_Self SpanExcluding( const_str charSet ) const;
	_Self Tokenize( const_str charSet, int &nStart ) const;
	_Self Mid( size_type nFirst,size_type nCount=npos ) const {  return substr(nFirst,nCount); };

	_Self Left( size_type count ) const;
	_Self Right( size_type count ) const;
	//////////////////////////////////////////////////////////////////////////

	// public utilities.
	static size_type _strlen( const_str str );
	static const_str _strchr( const_str str, value_type c);
	static value_type* _strstr( value_type* str, const_str strSearch);
	static bool _IsValidString( const_str str );

#if defined(WIN32) || defined(WIN64)
	static int _vscpf( const_str format, va_list args);
	static int _vsnpf( value_type* buf, int cnt, const_str format, va_list args);
#endif

public:
	//////////////////////////////////////////////////////////////////////////
	// Only used for debugging statistics.
	//////////////////////////////////////////////////////////////////////////
	static int _usedMemory( int size )
	{
		static int s_used_memory = 0;
		s_used_memory += size;
		return s_used_memory;
	}

private:
	value_type* m_str; // pointer to ref counted string data

	// String header, immediately after this header in memory starts actual string data.
	struct StrHeader
	{
		// Reference count of this string.
		int nRefCount;
		// Current length of string.
		int nLength;
		// Size of memory allocated at the end of this class.
		int nAllocSize;
		// Here in memory starts character buffer of size nAllocSize.
		//value_type data[nAllocSize]
		
		value_type* GetChars() { return (value_type*)(this+1); }
		void  AddRef() { nRefCount++; /*InterlockedIncrement(&_header()->nRefCount);*/};
		int   Release() { return --nRefCount; };
	};
	static StrHeader* _emptyHeader()
	{
		static int sEmptyStringBuffer[] = { -1, 0, 0, 0 };
		return (StrHeader*)(&sEmptyStringBuffer);
	}

	// implementation helpers
	StrHeader* _header() const;
	
	void _AllocData( size_type nLen );
	static void _FreeData( StrHeader* pData );
	void _Free();
	void _Initialize();

	void _Concatenate( const_str sStr1,size_type nLen1,const_str sStr2,size_type nLen2 );
	void _ConcatenateInPlace( const_str sStr,size_type nLen );
	void _Assign( const_str sStr,size_type nLen );
	void _MakeUnique();

	static void _copy( value_type *dest,const value_type *src,size_type count );
	static void _move( value_type *dest,const value_type *src,size_type count );
	static void _set( value_type *dest,value_type ch,size_type count );
};

// Variant of CryStringT<T> which does not share memory with other strings.
class CryStringLocal: public CryStringT<char>
{
public:
	typedef CryStringT<char> String;
	CryStringLocal() 
		{}
	CryStringLocal( const CryStringLocal& str )
		: String(str.c_str())
		{}
	CryStringLocal( const String& str )
		: String(str.c_str())
		{}
	CryStringLocal( const_str str )
		: String(str)
		{}

	CryStringLocal& operator=( const CryStringLocal& str )
	{
		String::operator=(str.c_str());
		return *this;
	}
	CryStringLocal& operator=( const String& str )
	{
		String::operator=(str.c_str());
		return *this;
	}
	CryStringLocal& operator=( const_str str )
	{
		String::operator=(str);
		return *this;
	}
};


// wrapper class for creation of strings without memory allocation
// it creates a string with pointer pointing to const char* location
// destructor sets the string to empty
// NOTE: never copy a string from it, just use it as function parameters instead of const char* itself
class CConstCharWrapper
{
public:
	//passing *this is safe since the char pointer is already set and therefore is the this-ptr constructed complete enough
#pragma warning (disable : 4355) 
	CConstCharWrapper(const char *const cpString) : cpChar(cpString), str(*this){assert(cpString); }//create stack string
#pragma warning (default : 4355) 
	~CConstCharWrapper(){str.m_str = CryStringT<char>::_emptyHeader()->GetChars();}//reset string
	operator const CryStringT<char>&() const {return str;}//cast operator to const string reference 
private:
	const char *const cpChar;	
	CryStringT<char> str;

	char *GetCharPointer() const {return const_cast<char*>(cpChar);}	//access function for string ctor

	friend class CryStringT<char>;	//both are bidirectional friends to avoid any other accesses
};

#ifndef XENON
//macro needed because compiler somehow cannot find the cast operator when not invoked directly
#define CONST_TEMP_STRING(a) ((const string&)CConstCharWrapper(a))	




#endif

/////////////////////////////////////////////////////////////////////////////
// CryStringT<T> Implementation
//////////////////////////////////////////////////////////////////////////

template <class T>
inline typename CryStringT<T>::StrHeader* CryStringT<T>::_header() const
{
	assert(m_str != NULL); return ((StrHeader*)m_str)-1;
}

template <class T>
inline typename CryStringT<T>::size_type CryStringT<T>::_strlen( const_str str )
{
	return (str == NULL) ? 0 : (size_type)::strlen(str);
}

template <>
inline CryStringT<wchar_t>::size_type CryStringT<wchar_t>::_strlen( const_str str )
{
	return (str == NULL) ? 0 : (size_type)::wcslen(str);
}

template <class T>
inline typename CryStringT<T>::const_str CryStringT<T>::_strchr( const_str str,value_type c )
{
	return (str == NULL) ? 0 : ::strchr(str, c);
}

template <>
inline CryStringT<wchar_t>::const_str CryStringT<wchar_t>::_strchr( const_str str,value_type c )
{
	return (str == NULL) ? 0 : ::wcschr(str, c);
}

template <class T>
inline typename CryStringT<T>::value_type* CryStringT<T>::_strstr( value_type* str,const_str strSearch )
{
	return (str == NULL) ? 0 : ::strstr(str, strSearch);
}

template <>
inline CryStringT<wchar_t>::value_type* CryStringT<wchar_t>::_strstr( value_type* str,const_str strSearch )
{
	return (str == NULL) ? 0 : ::wcsstr(str, strSearch);
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_copy( value_type *dest,const value_type *src,size_type count )
{
	memcpy( dest,src,count*sizeof(value_type) );
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_move( value_type *dest,const value_type *src,size_type count )
{
	memmove( dest,src,count*sizeof(value_type) );
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_set( value_type *dest,value_type ch,size_type count )
{
	memset( dest,ch,count*sizeof(value_type) );
}

//////////////////////////////////////////////////////////////////////////
template <>
inline void CryStringT<wchar_t>::_set( value_type *dest,value_type ch,size_type count )
{
	wmemset( dest,ch,count );
}

#if defined(WIN32) || defined(WIN64)

template<>
inline int CryStringT<char>::_vscpf(const_str format, va_list args)
{
	return _vscprintf(format, args);
}

template<>
inline int CryStringT<wchar_t>::_vscpf(const_str format, va_list args)
{
	return _vscwprintf(format, args);
}

template<>
inline int CryStringT<char>::_vsnpf(value_type* buf, int cnt, const_str format, va_list args)
{
	return _vsnprintf(buf, cnt, format, args);
}

template<>
inline int CryStringT<wchar_t>::_vsnpf(value_type* buf, int cnt, const_str format, va_list args)
{
	return _vsnwprintf(buf, cnt, format, args);
}

#endif

//////////////////////////////////////////////////////////////////////////
template <class T>
inline bool CryStringT<T>::_IsValidString( const_str  )
{
	/*
	if (str == NULL)
		return false;
	int nLength = _strlen(str);
	return !::IsBadStringPtrA(str, nLength);
	*/
	return true;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_Assign( const_str sStr,size_type nLen )
{
	// Check if this string is shared (reference count greater then 1) or not enough capacity to store new string.
	// Then allocate new string buffer.
	if (_header()->nRefCount > 1 || nLen > capacity())
	{
		_Free();
		_AllocData(nLen);
	}
	// Copy characters from new string to this buffer.
	_copy( m_str,sStr,nLen );
	// Set new length.
	_header()->nLength = nLen;
	// Make null terminated string.
	m_str[nLen] = 0;
	CRY_STRING_DEBUG(m_str)
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_Concatenate( const_str sStr1,size_type nLen1,const_str sStr2,size_type nLen2 )
{
	size_type nLen = nLen1 + nLen2;
	if (nLen1 * 2 > nLen)
		nLen = nLen1 * 2;
	if (nLen != 0)
	{
		if (nLen < 8)
			nLen = 8;
		_AllocData(nLen);
		_copy( m_str,sStr1,nLen1 );
		_copy( m_str+nLen1, sStr2,nLen2 );
		_header()->nLength = nLen1 + nLen2;
		m_str[nLen1 + nLen2] = 0;
	}
	CRY_STRING_DEBUG(m_str)
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_ConcatenateInPlace( const_str sStr,size_type nLen )
{
	if (nLen != 0)
	{
		// Check if this string is shared (reference count greater then 1) or not enough capacity to store new string.
		// Then allocate new string buffer.
		if (_header()->nRefCount > 1 || length()+nLen > capacity())
		{
			StrHeader* pOldData = _header();
			_Concatenate( m_str,length(),sStr,nLen );
			_FreeData(pOldData);
		}
		else
		{
			_copy( m_str+length(), sStr, nLen );
			_header()->nLength += nLen;
			m_str[_header()->nLength] = 0; // Make null terminated string.
		}
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_MakeUnique()
{
	if (_header()->nRefCount > 1)
	{
		// If string is shared, make a copy of string buffer.
		StrHeader *pOldData = _header();
		// This will not free header because reference count is greater then 1.
		_Free();
		// Allocate a new string buffer.
		_AllocData( pOldData->nLength );
		// Full copy of null terminated string.
		_copy( m_str,pOldData->GetChars(),pOldData->nLength+1 );
		CRY_STRING_DEBUG(m_str)
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_Initialize()
{
	m_str = _emptyHeader()->GetChars();
}

// always allocate one extra character for '\0' termination
// assumes [optimistically] that data length will equal allocation length
template <class T>
inline void CryStringT<T>::_AllocData( size_type nLen )
{
	assert(nLen >= 0);
	assert(nLen <= INT_MAX-1);    // max size (enough room for 1 extra)

	if (nLen == 0)
		_Initialize();
	else
	{
		size_type allocLen = sizeof(StrHeader) + (nLen+1)*sizeof(value_type);
		StrHeader* pData = (StrHeader*)CryModuleMalloc( allocLen );

		_usedMemory( allocLen ); // For statistics.

		pData->nRefCount = 1;
		m_str = pData->GetChars();
		pData->nLength = nLen;
		pData->nAllocSize = nLen;
		m_str[nLen] = 0; // null terminated string.
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_Free()
{
	if (_header()->nRefCount >= 0) // Not empty string.
	{
		_FreeData( _header() );
		_Initialize();
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::_FreeData( StrHeader* pData )
{
	if (pData->nRefCount >= 0) // Not empty string.
	{
		assert(pData->nRefCount != 0);
		if (pData->Release() <= 0)
		{
			int allocLen = sizeof(StrHeader) + (pData->nAllocSize+1)*sizeof(value_type);
			_usedMemory( -allocLen ); // For statistics.

			CryModuleFree((void*)pData);
			//int allocLen = sizeof(StrHeader) + (pData->nAllocSize+1)*sizeof(value_type);
			//string_alloc::deallocate( (value_type*)pData,allocLen );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT()
{
	_Initialize();
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT( const CryStringT<T>& str )
{
	assert(str._header()->nRefCount != 0);
	if (str._header()->nRefCount >= 0)
	{
		m_str = str.m_str;
		_header()->AddRef();
	}
	else
	{
		_Initialize();
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT( const CryStringT<T>& str,size_type nOff,size_type nCount )
{
	_Initialize();
	assign( str,nOff,nCount );
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT( const_str str )
{
	_Initialize();
	// Make a copy of C string.
	size_type nLen = _strlen(str);
	if (nLen != 0)
	{
		_AllocData(nLen);
		_copy( m_str,str,nLen );
		CRY_STRING_DEBUG(m_str)
	}
}

template <class T>
inline CryStringT<T>::CryStringT(const CConstCharWrapper& str)
{
	_Initialize();
	m_str = const_cast<pointer>(str.GetCharPointer());
}

template<class T> 
template<size_t AnySize>
inline CryStringT<T>::CryStringT(const CryStackStringT<T, AnySize> & str) 
{
	_Initialize();
	const size_type nLength = str.length();
	if (nLength > 0)
	{
		_AllocData(nLength);
		_copy( m_str,str,nLength );
		CRY_STRING_DEBUG(m_str)
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT( const_str str,size_type nLength )
{
	_Initialize();
	if (nLength > 0)
	{
		_AllocData(nLength);
		_copy( m_str,str,nLength );
		CRY_STRING_DEBUG(m_str)
	}
}


//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT( value_type ch,size_type nRepeat )
{
	_Initialize();
	if (nRepeat > 0)
	{
		_AllocData( nRepeat );
		_set( m_str,ch,nRepeat );
		CRY_STRING_DEBUG(m_str)
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::CryStringT( const_iterator _First,const_iterator _Last )
{
	_Initialize();
	size_type nLength = (size_type)(_Last-_First);
	if (nLength > 0)
	{
		_AllocData(nLength);
		_copy( m_str,_First,nLength );
		CRY_STRING_DEBUG(m_str)
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>::~CryStringT()
{
	_FreeData( _header() );
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline typename CryStringT<T>::size_type CryStringT<T>::length() const
{
	return _header()->nLength;
}
template <class T>
inline typename CryStringT<T>::size_type CryStringT<T>::size() const
{
	return _header()->nLength;
}
template <class T>
inline typename CryStringT<T>::size_type CryStringT<T>::capacity() const
{
	return _header()->nAllocSize;
}

template <class T>
inline bool CryStringT<T>::empty() const
{
	return length() == 0;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::clear()
{
	if (length() == 0)
		return;
	if (_header()->nRefCount >= 0)
		_Free();
	else
		*this = "";
	assert( length() == 0);
	assert(_header()->nRefCount < 0 || capacity() == 0);
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::reserve( size_type nCount )
{
	// Reserve of 0 is shrinking container to fit number of characters in it..
	if (nCount > capacity())
	{
		StrHeader* pOldData = _header();
		_AllocData( nCount );
		_copy( m_str,pOldData->GetChars(),pOldData->nLength );
		_header()->nLength = pOldData->nLength;
        m_str[pOldData->nLength] = 0;
		_FreeData(pOldData);
	} else if (nCount == 0)
	{
		if (length() != capacity())
		{
			StrHeader* pOldData = _header();
			_AllocData(length());
			_copy( m_str,pOldData->GetChars(),pOldData->nLength );
			_FreeData(pOldData);
		}
	}
	CRY_STRING_DEBUG(m_str)
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::append( const_str _Ptr )
{
	*this += _Ptr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::append( const_str _Ptr, size_type nCount )
{
	_ConcatenateInPlace( _Ptr,nCount );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::append( const CryStringT<T>& _Str, size_type off, size_type nCount )
{
	size_type len = _Str.length();
	if (off > len)
		return *this;
	if (off+nCount > len)
		nCount = len - off;
	_ConcatenateInPlace( _Str.m_str+off,nCount );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::append( const CryStringT<T>& _Str )
{
	*this += _Str;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::append( size_type nCount, value_type _Ch )
{
	if (nCount > 0)
	{
		if (length()+nCount >= capacity())
		{
			StrHeader* pOldData = _header();
			_AllocData( length()+nCount );
			_copy( m_str, pOldData->GetChars(), pOldData->nLength );
			_set( m_str+pOldData->nLength,_Ch,nCount );
			_FreeData(pOldData);
		}
		else
		{
			size_type nOldLength = length();
			_set( m_str+nOldLength,_Ch,nCount );
			_header()->nLength = nOldLength + nCount;
			m_str[length()] = 0; // Make null terminated string.
		}
	}
	CRY_STRING_DEBUG(m_str)
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::append(  const_iterator _First,const_iterator _Last )
{
	append( _First,(size_type)(_Last-_First) );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::assign( const_str _Ptr )
{
	*this = _Ptr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::assign( const_str _Ptr, size_type nCount )
{
	size_type len = _strlen(_Ptr);
	_Assign( _Ptr,(nCount<len)?nCount:len );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::assign( const CryStringT<T>& _Str, size_type off, size_type nCount )
{
	size_type len = _Str.length();
	if (off > len)
		return *this;
	if (off+nCount > len)
		nCount = len - off;
	_Assign( _Str.m_str+off,nCount );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::assign( const CryStringT<T>& _Str )
{
	*this = _Str;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::assign( size_type nCount, value_type _Ch )
{
	if (nCount >= 1)
	{
		_AllocData( nCount );
		_set( m_str,_Ch,nCount );
		CRY_STRING_DEBUG(m_str)
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T>& CryStringT<T>::assign( const_iterator _First,const_iterator _Last )
{
	assign( _First,(size_type)(_Last-_First) );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline typename CryStringT<T>::value_type CryStringT<T>::at( size_type index ) const
{
	assert( index >= 0 && index < length() );
	return m_str[index];
}

/*
inline value_type& CryStringT<T>::at( size_type index )
{
	// same as GetAt
	assert( index >= 0 && index < length() );
	return m_str[index];
}
*/

/*
inline CryStringT<T>::value_type CryStringT<T>::operator[]( size_type index ) const
{
	assert( index >= 0 && index < length() );
	return m_str[index];
}
*/

/*
inline value_type& CryStringT<T>::operator[]( size_type index )
{
// same as GetAt
assert( index >= 0 && index < length() );
return m_str[index];
}
*/


//////////////////////////////////////////////////////////////////////////
template <class T>
inline int CryStringT<T>::compare( const CryStringT<T>& _Str ) const
{
	return strcmp( m_str,_Str.m_str );
}

template <class T>
inline int CryStringT<T>::compare( size_type _Pos1,size_type _Num1,const CryStringT<T>& _Str ) const
{
	return compare( _Pos1,_Num1,_Str.m_str,npos );
}

template <class T>
inline int CryStringT<T>::compare( size_type _Pos1,size_type _Num1,const CryStringT<T>& _Str,size_type nOff,size_type nCount ) const
{
	assert( nOff < _Str.length() );
	return compare( _Pos1,_Num1,_Str.m_str+nOff,nCount );
}

template <class T>
inline int CryStringT<T>::compare( const value_type* _Ptr	) const
{
	return strcmp( m_str,_Ptr );
}

template <class T>
inline int CryStringT<T>::compare( size_type _Pos1,size_type _Num1,const value_type* _Ptr	) const
{
	return compare( _Pos1,_Num1,_Ptr,npos );
}

template <class T>
inline int CryStringT<T>::compare( size_type _Pos1,size_type _Num1,const value_type* _Ptr,size_type _Num2 ) const
{
	assert( _Pos1 < length() );
	if (length() - _Pos1 < _Num1)
		_Num1 = length() - _Pos1; // trim to size

	int res = _Num1 == 0 ? 0 : strncmp( m_str+_Pos1,_Ptr,(_Num1 < _Num2)?_Num1:_Num2);
	return (res != 0 ? res : _Num1 < _Num2 ? -1 : _Num1 == _Num2 ? 0 : +1);
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline int CryStringT<T>::compareNoCase( const CryStringT<T>& _Str ) const
{
	return stricmp( m_str,_Str.m_str );
}

template <class T>
inline int CryStringT<T>::compareNoCase( size_type _Pos1,size_type _Num1,const CryStringT<T>& _Str ) const
{
	return compareNoCase( _Pos1,_Num1,_Str.m_str,npos );
}

template <class T>
inline int CryStringT<T>::compareNoCase( size_type _Pos1,size_type _Num1,const CryStringT<T>& _Str,size_type nOff,size_type nCount ) const
{
	assert( nOff < _Str.length() );
	return compareNoCase( _Pos1,_Num1,_Str.m_str+nOff,nCount );
}

template <class T>
inline int CryStringT<T>::compareNoCase( const value_type* _Ptr	) const
{
	return stricmp( m_str,_Ptr );
}

template <class T>
inline int CryStringT<T>::compareNoCase( size_type _Pos1,size_type _Num1,const value_type* _Ptr	) const
{
	return compareNoCase( _Pos1,_Num1,_Ptr,npos );
}

template <class T>
inline int CryStringT<T>::compareNoCase( size_type _Pos1,size_type _Num1,const value_type* _Ptr,size_type _Num2 ) const
{
	assert( _Pos1 < length() );
	if (length() - _Pos1 < _Num1)
		_Num1 = length() - _Pos1; // trim to size

	int res = _Num1 == 0 ? 0 : strnicmp( m_str+_Pos1,_Ptr,(_Num1 < _Num2)?_Num1:_Num2);
	return (res != 0 ? res : _Num1 < _Num2 ? -1 : _Num1 == _Num2 ? 0 : +1);
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline typename CryStringT<T>::size_type CryStringT<T>::copy( value_type* _Ptr, size_type nCount,size_type nOff ) const
{
	assert(nOff < length());
	if (nCount < 0)
		nCount = 0;
	if (nOff+nCount > length()) // trim to offset.
		nCount = length() - nOff;

	_copy( _Ptr,m_str+nOff,nCount );
	return nCount;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline void CryStringT<T>::resize( size_type nCount,value_type _Ch )
{
	_MakeUnique();
	if (nCount > length())
	{
		size_type numToAdd = nCount - length();
		append(numToAdd,_Ch);
	}
	else if (nCount < length())
	{
		_header()->nLength = nCount;
		m_str[length()] = 0; // Make null terminated string.

	}
}

//////////////////////////////////////////////////////////////////////////
//! compare helpers
template <class T> inline bool operator==(const CryStringT<T>& s1, const CryStringT<T>& s2)
{ return s1.compare(s2) == 0; }
template <class T> inline bool operator==(const CryStringT<T>& s1, const typename CryStringT<T>::value_type* s2)
{ return s1.compare(s2) == 0; }
template <class T> inline bool operator==(const typename CryStringT<T>::value_type* s1, const CryStringT<T>& s2)
{ return s2.compare(s1) == 0; }
template <class T> inline bool operator!=(const CryStringT<T>& s1, const CryStringT<T>& s2)
{ return s1.compare(s2) != 0; }
template <class T> inline bool operator!=(const CryStringT<T>& s1, const typename CryStringT<T>::value_type* s2)
{ return s1.compare(s2) != 0; }
template <class T> inline bool operator!=(const typename CryStringT<T>::value_type* s1, const CryStringT<T>& s2)
{ return s2.compare(s1) != 0; }
template <class T> inline bool operator<(const CryStringT<T>& s1, const CryStringT<T>& s2)
{ return s1.compare(s2) < 0; }
template <class T> inline bool operator<(const CryStringT<T>& s1, const typename CryStringT<T>::value_type* s2)
{ return s1.compare(s2) < 0; }
template <class T> inline bool operator<(const typename CryStringT<T>::value_type* s1, const CryStringT<T>& s2)
{ return s2.compare(s1) > 0; }
template <class T> inline bool operator>(const CryStringT<T>& s1, const CryStringT<T>& s2)
{ return s1.compare(s2) > 0; }
template <class T> inline bool operator>(const CryStringT<T>& s1, const typename CryStringT<T>::value_type* s2)
{ return s1.compare(s2) > 0; }
template <class T> inline bool operator>(const typename CryStringT<T>::value_type* s1, const CryStringT<T>& s2)
{ return s2.compare(s1) < 0; }
template <class T> inline bool operator<=(const CryStringT<T>& s1, const CryStringT<T>& s2)
{ return s1.compare(s2) <= 0; }
template <class T> inline bool operator<=(const CryStringT<T>& s1, const typename CryStringT<T>::value_type* s2)
{ return s1.compare(s2) <= 0; }
template <class T> inline bool operator<=(const typename CryStringT<T>::value_type* s1, const CryStringT<T>& s2)
{ return s2.compare(s1) >= 0; }
template <class T> inline bool operator>=(const CryStringT<T>& s1, const CryStringT<T>& s2)
{ return s1.compare(s2) >= 0; }
template <class T> inline bool operator>=(const CryStringT<T>& s1, const typename CryStringT<T>::value_type* s2)
{ return s1.compare(s2) >= 0; }
template <class T> inline bool operator>=(const typename CryStringT<T>::value_type* s1, const CryStringT<T>& s2)
{ return s2.compare(s1) <= 0; }

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::operator=( value_type ch )
{
	_Assign( &ch,1 );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T> operator+( const CryStringT<T>& string1,typename CryStringT<T>::value_type ch )
{
	CryStringT<T> s(string1);
	s.append(1,ch);
	return s;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T> operator+( typename CryStringT<T>::value_type ch, const CryStringT<T>& str )
{
	CryStringT<T> s;
	s.reserve(str.size()+1);
	s.append(1,ch);
	s.append(str);
	return s;
}

//////////////////////////////////////////////////////////////////////////
template <class T>
inline CryStringT<T> operator+( const CryStringT<T>& string1, const CryStringT<T>& string2 )
{
	CryStringT<T> s(string1);
	s.append( string2 );
	return s;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T> operator+( const CryStringT<T>& str1, const typename CryStringT<T>::value_type* str2 )
{
	CryStringT<T> s(str1);
	s.append( str2 );
	return s;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T> operator+( const typename CryStringT<T>::value_type* str1, const CryStringT<T>& str2 )
{
	assert(str1 == NULL || CryStringT<T>::_IsValidString(str1));
	CryStringT<T> s;
	s.reserve( CryStringT<T>::_strlen(str1) + str2.size() );
	s.append(str1);
	s.append(str2);
	return s;
}

template <class T> 
inline CryStringT<T>& CryStringT<T>::operator=(const CryStringT<T>& str)
{
	if (m_str != str.m_str)
	{
		if (_header()->nRefCount < 0)
		{
			// Empty string.
//			_Assign( str.m_str,str.length() );
			if (str._header()->nRefCount < 0)
				; // two empty strings...
			else
			{
				m_str = str.m_str;
				_header()->AddRef();
			}
		}
		else if (str._header()->nRefCount < 0)
		{
			// If source string is empty.
			_Free();
			m_str = str.m_str;
		}
		else
		{
			// Copy string reference.
			_Free();
			m_str = str.m_str;
			_header()->AddRef();
		}
	}
	return *this;
}

template <class T> 
inline CryStringT<T>& CryStringT<T>::operator=( const_str str )
{
	assert(str == NULL || _IsValidString(str));
	_Assign( str,_strlen(str) ) ;
	return *this;
}

template <class T> 
inline CryStringT<T>& CryStringT<T>::operator+=( const_str str )
{
	assert(str == NULL || _IsValidString(str));
	_ConcatenateInPlace( str,_strlen(str) );
	return *this;
}

template <class T> 
inline CryStringT<T>& CryStringT<T>::operator+=( value_type ch )
{
	_ConcatenateInPlace( &ch,1 );
	return *this;
}

template <class T> 
inline CryStringT<T>& CryStringT<T>::operator+=( const CryStringT<T>& str )
{
	_ConcatenateInPlace( str.m_str,str.length() );
	return *this;
}

//! find first single character
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find( value_type ch,size_type pos ) const
{
	if (!( pos >= 0 && pos <= length()) )
		return (typename CryStringT<T>::size_type)npos;
	const_str str = _strchr( m_str+pos,ch );
	// return npos if not found and index otherwise
	return (str == NULL) ? npos : (size_type)(str - m_str);
}

//! find a sub-string (like strstr)
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find( const_str subs,size_type pos ) const
{
	assert( _IsValidString(subs) );
	if (!( pos >= 0 && pos <= length()) )
		return npos;
	
	// find first matching substring
	const_str str = _strstr( m_str+pos,subs );
	
	// return npos for not found, distance from beginning otherwise
	return (str == NULL) ? npos : (size_type)(str - m_str);

}

//! find last single character
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::rfind( value_type ch,size_type pos ) const
{
	const_str str;
	if (pos == npos) {
		// find last single character
		str = strrchr( m_str,ch );
		// return -1 if not found, distance from beginning otherwise
		return (str == NULL) ? -1 : (size_type)(str - m_str);
	} else {
		if (pos == npos)
			pos = length();
		if (!( pos >= 0 && pos <= length()) )
			return npos;

		value_type tmp = m_str[pos+1];
		m_str[pos+1] = 0;
		str = strrchr( m_str,ch );
		m_str[pos+1] = tmp;
	}
	// return -1 if not found, distance from beginning otherwise
	return (str == NULL) ? -1 : (size_type)(str - m_str);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_of( const CryStringT<T>& _Str, size_type _Off ) const
{
	return find_first_of( _Str.m_str,_Off );
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_of( value_type _Ch,size_type nOff ) const
{
	if (!( nOff >= 0 && nOff <= length()) )
		return npos;
	value_type charSet[2] = { _Ch,0 };
	const_str str = strpbrk( m_str+nOff,charSet );
	return (str == NULL) ? -1 : (size_type)(str - m_str);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_of( const_str charSet,size_type nOff ) const
{
	assert(_IsValidString(charSet));
	if (!( nOff >= 0 && nOff <= length()) )
		return npos;
	const_str str = strpbrk( m_str+nOff,charSet );
	return (str == NULL) ? -1 : (size_type)(str - m_str);
}

//size_type find_first_not_of(const _Self& __s, size_type __pos = 0) const 
//{ return find_first_not_of(__s._M_start, __pos, __s.size()); }

//size_type find_first_not_of(const _CharT* __s, size_type __pos = 0) const 
//{ _STLP_FIX_LITERAL_BUG(__s) return find_first_not_of(__s, __pos, _Traits::length(__s)); }

//size_type find_first_not_of(const _CharT* __s, size_type __pos,	size_type __n) const;

//size_type find_first_not_of(_CharT __c, size_type __pos = 0) const;


//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_not_of( const value_type* _Ptr,size_type _Off ) const
{ return find_first_not_of( _Ptr,_Off,_strlen(_Ptr) ); }

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_not_of( const CryStringT<T>& _Str,size_type _Off ) const
{ return find_first_not_of( _Str.m_str,_Off ); }

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_not_of( value_type _Ch,size_type _Off ) const
{
	if (_Off > length())
		return npos;
	else {
		for (const value_type *str = begin()+_Off; str != end(); ++str)
			if (*str != _Ch)
				return size_type(str - begin()); // Character found!
		return npos;
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline typename CryStringT<T>::size_type CryStringT<T>::find_first_not_of( const value_type* _Ptr,size_type _Off,size_type _Count ) const
{
	if (_Off > length())
		return npos;
	else {
		const value_type *charsFirst = _Ptr,*charsLast = _Ptr+_Count;
		for (const value_type *str = begin()+_Off; str != end(); ++str)
		{
			const value_type *c;
			for (c = charsFirst; c != charsLast; ++c)
			{
				if (*c == *str)
					break;
			}
			if (c == charsLast)
				return size_type(str - begin());// Current character not in char set.
		}
		return npos;
	}
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T> CryStringT<T>::substr( size_type pos, size_type count ) const
{
	if (pos >= length())
		return CryStringT<T>();
	if (count == npos)
	{
		count = length() - pos;
	}
	if (pos + count > length())
		count = length() - pos;
	return CryStringT<T>( m_str+pos,count );
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::erase( size_type nIndex,size_type nCount )
{
	if (nIndex < 0)
		nIndex = 0;
	if (nCount < 0 || nCount > length() - nIndex)
		nCount = length() - nIndex;
	if (nCount > 0 && nIndex < length())
	{
		_MakeUnique();
		size_type nNumToCopy = length() - (nIndex + nCount) + 1;
		_move( m_str+nIndex,m_str+nIndex+nCount,nNumToCopy );
		_header()->nLength = length() - nCount;
	}
	
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::insert( size_type nIndex, value_type ch )
{
	return insert(nIndex,1,ch);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::insert( size_type nIndex,size_type nCount,value_type ch )
{
	_MakeUnique();

	if (nIndex < 0)
		nIndex = 0;

	size_type nNewLength = length();
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength += nCount;

	if (capacity() < nNewLength)
	{
		StrHeader* pOldData = _header();
		const_str pstr = m_str;
		_AllocData(nNewLength);
		_copy( m_str,pstr,pOldData->nLength+1 );
		_FreeData(pOldData);
	}

	_move( m_str+nIndex+nCount, m_str+nIndex, (nNewLength-nIndex) );
	_set( m_str+nIndex,ch,nCount );
	_header()->nLength = nNewLength;
	CRY_STRING_DEBUG(m_str)

	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::insert( size_type nIndex,const_str pstr,size_type nCount )
{
	if (nIndex < 0)
		nIndex = 0;

	size_type nInsertLength = nCount;
	size_type nNewLength = length();
	if (nInsertLength > 0)
	{
		_MakeUnique();
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		if (capacity() < nNewLength)
		{
			StrHeader* pOldData = _header();
			const_str pstr = m_str;
			_AllocData(nNewLength);
			_copy( m_str, pstr,(pOldData->nLength+1) );
			_FreeData(pOldData);
		}

		_move( m_str + nIndex + nInsertLength, m_str + nIndex, (nNewLength-nIndex-nInsertLength+1) );
		_copy( m_str + nIndex, pstr, nInsertLength );
		_header()->nLength = nNewLength;
		m_str[length()] = 0;
	}
	CRY_STRING_DEBUG(m_str)

	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::insert( size_type nIndex,const_str pstr )
{
	return insert( nIndex,pstr,_strlen(pstr) );
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::replace( size_type pos,size_type count,const_str strNew )
{
	return replace( pos,count,strNew,_strlen(strNew) );
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::replace( size_type pos,size_type count,const_str strNew,size_type count2 )
{
	erase( pos,count );
	insert( pos,strNew,count2 );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::replace( size_type pos,size_type count,size_type nNumChars,value_type chNew )
{
	erase( pos,count );
	insert( pos,nNumChars,chNew );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::replace( value_type chOld,value_type chNew )
{
	if (chOld != chNew)
	{
		_MakeUnique();
		value_type* strend = m_str + length();
		for (value_type* str = m_str; str != strend; ++str)
		{
			if (*str == chOld)
			{
				*str = chNew;
			}
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::replace( const_str strOld,const_str strNew )
{
	size_type nSourceLen = _strlen(strOld);
	if (nSourceLen == 0)
		return *this;
	size_type nReplacementLen = _strlen(strNew);

	size_type nCount = 0;
	value_type* strStart = m_str;
	value_type* strEnd = m_str + length();
	value_type* strTarget;
	while (strStart < strEnd)
	{
		while ((strTarget = _strstr(strStart, strOld)) != NULL)
		{
			nCount++;
			strStart = strTarget + nSourceLen;
		}
		strStart += _strlen(strStart) + 1;
	}

	if (nCount > 0)
	{
		_MakeUnique();

		size_type nOldLength = length();
		size_type nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
		if (capacity() < nNewLength || _header()->nRefCount > 1)
		{
			StrHeader* pOldData = _header();
			const_str pstr = m_str;
			_AllocData(nNewLength);
			_copy( m_str, pstr, pOldData->nLength );
			_FreeData(pOldData);
		}
		strStart = m_str;
		strEnd = m_str + length();

		while (strStart < strEnd)
		{
			while ( (strTarget = _strstr(strStart, strOld)) != NULL)
			{
				size_type nBalance = nOldLength - ((size_type)( strTarget - m_str) + nSourceLen);
				_move( strTarget + nReplacementLen, strTarget + nSourceLen, nBalance );
				_copy( strTarget, strNew, nReplacementLen );
				strStart = strTarget + nReplacementLen;
				strStart[nBalance] = 0;
				nOldLength += (nReplacementLen - nSourceLen);
			}
			strStart += _strlen(strStart) + 1;
		}
		_header()->nLength = nNewLength;
	}
	CRY_STRING_DEBUG(m_str)

	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline void CryStringT<T>::swap( CryStringT<T>& _Str )
{
	value_type *temp = _Str.m_str;
	_Str.m_str = m_str;
	m_str = temp;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::Format( const_str format,... )
{
	assert(_IsValidString(format));

#if defined(WIN32) || defined(WIN64)
	va_list argList;
	va_start(argList, format );
	int n = _vscpf(format, argList);
	if (n < 0) n = 0;
	resize(n);
	_vsnpf(m_str, n, format, argList);
	va_end(argList);
	return *this;
#else
	value_type temp[4096]; // Limited to 4096 characters!
	va_list argList;
	va_start(argList, format );
	vsnprintf( temp,4096,format,argList ); 
	temp[4095] = '\0';
	va_end(argList);
	*this = temp;
	return *this;
#endif
}

//////////////////////////////////////////////////////////////////////////
#define __ascii_tolower(c)      ( (((c) >= 'A') && ((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c) )
template <class T> 
inline CryStringT<T>& CryStringT<T>::MakeLower()
{
	_MakeUnique();
	for (value_type *s = m_str; *s != 0; s++)
	{
		*s = __ascii_tolower(*s);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::MakeLowerLocale()
{
	_MakeUnique();
	for (value_type *s = m_str; *s != 0; s++)
	{
		*s = ::tolower(*s);
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::MakeUpper()
{
	_MakeUnique();
	for (value_type *s = m_str; *s != 0; s++)
	{
		*s = ::toupper(*s);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::Trim()
{
	return TrimRight().TrimLeft();
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::Trim( value_type ch )
{
	_MakeUnique();
	const value_type chset[2] = { ch,0 };
	return TrimRight(chset).TrimLeft(chset);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::Trim( const value_type *sCharSet )
{
	_MakeUnique();
	return TrimRight(sCharSet).TrimLeft(sCharSet);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::TrimRight( value_type ch )
{
	const value_type chset[2] = { ch,0 };
	return TrimRight(chset);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::TrimRight( const value_type *sCharSet )
{
	if (!sCharSet || !(*sCharSet) || length() < 1)
		return *this;

	const value_type *last = m_str + length()-1;
	const value_type *str = last;
	while( (str != m_str) && (_strchr(sCharSet,*str) != 0))
		str--;

	if (str != last)
	{
		// Just shrink length of the string.
		size_type nNewLength = (size_type)(str - m_str) + 1; // m_str can change in _MakeUnique
		_MakeUnique();
		_header()->nLength = nNewLength;
		m_str[nNewLength] = 0;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::TrimRight()
{
	if (length() < 1)
		return *this;

	const value_type *last = m_str + length() - 1;
	const value_type *str = last;
	while( (str != m_str) && (isspace(*str) != 0) )
		str--;

	if(str != last)		// something changed?
	{
		// Just shrink length of the string.
		size_type nNewLength = (size_type)(str - m_str) + 1; // m_str can change in _MakeUnique
		_MakeUnique();
		_header()->nLength = nNewLength;
		m_str[nNewLength] = 0;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::TrimLeft( value_type ch )
{
	const value_type chset[2] = { ch,0 };
	return TrimLeft(chset);
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::TrimLeft( const value_type *sCharSet )
{
	if (!sCharSet || !(*sCharSet))
		return *this;

	const value_type *str = m_str;
	while( (*str != 0) && (_strchr(sCharSet,*str) != 0))
		str++;

	if (str != m_str)
	{
		size_type nOff = (size_type)(str - m_str); // m_str can change in _MakeUnique
		_MakeUnique();
		size_type nNewLength = length() - nOff;
		_move( m_str,m_str+nOff,nNewLength+1 );
		_header()->nLength = nNewLength;
		m_str[nNewLength] = 0;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T>& CryStringT<T>::TrimLeft()
{
	const value_type *str = m_str;
	while( (*str != 0) && (isspace(*str) != 0))
		str++;

	if (str != m_str)
	{
		size_type nOff = (size_type)(str - m_str); // m_str can change in _MakeUnique
		_MakeUnique();
		size_type nNewLength = length() - nOff;
		_move( m_str,m_str+nOff,nNewLength+1 );
		_header()->nLength = nNewLength;
		m_str[nNewLength] = 0;
	}

	return *this;
}

template <class T> 
inline CryStringT<T> CryStringT<T>::Right( size_type count ) const
{
	if (count == npos)
		return CryStringT<T>();
	else if (count > length())
		return *this;

	return CryStringT<T>( m_str+length()-count,count );
}

template <class T> 
inline CryStringT<T> CryStringT<T>::Left( size_type count ) const
{
	if (count == npos)
		return CryStringT<T>();
	else if (count > length())
		count = length();

	return CryStringT<T>( m_str,count );
}

// strspn equivalent
template <class T> 
inline CryStringT<T> CryStringT<T>::SpanIncluding( const_str charSet ) const
{
	assert(_IsValidString(charSet));
	return Left( (size_type)strspn(m_str,charSet ) );
}

// strcspn equivalent
template <class T> 
inline CryStringT<T> CryStringT<T>::SpanExcluding(const_str charSet ) const
{
	assert(_IsValidString(charSet));
	return Left( (size_type)strcspn(m_str,charSet ) );
}

//////////////////////////////////////////////////////////////////////////
template <class T> 
inline CryStringT<T> CryStringT<T>::Tokenize( const_str charSet, int &nStart ) const
{
	if (nStart < 0)
	{
		return CryStringT<T>();
	}

	if (!charSet)
		return *this;

	const_str sPlace = m_str + nStart;
	const_str sEnd = m_str + length();
	if (sPlace < sEnd)
	{
		int nIncluding = (int)strspn(sPlace,charSet);

		if ((sPlace+nIncluding) < sEnd)
		{
			sPlace += nIncluding;
			int nExcluding = (int)strcspn(sPlace,charSet);
			int nFrom = nStart+nIncluding;
			nStart = nFrom + nExcluding + 1;

			return substr( nFrom,nExcluding );
		}
	}
	// Return empty string.
	nStart = -1;
	return CryStringT<T>();
}

#endif // __CryString_h__
