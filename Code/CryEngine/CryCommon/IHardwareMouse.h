/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:

System "Hardware mouse" cursor with reference counter.
This is needed because Menus / HUD / Profiler / or whatever
can use the cursor not at the same time be successively
=> We need to know when to enable/disable the cursor.

-------------------------------------------------------------------------
History:
- 18:12:2006   Created by Julien Darré

*************************************************************************/
#ifndef __IHARDWAREMOUSE_H__
#define __IHARDWAREMOUSE_H__

//-----------------------------------------------------------------------------------------------------
/*
	When double-click, the sequence is the following:
		- HARDWAREMOUSEEVENT_LBUTTONDOWN
		- HARDWAREMOUSEEVENT_LBUTTONUP
		- HARDWAREMOUSEEVENT_LBUTTONDOUBLECLICK
		- HARDWAREMOUSEEVENT_LBUTTONUP
*/
//-----------------------------------------------------------------------------------------------------

enum EHARDWAREMOUSEEVENT
{
	HARDWAREMOUSEEVENT_MOVE,
	HARDWAREMOUSEEVENT_LBUTTONDOWN,
	HARDWAREMOUSEEVENT_LBUTTONUP,
	HARDWAREMOUSEEVENT_LBUTTONDOUBLECLICK,
};

//-----------------------------------------------------------------------------------------------------

struct IHardwareMouseEventListener
{
	virtual void OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent) = 0;
};

//-----------------------------------------------------------------------------------------------------

struct IHardwareMouse
{
	virtual void Release() = 0;

	// We need to register after the creation of the device but before its init
	virtual void OnPreInitRenderer() = 0;

	// We need to register after the creation of input to emulate mouse
	virtual void OnPostInitInput() = 0;

	// Call that in your WndProc on WM_MOUSEMOVE, WM_LBUTTONDOWN and WM_LBUTTONUP
	virtual void Event(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent) = 0;

	// Add/Remove whoever is interested by the mouse cursor
	virtual void AddListener		(IHardwareMouseEventListener *pHardwareMouseEventListener) = 0;
	virtual void RemoveListener	(IHardwareMouseEventListener *pHardwareMouseEventListener) = 0;

	// Called only in Editor when switching from editing to game mode
	virtual void SetGameMode(bool bGameMode) = 0;

	// Increment when you want to show the cursor, decrement otherwise
	virtual void IncrementCounter() = 0;
	virtual void DecrementCounter() = 0;

	// Standard get/set functions, mainly for Gamepad emulation purpose
	virtual void GetHardwareMousePosition(float *pfX,float *pfY) = 0;
	virtual void SetHardwareMousePosition(float fX,float fY) = 0;

	// Same as above, but relative to upper-left corner to the client area of our app
	virtual void GetHardwareMouseClientPosition(float *pfX,float *pfY) = 0;
	virtual void SetHardwareMouseClientPosition(float fX,float fY) = 0;

	// I consider a call to that function as a workaround...
	virtual void Reset(bool bVisibleByDefault) = 0;
	virtual void ConfineCursor(bool confine) = 0;
};

//-----------------------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------------------