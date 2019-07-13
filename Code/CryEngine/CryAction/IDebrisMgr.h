/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements debris manager interface.

-------------------------------------------------------------------------
History:
- 24:1:2007: Created by Pavel Mores

*************************************************************************/

#ifndef __IDEBRIS_MGR_H__
#define __IDEBRIS_MGR_H__

struct IEntity;
class ICrySizer;

// NOTE Jan 26, 2007: <pvl> unfortunately this class hasn't received any heavy
// testing.  If anything behaves different from the docs, please let me know.

/**
 * Automatizes management of debris pieces' lifespan.
 *
 * Anyone who needs to use Entity-based debris can add the pieces here after
 * spawning and basically stop caring about it.
 *
 * Supports setting resource-usage policy.  The resources available to control
 * are expiration time and maximum number of debris pieces in the system at any
 * given time.  Expiration time can be controlled by setting minimum and
 * maximum expiration times or by setting it to "never expire" mode.
 *
 * Max number of debris pieces is treated separately from expiration.  I.e.
 * both expiring mode with unlimited list and non-expiring mode with limit on
 * list size are conceivable.
 *
 * The caller can set debris lifetime upon submitting it to DebrisMgr, or he
 * can leave it up to the DebrisMgr to determine the lifetime according to
 * a policy.  Note that if expiration policy is set to "never expire", any
 * caller supplied expiration times are ignored.  This is to make it possible
 * to switch the whole system between expiring and non-expiring modes just by
 * a single to call to DebrisMgr.
 *
 * Note that policy setting interface is designed to be used once, at the start
 * of the program.  If policy is reset after pieces of debris have already
 * been added, the behavior is not well defined (but basically, the new policy
 * will only affect newly added pieces).
 */
struct IDebrisMgr {

	class Time {
		float m_time;
	public:
		Time (float time);

		operator float () const;
		Time & operator+= (float );

		/// Value used to represent infinity, given in seconds.
		static const float NEVER;
	};

	/// Generates expiration time according to a system-wide policy.
	virtual void AddPiece (IEntity* ) = 0;

	/// Lets caller set expiration for this piece.
	virtual void AddPiece (IEntity* , Time expiration_time) = 0;

  /// Removes a piece from the list. Use for cleanup.
  virtual void RemovePiece(EntityId piece) = 0;

	virtual void Update() = 0;

	/// Call SetMinExpirationTime (Time::NEVER) for never-expiring debris.
	virtual void SetMinExpirationTime (Time ) = 0;
	virtual void SetMaxExpirationTime (Time ) = 0;

	/// Use 0 if no limit on debris list length should be placed.
	virtual void SetMaxDebrisListLen (int ) = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

// ---

inline IDebrisMgr::Time::Time (float time) : m_time (time)
{
}

inline IDebrisMgr::Time & IDebrisMgr::Time::operator+= (float rhs)
{
	m_time += rhs;
	return *this;
}

inline IDebrisMgr::Time::operator float () const
{
	return m_time;
}

#endif // __IDEBRIS_MGR_H__

