////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IDialogSystem.h
//  Version:     v1.00
//  Created:     26/7/2006 by AlexL.
//  Compilers:   Visual Studio.NET 2003
//  Description: Interface of the DialogSystem [mainly for Editor->CryAction]
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IDIALOGSYSTEM_H__
#define __IDIALOGSYSTEM_H__
#pragma once

struct IDialogScriptIterator
{
	struct SDialogScript
	{
		const char* id;
		const char* desc;
		int numRequiredActors;
		int numLines;
		bool bIsLegacyExcel;
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual bool Next(SDialogScript& ) = 0;
};
typedef _smart_ptr<IDialogScriptIterator> IDialogScriptIteratorPtr;


struct IDialogSystem
{
	virtual void Update(const float dt) = 0;
	virtual void Reset() = 0;
	virtual bool ReloadScripts() = 0;
	virtual IDialogScriptIteratorPtr CreateScriptIterator() = 0;
};

#endif
