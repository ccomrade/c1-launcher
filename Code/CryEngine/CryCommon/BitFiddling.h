/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: various integer bit fiddling workarounds
  
 -------------------------------------------------------------------------
  History:
  - 29:03:2006   12:44 : Created by Craig Tiller

*************************************************************************/
#ifndef __BITFIDDLING_H__
#define __BITFIDDLING_H__

#pragma once

// this function returns the integer logarithm of various numbers without branching
#define IL2VAL(mask,shift) \
	c |= ((x&mask)!=0) * shift; \
	x >>= ((x&mask)!=0) * shift

template <typename TInteger>
inline bool IsPowerOfTwo( TInteger x )
{
	return (x & (x-1)) == 0;
}

inline uint8 IntegerLog2( uint8 x )
{
	uint8 c = 0;
	IL2VAL(0xf0,4);
	IL2VAL(0xc,2);
	IL2VAL(0x2,1);
	return c;
}
inline uint16 IntegerLog2( uint16 x )
{
	uint16 c = 0;
	IL2VAL(0xff00,8);
	IL2VAL(0xf0,4);
	IL2VAL(0xc,2);
	IL2VAL(0x2,1);
	return c;
}
inline uint32 NumLeadingZeros( uint32 x )
{
	int y, m, n;
	y = -(int)(x>>16);
	m = (y >> 16) & 16;
	n = 16 - m;
	x = x >> m;

	y = x - 0x100;
	m = (y>>16) & 8;
	n = n + m;
	x = x << m;

	y = x - 0x1000;
	m = (y >> 16) & 4;
	n = n + m;
	x = x << m;

	y = x - 0x4000;
	m = (y >> 16) & 2;
	n = n + m;
	x = x << m;

	y = x >> 14;
	m = y & ~(y>>1);
	return n + 2 - m;
}
inline uint32 IntegerLog2( uint32 x )
{
	return 31 - NumLeadingZeros(x);
/*
	uint32 c = 0;
	IL2VAL(0xffff0000u,16);
	IL2VAL(0xff00,8);
	IL2VAL(0xf0,4);
	IL2VAL(0xc,2);
	IL2VAL(0x2,1);
	return c;
*/
}
inline uint64 IntegerLog2( uint64 x )
{
	uint64 c = 0;
	IL2VAL(0xffffffff00000000ull,32);
	IL2VAL(0xffff0000u,16);
	IL2VAL(0xff00,8);
	IL2VAL(0xf0,4);
	IL2VAL(0xc,2);
	IL2VAL(0x2,1);
	return c;
}
#undef IL2VAL

template <typename TInteger>
inline TInteger IntegerLog2_RoundUp( TInteger x )
{
	return 1 + IntegerLog2(x-1);
}

static ILINE uint8 BitIndex( uint8 v )
{
	uint8 c = (v & 0xaau) != 0;
	c |= (( v & 0xf0u ) != 0) << 2;
	c |= (( v & 0xccu ) != 0) << 1;
	return c;
}

static ILINE uint8 CountBits( uint8 v )
{
	uint8 c = v;
	c = ((c>>1) & 0x55) + (c&0x55);
	c = ((c>>2) & 0x33) + (c&0x33);
	c = ((c>>4) & 0x0f) + (c&0x0f);
	return c;
}

template <uint32 ILOG>
struct CompileTimeIntegerLog2
{
	static const uint32 result = 1 + CompileTimeIntegerLog2<(ILOG>>1)>::result;
};
template <>
struct CompileTimeIntegerLog2<0>
{
	static const uint32 result = 0;
};

template <uint32 ILOG>
struct CompileTimeIntegerLog2_RoundUp
{
	static const uint32 result = CompileTimeIntegerLog2<ILOG>::result + ((ILOG & (ILOG-1))>0);
};

// Character-to-bitfield mapping

inline uint32 AlphaBit(char c)
{
	return c >= 'a' && c <= 'z' ? 1 << (c-'z'+31) : 0;
}

inline uint32 AlphaBits(uint32 wc)
{
	// Handle wide multi-char constants, can be evaluated at compile-time.
	return AlphaBit((char)wc) 
		| AlphaBit((char)(wc>>8))
		| AlphaBit((char)(wc>>16))
		| AlphaBit((char)(wc>>24));
}

inline uint32 AlphaBits(const char* s)
{
	// Handle string of any length.
	uint32 n = 0;
	while (*s)
		n |= AlphaBit(*s++);
	return n;
}

// is a bit on in a new bit field, but off in an old bit field
static ILINE bool TurnedOnBit( unsigned bit, unsigned oldBits, unsigned newBits )
{
	return (newBits & bit) != 0 && (oldBits & bit) == 0;
}

#endif
