//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//	STL Utils header
//	
//	File: STLutils.h
//	Description : Various convenience utility functions for STL and alike
//  Used in Animation subsystem, and in some tools
//
//	History:
//	-:Created by Sergiy Migdalskiy
//
//////////////////////////////////////////////////////////////////////
#ifndef _STL_UTILS_HEADER_
#define _STL_UTILS_HEADER_

#include <algorithm>
#include <map>
#include <vector>

#define USE_HASH_MAP

#if (_MSC_VER >= 1400) && !defined(_STLP_BEGIN_NAMESPACE) // Visual Studio 2005 without STLPort
#include <hash_map>
#undef std__hash_map
#define std__hash_map stdext::hash_map
#elif defined(LINUX)
#include "platform.h"
#include <ext/hash_map>
#define std__hash_map __gnu_cxx::hash_map
#else
#include <hash_map>
#endif

#ifndef std__hash_map
#define std__hash_map std::hash_map
#endif

/*
{
	typename Map::const_iterator it = mapKeyToValue.find (key);
	if (it == mapKeyToValue.end())
		return valueDefault;
	else
		return it->second;
}

// searches the given entry in the map by key, and if there is none, returns the default value
// The values are taken/returned in REFERENCEs rather than values
template <typename Map>
inline typename Map::mapped_type& find_in_map_ref(Map& mapKeyToValue, typename Map::key_type key, typename Map::mapped_type& valueDefault)
{
	typename Map::iterator it = mapKeyToValue.find (key);
	if (it == mapKeyToValue.end())
		return valueDefault;
	else
		return it->second;
}
*/

// auto-cleaner: upon destruction, calls the clear() method
template <class T>
class CAutoClear
{
public:
	CAutoClear (T* p): m_p(p) {}
	~CAutoClear () {m_p->clear();}
protected:
	T* m_p;
};


template <class Container>
unsigned sizeofArray (const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type)*arr.size());
}

template <class Container>
unsigned sizeofVector (const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type)*arr.capacity());
}

template <class Container>
unsigned sizeofArray (const Container& arr, unsigned nSize)
{
	return arr.empty()?0u:(unsigned)(sizeof(typename Container::value_type)*nSize);
}

template <class Container>
unsigned capacityofArray (const Container& arr)
{
	return (unsigned)(arr.capacity()*sizeof(arr[0]));
}

template <class T>
unsigned countElements (const std::vector<T>& arrT, const T& x)
{
	unsigned nSum = 0;
	for (typename std::vector<T>::const_iterator iter = arrT.begin(); iter != arrT.end(); ++iter)
		if (x == *iter)
			++nSum;
	return nSum;
}

