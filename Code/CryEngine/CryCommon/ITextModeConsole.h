/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:
Allows creation of text mode displays the for dedicated server
-------------------------------------------------------------------------
History:
- Nov 22,2006:	Created by Craig Tiller

*************************************************************************/
#ifndef __ITEXTMODECONSOLE_H__
#define __ITEXTMODECONSOLE_H__

#pragma once

struct ITextModeConsole
{
	virtual Vec2_tpl<int> BeginDraw() = 0;
	virtual void PutText( int x, int y, const char * msg ) = 0;
	virtual void EndDraw() = 0;
};

#endif
