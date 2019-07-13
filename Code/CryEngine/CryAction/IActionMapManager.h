/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Action Map Manager interfaces.
  
 -------------------------------------------------------------------------
  History:
  - 8:9:2004   10:20 : Created by Márcio Martins

*************************************************************************/
#ifndef __IACTIONMAPMANAGER_H__
#define __IACTIONMAPMANAGER_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "IEntitySystem.h"
#include "CryName.h"						// CCryName
#include "smartptr.h"

typedef CCryName ActionId;

// Summary
//   Enumeration all all the possible activation mode used for the actions
enum EActionActivationMode
{
	eAAM_OnPress			= 0x0001, // Used when the action key is pressed
	eAAM_OnRelease		= 0x0002, // Used when the action key is released
	eAAM_OnHold				= 0x0004, // Used when the action key is on hold
	eAAM_Always				= 0x0008,

	// special modifiers
	eAAM_Retriggerable	= 0x2000,
	eAAM_NoModifiers		= 0x4000,
	eAAM_ConsoleCmd			= 0x8000,
};


enum EActionFilterType
{
	eAFT_ActionPass,
	eAFT_ActionFail,
};


//------------------------------------------------------------------------
struct IActionListener
{
	virtual void OnAction(const ActionId& action, int activationMode, float value) = 0;
	virtual void AfterAction(){};
};


//------------------------------------------------------------------------
struct SActionMapBindInfo
{
	const char*  action; // name of the action
	const char** keys;   // allocated/release by IActionMapBindInfoIterator::Next
	int          nKeys;  // number of key names in keys
};


//------------------------------------------------------------------------
struct IActionMapBindInfoIterator
{
	virtual const SActionMapBindInfo* Next() = 0; // returns the same pointer every call, 0 if no more info available
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<IActionMapBindInfoIterator> IActionMapBindInfoIteratorPtr;


//------------------------------------------------------------------------
struct IActionMap
{
	virtual void GetMemoryStatistics( ICrySizer * ) = 0;
	virtual void Release() = 0;
	virtual void CreateAction(const char *name, int activationMode, const char* defaultKey0 = "<unknown>", const char* defaultKey1 = "<unknown>") = 0;
	virtual void BindAction(const char *name, const char *keyName, int keyNumber = -1) = 0;
	virtual void Reset() = 0;
	virtual bool SerializeXML(const XmlNodeRef& root, bool bLoading) = 0;
	virtual IActionMapBindInfoIteratorPtr CreateBindInfoIterator() = 0;
	virtual void SetActionListener(EntityId id) = 0;
	virtual const char* GetName() = 0;
	virtual void Enable(bool enable) = 0;
	virtual bool Enabled() = 0;

	// Summary:
	//   Retrieve key binding info for a specific action
	// Description:
	//   Unlike CreateBindInfoIterator this function looks up the key bindings for a specific action
	//   the outInfo needs already to be prepared, meaning keys must already be allocated up to maxKeys
	// Inputs:
	//   actionId   - id[name] of action which bindings should be returned
	//   outInfo    - SActionMapBindInfo structure.  The SActionMapBindInfo::keys member must already be allocated
	//                with maxKeys members.
	//   maxKeys    - number of [caller-allocated] entries in outInfo.keys. if 0, no entries will be filled in
	// Returns:
	//   true if BindInfo(=Action) found -> outInfo is filled with current params, false otherwise
	virtual bool GetBindInfo(const ActionId& actionId, SActionMapBindInfo& outInfo, int maxKeys) = 0;
};


//------------------------------------------------------------------------
struct IActionMapIterator
{
	virtual IActionMap* Next() = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<IActionMapIterator> IActionMapIteratorPtr;


//------------------------------------------------------------------------
struct IActionFilter
{
	virtual void GetMemoryStatistics( ICrySizer * ) = 0;
	virtual void Release() = 0;
	virtual void Filter(const ActionId& action) = 0;
	virtual bool SerializeXML(const XmlNodeRef& root, bool bLoading) = 0;
	virtual const char* GetName() = 0;
	virtual void Enable(bool enable) = 0;
	virtual bool Enabled() = 0;
};


//------------------------------------------------------------------------
struct IActionFilterIterator
{
	virtual IActionFilter* Next() = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<IActionFilterIterator> IActionFilterIteratorPtr;


//------------------------------------------------------------------------
struct IActionMapManager
{
	virtual void Update() = 0;
	virtual void Reset() = 0;
	virtual void Clear() = 0;

	virtual void LoadFromXML(const XmlNodeRef& node, bool bCheckVersion=false) = 0;
	virtual void SaveToXML(const XmlNodeRef& node) = 0;

	virtual IActionMap *CreateActionMap(const char *name) = 0;
	virtual IActionMap *GetActionMap(const char * name) = 0;
	virtual IActionFilter *CreateActionFilter(const char *name, EActionFilterType type) = 0;
	virtual IActionFilter *GetActionFilter(const char *name) = 0;
	virtual IActionMapIteratorPtr CreateActionMapIterator() = 0;
	virtual IActionFilterIteratorPtr CreateActionFilterIterator() = 0;

	virtual void Enable(bool enable) = 0;
	virtual void EnableActionMap(const char *name, bool enable) = 0;
	virtual void EnableFilter(const char *name, bool enable) = 0;
	virtual bool IsFilterEnabled(const char *name) = 0;
};


template <class T>
class TActionHandler
{
public:
	// Returns true if the action should also be forwarded to scripts
	typedef bool (T::*TOnActionHandler)(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	// setup action handlers
	void AddHandler(const ActionId& actionId, TOnActionHandler fnHandler)
	{
		m_actionHandlers.insert( std::make_pair(actionId, fnHandler) );
	}

	size_t GetNumHandlers() const
	{
		return m_actionHandlers.size();
	}

	// call action handler
	bool Dispatch(T* pThis, EntityId entityId, const ActionId& actionId, int activationMode, float value)
	{
		bool rVal = false;
		return Dispatch(pThis, entityId, actionId, activationMode, value, rVal);
	}

	// call action handler
	bool Dispatch(T* pThis, EntityId entityId, const ActionId& actionId, int activationMode, float value, bool& rVal)
	{
		rVal = false;

		TOnActionHandler fnHandler = GetHandler(actionId);
		if (fnHandler && pThis)
		{
			rVal = (pThis->*fnHandler)(entityId, actionId, activationMode, value);
			return true;
		}
		else
			return false;
	}

	// get action handler
	TOnActionHandler GetHandler(const ActionId& actionId)
	{
		typename TActionHandlerMap::iterator handler = m_actionHandlers.find(actionId);

		if (handler != m_actionHandlers.end())
		{
			return handler->second;
		}
		return 0;
	}

private:
	typedef std::multimap<ActionId, TOnActionHandler> TActionHandlerMap;

	TActionHandlerMap	m_actionHandlers;
};

#endif //__IACTIONMAPMANAGER_H__
