//////////////////////////////////////////////////////////////////////
//
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:IProcess.h
//	Description: Process common interface
//
//	History:
//	-September 03,2001:Created by Marco Corbetta
//
//////////////////////////////////////////////////////////////////////

#ifndef IPROCESS_H
#define IPROCESS_H

#if _MSC_VER > 1000
# pragma once
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
struct IProcess
{
	virtual	bool	Init() = 0;	
	virtual void	Update() = 0;
	virtual void	RenderWorld(const int nRenderFlags, const class CCamera &cam, const char *szDebugName, const int dwDrawFlags = -1, const int nFilterFlags=-1) = 0;
	virtual	void	ShutDown() = 0;		
	virtual	void	SetFlags(int flags) = 0;
	virtual	int		GetFlags(void) = 0;
};

#endif