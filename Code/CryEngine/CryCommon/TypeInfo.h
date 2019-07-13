////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2007.
// -------------------------------------------------------------------------
//  File name:   TypeInfo.h
//  Version:     v1.00
//  Created:     19/03/2007 by Scott.
//  Description: Macros and other definitions needed for TypeInfo declarations.
// -------------------------------------------------------------------------
//  History:		 Refactored out of Endian.h
//
////////////////////////////////////////////////////////////////////////////

#ifndef __TypeInfo_h__
#define __TypeInfo_h__

#pragma once

//////////////////////////////////////////////////////////////////////////
// Meta-type support.
//////////////////////////////////////////////////////////////////////////

// Currently enable type info for all platforms.
#if !defined(ENABLE_TYPE_INFO)
#define ENABLE_TYPE_INFO
#endif
#ifdef ENABLE_TYPE_INFO

struct CTypeInfo;

// If TypeInfo exists for T, it is accessed via TypeInfo(T*).
// Default TypeInfo() is implemented by a static struct member function.
template<class T>
inline const CTypeInfo& TypeInfo(T*)
{
	return T::TypeInfo();
}

// Declare a class's TypeInfo member
#define STRUCT_INFO											\
	static const CTypeInfo& TypeInfo();

#define NULL_STRUCT_INFO									\
	static const CTypeInfo& TypeInfo()	{ return *(CTypeInfo*)0; }

// Declare an override for a type without TypeInfo() member (e.g. basic type)
#define DECLARE_TYPE_INFO(Type)		\
	template<> const CTypeInfo& TypeInfo(Type*);

// Template version.
#define DECLARE_TYPE_INFO_T(Type)		\
	template<class T> const CTypeInfo& TypeInfo(Type<T>*);

// Type info declaration, with additional prototypes for string conversions.
#define BASIC_TYPE_INFO(Type)										\
	string ToString(Type const& val, int flags);	\
	bool FromString(Type& val, const char *s);	\
	DECLARE_TYPE_INFO(Type)

#else // ENABLE_TYPE_INFO

#define STRUCT_INFO
#define NULL_STRUCT_INFO
#define DECLARE_TYPE_INFO(Type)
#define DECLARE_TYPE_INFO_T(Type)
#define BASIC_TYPE_INFO(T)

#endif // ENABLE_TYPE_INFO

// Specify automatic tool generation of TypeInfo bodies.
#define AUTO_STRUCT_INFO							STRUCT_INFO
#define AUTO_TYPE_INFO								DECLARE_TYPE_INFO
#define AUTO_TYPE_INFO_T							DECLARE_TYPE_INFO_T

#define AUTO_STRUCT_INFO_LOCAL				STRUCT_INFO
#define AUTO_TYPE_INFO_LOCAL					DECLARE_TYPE_INFO
#define AUTO_TYPE_INFO_LOCAL_T				DECLARE_TYPE_INFO_T

// Overrides for basic types.
BASIC_TYPE_INFO(bool)

BASIC_TYPE_INFO(char)
BASIC_TYPE_INFO(wchar_t)
BASIC_TYPE_INFO(signed char)
BASIC_TYPE_INFO(unsigned char)
BASIC_TYPE_INFO(short)
BASIC_TYPE_INFO(unsigned short)
BASIC_TYPE_INFO(int)
BASIC_TYPE_INFO(unsigned int)
BASIC_TYPE_INFO(long)
BASIC_TYPE_INFO(unsigned long)
BASIC_TYPE_INFO(int64)
BASIC_TYPE_INFO(uint64)

BASIC_TYPE_INFO(float)
BASIC_TYPE_INFO(double)

BASIC_TYPE_INFO(void*)

BASIC_TYPE_INFO(string)

#endif // __TypeInfo_h__
