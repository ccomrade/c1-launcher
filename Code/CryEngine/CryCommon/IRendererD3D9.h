// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _CRY_RENDERERD3D9
#define _CRY_RENDERERD3D9

#include <Cry_Math.h>
#include <Cry_Geo.h>
#include <Cry_Color.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// export pointer to base-class of renderer                                  //
///////////////////////////////////////////////////////////////////////////////
#define DYNAMICLINKLIB
#define GFXDEBUG


#ifdef DYNAMICLINKLIB
	#ifdef RENDERER_DLL
	#define CRYRENDERER_API DLL_EXPORT // defined if building the DLL
	#else
	#define CRYRENDERER_API DLL_IMPORT // defined if building the application
	#endif
	extern "C" CRYRENDERER_API struct IRendererD3D9* GetInterfaceRenderer (); 
#else  
	#define CRYRENDERER_API
	CRYRENDERER_API struct IRendererD3D9* GetInterfaceRenderer ();
#endif



///////////////////////////////////////////////////////////////////////////////
// Definitions                                                               //
///////////////////////////////////////////////////////////////////////////////
#define IDI_MAIN_ICON          101 // Application icon
#define IDR_MAIN_ACCEL         113 // Keyboard accelerator
#define IDR_MENU               141 // Application menu
#define IDR_POPUP              142 // Popup menu
#define IDD_SELECTDEVICE       144 // "Change Device" dialog box

#define IDC_ADAPTER_COMBO							1002 // Adapter combobox for "SelectDevice" dlg
#define IDC_DEVICE_COMBO							1000 // Device combobox for "SelectDevice" dlg
#define IDC_ADAPTERFORMAT_COMBO				1003
#define IDC_RESOLUTION_COMBO					1004
#define IDC_MULTISAMPLE_COMBO					1005 // MultiSample combobox for "SelectDevice" dlg
#define IDC_REFRESHRATE_COMBO					1006
#define IDC_BACKBUFFERFORMAT_COMBO		1007
#define IDC_DEPTHSTENCILBUFFERFORMAT_COMBO 1008
#define IDC_VERTEXPROCESSING_COMBO		1009
#define IDC_PRESENTINTERVAL_COMBO			1010
#define IDC_MULTISAMPLE_QUALITY_COMBO	1011
#define IDC_DEVICECLIP_CHECK					1012
#define IDC_WINDOW										1016 // Radio button for windowed-mode
#define IDC_FULLSCREEN								1018 // Radio button for fullscreen-mode

#define IDM_CHANGEDEVICE     40002 // Command to invoke "Change Device" dlg
#define IDM_TOGGLEFULLSCREEN 40003 // Command to toggle fullscreen mode
#define IDM_TOGGLESTART      40004 // Command to toggle frame animation
#define IDM_SINGLESTEP       40005 // Command to single step frame animation
#define IDM_EXIT             40006 // Command to exit the application
#define IDM_HELP             40007 // Command to launch readme.txt

///////////////////////////////////////////////////////////////////////////////
// common structures                                                         //
///////////////////////////////////////////////////////////////////////////////
	struct Material
	{
		const char* TextureName;
		uint32 TextureName_Length;
		uint32 TextureID;

		const char* TextureNameDDN;
		uint32 TextureNameDDN_Length;
		uint32 TextureDDNID;

		uint32 FirstIndex;
		uint32 nNumIndices;
	};

	struct UV_Coordinates {	f32 u,v; };

	struct IvoGeometry
	{
		Matrix34 WMat; 

		Vec3 vtrans;  // transfervector to move vertices inside local-space of model 

		uint32 VertexCounter;  

		Vec3* pVertices; 
		ColorB* pColors;
		Vec3* pNormals; 
		Vec3* pTangents; 
		UV_Coordinates* pUV; 
		
		uint16* pIBuffer; 
		uint32 IndexCounter;
		
		uint32 MatAmount;
		Material mat[0x10];
	};



///////////////////////////////////////////////////////////////////////////////
// Typedef                                                                   //
///////////////////////////////////////////////////////////////////////////////
#ifdef GFXDEBUG
	#define VIRTUAL virtual
	#define PR =0   
