/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2005.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Helper template to provide correct action synchronization
	             for actors and vehicles
  
 -------------------------------------------------------------------------
  History:
  - 17:9:2004 : Created by Craig Tiller

*************************************************************************/

#ifndef __NETACTIONSYNC_H__
#define __NETACTIONSYNC_H__

#pragma once

#ifdef __GNUC__
// GCC requires a full decl of TSerialize, not just a fwd decl.
#include <ISerialize.h>
#endif

template <class ActionRep>
class CNetActionSync
{
public:
	CNetActionSync() : m_havePublished(false), m_lastReceived(0), m_lastCancelation(1) {}

	static const uint8 CONTROLLED_ASPECT = ActionRep::CONTROLLED_ASPECT;

	// publish current state of actions - returns true if net-sync required
	bool PublishActions( ActionRep rep )
	{
		bool changed = false;
		if (m_lastReceived > m_lastCancelation)
		{
			rep = m_received;
		}
		if (!m_havePublished || m_published != rep)
		{
			m_published = rep;
			changed = true;
		}
		m_havePublished = true;
		return changed;
	}
	void Serialize( TSerialize ser, unsigned aspects )
	{
		// ensure CONTROLLED_ASPECT only has one bit set
		assert((CONTROLLED_ASPECT & (CONTROLLED_ASPECT-1)) == 0);
		if ((aspects & CONTROLLED_ASPECT) == 0)
			return;
		if (ser.IsReading())
		{
			m_received.Serialize(ser, aspects);
			m_lastReceived = gEnv->pRenderer->GetFrameID();
		}
		else // writing
		{
			assert(m_havePublished);
			m_published.Serialize(ser, aspects);
		}
	}
	void CancelReceived()
	{
		m_lastCancelation = gEnv->pRenderer->GetFrameID();
	}
	void UpdateObject( typename ActionRep::UpdateObjectSink obj )
	{
		if (m_lastReceived > m_lastCancelation)
			m_received.UpdateObject( obj );
	}

private:
	ActionRep m_published;
	ActionRep m_received;
	bool m_havePublished;
	uint32 m_lastCancelation;
	uint32 m_lastReceived;
};

#endif
