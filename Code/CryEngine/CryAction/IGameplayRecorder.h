// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IGAMEPLAYRECORDER_H__
#define __IGAMEPLAYRECORDER_H__

#pragma once

typedef enum
{
	eGE_DiscreetSample = 0,
	eGE_GameReset,
	eGE_GameStarted,
	eGE_GameEnd,
	eGE_Connected,
	eGE_Disconnected,
	eGE_Renamed,
	eGE_ChangedTeam,
	eGE_Died,
	eGE_Scored,
	eGE_Currency,
	eGE_Rank,
  eGE_Spectator,
	eGE_ScoreReset,

	eGE_AttachedAccessory,

	eGE_ZoomedIn,
	eGE_ZoomedOut,

	eGE_Kill,
	eGE_Death,
	eGE_Revive,

	eGE_SuitModeChanged,

	eGE_Hit,
	eGE_Damage,

	eGE_WeaponHit,
	eGE_WeaponReload,
	eGE_WeaponShot,
	eGE_WeaponMelee,
	eGE_WeaponFireModeChanged,

	eGE_Explosion,

	eGE_ItemSelected,
	eGE_ItemPickedUp,
	eGE_ItemDropped,
	eGE_ItemBought,

	eGE_EnteredVehicle,
	eGE_LeftVehicle,

	eGE_Last
} EGameplayEvent;


struct GameplayEvent
{
	GameplayEvent(): event(0), description(0), value(0) {};
	GameplayEvent(uint8 evt, const char *desc=0, float val=0.0f, void *xtra=0)
		: event(evt), description(desc), value(val), extra(xtra) {};

	uint8				event;
	const char *description;
	float				value;
	void				*extra;
};


struct IGameplayListener
{
	virtual void OnGameplayEvent(IEntity *pEntity, const GameplayEvent &event) = 0;
};

struct IMetadata;

struct IGameplayRecorder
{
	virtual void RegisterListener(IGameplayListener *pGameplayListener) = 0;
	virtual void UnregisterListener(IGameplayListener *pGameplayListener) = 0;

	virtual CTimeValue GetSampleInterval() const = 0;

	virtual void Event(IEntity *pEntity, const GameplayEvent &event) = 0;

	virtual void OnGameData(const IMetadata* pGameData) = 0;
};


#endif //__IGAMEPLAYRECORDER_H__

