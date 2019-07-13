/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: UI draw functions interface

-------------------------------------------------------------------------
History:
- 07:11:2005: Created by Julien Darre

*************************************************************************/
#ifndef __IUIDRAW_H__
#define __IUIDRAW_H__

//-----------------------------------------------------------------------------------------------------

enum EUIDRAWHORIZONTAL
{
	UIDRAWHORIZONTAL_LEFT,
	UIDRAWHORIZONTAL_CENTER,
	UIDRAWHORIZONTAL_RIGHT
};

enum EUIDRAWVERTICAL
{
	UIDRAWVERTICAL_TOP,
	UIDRAWVERTICAL_CENTER,
	UIDRAWVERTICAL_BOTTOM
};

//-----------------------------------------------------------------------------------------------------

struct IUIDraw
{
	virtual void Release() = 0;

	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

	// TODO: uintARGB or float,float,float,float ?

	virtual uint GetColorARGB(uint8 ucAlpha,uint8 ucRed,uint8 ucGreen,uint8 ucBlue) = 0;

	virtual int CreateTexture(const char *strName) = 0;

	virtual void GetTextureSize(int iTextureID,float &rfSizeX,float &rfSizeY) = 0;

//	virtual void DrawTriangle(float fX0,float fY0,float fX1,float fY1,float fX2,float fY2,uint uiColor) = 0;

	virtual void DrawQuad(float fX,
												float fY,
												float fSizeX,
												float fSizeY,
												uint uiDiffuse=0,
												uint uiDiffuseTL=0,uint uiDiffuseTR=0,uint uiDiffuseDL=0,uint uiDiffuseDR=0,
												int iTextureID=0,
												float fUTexCoordsTL=0.0f,float fVTexCoordsTL=0.0f,
												float fUTexCoordsTR=1.0f,float fVTexCoordsTR=0.0f,
												float fUTexCoordsDL=0.0f,float fVTexCoordsDL=1.0f,
												float fUTexCoordsDR=1.0f,float fVTexCoordsDR=1.0f,
												bool bUse169=true) = 0;


	virtual void DrawImage(int iTextureID,float fX,
																				float fY,
																				float fSizeX,
																				float fSizeY,
																				float fAngleInDegrees,
																				float fRed,
																				float fGreen,
																				float fBlue,
																				float fAlpha,
																				float fS0=0.0f,
																				float fT0=0.0f,
																				float fS1=1.0f,
																				float fT1=1.0f) = 0;

	virtual void DrawImageCentered(int iTextureID,float fX,
																								float fY,
																								float fSizeX,
																								float fSizeY,
																								float fAngleInDegrees,
																								float fRed,
																								float fGreen,
																								float fBlue,
																								float fAlpha) = 0;

	virtual void DrawText(IFFont *pFont,
												float fX,
												float fY,
												float fSizeX,
												float fSizeY,
												const char *strText,
												float fAlpha,
												float fRed,
												float fGreen,
												float fBlue,
												EUIDRAWHORIZONTAL	eUIDrawHorizontalDocking,
												EUIDRAWVERTICAL		eUIDrawVerticalDocking,
												EUIDRAWHORIZONTAL	eUIDrawHorizontal,
												EUIDRAWVERTICAL		eUIDrawVertical) = 0;


	virtual void GetTextDim(IFFont *pFont,
													float *fWidth,
													float *fHeight,
													float fSizeX,
													float fSizeY,
													const char *strText) = 0;

	virtual void DrawTextW(	IFFont *pFont,
													float fX,
													float fY,
													float fSizeX,
													float fSizeY,
													const wchar_t *strText,
													float fAlpha,
													float fRed,
													float fGreen,
													float fBlue,
													EUIDRAWHORIZONTAL	eUIDrawHorizontalDocking,
													EUIDRAWVERTICAL		eUIDrawVerticalDocking,
													EUIDRAWHORIZONTAL	eUIDrawHorizontal,
													EUIDRAWVERTICAL		eUIDrawVertical) = 0;

	virtual void DrawWrappedTextW(	IFFont *pFont,
		float fX,
		float fY,
		float fMaxWidth,
		float fSizeX,
		float fSizeY,
		const wchar_t *strText,
		float fAlpha,
		float fRed,
		float fGreen,
		float fBlue,
		EUIDRAWHORIZONTAL	eUIDrawHorizontalDocking,
		EUIDRAWVERTICAL		eUIDrawVerticalDocking,
		EUIDRAWHORIZONTAL	eUIDrawHorizontal,
		EUIDRAWVERTICAL		eUIDrawVertical) = 0;

	virtual void GetTextDimW(	IFFont *pFont,
														float *fWidth,
														float *fHeight,
														float fSizeX,
														float fSizeY,
														const wchar_t *strText) = 0;

	virtual void GetWrappedTextDimW(	IFFont *pFont,
		float *fWidth,
		float *fHeight,
		float fMaxWidth,
		float fSizeX,
		float fSizeY,
		const wchar_t *strText) = 0;

};

//-----------------------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------------------