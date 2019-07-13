//////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2007.
// -------------------------------------------------------------------------
//  File name:   TypeInfo.h
//  Version:     v1.00
//  Created:     03/05/2005 by Scott.
//  Description: Declaration of CTypeInfo and related types.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CRY_TYPEINFO_H
#define __CRY_TYPEINFO_H
#pragma once

#include <platform.h>
#include <CryArray.h>

#include <ISystem.h>

typedef const char*	cstr;

//---------------------------------------------------------------------------
// Allocator using fixed pool, no heap memory.
// Suitable for app-init-time allocations.

template<size_t SIZE>
struct FStaticAlloc
{
	static inline void* Alloc( void* oldptr, int oldsize, int newsize )
	{  
		static char		aMem[SIZE];
		static size_t	nAlloc = 0;

		// Check for realloc; will succeed only if it was the last item alloced.
		char* newptr = aMem + nAlloc - oldsize;
		if (oldptr != 0 && oldptr != newptr)
			// Realloc will succeed only if it was the last item alloced.
			return 0;

		if (nAlloc + newsize-oldsize > SIZE)
			// Out of memory
			return 0;
		nAlloc += newsize-oldsize;
		return newptr;
	}
};

template<class INT>
inline INT ToInt(size_t size, const void* data)
{
	switch (size)
	{
		case sizeof(int8): return static_cast<INT>(*(const int8*)data);
		case sizeof(int16): return static_cast<INT>(*(const int16*)data);
		case sizeof(int32): return static_cast<INT>(*(const int32*)data);
		case sizeof(int64): return static_cast<INT>(*(const int64*)data);
		default: assert(0); return 0;
	}
}

template<class INT>
inline bool FromInt(size_t size, void* data, INT val)
{
	switch (size)
	{
		case sizeof(int8):	*(int8*)data = static_cast<int8>(val); return *(int8*)data == val;
		case sizeof(int16): *(int16*)data = static_cast<int16>(val); return *(int16*)data == val;
		case sizeof(int32): *(int32*)data = static_cast<int32>(val); return *(int32*)data == val;
		case sizeof(int64): *(int64*)data = val; return true;
		default: assert(0); return false;
	}
}

//---------------------------------------------------------------------------
// Type info base class

struct CTypeInfo
{
	cstr		Name;
	size_t	Size;

	CTypeInfo( cstr name, size_t size )
		: Name(name), Size(size)
	{}

	template<class T>
		bool IsType() const
	{
		return this == &TypeInfo((T*)0);
	}

	//
	// Data access interface.
	//

	// String conversion flags.
	static const int WRITE_SKIP_DEFAULT	= 1;	// Omit default values on writing.
	static const int WRITE_TRUNCATE_SUB = 2;	// Remove trailing empty sub-vaules.
	static const int READ_SKIP_EMPTY		= 4;	// Do not set values from empty strings (otherwise, set to zero).

	virtual string ToString(const void* data, int flags = 0, const void* def_data = 0) const
	{
		assert(0);
		return "";
	}

	// Get value from string, return success.
	virtual bool FromString(void* data, cstr str, int flags = 0) const
	{
		assert(0);
		return false;
	}

	// Track memory used by any internal structures (not counting object size itself).
	// Add to CrySizer as needed, return remaining mem count.
	virtual size_t GetMemoryUsage(ICrySizer* pSizer, const void* data) const
	{
		return 0;
	}

	//
	// Structure interface.
	//
	struct CVarInfo
	{
		cstr								Name;
		const CTypeInfo&		Type;
		size_t							ArrayDim: 29,				
												bBitfield: 1,				// ArrayDim is number of bits.
												bBaseClass: 1,			// Sub-var is actually a base class.
												bUnionAlias: 1;			// If 2nd or greater element of a union.
		size_t							Offset: 24,					// Offset in bytes from struct start.
												BitOffset: 6,				// Additional offset in bits for bitfields.
																						// Bit offset is computed in declaration order; on some platforms, it goes high to low.
												BitWordWidth: 2;		// Word width of bitfield, bits = 8 << BitWordWidth

