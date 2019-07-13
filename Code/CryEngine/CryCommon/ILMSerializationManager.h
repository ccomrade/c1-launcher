////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ILMSerializationManager.h
//  Version:     v1.00
//  Created:     02/07/2003 by Sergiy Migdalskiy
//  Compilers:   Visual Studio.NET
//  Description: LightMap Serialization interface
// -------------------------------------------------------------------------
//  History:
//	 02/07/2003  Extracted declaration from I3dEngine.h to be able to easy
//               modify it
////////////////////////////////////////////////////////////////////////////

#ifndef _CRY_COMMON_LM_SERIALIZATION_MANAGER_HDR_
#define _CRY_COMMON_LM_SERIALIZATION_MANAGER_HDR_

#include <IEntitySystem.h>

//! \brief Interface for lightmap serialization
struct ILMSerializationManager
{
	virtual void Release() = 0;

	//!
	virtual bool ApplyLightmapfile( const char *pszFileName, struct IRenderNode ** pIGLMs, int32 nIGLMNumber ) = 0;
	//!

	virtual bool Load( const char *pszFileName, const bool cbNoTextures ) = 0;

	//!
	virtual unsigned int InitLMUpdate(const char *pszFilePath, const bool bAppend ) = 0;

	//!
	virtual bool LoadFalseLight(const char *pszFileName, struct IRenderNode ** pIGLMs, int32 nIGLMNumber) = 0;

	//!
	virtual unsigned int SaveFalseLight(const char *pszFileName, const int nObjectNumber,const struct sFalseLightSerializationStructure* pFalseLightList) = 0;

	//!
	virtual unsigned int Save( const char *pszFileName, struct LMGenParam sParams, const bool cbAppend = false ) = 0;
	//!
	//! /param _pColorLerp4 if !=0 this memory is copied
	//! /param _pDomDirection3 if !=0 this memory is copied
	virtual RenderLMData *UpdateLMData( 
		const uint32 indwWidth, const uint32 indwHeight, const std::pair<int32,int32>* pGLM_IDs, const int32 nGLM_IDNumber,
		BYTE *_pColorLerp4, BYTE *_pHDRColorLerp4, BYTE *_pDomDirection3, BYTE *_pOcclusion, BYTE *_pRAE, const char* strDirPath, int32* pTextureID) = 0;
	//!
	virtual void AddTexCoordData( const struct TexCoord2Comp* pTexCoords, const int32 nTexCoordNumber, const int iGLM_ID_UsingTexCoord,const int iGLM_Idx_SubObj, const uint32 indwHashValue, const EntityId* pOcclIDsFirst,const EntityId* pOcclIDsSecond, const int32 nOcclIDNumber, const int8 nFirstOcclusionChannel ) = 0;

	//!
	virtual void RescaleTexCoordData(const int iGLM_ID_UsingTexCoord,const int iGLM_Idx_SubObj, const f32 fScaleU, const f32 fScaleV ) = 0;

	//! for rebuild changes feature
	//! /return 0x12341234 if this object wasn't in the list
	virtual uint32 GetHashValue( const std::pair<int32,int32> iniGLM_ID_UsingTexCoord ) const=0;

	virtual bool ExportDLights(const char *pszFileName, const CDLight **ppLights, UINT iNumLights, bool bNewZip = true) const = 0;
};

#endif