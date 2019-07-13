// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _CRY_COMMON_CRY_RESOURCECOLLECTOR_INTERFACE_HDR_
#define _CRY_COMMON_CRY_RESOURCECOLLECTOR_INTERFACE_HDR_

#pragma once

// used to collect the assets needed for streaming and to gather statistics
struct IResourceCollector
{
	// Arguments:
	//   dwMemSize 0xffffffff if size is unknown
	// Returns:
	//   true=new resource was added, false=resource was already registered
	virtual bool AddResource( const char *szFileName, const uint32 dwMemSize ) { return true; }

	// Arguments:
	//   szFileName - needs to be registered before with AddResource()
	//   pInstance - must not be 0
	virtual void AddInstance( const char *szFileName, void *pInstance ) {}
	//
	// Arguments:
	//   szFileName - needs to be registered before with AddResource()
	virtual void OpenDependencies( const char *szFileName ) {}
	//
	virtual void CloseDependencies() {}
};




#endif // _CRY_COMMON_CRY_RESOURCECOLLECTOR_INTERFACE_HDR_


