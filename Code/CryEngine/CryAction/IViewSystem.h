/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: View System interfaces.
  
 -------------------------------------------------------------------------
  History:
  - 17:9:2004 : Created by Filippo De Luca

*************************************************************************/
#ifndef __IVIEWSYSTEM_H__
#define __IVIEWSYSTEM_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <ISerialize.h>

//
#define VIEWID_NORMAL 0
#define VIEWID_FOLLOWHEAD 1
#define VIEWID_VEHICLE 2
#define VIEWID_RAGDOLL 3

enum EMotionBlurType
{
	eMBT_None = 0,
	eMBT_Accumulation = 1,
	eMBT_Velocity = 2
};

struct SViewParams
{
	SViewParams() :
		position(ZERO),
		rotation(IDENTITY),
		nearplane(0.0f),
		fov(0.0f),
		viewID(0),
		groundOnly(false),
		shakingRatio(0.0f),
		currentShakeQuat(IDENTITY),
		currentShakeShift(ZERO),
		idTarget(0),
		targetPos(ZERO),
		frameTime(0.0f),
		angleVel(0.0f),
		vel(0.0f),
		dist(0.0f),
		blend(true),
		blendPosSpeed(5.0f),
		blendRotSpeed(10.0f),
		blendPosOffset(ZERO),
		blendRotOffset(IDENTITY),
		viewIDLast(0),
		positionLast(ZERO),
		rotationLast(IDENTITY)
	{
	}

	void SetViewID(uint8 id,bool blend=true)
	{
		viewID = id;
		if (!blend)
			viewIDLast = id;
	}

	void UpdateBlending(float frameTime)
	{
		//if necessary blend the view
		if (blend)
		{
			if (viewIDLast != viewID)
			{
				blendPosOffset = positionLast - position;
				blendRotOffset = (rotationLast / rotation).GetNormalized();
			}
			else
			{
				blendPosOffset -= blendPosOffset * min(1.0f,blendPosSpeed * frameTime);
				blendRotOffset = Quat::CreateSlerp(blendRotOffset, IDENTITY, frameTime * blendRotSpeed);
			}

			position += blendPosOffset;
			rotation *= blendRotOffset;
		}
		else
		{
			blendPosOffset.zero();
			blendRotOffset.SetIdentity();
		}

		viewIDLast = viewID;
	}

	void BlendFrom(const SViewParams& params)
	{
		positionLast = params.position;
		rotationLast = params.rotation;
		blend = true;
		viewIDLast = 0xff;
	}

	void SaveLast()
	{
		if (viewIDLast != 0xff)
		{
			positionLast = position;
			rotationLast = rotation;
		}
		else
		{
			viewIDLast = 0xfe;
		}
	}

	void ResetBlending()
	{
		blendPosOffset.zero();
		blendRotOffset.SetIdentity();
	}

	//
	Vec3 position;//view position
	Quat rotation;//view orientation

	float nearplane;//custom near clipping plane, 0 means use engine defaults
	float	fov;

	uint8 viewID;
	
	//view shake status
	bool  groundOnly;
	float shakingRatio;//whats the ammount of shake, from 0.0 to 1.0
	Quat currentShakeQuat;//what the current angular shake
	Vec3 currentShakeShift;//what is the current translational shake

  // For damping camera movement.
  EntityId idTarget;  // Who we're watching. 0 == nobody.
  Vec3 targetPos;     // Where the target was.
  float frameTime;    // current dt.
  float angleVel;     // previous rate of change of angle.
  float vel;          // previous rate of change of dist between target and camera.
  float dist;         // previous dist of cam from target

	//blending
	bool	blend;
	float blendPosSpeed;
	float blendRotSpeed;
	Vec3	blendPosOffset;
	Quat	blendRotOffset;

private:
	uint8 viewIDLast;
	Vec3 positionLast;//last view position
	Quat rotationLast;//last view orientation
};

struct IGameObject;
struct IEntity;
struct IAnimSequence;
struct SCameraParams;
struct ISound;

struct IView
{
	virtual void Update(float frameTime,bool isActive) = 0;
	virtual void LinkTo(IGameObject *follow) = 0;
	virtual void LinkTo(IEntity *follow) = 0;
	virtual unsigned int GetLinkedId() = 0;

	virtual void SetCurrentParams( SViewParams &params ) = 0;
	virtual const SViewParams * GetCurrentParams() = 0;
	virtual void SetViewShake(Ang3 shakeAngle,Vec3 shakeShift,float duration,float frequency,float randomness,int shakeID, bool bFlipVec = true, bool bUpdateOnly=false, bool bGroundOnly=false) = 0;
	virtual void ResetShaking() = 0;
};

struct IViewSystemListener
{
	virtual bool OnBeginCutScene(IAnimSequence* pSeq, bool bResetFX) = 0;
	virtual bool OnEndCutScene(IAnimSequence* pSeq) = 0;
	virtual void OnPlayCutSceneSound(IAnimSequence* pSeq, ISound* pSound) = 0;
	virtual bool OnCameraChange(const SCameraParams& cameraParams) = 0;
};

struct IViewSystem
{
	virtual IView *CreateView() = 0;

	virtual void SetActiveView(IView *pView) = 0;
	virtual void SetActiveView(unsigned int viewId) = 0;

	//utility functions
	virtual IView *GetView(unsigned int viewId) = 0;
	virtual IView *GetActiveView() = 0;

	virtual unsigned int GetViewId(IView *pView) = 0;
	virtual unsigned int GetActiveViewId() = 0;

	virtual IView *GetViewByEntityId(unsigned int id, bool forceCreate = false) = 0;

	virtual bool AddListener(IViewSystemListener* pListener) = 0;
	virtual bool RemoveListener(IViewSystemListener* pListener) = 0;

	virtual void Serialize(TSerialize ser) = 0;

	// Get default distance to near clipping plane.
	virtual float GetDefaultZNear() = 0;

	virtual void SetBlendParams(float fBlendPosSpeed, float fBlendRotSpeed, bool performBlendOut) = 0;

	// Used by time demo playback.
	virtual void SetOverrideCameraRotation( bool bOverride,Quat rotation ) = 0;

	virtual bool IsPlayingCutScene() const = 0;
};

#endif //__IVIEWSYSTEM_H__
