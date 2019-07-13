// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IWORLDQUERY_H__
#define __IWORLDQUERY_H__

#pragma once

#include "IGameObject.h"

typedef std::vector<EntityId> Entities;

struct IWorldQuery : IGameObjectExtension
{
	virtual IEntity *					GetEntityInFrontOf() = 0;
	virtual const Entities&		GetEntitiesInFrontOf() = 0;
	virtual const Vec3&				GetPos() const = 0;
	virtual const EntityId		GetLookAtEntityId()= 0;
	virtual const ray_hit*		GetLookAtPoint(const float fMaxDist = 0)= 0;
};

#endif