// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IANIMATIONGRAPHSYSTEM_H__
#define __IANIMATIONGRAPHSYSTEM_H__

#pragma once

#include "IEntitySystem.h"

struct IAnimationGraphCategoryIterator
{
	struct SCategory
	{
		const char * name;
		bool overridable;
    bool usableWithTemplate;
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	
	virtual bool Next( SCategory& ) = 0;
};
typedef _smart_ptr<IAnimationGraphCategoryIterator> IAnimationGraphCategoryIteratorPtr;

typedef bool (*AnimationGraphUpgradeFunction)(XmlNodeRef node);

struct IAnimationGraphStateFactoryIterator
{
	struct SStateFactoryParameter
	{
		bool required;
		const char * type;
		const char * name;
		const char * humanName;
		const char * defaultValue;
		AnimationGraphUpgradeFunction upgradeFunction;
	};

	struct SStateFactory
	{
		const char * name;
		const char * humanName;
		const char * category;
		const SStateFactoryParameter * pParams;
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual bool Next( SStateFactory& ) = 0;
};
typedef _smart_ptr<IAnimationGraphStateFactoryIterator> IAnimationGraphStateFactoryIteratorPtr;

struct IAnimationGraphStateIterator
{
	struct SState
	{
		const char * name;
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual bool Next( SState& ) = 0;
};
typedef _smart_ptr<IAnimationGraphStateIterator> IAnimationGraphStateIteratorPtr;

struct IAnimationGraphInputs
{
	enum InputType
	{
		eAGIT_Integer,
		eAGIT_Float,
		eAGIT_String
	};

	struct SInput
	{
		virtual InputType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetValueCount() const = 0;
		virtual const char* GetValue(int) const= 0;
	};

	virtual int GetNumInputs() = 0;
	virtual const SInput* GetInput(int index) = 0;

	virtual void AddRef() = 0;
	virtual void Release() = 0;

};
typedef _smart_ptr<IAnimationGraphInputs> IAnimationGraphInputsPtr;

struct IAnimationGraphDeadInputReportCallback
{
	virtual void OnDeadInputValues( std::pair<const char*, const char*>* pInputValues ) = 0;
};


struct IAnimationGraphSystem
{
	virtual IAnimationGraphCategoryIteratorPtr CreateCategoryIterator() = 0;
	virtual IAnimationGraphStateFactoryIteratorPtr CreateStateFactoryIterator() = 0;
	virtual bool TrialAnimationGraph( const char * name, XmlNodeRef animGraph, bool loadBinary = false) = 0;
	virtual void FindDeadInputValues( IAnimationGraphDeadInputReportCallback * pCallback, XmlNodeRef animGraph ) = 0;

	virtual void SetAnimationGraphActivation( EntityId id, bool bActivation ) = 0;

	virtual IAnimationGraphStateIteratorPtr CreateStateIterator( EntityId id) = 0;
	virtual IAnimationGraphInputsPtr RetrieveInputs( EntityId id) = 0;
	virtual void ExportXMLToBinary( const char * filename, XmlNodeRef animGraph ) = 0;
};

#endif
