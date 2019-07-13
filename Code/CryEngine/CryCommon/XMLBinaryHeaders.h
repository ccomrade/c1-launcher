//---------------------------------------------------------------------------
// Copyright 2006 Crytek GmbH
// Created by: Michael Smith
//---------------------------------------------------------------------------
#ifndef __XMLBINARYHEADERS_H__
#define __XMLBINARYHEADERS_H__

namespace XMLBinary
{
	class IError
	{
	public:
		virtual ~IError() {}
		virtual void GetDescription(int nBufferSize, char* szDescriptionBuffer) = 0;
	};

	class Node
	{
	public:
		int nTagStringIndex;
		int nContentStringIndex;
		int nParentIndex;
		int nFirstAttributeIndex;
		int nAttributeCount;

		AUTO_STRUCT_INFO
	};

	class Attribute
	{
	public:
		int nKeyStringIndex;
		int nValueStringIndex;

		AUTO_STRUCT_INFO
	};

	class BinaryFileHeader
	{
	public:
		static const char* sk_szCorrectSignature;
		char szSignature[8];
		int nXMLSize;
		int nNodeTablePosition;
		int nNodeCount;
		int nAttributeTablePosition;
		int nAttributeCount;
		int nStringTablePosition;
		int nStringCount;
		int nStringDataPosition;
		int nStringDataSize;

		AUTO_STRUCT_INFO
	};

	class String
	{
	public:
		int nPosition;
		int nLength;

		AUTO_STRUCT_INFO
	};
}

#endif //__XMLBINARYHEADERS_H__