// [Timur]
/** Contain extensions for STL library.
*/
namespace stl
{
	//////////////////////////////////////////////////////////////////////////
	//! Searches the given entry in the map by key, and if there is none, returns the default value
	//////////////////////////////////////////////////////////////////////////
	template <typename Map>
	inline typename Map::mapped_type find_in_map(const Map& mapKeyToValue, const typename Map::key_type& key, typename Map::mapped_type valueDefault)
	{
		typename Map::const_iterator it = mapKeyToValue.find (key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	// searches the given entry in the map by key, and if there is none, returns the default value
	// The values are taken/returned in REFERENCEs rather than values
	template <typename Map>
	inline typename Map::mapped_type& find_in_map_ref(Map& mapKeyToValue, const typename Map::key_type& key, typename Map::mapped_type& valueDefault)
	{
		typename Map::iterator it = mapKeyToValue.find (key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Fills vector with contents of map.
	//////////////////////////////////////////////////////////////////////////
	template <class Map,class Vector>
	inline void map_to_vector( const Map& theMap,Vector &array )
	{
		array.resize(0);
		array.reserve( theMap.size() );
		for (typename Map::const_iterator it = theMap.begin(); it != theMap.end(); ++it)
		{
			array.push_back( it->second );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Fills vector with contents of set.
	//////////////////////////////////////////////////////////////////////////
	template <class Set,class Vector>
	inline void set_to_vector( const Set& theSet,Vector &array )
	{
		array.resize(0);
		array.reserve( theSet.size() );
		for (typename Set::const_iterator it = theSet.begin(); it != theSet.end(); ++it)
		{
			array.push_back( *it );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find and erase element from container.
	// @return true if item was find and erased, false if item not found.
	//////////////////////////////////////////////////////////////////////////
	template <class Container,class Value>
		inline bool find_and_erase( Container& container,const Value &value )
	{
		typename Container::iterator it = std::find( container.begin(),container.end(),value );
		if (it != container.end())
		{
			container.erase( it );
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Push back to container unique element.
	// @return true if item added, false overwise.
	template <class Container,class Value>
		inline bool push_back_unique( Container& container,const Value &value )
	{
		if (std::find(container.begin(),container.end(),value) == container.end())
		{
			container.push_back( value );
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Push back to container unique element.
	// @return true if item added, false overwise.
	template <class Container,class Iter>
	inline void push_back_range( Container& container,Iter begin,Iter end )
	{
		for (Iter it = begin; it != end; ++it)
		{
			container.push_back(*it);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in container.
	// @return true if item found.
	template <class Container,class Value>
		inline bool find( Container& container,const Value &value )
	{
		return std::find(container.begin(),container.end(),value) != container.end();
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	//
	template <class Iterator,class T>
		inline Iterator binary_find(Iterator first,Iterator last,const T& value)
	{
		Iterator it = std::lower_bound(first,last,value);
		return (it == last || value != *it) ? last : it;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	// @return true if item was inserted.
	template <class Container,class Value>
		inline bool binary_insert_unique( Container& container,const Value &value )
	{
		typename Container::iterator it = std::lower_bound(container.begin(),container.end(),value);
		if (it != container.end())
		{
			if (*it == value)
				return false;
			container.insert( it,value );
		}
		else
			container.insert( container.end(),value );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	// and erases if element found.
	// @return true if item was erased.
	template <class Container,class Value>
		inline bool binary_erase( Container& container,const Value &value )
	{
		typename Container::iterator it = std::lower_bound(container.begin(),container.end(),value);
		if (it != container.end() && *it == value)
		{
			container.erase(it);
			return true;
		}
		return false;
	}

	struct container_object_deleter
	{
		template<typename T>
			void operator()(const T* ptr) const
		{
			delete ptr;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Convert arbitary class to const char*
	//////////////////////////////////////////////////////////////////////////
	template <class Type>
		inline const char* constchar_cast( const Type &type )
	{
		return type;
	}

	//! Specialization of string to const char cast.
	template <>
		inline const char* constchar_cast( const string &type )
	{
		return type.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	//! Case sensetive less key for any type convertable to const char*.
	//////////////////////////////////////////////////////////////////////////
	template <class Type>
	struct less_strcmp : public std::binary_function<Type,Type,bool> 
	{
		bool operator()( const Type &left,const Type &right ) const
		{
			return strcmp(constchar_cast(left),constchar_cast(right)) < 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case insensetive less key for any type convertable to const char*.
	template <class Type>
	struct less_stricmp : public std::binary_function<Type,Type,bool> 
	{
		bool operator()( const Type &left,const Type &right ) const
		{
			return stricmp(constchar_cast(left),constchar_cast(right)) < 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// Hash map usage:
	// typedef stl::hash_map<string,int, stl::hash_stricmp<string> > StringToIntHash;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//! Case sensetive string hash map compare structure.
	//////////////////////////////////////////////////////////////////////////
	template <class Key>
	class hash_simple
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8	};// min_buckets = 2 ^^ N, 0 < N

			size_t operator()( const Key& key ) const
			{
				return size_t(key);
			};
			bool LessThan( const Key& key1,const Key& key2 ) const
			{
				return key1 < key2;
			}
			bool Equals( const Key& key1,const Key& key2 ) const
			{
				return key1 == key2;
			}
	};

	// simple hash class that has the avalanche property (a change in one bit affects all others)
	// ... use this if you have uint32 key values!
	class hash_uint32
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8	};// min_buckets = 2 ^^ N, 0 < N

			ILINE size_t operator()( uint32 a ) const
			{
				a = (a+0x7ed55d16) + (a<<12);
				a = (a^0xc761c23c) ^ (a>>19);
				a = (a+0x165667b1) + (a<<5);
				a = (a+0xd3a2646c) ^ (a<<9);
				a = (a+0xfd7046c5) + (a<<3);
				a = (a^0xb55a4f09) ^ (a>>16);
				return a;
			};
			ILINE bool LessThan( const uint32& key1,const uint32& key2 ) const
			{
				return key1 < key2;
			}
			ILINE bool Equals( const uint32& key1,const uint32& key2 ) const
			{
				return key1 == key2;
			}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case sensetive string hash map compare structure.
	//////////////////////////////////////////////////////////////////////////
	template <class Key>
	class hash_strcmp
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8	};// min_buckets = 2 ^^ N, 0 < N

			size_t operator()( const Key& key ) const
			{
				unsigned int h = 0; 
				const char *s = constchar_cast(key);
				for (; *s; ++s) h = 5*h + *(unsigned char*)s;
				return size_t(h);

			};
			bool LessThan( const Key& key1,const Key& key2 ) const
			{
				return strcmp(constchar_cast(key1),constchar_cast(key2)) < 0;
			}
			bool Equals( const Key& key1,const Key& key2 ) const
			{
				return strcmp(constchar_cast(key1),constchar_cast(key2)) == 0;
			}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case insensetive string hash map compare structure.
	template <class Key>
	class hash_stricmp
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8	};// min_buckets = 2 ^^ N, 0 < N

			size_t operator()( const Key& key ) const
			{
				unsigned int h = 0; 
				const char *s = constchar_cast(key);
				for (; *s; ++s) h = 5*h + tolower(*(unsigned char*)s);
				return size_t(h);

			};
			bool LessThan( const Key& key1,const Key& key2 ) const
			{
				return stricmp(constchar_cast(key1),constchar_cast(key2)) < 0;
			}
			bool Equals( const Key& key1,const Key& key2 ) const
			{
				return stricmp(constchar_cast(key1),constchar_cast(key2)) == 0;
			}
	};

#ifdef USE_HASH_MAP
	template <class CryHash>
	struct HashMap_Equality : public CryHash
	{
		template <class T>
		ILINE bool operator()( const T& a, const T& b ) const
		{
			return this->Equals(a,b);
		}
	};

	template <class CryHash>
	struct HashMap_LessThan : public CryHash
	{
		template <class T>
		ILINE bool operator()( const T& a, const T& b ) const
		{
			return this->LessThan(a,b);
		}
		template <class T>
		ILINE size_t operator()( const T& a ) const
		{
			return CryHash::operator()(a);
		}
	};

	// Support for both Microsoft and SGI kind of hash_map.
	template <class Key,class Value,class HashFunc,class Alloc = std::allocator< std::pair<Key,Value> > >
	class hash_map : 
#ifdef _STLP_HASH_MAP // STL Port
		public std__hash_map<Key,Value,HashFunc,HashMap_Equality<HashFunc>,Alloc>
	{
	public:
		hash_map() {};
		hash_map( int nHashTableSize ) : std__hash_map<Key,Value,HashFunc,HashMap_Equality<HashFunc>,Alloc>( nHashTableSize ) {}
	};
#else // _STLP_HASH_MAP
		public std__hash_map<Key,Value,HashMap_LessThan<HashFunc>,Alloc>
	{
	public:
		hash_map() {};
		hash_map( int nHashTableSize ) : std__hash_map<Key,Value,HashMap_LessThan<HashFunc>,Alloc>() {}
	};
#endif // STL Port
#else // USE_HASH_MAP
		
#endif //USE_HASH_MAP
}

#endif
