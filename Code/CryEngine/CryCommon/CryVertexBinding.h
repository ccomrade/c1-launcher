/////////////////////////////////////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//	Crytek Character Animation source code
//	
//	History:
//	11/05/2002 - Created by Sergiy Migdalskiy <sergiy@crytek.de>
//
//  Contains:
//  Declaration of CryVertexBinding, a class incapsulating the array of links of a vertex to bone.
//  This class is only used during construction of the geometry, and shouldn't be used for 
//  calculating the actual skin in the run time.
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CRY_VERTEX_BINDING_HDR_
#define _CRY_VERTEX_BINDING_HDR_

#include <algorithm>

// array of crylinks for one vertex
class CryVertexBinding: public std::vector<CryLink>
{
public:
	CryVertexBinding () {};
	// scales all the link offsets multiplying the offset by the given scale
	void scaleOffsets(float fScale);
	// sorts the links by the blending factor, descending order
	void sortByBlendingDescending();
	// normalizes the weights of links so that they sum up to 1
	void normalizeBlendWeights();
	// prunes the weights that are less than the specified minimal blending factor.
	// Leaves (unpruned) at least the first numMinLinks links
	// ASSUMES:that the links are already sorted by the blending factors in descending order
	void pruneSmallWeights(float fMinBlending, unsigned numMinLinks = 1);
	// remaps the bone ids
	void remapBoneIds (class CCharacterModel* pModel, const unsigned* arrBoneIdMap, unsigned numBoneIds);

	// returns the maximum BoneID in the array of links
	unsigned maxBoneID ()const;
	// returns the minimal BoneID in the array of links
	unsigned minBoneID () const;

	// returns the link weight to the given bone
	float getBoneWeight (int nBoneID) const;

	// returns true if there is such bone weight
	bool hasBoneWeight (int nBoneID, float fWeight) const;
};



// normalizes the weights of links so that they sum up to 1
inline void CryVertexBinding::normalizeBlendWeights()
{
	// renormalize blending
	float fBlendSumm = 0;
	unsigned j;
	for (j = 0; j < size(); j++)
		fBlendSumm += (*this)[j].Blending;

	assert (fBlendSumm > 0.1f && fBlendSumm <=1.001f);

	for (j=0; j<size(); j++)
		(*this)[j].Blending /= fBlendSumm;
}


// prunes the weights that are less than the specified minimal blending factor
// ASSUMES: that the links are already sorted by the blending factors in descending order
inline void CryVertexBinding::pruneSmallWeights(float fMinBlending, unsigned numMinLinks)
{
	// remove 0 blending links and merge the links to the same bones
	unsigned j;
	for (j = numMinLinks; j < size(); j++)
	{
		assert (j == 0 || (*this)[j].Blending <= (*this)[j-1].Blending);
		if((*this)[j].Blending <= fMinBlending)
		{
			resize(j);
			assert(j);
			break;
		}
	}
}


// remaps the bone ids
inline void CryVertexBinding::remapBoneIds (CCharacterModel* pModel, const unsigned* arrBoneIdMap, unsigned numBoneIds)
{
	for (iterator it = begin(); it != end(); ++it)
	{
		// if you get this assert, most probably there is dissynchronization between different LODs of the same model
		// - all of them must be exported with exactly the same skeletons.
		if(it->BoneID >= 0 && it->BoneID < (int)numBoneIds)
			it->BoneID = arrBoneIdMap[it->BoneID];
		else
		{
#ifdef _CRY_ANIMATION_BASE_HEADER_
			//g_pILog->LogError ("bone index is out of range");
#endif
			it->BoneID = 0;
		}
	}
}


// scales all the link offsets multiplying the offset by the given scale
inline void CryVertexBinding::scaleOffsets(float fScale)
{
	for (iterator itLink = begin(); itLink != end(); ++itLink)
		itLink->offset *= fScale;
}

// structure used to sort the crylink array by the blending factor, descending
inline bool CryLinkOrderByBlending(const CryLink& left, const CryLink& right)
{
	return left.Blending > right.Blending;
}

// sorts the links by the blending factor, descending order
inline void CryVertexBinding::sortByBlendingDescending()
{
	// sort the links by blend factor to allow skip unimportant ones
	std::sort (begin(), end(), CryLinkOrderByBlending);
}

// returns the maximum BoneID in the array of links
inline unsigned CryVertexBinding::maxBoneID ()const
{
	unsigned nResult = 0;
	for (unsigned i = 0; i < this->size(); ++i)
		nResult = max((unsigned)(*this)[i].BoneID, nResult);
	return nResult;
}

// returns the minimal BoneID in the array of links
inline unsigned CryVertexBinding::minBoneID () const
{
	unsigned nResult = (unsigned)(*this)[0].BoneID;
	for (unsigned i = 1; i < this->size(); ++i)
		nResult = min((unsigned)(*this)[i].BoneID, nResult);
	return nResult;
}

// returns the link weight to the given bone
inline float CryVertexBinding::getBoneWeight (int nBoneID)const
{
	for (unsigned i = 0; i < this->size(); ++i)
		if ((*this)[i].BoneID == nBoneID)
			return (*this)[i].Blending;
	return 0;
}

// returns true if there is such bone weight
inline bool CryVertexBinding::hasBoneWeight (int nBoneID, float fWeight) const
{
	for (unsigned i = 0; i < this->size(); ++i)
		if ((*this)[i].BoneID == nBoneID && (*this)[i].Blending == fWeight)
			return true;
	return false;
}


#endif