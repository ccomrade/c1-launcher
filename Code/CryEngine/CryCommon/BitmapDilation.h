// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#pragma once

// dependencies: uint32

#include <vector>									// STL vector

// class should be simple to integrated into every project (SRF Processing, CryRenderer) so dependencies are kept minimal
// radius is limited
class CBitmapDilation
{
public:

	// image is created with all bits set invalid
	CBitmapDilation( const uint32 dwWidth, const uint32 dwHeight ): m_dwWidth(dwWidth), m_dwHeight(dwHeight)
	{
		assert(dwWidth);
		assert(dwHeight);

		m_Mask.resize((dwWidth*dwHeight+7)/8,0);		// allocate at least dwWidth*dwHeight bits and clear to invalid

	}

	void SetValid( const uint32 dwX, const uint32 dwY )
	{
		assert(dwX<m_dwWidth);
		assert(dwY<m_dwHeight);

		uint32 dwGlobalBitNo = dwX+dwY*m_dwWidth;
		uint32 dwGlobalByteNo = dwGlobalBitNo>>3;
		uint32 dwLocalBitNo= dwGlobalBitNo&0x7;

		m_Mask[dwGlobalByteNo] |= 1<<dwLocalBitNo;
	}

	// Arguments:
	//   dwInX - 0..m_dwWidth-1
	//   dwInX - 0..m_dwHeight-1
	//   dwRadius - 0=no dilation, .. 9
	// Returns:
	//   if no valid sample was found 0,0 is returned
	bool GetBestSampleBorder( const uint32 dwInX, const uint32 dwInY, uint32 &dwOutX, uint32 &dwOutY, const uint32 dwRadius=9 ) const
	{
		uint32 dwSampleNo=0;
		int iLocalX,iLocalY;

		uint32 dwNoEnd = ComputeEndNoAtCertainDistance(dwRadius);

		for(uint32 dwI=0;dwI<dwNoEnd;++dwI)
		{
			bool bOk = GetSpiralPoint(dwSampleNo++,iLocalX,iLocalY);		assert(bOk);

			if(IsValidBorder(iLocalX+dwInX,iLocalY+dwInY))
			{
				dwOutX = (uint32)(iLocalX+dwInX);
				dwOutY = (uint32)(iLocalY+dwInY);
				return true;
			}
		}

		dwOutX=0;dwOutY=0;		// nothing found
		return false;
	}
/*
	// Arguments:
	//   dwInX - 0..m_dwWidth-1
	//   dwInX - 0..m_dwHeight-1
	//   dwOutX - [0..dwArrySize-1]
	//   dwOutY - [0..dwArrySize-1]
	//   dwArraySize - should be at least 4 to make good use of the function (%4=0 in general)
	// Returns:
	//   array elements used always >0
	uint32 GetBestSamplesBorder( const uint32 dwInX, const uint32 dwInY, uint32 dwOutX[], uint32 dwOutY[], const uint32 dwArraySize ) const
	{
		assert(dwArraySize);

		uint32 dwSampleNo=0;
		int iLocalX,iLocalY;

		while(GetSpiralPoint(dwSampleNo++,iLocalX,iLocalY))
		{
			if(IsValidTiled(iLocalX+dwInX,iLocalY+dwInY))
			{
				uint32 dwMaxRadius2 = iLocalX*iLocalX+iLocalY*iLocalY;

				uint32 dwOut=0;

				for(dwOut=0;dwOut<dwArraySize;++dwOut)
				{
					uint32 dwLocalRadius2 = iLocalX*iLocalX+iLocalY*iLocalY;

					if(dwLocalRadius2>dwMaxRadius2)
						break;

					dwOutX[dwOut] = (uint32)(iLocalX+dwInX);
					dwOutY[dwOut] = (uint32)(iLocalY+dwInY);

				} while( GetSpiralPoint(dwSampleNo++,iLocalX,iLocalY) );
			
				return dwOut;
			}
		}

		dwOutX=0;dwOutY=0;		// nothing found
		return 1;							// return mid sample
	}
*/
//test private: // -------------------------------------------------------------------------

	uint32								m_dwWidth;			// >0
	uint32								m_dwHeight;			// >0
	std::vector<uint8>		m_Mask;					// bits define the valid areas onthe image (1=valid, 0=invalid)  

	// --------------------------------------------------------------------------------

	// border is wrapped
	inline bool IsValidTiled( const int iX, const int iY ) const
	{
		const uint32 dwX=(uint32)iX;
		const uint32 dwY=(uint32)iY;

		return _IsValid(dwX%m_dwWidth,dwY%m_dwHeight);
	}

	// border is treated invalid
	inline bool IsValidBorder( const int iX, const int iY ) const
	{
		const uint32 dwX=(uint32)iX;
		const uint32 dwY=(uint32)iY;
		
		if(dwX>=m_dwWidth)
			return false;
		
		if(dwY>=m_dwHeight)
			return false;

		return _IsValid(dwX,dwY);
	}

	inline bool _IsValid( const uint32 dwX, const uint32 dwY ) const
	{
		assert(dwX<m_dwWidth);
		assert(dwY<m_dwHeight);

		uint32 dwGlobalBitNo = dwX+dwY*m_dwWidth;
		uint32 dwGlobalByteNo = dwGlobalBitNo>>3;
		uint32 dwLocalBitNo= dwGlobalBitNo&0x7;

		return (m_Mask[dwGlobalByteNo] & (1<<dwLocalBitNo)) !=0;
	}

