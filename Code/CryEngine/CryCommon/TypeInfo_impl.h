
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2007.
// -------------------------------------------------------------------------
//  File name:   TypeInfo.h
//  Version:     v1.00
//  Created:     03/05/2005 by Scott.
//  Description: Declaration of CTypeInfo, and other things to access meta-type info.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#include "CryTypeInfo.h"

//---------------------------------------------------------------------------
// DECLARATION MACROS
// Used to construct meta TypeInfo objects in AutoTypeInfo files.
// Two possible levels of TypeInfo: default, with size and offset info only, allowing Endian conversion;
// and full, with string, attr, and enum info, allowing UI and serialisation.
// The full version is selected by the MAKE_FULL_TYPE_INFO macro.

#ifdef ENABLE_TYPE_INFO

// For now, always compile full TypeInfo, 
// as there are issues with full TypeInfo objects referencing basic TypeInfo objects.
#ifndef MAKE_FULL_TYPE_INFO
	#define MAKE_FULL_TYPE_INFO
#endif

#ifdef MAKE_FULL_TYPE_INFO
	#define TYPE_INFO_NAME(n)					#n
#else
	#define TYPE_INFO_NAME(n)					""
#endif

// Define TypeInfo for a basic type (undecomposable as far as TypeInfo cares), with external string converters.
#define TYPE_INFO_BASIC(T)													\
	template<> const CTypeInfo& TypeInfo(T*) {				\
		static TTypeInfo< T > Info(TYPE_INFO_NAME(T));	\
		return Info;																		\
	}																									\

//---------------------------------------------------------------------------
// Define TypeInfo for a primitive type, without string conversion.
#define TYPE_INFO_PLAIN(T)															\
	template<> const CTypeInfo& TypeInfo(T*) {						\
	static CTypeInfo Info(TYPE_INFO_NAME(T), sizeof(T));	\
		return Info;																				\
	}																											\

//---------------------------------------------------------------------------
// Macros for constructing StructInfos (invoked by AutoTypeInfo.h)

#define STRUCT_INFO_EMPTY_BODY(T)												\
	{																											\
		static CStructInfo Info(TYPE_INFO_NAME(T), sizeof(T));				\
		return Info;																				\
	}																											\

#define STRUCT_INFO_EMPTY(T)											\
	const CTypeInfo& T::TypeInfo()									\
		STRUCT_INFO_EMPTY_BODY(T)											\

#define STRUCT_INFO_BEGIN(T)											\
	const CTypeInfo& T::TypeInfo() {								\
		typedef T STRUCTYPE;													\
		static CStructInfo::CVarInfo Vars[] = {				\

// Version of offsetof that takes the address of base classes.
// In Visual C++ at least, the fake address of 0 will NOT work;
// the compiler shifts ALL base classes to 0.
#define base_offsetof(s, b)			(size_t( static_cast<b*>( reinterpret_cast<s*>(0x100) )) - 0x100)

#define STRUCT_BASE_INFO(BaseType)																													\
			TYPE_INFO(BaseType) .SetBaseClass() .SetOffset(base_offsetof(STRUCTYPE, BaseType)),		\

#define STRUCT_VAR_INFO(VarName, VarType)																										\
			VarType .SetName(TYPE_INFO_NAME(VarName)) .SetOffset(offsetof(STRUCTYPE, VarName)),		\

#define TYPE_INFO(t)						CStructInfo::CVarInfo(::TypeInfo((t*)0))
#define TYPE_ARRAY(n, info)			(info).SetArray(n)
#define	TYPE_POINTER(info)			CStructInfo::CVarInfo(::TypeInfo((void**)0))
#define TYPE_REF(info)					CStructInfo::CVarInfo(::TypeInfo((void**)0))

#define STRUCT_BITFIELD_INFO(VarName, VarType, Bits)																													\
			CStructInfo::CVarInfo( ::TypeInfo((VarType*)0) ) .SetName(TYPE_INFO_NAME(VarName)) .SetBitfield(Bits),	\

// Var attrs.
#ifdef MAKE_FULL_TYPE_INFO
	#define ATTR_INFO(Name,Val)			.AddAttr(#Name,Val)
#else
	#define ATTR_INFO(Name,Val)
#endif

#define STRUCT_INFO_END(T)																										\
	};																																					\
	static CStructInfo Info(TYPE_INFO_NAME(T), sizeof(T), sizeof Vars / sizeof *Vars, Vars);		\
	return Info;																																\
}																																							\

