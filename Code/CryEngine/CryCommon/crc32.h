////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   crc32.h
//  Version:     v1.00
//  Created:     31/10/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __crc32_h__
#define __crc32_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <string.h>	//for strlen

class Crc32Gen 
{
public:
	Crc32Gen();
	//! Creates a CRC from a text string 
	unsigned int GetCRC32( const char *text ) const;
	unsigned int GetCRC32( const char *data,int size,unsigned int ulCRC ) const;

	unsigned int GetCRC32Lowercase( const char *text ) const;
	unsigned int GetCRC32Lowercase( const char *data,int size,unsigned int ulCRC ) const;

protected:
	unsigned int crc32_table[256];  //!< Lookup table array 
	void init_CRC32_Table();  //!< Builds lookup table array 
	unsigned int reflect( unsigned int ref, char ch); //!< Reflects CRC bits in the lookup table 
	unsigned int get_CRC32( const char *data,int size,unsigned int ulCRC ) const;
	unsigned int get_CRC32Lowercase( const char *data,int size,unsigned int ulCRC ) const;
};

inline unsigned int Crc32Gen::GetCRC32( const char *text ) const
{
	int len = strlen(text);
	return GetCRC32( text,len,0xffffffff );
}

inline unsigned int Crc32Gen::GetCRC32( const char *data, int size, unsigned int ulCRC ) const
{
	return get_CRC32( data,size,ulCRC );
}

inline unsigned int Crc32Gen::GetCRC32Lowercase( const char *text ) const
{
	int len = strlen(text);
	return GetCRC32Lowercase( text,len,0xffffffff );
}

inline unsigned int Crc32Gen::GetCRC32Lowercase( const char *data, int size, unsigned int ulCRC ) const
{
	return get_CRC32Lowercase( data,size,ulCRC );
}

inline unsigned int Crc32Gen::get_CRC32Lowercase( const char *data,int size,unsigned int ulCRC ) const
{	
	int len; 
	unsigned char* buffer;

	// Get the length. 
	len = size; 

	// Save the text in the buffer. 
	buffer = (unsigned char*)data; 
	// Perform the algorithm on each character in the string, using the lookup table values. 

	while(len--) 
	ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ tolower(*buffer++)]; 
	// Exclusive OR the result with the beginning value. 
	return ulCRC ^ 0xffffffff; 
} 

inline unsigned int Crc32Gen::get_CRC32( const char *data, int size, unsigned int ulCRC ) const
{
	// Pass a text string to this function and it will return the CRC.

	// Once the lookup table has been filled in by the two functions above,
	// this function creates all CRCs using only the lookup table.

	// Be sure to use unsigned variables, because negative values introduce high bits where zero bits are required.

	// Start out with all bits set high.
	int len; 
	unsigned char* buffer;

	// Get the length. 
	len = size; 

	// Save the text in the buffer. 
	buffer = (unsigned char*)data; 
	// Perform the algorithm on each character in the string, using the lookup table values. 

	while(len--) 
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++]; 
	// Exclusive OR the result with the beginning value. 
	return ulCRC ^ 0xffffffff; 
} 



#endif // __crc32_h__