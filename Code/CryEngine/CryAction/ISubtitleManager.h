////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2007.
// -------------------------------------------------------------------------
//  File name:   ISubtitleManager.h
//  Version:     v1.00
//  Created:     29/01/2007 by AlexL.
//  Compilers:   Visual Studio.NET 2005
//  Description: Interface to the Subtitle Manager 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ISUBTITLEMANAGER_H__
#define __ISUBTITLEMANAGER_H__
#pragma once

//////////////////////////////////////////////////////////////////////////
struct ISubtitleHandler
{
	virtual void ShowSubtitle(ISound* pSound, bool bShow) = 0;
	virtual void ShowSubtitle(const char* subtitleLabel, bool bShow) = 0;
};

//////////////////////////////////////////////////////////////////////////
struct ISubtitleManager 
{
	virtual void SetHandler(ISubtitleHandler* pHandler) = 0;
	virtual ISubtitleHandler* GetHandler() = 0;
	
	// enables/disables subtitles manager
	virtual void SetEnabled(bool bEnabled) = 0;

	// automatic mode. reacts on every voice sound being played
	// can be disabled/enabled
	virtual void SetAutoMode(bool bOn) = 0;

	virtual void ShowSubtitle(ISound* pSound, bool bShow) = 0;
	virtual void ShowSubtitle(const char* subtitleLabel, bool bShow) = 0;

};

#endif // __ISUBTITLEMANAGER_H__