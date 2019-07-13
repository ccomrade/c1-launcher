// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifdef WIN32
#pragma once
#include "unknwn.h"		// IUnknown

struct IDirectBee :public IUnknown
{
	// call this before creation of texture/shader/..  to pass the name to DirectBee
	// \param szFileName can be 0
	// \param szSubName can be 0
	virtual void PushName( const char *szFileName=0, const char *szSubName=0 )=0;

	//! \param szFileName must not be 0
	//! \return success
	virtual bool LoadFrame( const char *szFileName )=0;

//	virtual void SetName( const void *pPtr, const char *szFileName, const char *szSubName )=0;
};

static const GUID IDD_IDirectBee = { 0x29ad19a0, 0x3c26, 0x49f5, { 0xa3, 0xba, 0x9a, 0x66, 0x55, 0x33, 0xd4, 0xe1 } };	// {29AD19A0-3C26-49f5-A3BA-9A665533D4E1}

#endif // WIN32