// Template versions

#define STRUCT_INFO_T_EMPTY(T, TArgs, TDecl)	\
	template TDecl STRUCT_INFO_EMPTY(T TArgs)

#define STRUCT_INFO_T_BEGIN(T, TArgs, TDecl)	\
	template TDecl STRUCT_INFO_BEGIN(T TArgs)

#define STRUCT_INFO_T_END(T, TArgs, TDecl)		\
	STRUCT_INFO_END(T TArgs)

#define STRUCT_INFO_T_INSTANTIATE(T, TArgs)	\
	template const CTypeInfo& T TArgs::TypeInfo();

// External versions

#define STRUCT_INFO_TYPE_EMPTY(T)							\
	template<> const CTypeInfo& TypeInfo(T*)		\
		STRUCT_INFO_EMPTY_BODY(T)									\

#define STRUCT_INFO_TYPE_BEGIN(T)							\
	template<> const CTypeInfo& TypeInfo(T*) {	\
		typedef T STRUCTYPE;											\
		static CStructInfo::CVarInfo Vars[] = {		\

#define STRUCT_INFO_TYPE_END(T)								\
	STRUCT_INFO_END(T)


// Template versions

#define STRUCT_INFO_TYPE_T_EMPTY(T, TArgs, TDecl)			\
	template TDecl const CTypeInfo& TypeInfo(T TArgs*)	\
		STRUCT_INFO_EMPTY_BODY(T TArgs)										\

//---------------------------------------------------------------------------
// Enum type info

#ifdef MAKE_FULL_TYPE_INFO

	// Enums represented as full CEnumInfo types, with string conversion.
	#define ENUM_INFO_BEGIN(T)											\
		template<> const CTypeInfo& TypeInfo(T*) {		\
			static CEnumInfo::CEnumElem Elems[] = {			\

	#define ENUM_ELEM_INFO(Scope, Elem)							\
				{ Scope Elem, #Elem },

	#define ENUM_INFO_END(T)												\
			};																																						\
			static CEnumInfo Info(#T, sizeof(T), sizeof Elems / sizeof *Elems, Elems);		\
			return Info;																																	\
		}																																								\

#else // MAKE_FULL_TYPE_INFO

	// Enums represented as simple types, with no elements or string conversion.
	#define ENUM_INFO_BEGIN(T)																												\
		inline string ToString(T const& val, int flags)  { assert(0); return ""; }			\
		inline bool FromString(T& val, const char *s)  { assert(0); return false; }			\
		TYPE_INFO_BASIC(T)																															\

	#define ENUM_ELEM_INFO(Scope, Elem)
	#define ENUM_INFO_END(T)

#endif // MAKE_FULL_TYPE_INFO}

#else // ENABLE_TYPE_INFO

// Empty definitions for all type infos
#define TYPE_INFO_PLAIN(T)
#define STRUCT_INFO_EMPTY(T)
#define STRUCT_INFO_BEGIN(T)
#define STRUCT_BASE_INFO(BaseType)
#define STRUCT_VAR_INFO(VarName, VarType)
#define STRUCT_BITFIELD_INFO(VarName, VarType, Bits)
#define STRUCT_INFO_END(T)

// Template versions
#define STRUCT_INFO_T_EMPTY(T, TArgs, TDecl)
#define STRUCT_INFO_T_BEGIN(T, TArgs, TDecl)
#define STRUCT_INFO_T_END(T, TArgs, TDecl)

#define STRUCT_INFO_T_INSTANTIATE(T, TArgs)

// External versions
#define STRUCT_INFO_TYPE_EMPTY(T)
#define STRUCT_INFO_TYPE_BEGIN(T)
#define STRUCT_INFO_TYPE_END(T)

// Template versions
#define STRUCT_INFO_TYPE_T_EMPTY(T, TArgs, TDecl)

// Enum type info

#define ENUM_INFO_BEGIN(T)
#define ENUM_ELEM_INFO(Scope, Elem)
#define ENUM_INFO_END(T)

#endif // ENABLE_TYPE_INFO