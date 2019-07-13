// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _IRENDERAUXGEOM_H_
#define _IRENDERAUXGEOM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SAuxGeomRenderFlags;


enum EBoundingBoxDrawStyle
{
	eBBD_Faceted,
	eBBD_Extremes_Color_Encoded
};

// Summary
//   Auxiliary geometry render interface
// Description
//   Used mostly for debugging, editor purposes, the Auxiliary geometry render 
//   interface provide functions to render 2d geometry and also text.
struct IRenderAuxGeom
{
	// render flags
	virtual void SetRenderFlags( const SAuxGeomRenderFlags& renderFlags ) = 0;
	virtual SAuxGeomRenderFlags GetRenderFlags() = 0;

	// 2D/3D rendering
	virtual void DrawPoint( const Vec3& v, const ColorB& col, uint8 size = 1 ) = 0;
	virtual void DrawPoints( const Vec3* v, uint32 numPoints, const ColorB& col, uint8 size = 1 ) = 0;	
	virtual void DrawPoints( const Vec3* v, uint32 numPoints, const ColorB* col, uint8 size = 1 ) = 0;	

	virtual void DrawLine( const Vec3& v0, const ColorB& colV0, const Vec3& v1, const ColorB& colV1, float thickness = 1.0f ) = 0;
	virtual void DrawLines( const Vec3* v, uint32 numPoints, const ColorB& col, float thickness = 1.0f ) = 0;
	virtual void DrawLines( const Vec3* v, uint32 numPoints, const ColorB* col, float thickness = 1.0f ) = 0;
	virtual void DrawLines( const Vec3* v, uint32 numPoints, const uint16* ind, uint32 numIndices, const ColorB& col, float thickness = 1.0f ) = 0;
	virtual void DrawLines( const Vec3* v, uint32 numPoints, const uint16* ind, uint32 numIndices, const ColorB* col, float thickness = 1.0f ) = 0;
	virtual void DrawPolyline( const Vec3* v, uint32 numPoints, bool closed, const ColorB& col, float thickness = 1.0f ) = 0;
	virtual void DrawPolyline( const Vec3* v, uint32 numPoints, bool closed, const ColorB* col, float thickness = 1.0f ) = 0;

	virtual void DrawTriangle( const Vec3& v0, const ColorB& colV0, const Vec3& v1, const ColorB& colV1, const Vec3& v2, const ColorB& colV2 ) = 0;
	virtual void DrawTriangles( const Vec3* v, uint32 numPoints, const ColorB& col ) = 0;
	virtual void DrawTriangles( const Vec3* v, uint32 numPoints, const ColorB* col ) = 0;
	virtual void DrawTriangles( const Vec3* v, uint32 numPoints, const uint16* ind, uint32 numIndices, const ColorB& col ) = 0;
	virtual void DrawTriangles( const Vec3* v, uint32 numPoints, const uint16* ind, uint32 numIndices, const ColorB* col ) = 0;

	virtual void DrawAABB( const AABB& aabb, bool bSolid, const ColorB& col, const EBoundingBoxDrawStyle& bbDrawStyle ) = 0;
	virtual void DrawAABB( const AABB& aabb, const Matrix34& matWorld, bool bSolid, const ColorB& col, const EBoundingBoxDrawStyle& bbDrawStyle ) = 0;

	virtual void DrawOBB( const OBB& obb, const Vec3& pos, bool bSolid, const ColorB& col, const EBoundingBoxDrawStyle& bbDrawStyle ) = 0;
	virtual void DrawOBB( const OBB& obb, const Matrix34& matWorld, bool bSolid, const ColorB& col, const EBoundingBoxDrawStyle& bbDrawStyle ) = 0;

	virtual void DrawSphere( const Vec3& pos, float radius, const ColorB& col, bool drawShaded = true ) = 0;
	virtual void DrawCone( const Vec3& pos, const Vec3& dir, float radius, float height, const ColorB& col, bool drawShaded = true ) = 0;
	virtual void DrawCylinder( const Vec3& pos, const Vec3& dir, float radius, float height, const ColorB& col, bool drawShaded = true ) = 0;

	virtual void DrawBone( const QuatT& rParent, const QuatT& rBone, ColorB col ) = 0;
	virtual void DrawBone( const Matrix34& rParent, const Matrix34& rBone, ColorB col ) = 0;

	// flushing
	virtual void Flush( bool discardGeometry = true ) = 0;
};


