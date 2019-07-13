
/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Marsenne Twister PRNG. See MT.h for more info.

-------------------------------------------------------------------------
History:
- 28:7:2005: File created and minor changes by Marco Corbetta
- Implementation (c) 2003 by Jasper Bedaux

*************************************************************************/

#include "MTPseudoRandom.h"
// non-inline function definitions and static member definitions cannot
// reside in header file because of the risk of multiple declarations

// initialization of static private members
unsigned int CMTRand_int32::m_nState[n+1]; 
int CMTRand_int32::p = 0; 
bool CMTRand_int32::m_bInit = false;

void CMTRand_int32::gen_state() { // generate new m_nState vector
	for (int i = 0; i < (n - m); ++i)
		m_nState[i] = m_nState[i + m] ^ twiddle(m_nState[i], m_nState[i + 1]);
	for (int i = n - m; i < (n - 1); ++i)
		m_nState[i] = m_nState[i + m - n] ^ twiddle(m_nState[i], m_nState[i + 1]);
	m_nState[n - 1] = m_nState[m - 1] ^ twiddle(m_nState[n - 1], m_nState[0]);
	p = 0; // reset position
}

void CMTRand_int32::seed(unsigned int s) {  // m_bInit by 32 bit seed
	//if (s == 0)
		//m_nRandom = 1;	
	for (int i = 0; i < n; ++i)
		m_nState[i]=0x0UL;
	m_nState[0] = s & 0xFFFFFFFFUL; // for > 32 bit machines
	for (int i = 1; i < n; ++i) {
		m_nState[i] = 1812433253UL * (m_nState[i - 1] ^ (m_nState[i - 1] >> 30)) + i;
		// see Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier
		// in the previous versions, MSBs of the seed affect only MSBs of the array m_nState
		// 2002/01/09 modified by Makoto Matsumoto
		m_nState[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
	}
	p = n; // force gen_state() to be called for next random number
}

void CMTRand_int32::seed(const unsigned int* array, int size) { // m_bInit by array
	seed(19650218UL);
	int i = 1, j = 0;
	for (int k = ((n > size) ? n : size); k; --k) {
		m_nState[i] = (m_nState[i] ^ ((m_nState[i - 1] ^ (m_nState[i - 1] >> 30)) * 1664525UL))
			+ array[j] + j; // non linear
		m_nState[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
		++j; j %= size;
		if ((++i) == n) { m_nState[0] = m_nState[n - 1]; i = 1; }
	}
	for (int k = n - 1; k; --k) {
		m_nState[i] = (m_nState[i] ^ ((m_nState[i - 1] ^ (m_nState[i - 1] >> 30)) * 1566083941UL)) - i;
		m_nState[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
		if ((++i) == n) { m_nState[0] = m_nState[n - 1]; i = 1; }
	}
	m_nState[0] = 0x80000000UL; // MSB is 1; assuring non-zero initial array
	p = n; // force gen_state() to be called for next random number
}