	// use like this:
	//   for(uint32 dwY=0;dwY<m_dwWidth;++dwY)
	//   for(uint32 dwX=0;dwY<m_dwHeight;++dwX)
	//   uint32 dwEndNo = dil.ComputeNoAtCertainDistance(dwR);
	//   for(uint32 dwI=0;dwI<dwEndNo;++dwI)
	//     { uint32 dwLX,dwLY; if(dil.GetBestSampleBorder(dwX,dwY,dwLX,dwLY) {} }
	// Arguments
	//   dwDist - 0..9, 0 means no dilation
	inline uint32 ComputeEndNoAtCertainDistance( const uint32 dwDist ) const
	{
		switch(dwDist)
		{
			case 0: return 1+0;		//
			case 1: return 1+4;		//
			case 2: return 1+12;	//
			case 3: return 1+36;	//
			case 4: return 1+60;	//
			case 5: return 1+88;	//
			case 6: return 1+120;	//
			case 7: return 1+176;	//
			case 8: return 1+212;	//
			case 9: return 1+292;	//

			default: assert(0);	// more needs to be done through multiple calls
		}

		return 0;
	}

	// Returns:
	//   false = end reached, false otherwise
	inline bool GetSpiralPoint( const uint32 dwNo, int &iX, int &iY ) const
	{
		// x,y pairs in a circle around 0,0 with radius 9 sorted by distance (~500 bytes)
		// data was generated by code
		static const char SpiraleArray[]=
		{
			0,0, 0,1, 1,0, 0,-1, -1,0, 1,1, 1,-1, -1,-1, -1,1, 0,2, 
			2,0, 0,-2, -2,0, -1,2, 1,2, 2,1, 2,-1, 1,-2, -1,-2, -2,-1, 
			-2,1, 2,-2, -2,-2, -2,2, 2,2, 0,3, 3,0, 0,-3, -1,-3, -3,-1, 
			-3,0, -3,1, -1,3, 1,3, 3,1, 3,-1, 1,-3, 3,-2, 2,-3, -2,-3, 
			-3,-2, -3,2, -2,3, 2,3, 3,2, 0,4, 4,0, 3,-3, 1,-4, 0,-4, 
			-1,-4, -3,-3, -4,-1, -4,0, -4,1, -3,3, -1,4, 1,4, 3,3, 4,1, 
			4,-1, 4,2, 4,-2, 2,-4, -2,-4, -4,-2, -4,2, -2,4, 2,4, 3,4, 
			4,3, 4,-3, 3,-4, -3,-4, -4,-3, -4,3, -3,4, 0,5, 5,0, 1,-5, 
			0,-5, -1,-5, -5,-1, -5,0, -5,1, -1,5, 1,5, 5,1, 5,-1, 2,-5, 
			-2,-5, -5,-2, -5,2, -2,5, 2,5, 5,2, 5,-2, -4,-4, -4,4, 4,4, 
			4,-4, -3,-5, -5,-3, -5,3, -3,5, 3,5, 5,3, 5,-3, 3,-5, 0,6, 
			6,1, 6,0, 6,-1, 1,-6, 0,-6, -1,-6, -6,-1, -6,0, -6,1, -1,6, 
			1,6, 2,6, 4,5, 5,4, 6,2, 6,-2, 5,-4, 4,-5, 2,-6, -2,-6, 
			-4,-5, -5,-4, -6,-2, -6,2, -5,4, -4,5, -2,6, 3,-6, -3,-6, -6,-3, 
			-6,3, -3,6, 3,6, 6,3, 6,-3, -5,5, 5,5, 5,-5, -5,-5, 0,7, 
			7,1, 7,0, 7,-1, 6,-4, 4,-6, 1,-7, 0,-7, -1,-7, -4,-6, -6,-4, 
			-7,-1, -7,0, -7,1, -6,4, -4,6, -1,7, 1,7, 4,6, 6,4, -7,2, 
			-2,7, 2,7, 7,2, 7,-2, 2,-7, -2,-7, -7,-2, -7,-3, -7,3, -3,7, 
			3,7, 7,3, 7,-3, 3,-7, -3,-7, 5,6, 6,5, 6,-5, 5,-6, -5,-6, 
			-6,-5, -6,5, -5,6, -4,7, 4,7, 7,4, 7,-4, 4,-7, -4,-7, -7,-4, 
			-7,4, 0,8, 8,1, 8,0, 8,-1, 1,-8, 0,-8, -1,-8, -8,-1, -8,0, 
			-8,1, -1,8, 1,8, -2,-8, -8,-2, -8,2, -2,8, 2,8, 8,2, 8,-2, 
			2,-8, -6,6, 6,6, 6,-6, -6,-6, 5,7, 7,5, 8,3, 8,-3, 7,-5, 
			5,-7, 3,-8, -3,-8, -5,-7, -7,-5, -8,-3, -8,3, -7,5, -5,7, -3,8, 
			3,8, -8,4, -4,8, 4,8, 8,4, 8,-4, 4,-8, -4,-8, -8,-4, 0,9, 
			9,1, 9,0, 9,-1, 7,-6, 6,-7, 1,-9, 0,-9, -1,-9, -6,-7, -7,-6, 
			-9,-1, -9,0, -9,1, -7,6, -6,7, -1,9, 1,9, 6,7, 7,6, 2,-9, 
			-2,-9, -9,-2, -9,2, -2,9, 2,9, 9,2, 9,-2, -5,-8, -8,-5, -8,5, 
			-5,8, 5,8, 8,5, 8,-5, 5,-8, -9,-3, -9,3, -3,9, 3,9, 9,3, 
			9,-3, 3,-9, -3,-9
		};

		if(dwNo>=sizeof(SpiraleArray)/2)
			return false;

		uint32 dwEl = dwNo*2;

		iX = (int)SpiraleArray[dwEl];iY = (int)SpiraleArray[dwEl+1];
		return true;
	}
};
