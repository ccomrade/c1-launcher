////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2006.
// -------------------------------------------------------------------------
//  File name:   ScopedVariableSetter.h
//  Version:     v1.00
//  Created:     22/8/2006 by MichaelS.
//  Compilers:   Visual Studio.NET 2005
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __SCOPEDVARIABLESETTER_H__
#define __SCOPEDVARIABLESETTER_H__

// The idea of this class is to set the value of a variable in its constructor,
// and then in the destructor to reset it to its initial value.
template <typename T> class CScopedVariableSetter
{
public:
	typedef T Value;

	CScopedVariableSetter(Value& variable, const Value& temporaryValue)
		:	m_oldValue(variable),
			m_variable(variable)
	{
		m_variable = temporaryValue;
	}

	~CScopedVariableSetter()
	{
		m_variable = m_oldValue;
	}

private:
	Value m_oldValue;
	Value& m_variable;
};

#endif //__SCOPEDVARIABLESETTER_H__
