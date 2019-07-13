// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _IAIGROUP_H_
#define _IAIGROUP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "IAISystem.h"
#include "SerializeFwd.h"

#ifdef LINUX
#	include "platform.h"
#endif

#ifdef max
#undef max
#endif


struct IAIGroup
{
	enum eAvPositionMode
	{
		AVMODE_ANY,
		AVMODE_PROPERTIES,
		AVMODE_CLASS
	};

	virtual ~IAIGroup() {}
	/// Returns the ID of the group.
	virtual int					GetGroupId() = 0;
	/// Returns the number of units in the group, see IAISystem::EGroupFlags.
	virtual int					GetGroupCount(int flags) = 0;
	/// Returns one unit (IAIObject*) which has the given rank.
	virtual IAIObject*	GetUnitInRank(int rank) const = 0;
	/// Returns true if the given AIObject is in the group
	virtual bool				IsMember(const IAIObject* pAI) const = 0;
	/// Sets specified unit's properties, see EUnitProperties.
	virtual void				SetUnitProperties(const IAIObject* obj, uint32 properties) = 0;
	virtual IAIObject*	GetAttentionTarget(bool bHostileOnly, bool bLiveOnly, const IAIObject* pSkipTarget = NULL) const = 0;
	virtual Vec3				GetAveragePosition(eAvPositionMode mode = AVMODE_ANY, uint32 unitClass = UNIT_ALL) const = 0;
	virtual int					GetUnitCount(uint32 unitPropMask=UPR_ALL) const = 0;
	/// Notifies tactical evaluation of specified state change.
	virtual void				NotifyGroupTacticState(IAIObject* pRequester, int tac, int type, float param) = 0;
	/// Returns tactical evaluation status of specified evaluation type.
	virtual int					GetGroupTacticState(IAIObject* pRequester, int tac, int type, float param) = 0;
	/// Returns tactical evaluation position of specified evaluation type.
	virtual Vec3				GetGroupTacticPoint(IAIObject* pRequester, int tac, int type, float param) = 0;
	/// Request a group readability sound; if the sound has been requested recently by one of the group members, return false
	virtual bool				RequestReadabilitySound(int id, float duration) = 0;
	/// Gets the number of attention targets in the group
	virtual int					GetTargetCount(bool bHostileOnly, bool bLiveOnly) const = 0;
	/// triggers reinforcements state
	virtual void				NotifyReinfDone(const IAIObject* obj, bool isDone) = 0;
};


/// Query types for AIGroupTactic.GetState.
/// The exact meaning is described along side with each group tactic implementation.
enum EGroupStateType
{
	GE_GROUP_STATE,
	GE_UNIT_STATE,
	GE_ADVANCE_POS,
	GE_SEEK_POS,
	GE_DEFEND_POS,
	GE_LEADER_COUNT,
	GE_MOST_LOST_UNIT,
	GE_MOVEMENT_SIGNAL,
	GE_NEAREST_SEEK,
};

/// Notify types for AIGroupTactic.Notify.
/// The exact meaning is described along side with each group tactic implementation.
enum EGroupNotifyType
{
	GN_INIT,
	GN_MARK_DEFEND_POS,
	GN_CLEAR_DEFEND_POS,
	GN_AVOID_CURRENT_POS,
	GN_PREFER_ATTACK,
	GN_PREFER_FLEE,
	GN_NOTIFY_ADVANCING,
	GN_NOTIFY_COVERING,
	GN_NOTIFY_WEAK_COVERING,
	GN_NOTIFY_HIDING,
	GN_NOTIFY_SEEKING,
	GN_NOTIFY_ALERTED,
	GN_NOTIFY_UNAVAIL,
	GN_NOTIFY_IDLE,
	GN_NOTIFY_SEARCHING,
	GN_NOTIFY_REINFORCE,
};

/// State describing the over group state.
enum EGroupState
{
	GS_IDLE,
	GS_ALERTED,
	GS_COVER,
	GS_ADVANCE,
	GS_SEEK,
	GS_SEARCH,
	GS_REINFORCE,
	LAST_GS // must be last
};

/// List of possible unit types to register with GN_INIT.
enum EGroupUnitType
{
	GU_HUMAN_CAMPER,
	GU_HUMAN_COVER,
	GU_HUMAN_SNEAKER,
	GU_HUMAN_LEADER,
	GU_HUMAN_SNEAKER_SPECOP,
	GU_ALIEN_MELEE,
	GU_ALIEN_ASSAULT,
	GU_ALIEN_MELEE_DEFEND,
	GU_ALIEN_ASSAULT_DEFEND,
	GU_ALIEN_EVADE,
};


struct IAIGroupTactic
{
	virtual ~IAIGroupTactic() {};
	/// Updates the group tactic.
	virtual void	Update(float dt) = 0;
	/// Callback called when an AIObject is removed.
	virtual void	OnObjectRemoved(IAIObject* pObject) = 0;
	/// Called when new member is added to group.
	virtual void	OnMemberAdded(IAIObject* pMember) = 0;
	/// Called when new member is removed from the group.
	virtual void	OnMemberRemoved(IAIObject* pMember) = 0;
	/// Callback called when an attention target of a unit belonging to the group is changed.
	virtual void	OnUnitAttentionTargetChanged() = 0;
	/// Called on AIsystem reset, should reset the state of the tactic.
	virtual void	Reset() = 0;
	/// Notify or report tactic, see EGroupNotifyType.
	virtual void	NotifyState(IAIObject* pRequester, int type, float param) = 0;
	/// Returns queried state related to the tactic, see EGroupStateType.
	virtual int		GetState(IAIObject* pRequester, int type, float param) = 0;
	/// Returns queried point related to the tactic, see EGroupStateType.
	virtual Vec3	GetPoint(IAIObject* pRequester, int type, float param) = 0;
	/// Serialize the state of the tactic.
	virtual void	Serialize(TSerialize ser, class CObjectTracker& objectTracker) = 0;
	/// Draw debug information about the group tactic. See console var ai_DrawGroupTactic.
	virtual void	DebugDraw(IRenderer* pRend) = 0;
};

#endif
