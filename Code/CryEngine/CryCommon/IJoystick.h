////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2006.
// -------------------------------------------------------------------------
//  File name:   IJoystick.h
//  Version:     v1.00
//  Created:     7/8/2006 by MichaelS.
//  Compilers:   Visual Studio.NET 2005
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __IJOYSTICK_H__
#define __IJOYSTICK_H__

struct ISplineInterpolator;

class IJoystickChannel
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual const char* GetName() const = 0;

	virtual void* GetTarget() = 0;

	virtual void SetFlipped(bool flipped) = 0;
	virtual bool GetFlipped() const = 0;

	virtual void SetVideoMarkerOffset(float offset) = 0;
	virtual float GetVideoMarkerOffset() const = 0;
	virtual void SetVideoMarkerScale(float scale) = 0;
	virtual float GetVideoMarkerScale() const = 0;

	virtual void CleanupKeys(float fErrorMax) = 0;

	// TODO: Many of the above functions can be removed due to this function.
	virtual int GetSplineCount() = 0;
	virtual ISplineInterpolator* GetSpline(int splineIndex) = 0;
};

class IJoystick
{
public:
	typedef Vec3_tpl<unsigned char> Color;

	enum ChannelType
	{
		ChannelTypeHorizontal,
		ChannelTypeVertical
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual uint64 GetID() const = 0;

	virtual void SetName(const char* szName) = 0;
	virtual const char* GetName() = 0;

	virtual IJoystickChannel* GetChannel(ChannelType type) = 0;
	virtual void SetChannel(ChannelType type, IJoystickChannel* pChannel) = 0;

	virtual const Vec2& GetCentre() const = 0;
	virtual void SetCentre(const Vec2& vCentre) = 0;
	virtual const Vec2& GetDimensions() const = 0;
	virtual void SetDimensions(const Vec2& vDimensions) = 0;

	virtual void SetColor(const Color& colour) = 0;
	virtual Color GetColor() const = 0;
};

class IJoystickSet
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual void SetName(const char* name) = 0;
	virtual const char* GetName() const = 0;
	virtual void AddJoystick(IJoystick* pJoystick) = 0;
	virtual void RemoveJoystick(IJoystick* pJoystick) = 0;
	virtual int GetJoystickCount() const = 0;
	virtual IJoystick* GetJoystick(int index) = 0;
	virtual IJoystick* GetJoystickAtPoint(const Vec2& vPosition) = 0;
	virtual IJoystick* GetJoystickByID(uint64 id) = 0;
	virtual void Serialize(XmlNodeRef &nodeJoysticks,bool bLoading) = 0;
};

class IJoystickContext
{
public:
	// Create a new joystick channel for an effector.
	virtual IJoystick* CreateJoystick(uint64 id) = 0;
	virtual IJoystickSet* CreateJoystickSet() = 0;
	virtual IJoystickSet* LoadJoystickSet(const char* filename, bool bNoWarnings = false) = 0;
};

#endif //__IJOYSTICK_H__
