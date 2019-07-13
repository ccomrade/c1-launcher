/*=============================================================================
I3DSampler.h : 
Copyright 2006 Crytek Studios. All Rights Reserved.

Revision history:
* Created by Tamas Schlagl
=============================================================================*/
#ifndef __I3DSAMPLER_H__
#define __I3DSAMPLER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define	_3DSAMPLER_EXPORT_FILE_NAME "3DSampler.dat"
/*
	The 3D sampling types - define what data do you wanted to calculate / store in the file.
*/
enum e3DSamplingType
{
	_3DSMP_UNKNOWN,																					//Wrong - not initialized type
	_3DSMP_DIRECTLIGHT,																			//Direct light datas

	//They aren't implemented, just presented how to use that system
	_3DSMP_AO,
	_3DSMP_SH9,
};

/*
 Spatial structure based for 3d samples
*/
class I3DSampler
{
public:
	I3DSampler():m_eSamplingType(_3DSMP_UNKNOWN),m_nPointNumber(0),m_pPointDatas(NULL),m_pPositions(NULL)
	{}

	~I3DSampler()
	{
		SAFE_DELETE_ARRAY(m_pPointDatas);
		SAFE_DELETE_ARRAY(m_pPositions);
	}

	//Runtime functions
	e3DSamplingType	GetSamplingType() const
	{
		return m_eSamplingType;
	};																																																								//Give back the sampling type

	//If the system stores more data, needed to re-implement this function
	virtual int	GetNumberOfFloats() const
	{
		//Here needed to be declare the number of floats needed for a point
		switch( m_eSamplingType )
		{
			case _3DSMP_DIRECTLIGHT:
				return 3;
			case _3DSMP_AO:
				return 1;
			case _3DSMP_SH9:
				return (9*3);
		}
		return -1;
	}																																																									//Give back the number of float needed for this data type (-1 means wrong data type!)

	virtual bool	GetInterpolatedData( const Vec3& vPosition, f32* pFloats ) = 0;																			//Give back the interpolated datas from the point

	//serialization
	virtual bool	Save( const char* szFileName ) = 0;																																	// Save all data
	virtual bool	Load( const char* szFileName ) = 0;																																	// Load all data


	//Helper functions to create new database

	f32*	GetPointArray() const																																												// Give back the "point data" array
	{
		return m_pPointDatas;
	}

	f32*	GetPositionArray() const																																												// Give back the "point data" array
	{
		return m_pPositions;
	}

	int GetPointNumber() const
	{
		return m_nPointNumber;																																														// Give back the number of points
	}

	void		SetSamplingType( const e3DSamplingType eSamplingType )																											// Set the sampling type (Just for calculation ,load care about it!)
	{ 
		m_eSamplingType = eSamplingType;
	}

	//Creation helper functions
	virtual bool  CreateIrregularSampling( const AABB& GlobalBBox, const Vec3& vScale , const int nPointNumber, const f32* pPoints ) = 0;	//Create the spatial hiearchy for that points

	bool	ResizePointArray( const int nPointNumber )																																// Helper class to create the necessary size of array for the points
	{
		if( nPointNumber > m_nPointNumber )
		{
			//for every point the informations needed to be interpolated
			int nPointSize = GetNumberOfFloats();
			if( -1 == nPointSize )
				return false;
			f32* pNewArray = new f32[ nPointSize*nPointNumber ];
			if( NULL == pNewArray )
				return false;
			memset( pNewArray, 0, sizeof(f32)*nPointSize*nPointNumber );
			if( m_pPointDatas )
			{
				memcpy( pNewArray, m_pPointDatas, nPointSize*sizeof(f32)*m_nPointNumber );
				delete [] m_pPointDatas;
			}
			m_pPointDatas = pNewArray;


			pNewArray = new f32[ 3*nPointNumber ];
			if( NULL == pNewArray )
				return false;
			memset( pNewArray, 0, sizeof(f32)*3*nPointNumber );
			if( m_pPositions )
			{
				memcpy( pNewArray, m_pPositions, 3*sizeof(f32)*m_nPointNumber );
				delete [] m_pPositions;
			}
			m_pPositions = pNewArray;

			m_nPointNumber = nPointNumber;
		}
		return true;
	}

protected:
	e3DSamplingType		m_eSamplingType;																																								// The sampling type
	f32*							m_pPointDatas;																																									// The point informations (cold datas - used only at the end of interpolations)
	f32*						  m_pPositions;																																										// Point positions - used at every query - warm datas seperated form interpolated informations
	int								m_nPointNumber;																																									// The number of points
};

#endif//__I3DSAMPLER_H__