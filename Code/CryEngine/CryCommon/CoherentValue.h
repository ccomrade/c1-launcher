/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: a value which can tell whether it's changed

-------------------------------------------------------------------------
History:
- 06:05:2007  : Created by Marco Koegler

*************************************************************************/
#ifndef __COHERENTVALUE_H__
#define __COHERENTVALUE_H__

#pragma once

#include "IScriptSystem.h"

template <typename T>
class CCoherentValue
{
public:
	CCoherentValue() : m_dirty(false) {}
	CCoherentValue(const T& val) : m_val(val), m_dirty(true) {}
	CCoherentValue(const CCoherentValue& other )
	{
		if (m_val != other.m_val)
		{
			m_dirty = true;
		}
		m_val = other.m_val;
	}

	CCoherentValue& operator=( const CCoherentValue& rhs )
	{
		if (this != &rhs)
		{
			if (m_val != rhs.m_val)
			{
				m_dirty = true;
			}
			m_val = rhs.m_val;
		}
		return *this;
	}

	ILINE bool IsDirty() const {	return m_dirty;	}
	ILINE void Clear() {	m_dirty = false;	}
	ILINE const T& Value() const {	return m_val;	}

	ILINE void SetDirtyValue(CScriptSetGetChain& chain, const char* name)
	{
		if (IsDirty())
		{
			chain.SetValue(name, m_val);
			Clear();
		}
	}
	ILINE void Serialize(TSerialize ser, const char* name)
	{
		ser.Value(name, m_val);
		if (ser.IsReading())
		{
			m_dirty = true;
		}
	}
	operator bool() const 
	{
		return m_val != NULL;
	};
	bool operator !() const 
	{
		return m_val == NULL;
	};
private:
	T			m_val;
	bool	m_dirty;
};

#endif//__COHERENTVALUE_H__