enum EAuxGeomPublicRenderflagBitMasks
{
	// Don't change the xxxShift values blindly as they affect the rendering output
	// that is two primitives have to be rendered after 3d primitives, alpha blended 
	// geometry have to be rendered after opaque ones, etc.
	// This also applies to the individual flags in EAuxGeomPublicRenderflags_*!

	e_Mode2D3DShift				= 31,
	e_Mode2D3DMask				= 0x1 << e_Mode2D3DShift,	

	e_AlphaBlendingShift	= 29,
	e_AlphaBlendingMask		= 0x3 << e_AlphaBlendingShift,

	e_DrawInFrontShift		= 28,
	e_DrawInFrontMask			= 0x1 << e_DrawInFrontShift,

	e_FillModeShift				= 26,
	e_FillModeMask				= 0x3 << e_FillModeShift,

	e_CullModeShift				= 24,
	e_CullModeMask				= 0x3 << e_CullModeShift,

	e_DepthWriteShift			= 23,
	e_DepthWriteMask			= 0x1 << e_DepthWriteShift,	

	e_DepthTestShift			= 22,
	e_DepthTestMask				= 0x1 << e_DepthTestShift,	

	e_PublicParamsMask		= e_Mode2D3DMask | e_AlphaBlendingMask | e_DrawInFrontMask | e_FillModeMask | 
		e_CullModeMask | e_DepthWriteMask | e_DepthTestMask 

	// Bits 0 - 22 are currently reserved for prim type and per draw call render parameters (point size, etc.)
	// Check RenderAuxGeom.h in ../RenderDll/Common
};


enum EAuxGeomPublicRenderflags_Mode2D3D
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_Mode3D							= 0x0 << e_Mode2D3DShift,
	e_Mode2D							= 0x1 << e_Mode2D3DShift,
};


enum EAuxGeomPublicRenderflags_AlphaBlendMode
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_AlphaNone						= 0x0 << e_AlphaBlendingShift,
	e_AlphaAdditive				= 0x1 << e_AlphaBlendingShift,
	e_AlphaBlended				= 0x2 << e_AlphaBlendingShift,
};


enum EAuxGeomPublicRenderflags_DrawInFrontMode
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_DrawInFrontOff			= 0x0 << e_DrawInFrontShift,
	e_DrawInFrontOn				= 0x1 << e_DrawInFrontShift,
};


enum EAuxGeomPublicRenderflags_FillMode
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_FillModeSolid				= 0x0 << e_FillModeShift,
	e_FillModeWireframe		= 0x1 << e_FillModeShift,
	e_FillModePoint 			= 0x2 << e_FillModeShift,
};


enum EAuxGeomPublicRenderflags_CullMode
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_CullModeNone				= 0x0 << e_CullModeShift,
	e_CullModeFront				= 0x1 << e_CullModeShift,
	e_CullModeBack				= 0x2 << e_CullModeShift,
};


enum EAuxGeomPublicRenderflags_DepthWrite
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_DepthWriteOn				= 0x0 << e_DepthWriteShift,
	e_DepthWriteOff				= 0x1 << e_DepthWriteShift,
};


enum EAuxGeomPublicRenderflags_DepthTest
{
	// See comments for EAuxGeomPublicRenderflagBitMasks!
	e_DepthTestOn					= 0x0 << e_DepthTestShift,
	e_DepthTestOff				= 0x1 << e_DepthTestShift,
};


enum EAuxGeomPublicRenderflags_Defaults
{
	// default render flags for 3d primitives
	e_Def3DPublicRenderflags = e_Mode3D | e_AlphaNone | e_DrawInFrontOff | e_FillModeSolid | 
		e_CullModeBack | e_DepthWriteOn | e_DepthTestOn,
	
		// default render flags for 2d primitives
	e_Def2DPublicRenderflags = e_Mode2D | e_AlphaNone | e_DrawInFrontOff | e_FillModeSolid | 
		e_CullModeBack | e_DepthWriteOn | e_DepthTestOn
};


struct SAuxGeomRenderFlags
{
	uint32 m_renderFlags;

	SAuxGeomRenderFlags();
	SAuxGeomRenderFlags( const SAuxGeomRenderFlags& rhs );
	SAuxGeomRenderFlags( uint32 renderFlags );
	SAuxGeomRenderFlags& operator =( const SAuxGeomRenderFlags& rhs );
	SAuxGeomRenderFlags& operator =( uint32 rhs );

