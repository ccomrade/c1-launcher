// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _TIMEVALUE_H_
#define _TIMEVALUE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTimeValue
{
public:
	//! default constructor
	CTimeValue()
	{
		m_lValue=0;
	}

	//! constructor
	CTimeValue( const float fSeconds )
	{
		SetSeconds(fSeconds);
	}

	CTimeValue( const double fSeconds )
	{
		SetSeconds(fSeconds);
	}

	//! constructor
	// /param inllValue positive negative, absolute or relative in 1 second= TIMEVALUE_PRECISION units
	CTimeValue( const int64 &inllValue )
	{
		m_lValue=inllValue;
	}

	//! copy constructor
	CTimeValue( const CTimeValue &inValue )
	{
		m_lValue=inValue.m_lValue;
	}

	//! destructor
	~CTimeValue(){}
	
	//! assignment operator
	//! /param inRhs right side
	CTimeValue operator=( const CTimeValue &inRhs )
	{
		m_lValue = inRhs.m_lValue;
		return *this;
	};

	//! use only for relative value, absolute values suffer a lot from precision loss
	float GetSeconds() const
	{
		return m_lValue * (1.f/TIMEVALUE_PRECISION);
	}

	//!
	void SetSeconds( const float infSec )
	{
		m_lValue = (int64)(infSec*TIMEVALUE_PRECISION);
	}

	//!
	void SetSeconds( const double infSec )
	{
		m_lValue = (int64)(infSec*TIMEVALUE_PRECISION);
	}

	//!
	void SetSeconds( const int64 indwSec )
	{
		m_lValue=indwSec*TIMEVALUE_PRECISION;
	}

	//!
	void SetMilliSeconds( const int64 indwMilliSec )
	{
		m_lValue=indwMilliSec*(TIMEVALUE_PRECISION/1000);
	}

	//! use only for relative value, absolute values suffer a lot from precision loss
	float GetMilliSeconds() const
	{
		return m_lValue * (1000.f/TIMEVALUE_PRECISION);
	}

	int64 GetMilliSecondsAsInt64() const
	{
		return m_lValue * 1000 / TIMEVALUE_PRECISION;
	}

	int64 GetValue() const
	{
		return m_lValue;
	}

	void SetValue( int64 val )
	{
		m_lValue = val;
	}

	// math operations -----------------------

	//! minus
	ILINE CTimeValue operator-( const CTimeValue &inRhs ) const {	CTimeValue ret;	ret.m_lValue = m_lValue - inRhs.m_lValue;return ret; };
	//! plus
	ILINE CTimeValue operator+( const CTimeValue &inRhs ) const {	CTimeValue ret;	ret.m_lValue = m_lValue + inRhs.m_lValue;return ret;	};
	//! unary minus
	ILINE CTimeValue operator-() const { CTimeValue ret; ret.m_lValue = -m_lValue;return ret; };

	ILINE CTimeValue& operator+=( const CTimeValue& inRhs ) { m_lValue += inRhs.m_lValue; return *this; }
	ILINE CTimeValue& operator-=( const CTimeValue& inRhs ) { m_lValue -= inRhs.m_lValue; return *this; }

	ILINE CTimeValue& operator/=( int inRhs ) { m_lValue /= inRhs; return *this; }

	// comparison -----------------------

	ILINE bool operator<( const CTimeValue &inRhs ) const {	return m_lValue < inRhs.m_lValue; };
	ILINE bool operator>( const CTimeValue &inRhs ) const {	return m_lValue > inRhs.m_lValue;	};
	ILINE bool operator>=( const CTimeValue &inRhs ) const { return m_lValue >= inRhs.m_lValue;	};
	ILINE bool operator<=( const CTimeValue &inRhs ) const { return m_lValue <= inRhs.m_lValue;	};
	ILINE bool operator==( const CTimeValue &inRhs ) const { return m_lValue == inRhs.m_lValue;	};
	ILINE bool operator!=( const CTimeValue &inRhs ) const { return m_lValue != inRhs.m_lValue;	};

	AUTO_STRUCT_INFO

private: // ----------------------------------------------------------

	int64									m_lValue;											// absolute or relative value in 1/TIMEVALUE_PRECISION, might be negative
	static const int64		TIMEVALUE_PRECISION=100000;			//


	friend class CTimer;
};

#endif // _TIMEVALUE_H_
