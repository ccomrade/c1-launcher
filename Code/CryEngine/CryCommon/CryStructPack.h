/////////////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//  Packing and unpacking of stored structures
//
//  File: CryStructPack.h
//  Description:
//		Utility functions for packing and unpacking stored structures.
//		Structure packing/unpacking is required for machines where the size of
//		a pointer is different from 4 bytes (e.g. Cell PPU).
//
//  History:
//		- Created by Sascha Demetrio <saschad@crytek.com>
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __STRUCTPACK_H
#define __STRUCTPACK_H
#pragma once

#include <algorithm> // functor.h needs std::find
#include "functor.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#define NEED_STRUCT_PACK	(SIZEOF_PTR != 4)

#if NEED_STRUCT_PACK
uint32 StructSize(const CTypeInfo& typeInfo, uint32 limit = UINT_MAX);
uint32 StructUnpack(const CTypeInfo& typeInfo, uint8* ptr, const uint8* buffer,
										uint32 limit = UINT_MAX, bool expandPointers = false);
uint32 StructPack(const CTypeInfo& typeInfo, const uint8* ptr, uint8* buffer,
									uint32 limit = UINT_MAX, bool packPointers = false);
#else
static inline uint32 StructSize(const CTypeInfo& typeInfo,
																uint32 limit = UINT_MAX)
{
	return std::max(limit, typeInfo.Size);
}

static inline uint32 StructUnpack(const CTypeInfo& typeInfo,
																	uint8* ptr,
																	const uint8* buffer,
																	uint32 limit = UINT_MAX,
																	bool expandPointers = false)
{
	const uint32 size = StructSize(typeInfo, limit);

	memcpy(ptr, buffer, size);
	if (size < typeInfo.Size)
	{
		assert(size == limit);
		memset(ptr + size, 0, typeInfo.Size - size);
	}
#ifdef NEED_ENDIAN_SWAP
	SwapEndian(ptr, 1, typeInfo, typeInfo.Size);
#endif
	return size;
}

static inline uint32 StructPack(const CTypeInfo& typeInfo,
																const uint8* ptr,
																uint8* buffer,
                                uint32 limit = UINT_MAX,
																bool packPointers = false)
{
	const uint32 size = StructSize(typeInfo, limit);

	memcpy(buffer, ptr, size);
	if (size < typeInfo.Size)
	{
		assert(size == limit);
		memset(buffer + size, 0, typeInfo.Size - size);
	}
#ifdef NEED_ENDIAN_SWAP
	SwapEndian(buffer, 1, typeInfo, typeInfo.Size);
#endif
	return size;
}

#endif

// Get the size of a packed structure. If only part of the structure is
// relevant, then (limit) must be specified as the offset of the first
// structure element not relevant for packing/unpacking. The offset may
// be determined using the offsetof() macro.
template <class T>
static inline uint32 StructSize(uint32 limit = UINT_MAX)
{
#if NEED_STRUCT_PACK
	return StructSize(T::TypeInfo(), limit);
#else
	return std::max(limit, sizeof(T));
#endif
}

// Unpack a stored structure.
// @param ptr
//		The unpacked object will be stored to *ptr. The function takes care
//		of endian swapping if needed.
// @param buffer
//		Buffer holding the raw data read from storage.
// @param limit
//		Optional. If only part of the structure should be unpacked, then limit is
//		the offset of the first structure element that should be skipped. The
//		limit can be determined using the offsetof() macro.
// @param expandPointers
//		Flag indicating if pointers should be expanded/extracted. The default is
//		false. If this flag is not set, then all pointers are set to NULL in
//		the extracted structure.
//
// Note: If a limit is specified, then the structure fields not read from
// storage are filled with zeroes. As a consequence, the size of the buffer
// referenced by ptr must be at least sizeof(T).
template <class T>
static inline void StructUnpack(T* ptr,
																const void *buffer,
																uint32 limit = UINT_MAX,
																bool expandPointers = false)
{
#if NEED_STRUCT_PACK
	StructUnpack(T::TypeInfo(), (uint8*)ptr, (const uint8*)buffer, limit, expandPointers);
#else
	memcpy(ptr, buffer, std::max(limit, sizeof(T)));
	SwapEndian(ptr);
#endif
}

// Unpack a stored structure.
// This function is a variant of the StructUnpack() function above. The
// readFn functor is used for reading the structure from the storage.
template <class T> 
static inline void StructUnpack(T* ptr,
																const Functor2<void*, uint32>& readFn,
																uint32 limit = UINT_MAX,
																bool expandPointers = false)
{
#if SIZE_OF_PTR != 4
#	if defined(__GNUC__)
	const uint32 size = StructSize<T>(limit);
	uint8 buffer[size];
# else
	const uint32 size = StructSize<T>(limit);
	uint8* buffer = (uint8*) alloca(size);
# endif
	readFn(buffer, size);
	StructUnpack(ptr, buffer, limit, expandPointers);
#else
	readFn(ptr, std::max(limit, sizeof T));
	SwapEndian(ptr);
#endif
}

// Pack a structure for storage.
template <class T>
static inline void StructPack(const T* ptr,
															void *buffer,
															uint32 limit = UINT_MAX,
															bool packPointers = false)
{
#if NEED_STRUCT_PACK
	StructPack(T::TypeInfo(), (const void *)ptr, buffer, limit, packPointers);
#else
	const uint32 size = std::max(limit, sizeof(T));
	const CTypeInfo& typeInfo = T::TypeInfo();

#ifdef NEED_ENDIAN_SWAP
	if (size < typeInfo.Size) {
		assert(size == limit);
		uint8 tmpBuffer[typeInfo.Size];
		memcpy(tmpBuffer, ptr, size);
    memset(tmpBuffer+ size, 0, typeInfo.Size - size);
		SwapEndian(tmpBuffer, 1, typeInfo, typeInfo.Size);
		memcpy(buffer, tmpBuffer, size);
	} else
	{
		assert(size == typeInfo.Size);
		memcpy(buffer, ptr, size);
		SwapEndian(buffer, 1, typeInfo, size);
	}
#else
	memcpy(buffer, ptr, size);
#endif
#endif
}

template <class T>
static inline void StructPack(const T* ptr,
															const Functor2<const void*, uint32>& writeFn,
															uint32 limit = UINT_MAX,
															bool packPointers = false)
{
#if NEED_STRUCT_PACK
	const uint32 size = StructSize<T>(limit);
	uint8 buffer[size];

	StructPack(ptr, buffer, limit, packPointers);
	writeFn(buffer, size);
#else
	const uint32 size = std::max(limit, sizeof(T));
#ifdef NEED_ENDIAN_SWAP
	const CTypeInfo& typeInfo = T::TypeInfo();
	uint8 buffer[typeInfo.Size];
	memcpy(buffer, ptr, size);
	if (size < typeInfo.Size)
	{
		assert(size == limit);
		memset(buffer + size, 0, typeInfo.Size - size);
	}
	SwapEndian(buffer, 1, T::TypeInfo(), size);
	writeFn(buffer, size);
#else
	writeFn(ptr, size);
#endif
#endif
}

#endif // __STRUCTPACK_H ////////////////////////////////////////////////////