	bool operator ==( const SAuxGeomRenderFlags& rhs ) const;
	bool operator ==( uint32 rhs ) const;
	bool operator !=( const SAuxGeomRenderFlags& rhs ) const;
	bool operator !=( uint32 rhs ) const;

	EAuxGeomPublicRenderflags_Mode2D3D GetMode2D3DFlag() const;
	void SetMode2D3DFlag( const EAuxGeomPublicRenderflags_Mode2D3D& state );

	EAuxGeomPublicRenderflags_AlphaBlendMode GetAlphaBlendMode() const;
	void SetAlphaBlendMode( const EAuxGeomPublicRenderflags_AlphaBlendMode& state );

	EAuxGeomPublicRenderflags_DrawInFrontMode GetDrawInFrontMode() const;
	void SetDrawInFrontMode( const EAuxGeomPublicRenderflags_DrawInFrontMode& state );

	EAuxGeomPublicRenderflags_FillMode GetFillMode() const;
	void SetFillMode( const EAuxGeomPublicRenderflags_FillMode& state );

	EAuxGeomPublicRenderflags_CullMode GetCullMode() const;
	void SetCullMode( const EAuxGeomPublicRenderflags_CullMode& state );

	EAuxGeomPublicRenderflags_DepthWrite GetDepthWriteFlag() const;
	void SetDepthWriteFlag( const EAuxGeomPublicRenderflags_DepthWrite& state );

	EAuxGeomPublicRenderflags_DepthTest GetDepthTestFlag() const;
	void SetDepthTestFlag( const EAuxGeomPublicRenderflags_DepthTest& state );
};


inline
SAuxGeomRenderFlags::SAuxGeomRenderFlags()
: m_renderFlags( e_Def3DPublicRenderflags )
{
}

inline
SAuxGeomRenderFlags::SAuxGeomRenderFlags( const SAuxGeomRenderFlags& rhs )
: m_renderFlags( rhs.m_renderFlags )
{
}


inline
SAuxGeomRenderFlags::SAuxGeomRenderFlags( uint32 renderFlags )
: m_renderFlags( renderFlags )
{
}


inline SAuxGeomRenderFlags&
SAuxGeomRenderFlags::operator =( const SAuxGeomRenderFlags& rhs )
{
	m_renderFlags = rhs.m_renderFlags;
	return( *this );
}


inline SAuxGeomRenderFlags& 
SAuxGeomRenderFlags::operator =( uint32 rhs )
{
	m_renderFlags = rhs;
	return( *this );
}	


inline bool
SAuxGeomRenderFlags::operator ==( const SAuxGeomRenderFlags& rhs ) const
{
	return( m_renderFlags == rhs.m_renderFlags );		
}	


inline bool
SAuxGeomRenderFlags::operator ==( uint32 rhs ) const
{
	return( m_renderFlags == rhs );		
}	


inline bool
SAuxGeomRenderFlags::operator !=( const SAuxGeomRenderFlags& rhs ) const
{
	return( m_renderFlags != rhs.m_renderFlags );		
}	


inline bool
SAuxGeomRenderFlags::operator !=( uint32 rhs ) const
{
	return( m_renderFlags != rhs );		
}	


inline EAuxGeomPublicRenderflags_Mode2D3D 
SAuxGeomRenderFlags::GetMode2D3DFlag() const
{
	uint32 mode2D3D( m_renderFlags & e_Mode2D3DMask );
	switch( mode2D3D )
	{
	case e_Mode2D:
		{
			return( e_Mode2D );
		}
	case e_Mode3D:
	default:
		{
			assert( e_Mode3D == mode2D3D );
			return( e_Mode3D );
		}
	}
}


inline void 
SAuxGeomRenderFlags::SetMode2D3DFlag( const EAuxGeomPublicRenderflags_Mode2D3D& state )
{
	m_renderFlags &= ~e_Mode2D3DMask;
	m_renderFlags |= state;
}


inline EAuxGeomPublicRenderflags_AlphaBlendMode 
SAuxGeomRenderFlags::GetAlphaBlendMode() const
{
	uint32 alphaBlendMode( m_renderFlags & e_AlphaBlendingMask );
	switch( alphaBlendMode )
	{
	case e_AlphaAdditive:
		{
			return( e_AlphaAdditive );
		}
	case e_AlphaBlended:
		{
			return( e_AlphaBlended );
		}
	case e_AlphaNone:
	default:
		{
			assert( e_AlphaNone == alphaBlendMode );
			return( e_AlphaNone );
		}
	}
}


