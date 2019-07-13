////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2007.
// -------------------------------------------------------------------------
//  File name:   Endian.h
//  Version:     v1.00
//  Created:     16/2/2006 by Scott,Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:		 19/3/2007: Separated Endian support from basic TypeInfo declarations.
//
////////////////////////////////////////////////////////////////////////////

#ifndef __Endian_h__
#define __Endian_h__
#pragma once

//////////////////////////////////////////////////////////////////////////
// Endian support
//////////////////////////////////////////////////////////////////////////

#if defined(NEED_ENDIAN_SWAP) || defined(_DEBUG)

// The endian swapping function.
void SwapEndian(void* pData, size_t nCount, const CTypeInfo& Info, size_t nSizeCheck);

template<class T>
inline void SwapEndian(T* t, size_t nCount)
{
	SwapEndian(t, nCount, TypeInfo((T*)0), sizeof(T));
#if !defined(NEED_ENDIAN_SWAP)
	SwapEndian(t, nCount, TypeInfo((T*)0), sizeof(T));
#endif
}

#else	// NEED_ENDIAN_SWAP || _DEBUG

// Null endian swapping.
template<class T>
inline void SwapEndian(T* t, size_t nCount)
{
}

#endif	// NEED_ENDIAN_SWAP || _DEBUG

// Derivative functions.
template<class T>
inline void SwapEndian(T& t)
{
	SwapEndian(&t, 1);
}

template<class T>
inline T SwapEndianValue(T t)
{
	SwapEndian(t);
	return t;
}

// Object-oriented data extraction for endian-swapping reading.
template<class T>
inline T* StepData(unsigned char*& pData, int* pnDataSize = NULL, int nCount = 1, bool bSwap = true)
{
	T* Elems = (T*)pData;
	if (bSwap)
		SwapEndian(Elems, nCount);
	pData += sizeof(T)*nCount;
	if (pnDataSize)
		*pnDataSize -= sizeof(T)*nCount;
	return Elems;
}

template<class T>
inline void StepData(T*& Result, unsigned char*& pData, int* pnDataSize = NULL, int nCount = 1, bool bSwap = true)
{
	Result = StepData<T>(pData, pnDataSize, nCount, bSwap);
}

template<class T>
inline void StepDataCopy(T* Dest, unsigned char*& pData, int* pnDataSize = NULL, int nCount = 1, bool bSwap = true)
{
	memcpy(Dest, StepData<T>(pData, pnDataSize, nCount, bSwap), nCount*sizeof(T));
}

#endif // __Endian_h__