		explicit CVarInfo( const CTypeInfo& type, int dim = 1, bool bBase = false)
			: Type(type), ArrayDim(dim), bBaseClass(bBase), bBitfield(0), bUnionAlias(0), Offset(0), BitOffset(0), BitWordWidth(0), Name("")
		{}

		// Manipulators and accessors.
		CVarInfo& SetName(cstr name)					
		{ 
			// Skip prefixes in Name.
			cstr sMain = name;
			while (islower(*sMain) || *sMain == '_')
				sMain++;
			if (isupper(*sMain))
				Name = sMain;
			else
				Name = name;
			return *this;
		}
		CVarInfo& SetOffset(size_t offset)		{ Offset = offset; return *this; }
		CVarInfo& SetArray(int dim)						{ ArrayDim *= dim; return *this; }
		CVarInfo& SetBitfield(int bits)				{ bBitfield = 1; ArrayDim = bits; return *this; }
		CVarInfo& SetBaseClass()							{ bBaseClass = 1; return *this; }

		cstr GetDisplayName() const						{ assert(*Name || bBaseClass); return Name; }
		size_t GetDim() const									{ return bBitfield ? 1 : ArrayDim; }
		size_t GetSize() const								{ return bBitfield? (size_t)1 << BitWordWidth : Type.Size * ArrayDim; }
		size_t GetElemSize() const						{ return bBitfield? (size_t)1 << BitWordWidth : Type.Size; }
		size_t GetBits() const								{ return bBitfield ? ArrayDim : ArrayDim * Type.Size * 8; }
		bool IsBaseClass() const							{ return bBaseClass; }
	
		// Useful functions.
		void* GetAddress(void* base) const
		{
			return (char*)base + Offset;
		}
		bool FromString(void* base, cstr str, int flags = 0) const
		{
			assert(!bBitfield);
			return Type.FromString( (char*)base + Offset, str, flags );
		}
		string ToString(void* base, int flags = 0) const
		{
			assert(!bBitfield);
			return Type.ToString( (char*)base + Offset, flags );
		}

		// Variable attrs.
		CVarInfo& AddAttr(cstr name, float val)	{ CVarAttr attr = { name, val, 0 }; Attrs.push_back(attr); return *this; }
		CVarInfo& AddAttr(cstr name, int val)		{ CVarAttr attr = { name, f32(val), 0 }; Attrs.push_back(attr); return *this; }
		CVarInfo& AddAttr(cstr name, cstr text)	{ CVarAttr attr = { name, 0, text }; Attrs.push_back(attr); return *this; }

		bool GetAttr(cstr name) const								{ float _num; cstr _str; return GetAttr(name, _num, _str); }
		bool GetAttr(cstr name, float& val) const		{ cstr _str; return GetAttr(name, val, _str); }
		bool GetAttr(cstr name, cstr& val) const		{ float _num; return GetAttr(name, _num, val); }

	protected:

		struct CVarAttr
		{
			cstr	Name;
			float	fValue;
			cstr	sValue;
		};
		DynArray< CVarAttr, FStaticAlloc<0x4000> >	Attrs;

		bool GetAttr(cstr name, float& num, cstr& str) const;
	};

	// Structure var iteration:
	virtual const CVarInfo* NextSubVar(const CVarInfo* pPrev) const		{ return 0; }
	inline bool HasSubVars() const  { return NextSubVar(0) != 0; }
	#define AllSubVars( pVar, Info ) (const CTypeInfo::CVarInfo* pVar = 0; pVar = (Info).NextSubVar(pVar); )

	// Named var search.
	virtual const CVarInfo* FindSubVar(cstr name) const  { return 0; }

