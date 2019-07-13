//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:CrySizer.h
//  Declaration and definition of the CrySizer class, which is used to
//  calculate the memory usage by the subsystems and components, to help
//  the artists keep the memory budged low.
//
//	History:
//	December 03, 2002 : Created by Sergiy Migdalskiy <sergiy@crytek.de>
//
//////////////////////////////////////////////////////////////////////

#ifndef _CRY_COMMON_CRY_SIZER_INTERFACE_HDR_
#define _CRY_COMMON_CRY_SIZER_INTERFACE_HDR_

#ifdef WIN64
#include <string.h> // workaround for Amd64 compiler
#endif

#include "IResourceCollector.h"				// IResourceCollector

// flags applicable to the ICrySizer (retrieved via getFlags() method)
//
enum ICrySizerFlagsEnum
{
	// if this flag is set, during getSize(), the subsystem must count all the objects
	// it uses in the other subsystems also
	CSF_RecurseSubsystems = 1 << 0,

	CSF_Reserved1 = 1 << 1,
	CSF_Reserved2 = 1 << 2
};


//////////////////////////////////////////////////////////////////////////
// interface ICrySizer
// USAGE
//   An instance of this class is passed down to each and every component in the system.
//   Every component it's passed to optionally pushes its name on top of the 
//     component name stack (thus ensuring that all the components calculated down
//     the tree will be assigned the correct subsystem/component name)
//   Every component must Add its size with one of the Add* functions, and Add the
//     size of all its subcomponents recursively
//   In order to push the component/system name on the name stack, the clients must
//     use the SIZER_COMPONENT_NAME macro or CrySizerComponentNameHelper class:
//
//   void X::getSize (ICrySizer* pSizer)
//   {
//     SIZER_COMPONENT_NAME(pSizer, X);
//     if (!pSizer->Add (this))
//       return;
//     pSizer->Add (m_arrMySimpleArray);
//     pSizer->Add (m_setMySimpleSet);
//     m_pSubobject->getSize (pSizer);
//   }
//
//   The Add* functions return bool. If they return true, then the object has been added
//   to the set for the first time, and you should go on recursively adding all its children.
//   If it returns false, then you can spare time and rather not go on into recursion;
//   however it doesn't reflect on the results: an object that's added more than once is
//   counted only once.
//
// WARNING:
//   If you have an array (pointer), you should Add its size with addArray
class ICrySizer
{
public:
	// this class is used to push/pop the name to/from the stack automatically
	// (to exclude stack overruns or underruns at runtime)
	friend class CrySizerComponentNameHelper;

	virtual void Release() = 0;

	// Return total calculated size.
	virtual size_t GetTotalSize() = 0;

	// adds an object identified by the unique pointer (it needs not be
	// the actual object position in the memory, though it would be nice,
	// but it must be unique throughout the system and unchanging for this object)
	// nCount parameter is only used for counting number of objects, it doesnt affect the size of the object.
	// RETURNS: true if the object has actually been added (for the first time)
	//          and calculated
	virtual bool AddObject (const void* pIdentifier, size_t nSizeBytes,int nCount=1) = 0;

	template <typename T>
	bool Add (const T* pId, size_t num)
	{
		return AddObject(pId, num * sizeof(T));
	}

	template <class T>
	bool Add (const T& rObject)
	{
		return AddObject (&rObject, sizeof(T));
	}

	// used to collect the assets needed for streaming and to gather statistics
	// always returns a valid reference
	virtual IResourceCollector &GetResourceCollector()=0;

	bool Add (const char* szText)
	{
		return AddObject(szText, strlen(szText)+1);
	}

	template <class StringCls>
	bool AddString (const StringCls& strText)
	{
		if (!strText.empty())
    {
      // Workaround: X360 compiler crash (XDK: 5426)
#ifndef XENON
			return AddObject (strText.c_str(), strText.GetAllocatedMemory());


#endif
    }
		else
			return false;
	}
#ifdef _XSTRING_
	template <class Elem, class Traits, class Allocator>
	bool Add (const std::basic_string<Elem, Traits, Allocator>& strText)
	{
		AddString (strText);
		return true;
	}
#endif

 #ifndef NOT_USE_CRY_STRING
	bool Add (const string& strText)
	{
		AddString(strText);
		return true;
	}
 #endif

	template <class Container>
	bool AddContainer (const Container& rContainer)
	{
		if (!rContainer.empty())
			return AddObject (&(*rContainer.begin()),rContainer.size()*sizeof(typename Container::value_type));
		return false;
	}

	// returns the flags
	unsigned GetFlags()const {return m_nFlags;}

protected:
	// these functions must operate on the component name stack
	// they are to be only accessible from within class CrySizerComponentNameHelper
	// which should be used through macro SIZER_COMPONENT_NAME
	virtual void Push (const char* szComponentName) = 0;
	// pushes the name that is the name of the previous component . (dot) this name
	virtual void PushSubcomponent (const char* szSubcomponentName) = 0;
	virtual void Pop () = 0;

	unsigned m_nFlags;
};

//////////////////////////////////////////////////////////////////////////
// This is on-stack class that is only used to push/pop component names
// to/from the sizer name stack.
// 
// USAGE:
//
//   Create an instance of this class at the start of a function, before
//   calling Add* methods of the sizer interface. Everything added in the
//   function and below will be considered this component, unless
//   explicitly set otherwise.
//
class CrySizerComponentNameHelper
{
public:
	// pushes the component name on top of the name stack of the given sizer
	CrySizerComponentNameHelper (ICrySizer* pSizer, const char* szComponentName, bool bSubcomponent):
		m_pSizer(pSizer)
	{
		if (bSubcomponent)
			pSizer->PushSubcomponent (szComponentName);
		else
			pSizer->Push (szComponentName);
	}

	// pops the component name off top of the name stack of the sizer 
	~CrySizerComponentNameHelper()
	{
		m_pSizer->Pop();
	}

protected:
	ICrySizer* m_pSizer;
};

// use this to push (and automatically pop) the sizer component name at the beginning of the
// getSize() function
#define SIZER_COMPONENT_NAME(pSizerPointer, szComponentName) CrySizerComponentNameHelper __sizerHelper(pSizerPointer, szComponentName, false)
#define SIZER_SUBCOMPONENT_NAME(pSizerPointer, szComponentName) CrySizerComponentNameHelper __sizerHelper(pSizerPointer, szComponentName, true)


//////////////////////////////////////////////////////////////////////////
// Helper functions to calculate size of the std containers.
//////////////////////////////////////////////////////////////////////////
namespace stl
{
	struct MapLikeStruct
	{
		bool color;
		void *parent;
		void *left;
		void *right;
	};
	template <class Map>
	inline size_t size_of_map( const Map &m )
	{
		if (!m.empty())
		{
			return m.size()*sizeof(typename Map::value_type) + m.size()*sizeof(MapLikeStruct);
		}
		return 0;
	}
	template <class Map>
	inline size_t size_of_set( const Map &m )
	{
		if (!m.empty())
		{
			return m.size()*sizeof(typename Map::value_type) + m.size()*sizeof(MapLikeStruct);
		}
		return 0;
	}
};

#endif //_CRY_COMMON_CRY_SIZER_INTERFACE_HDR_