inline void
SAuxGeomRenderFlags::SetAlphaBlendMode( const EAuxGeomPublicRenderflags_AlphaBlendMode& state )
{
	m_renderFlags &= ~e_AlphaBlendingMask;
	m_renderFlags |= state;
}


inline EAuxGeomPublicRenderflags_DrawInFrontMode
SAuxGeomRenderFlags::GetDrawInFrontMode() const
{
	uint32 drawInFrontMode( m_renderFlags & e_DrawInFrontMask );
	switch( drawInFrontMode )
	{
	case e_DrawInFrontOff:
		{
			return( e_DrawInFrontOff );
		}
	case e_DrawInFrontOn:
	default:
		{
			assert( e_DrawInFrontOn == drawInFrontMode );
			return( e_DrawInFrontOn );
		}
	}
}


inline void 
SAuxGeomRenderFlags::SetDrawInFrontMode( const EAuxGeomPublicRenderflags_DrawInFrontMode& state )
{
	m_renderFlags &= ~e_DrawInFrontMask;
	m_renderFlags |= state;
}


inline EAuxGeomPublicRenderflags_FillMode 
SAuxGeomRenderFlags::GetFillMode() const
{
	uint32 fillMode( m_renderFlags & e_FillModeMask );
	switch( fillMode )
	{
	case e_FillModePoint:
		{
			return( e_FillModePoint );
		}
	case e_FillModeWireframe:
		{
			return( e_FillModeWireframe );
		}
	case e_FillModeSolid:
	default:
		{
			assert( e_FillModeSolid == fillMode );
			return( e_FillModeSolid );
		}
	}
}


inline void 
SAuxGeomRenderFlags::SetFillMode( const EAuxGeomPublicRenderflags_FillMode& state )
{
	m_renderFlags &= ~e_FillModeMask;
	m_renderFlags |= state;
}


inline EAuxGeomPublicRenderflags_CullMode 
SAuxGeomRenderFlags::GetCullMode() const
{
	uint32 cullMode( m_renderFlags & e_CullModeMask );
	switch( cullMode )
	{
	case e_CullModeNone:
		{
			return( e_CullModeNone );
		}
	case e_CullModeFront:
		{
			return( e_CullModeFront );
		}
	case e_CullModeBack:
	default:
		{
			assert( e_CullModeBack == cullMode );
			return( e_CullModeBack );
		}
	}
}


inline void 
SAuxGeomRenderFlags::SetCullMode( const EAuxGeomPublicRenderflags_CullMode& state )
{
	m_renderFlags &= ~e_CullModeMask;
	m_renderFlags |= state;
}


inline EAuxGeomPublicRenderflags_DepthWrite 
SAuxGeomRenderFlags::GetDepthWriteFlag() const
{
	uint32 depthWriteFlag( m_renderFlags & e_DepthWriteMask );
	switch( depthWriteFlag )
	{
	case e_DepthWriteOff:
		{
			return( e_DepthWriteOff );
		}
	case e_DepthWriteOn:
	default:
		{
			assert( e_DepthWriteOn == depthWriteFlag );
			return( e_DepthWriteOn );
		}
	}
}


inline void 
SAuxGeomRenderFlags::SetDepthWriteFlag( const EAuxGeomPublicRenderflags_DepthWrite& state )
{
	m_renderFlags &= ~e_DepthWriteMask;
	m_renderFlags |= state;
}


inline EAuxGeomPublicRenderflags_DepthTest 
SAuxGeomRenderFlags::GetDepthTestFlag() const
{
	uint32 depthTestFlag( m_renderFlags & e_DepthTestMask );
	switch( depthTestFlag )
	{
	case e_DepthTestOff:
		{
			return( e_DepthTestOff );
		}
	case e_DepthTestOn:
	default:
		{
			assert( e_DepthTestOn == depthTestFlag );
			return( e_DepthTestOn );
		}
	}
}


inline void 
SAuxGeomRenderFlags::SetDepthTestFlag( const EAuxGeomPublicRenderflags_DepthTest& state )
{
	m_renderFlags &= ~e_DepthTestMask;
	m_renderFlags |= state;
}


#endif // #ifndef _IRENDERAUXGEOM_H_