	//
	// Enumeration interface.
	//
	struct CEnumElem
	{
		int			Value;
		cstr		FullName;
		cstr		ShortName;
	};
	virtual int EnumElemCount() const										{ return 0; }
	virtual CEnumElem const* EnumElem(int nIndex) const { return 0; }
};

//---------------------------------------------------------------------------
// Template for base types, using global To/FromString functions.

template<class T>
struct TTypeInfo: CTypeInfo
{
	TTypeInfo( cstr name )
		: CTypeInfo( name, sizeof(T) )
	{}
	virtual string ToString(const void* data, int flags = 0, const void* def_data = 0) const
	{
		if (def_data)
		{
			// Convert and compare default values separately.
			string val = ::ToString(*(const T*)data, flags & ~WRITE_SKIP_DEFAULT);
			if (val.length())
			{
				string def_val = ::ToString(*(const T*)def_data, flags & ~WRITE_SKIP_DEFAULT);
				if (val == def_val)
					return string();
			}
			return val;
		}
		else
			return ::ToString(*(const T*)data, flags);
	}
	virtual bool FromString(void* data, cstr str, int flags = 0) const
	{
		if (!*str || !::FromString(*(T*)data, str))
		{
			if (flags & READ_SKIP_EMPTY)
				return false;
			*(T*)data = T();
		}
		return true;
	}
	virtual size_t GetMemoryUsage(ICrySizer* pSizer, void const* data) const
	{
		return 0;
	}
};

//---------------------------------------------------------------------------
// Memory usage override.
template<> 
size_t TTypeInfo<string>::GetMemoryUsage(ICrySizer* pSizer, void const* data) const;

//---------------------------------------------------------------------------
// TypeInfo for structs

struct CStructInfo: CTypeInfo
{
	CStructInfo( cstr name, size_t size, size_t num_vars = 0, CVarInfo* vars = 0 );
	virtual string ToString(const void* data, int flags = 0, const void* def_data = 0) const;
	virtual bool FromString(void* data, cstr str, int flags = 0) const;
	virtual size_t GetMemoryUsage(ICrySizer* pSizer, void const* data) const;

	virtual const CVarInfo* NextSubVar(const CVarInfo* pPrev) const
	{
		pPrev = pPrev ? pPrev+1 : Vars.begin();
		return pPrev < Vars.end() ? pPrev : 0;
	}
	virtual const CVarInfo* FindSubVar(cstr name) const;

protected:
	Array<CVarInfo>	Vars;

	bool FromStringParse(void* data, cstr& str, int flags) const;
};

//---------------------------------------------------------------------------
// TypeInfo for enums

struct CEnumInfo: CTypeInfo
{
	CEnumInfo( cstr name, size_t size, size_t num_elems = 0, CEnumElem* elems = 0 );

	virtual string ToString(const void* data, int flags = 0, const void* def_data = 0) const
	{
		if (flags & WRITE_SKIP_DEFAULT)
		{
			if (def_data && ToInt<int>(Size, def_data) == ToInt<int>(Size, data))
				return "";
			flags &= ~WRITE_SKIP_DEFAULT;
		}
		return ToString(ToInt<int>(Size, data), flags);
	}
	virtual bool FromString(void* data, cstr str, int flags = 0) const
	{
		int val;
		return FromString(val, str, flags) && FromInt(Size, data, val);
	}
	virtual int EnumElemCount() const
	{
		return Elems.size();
	}
	virtual CEnumElem const* EnumElem(int nIndex) const 
	{ 
		return nIndex < (int)Elems.size() ? &Elems[nIndex] : 0;
	}

protected:
	Array<CEnumElem>	Elems;
	bool							bRegular;
	int								MinValue, MaxValue;

	string ToString(int val, int flags = 0) const;
	bool FromString(int& val, cstr str, int flags = 0) const;
};

#include "CryStructPack.h"

#endif // __CRY_TYPEINFO_H