#else
	#define VIRTUAL ILINE
	#define PR {}   
#endif


///////////////////////////////////////////////////////////////////////////////
//            Interface of D3D9-Renderer                                     //
///////////////////////////////////////////////////////////////////////////////
struct IRendererD3D9
{

	virtual int Create( const CCamera* cam, void* hWnd, uint8* pKeyflip ) = 0;
	virtual int LoadResources() = 0;

	virtual void    Pause( bool bPause ) = 0;
	virtual int Paint() = 0;
	virtual int Cleanup() = 0;
	virtual int UserSelectNewDevice() = 0;
	virtual int HandlePossibleSizeChange() = 0;
	virtual int MouseMove() = 0;
	virtual int ToggleFullscreen() = 0;
	virtual int SingleStep() = 0;
	virtual int ToggleStart() = 0;

	virtual int Render3DEnvironment(const CCamera* pCamera)=0;

	virtual bool IsWindowed(void) = 0;
	virtual bool IsActive(void) = 0;
	virtual bool IsDeviceLost(void) = 0;

	virtual bool IsWMinimized(void) = 0;
	virtual bool IsWMaximized(void) = 0;
	virtual bool IsFrameMoving(void) = 0;

	virtual void SetMinimized(bool b) = 0;
	virtual void SetMaximized(bool b) = 0;
	virtual void SetFrameMoving(bool b) = 0;
	virtual void SetWindowStyle( uint32 w) = 0;
	virtual bool IsClipCursorWhenFullscreen(void) = 0;

	virtual int	GetViewSurfaceX() = 0;
	virtual int GetViewSurfaceZ() = 0;



//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
	virtual void CreateD3D9Attachment( const IvoGeometry& GeoInfo  ) =0;
	virtual void RenderAttachment( const Matrix34& WMat, uint32 idx, uint32 sphere=0 ) =0;
	virtual uint32 GetAttachmentAmount() const =0; 
	virtual f32 GetAttachmentRadius(uint32 i) const =0;



	virtual void CopyMesh( const IvoGeometry& GeoInfo ) =0;
	virtual void CopyNormals( const Vec3* pVertices, const Vec3* pNormals, uint32 NormalCounter ) = 0;
	virtual void CopyTangents( const Vec3* pVertices, const Vec3* pTangents, uint32 NormalCounter ) = 0;

//--------------------------------------------------------------------------------------------------------
//----       interfaces for debug-output     --------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

	virtual void TextColor ( ColorB& c )=0;
	virtual void TextPosition ( uint8 p )=0;
	virtual int  D8Print ( const char *format, ... ) PRINTF_PARAMS(2, 3) = 0;
	virtual void Draw_PA( const ColorB &color, const float Y_Pos, const float TimeScale) = 0;


	VIRTUAL void Draw_Points(	const Vec3* p, const ColorB &c, uint32 amount ) PR;
	VIRTUAL void Draw_Lineseg(	const Vec3 &start, const ColorB &cstart,const Vec3 &end, const ColorB &cend ) PR;
	VIRTUAL void Draw_Triangle( const Vec3 &v0,const Vec3 &v1,const Vec3 &v2,   const ColorB &c0, const ColorB &c1, const ColorB &c2, bool t=0 ) PR;
	VIRTUAL void Draw_Sphere( const Sphere &s, const ColorB &col) PR;
	VIRTUAL void Draw_AAEllipsoid( const AAEllipsoid &e, const ColorB &color) PR;
	VIRTUAL void Draw_Ellipsoid( const Ellipsoid &e, const ColorB &color) PR;

	VIRTUAL void Draw_AABB_grid( const Vec3& pos, const AABB& aabb, const ColorB& col ) PR;
	VIRTUAL void Draw_AABB_solid( const Vec3 &pos, const AABB &aabb ) PR;

	VIRTUAL void Draw_OBB_grid( const Vec3& pos, const OBB& obb, const ColorB& col ) PR;
	VIRTUAL void Draw_OBB_solid( const Vec3& pos, const OBB& obb, char t=0 ) PR;

	VIRTUAL void Draw_CoordinateSystem() PR;
	
	virtual ~IRendererD3D9(void) {};
};


#endif _CRY_RENDERERD3D9
