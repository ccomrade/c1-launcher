//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File: Cry_Geo.h
//	Description: Common structures for geometry computations
//
//	History:
//	-March 15,2003: Created by Ivo Herzeg
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYGEOSTRUCTS_H
#define CRYGEOSTRUCTS_H

#if _MSC_VER > 1000
# pragma once
#endif


///////////////////////////////////////////////////////////////////////////////
// Forward declarations                                                      //
///////////////////////////////////////////////////////////////////////////////

struct Ray;
template <typename F> struct Lineseg_tpl;
template <typename F> struct Triangle_tpl;

struct AABB;
template <typename F> struct OBB_tpl;

struct Sphere;
struct AAEllipsoid;
struct Ellipsoid;

//-----------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
// Definitions                                                               //
///////////////////////////////////////////////////////////////////////////////

#define FINDMINMAX(x0,x1,x2,min,max) \
	min = max = x0;   \
	if(x1<min) min=x1;\
	if(x1>max) max=x1;\
	if(x2<min) min=x2;\
	if(x2>max) max=x2;


//
// Random geometry generation functions.
//

enum EGeomForm
{
	GeomForm_Vertices,
	GeomForm_Edges,
	GeomForm_Surface,
	GeomForm_Volume
};
AUTO_TYPE_INFO(EGeomForm)

enum EGeomType
{
	GeomType_None,
	GeomType_BoundingBox,
	GeomType_Physics,
	GeomType_Render,
};
AUTO_TYPE_INFO(EGeomType)

struct RandomPos
{
	Vec3	vPos;
	Vec3	vNorm;
};

struct RectF
{
	float	x, y, w, h;
	RectF()
		: x(0), y(0), w(1), h(1)
	{
	}
};

struct RectI
{
  int	x, y, w, h;
  RectI()
    : x(0), y(0), w(1), h(1)
  {
  }
  RectI(int inX, int inY, int inW, int inH)
    : x(inX), y(inY), w(inW), h(inH)
  {
  }
  _inline void Add(RectI rcAdd)
  {
    int x2 = x + w;
    int y2 = y + h;
    int x22 = rcAdd.x + rcAdd.w;
    int y22 = rcAdd.y + rcAdd.h;
    x =  __min(x, rcAdd.x);	y =  __min(y, rcAdd.y );
    x2 = __max(x2, x22);    y2 = __max(y2, y22);
    w = x2-x;
    h = y2-y;
  }
  _inline void Add(int inX, int inY, int inW, int inH)
  {
    Add(RectI(inX, inY, inW, inH));
  }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Line
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct Line {

	Vec3 pointonline;
	Vec3 direction; //caution: the direction is important for any intersection test

	//default Line constructor (without initialisation)
	inline Line( void ) {}
	inline Line( const Vec3 &o, const Vec3 &d ) {  pointonline=o; direction=d; }
	inline void operator () (  const Vec3 &o, const Vec3 &d  ) {  pointonline=o; direction=d; }

	~Line( void ) {};
};



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Ray
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct Ray {

	Vec3 origin;
	Vec3 direction;

	//default Ray constructor (without initialisation)
	inline Ray( void ) {}
	inline Ray( const Vec3 &o, const Vec3 &d ) {  origin=o; direction=d; }
	inline void operator () (  const Vec3 &o, const Vec3 &d  ) {  origin=o; direction=d; }

	~Ray( void ) {};
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Lineseg
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <typename F> struct Lineseg_tpl {

	Vec3_tpl<F> start;
	Vec3_tpl<F> end;

	//default Lineseg constructor (without initialisation)
	inline Lineseg_tpl( void ) {}
	inline Lineseg_tpl( const Vec3_tpl<F> &s, const Vec3_tpl<F> &e ) {  start=s; end=e; }
	inline void operator () (  const Vec3_tpl<F> &s, const Vec3_tpl<F> &e  ) {  start=s; end=e; }

  Vec3 GetPoint(F t) {return t * end + (F(1.0) - t) * start;}

	~Lineseg_tpl( void ) {};
};

typedef Lineseg_tpl<float> Lineseg;
typedef Lineseg_tpl<real> Linesegr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Triangle
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <typename F> struct Triangle_tpl {

	Vec3_tpl<F> v0,v1,v2;

	//default Lineseg constructor (without initialisation)
	inline Triangle_tpl( void ) {}
	inline Triangle_tpl( const Vec3_tpl<F>& a, const Vec3_tpl<F>& b, const Vec3_tpl<F>& c ) {  v0=a; v1=b; v2=c; }
	inline void operator () (  const Vec3_tpl<F>& a, const Vec3_tpl<F>& b, const Vec3_tpl<F>& c ) { v0=a; v1=b; v2=c; }

	~Triangle_tpl( void ) {};

	Vec3_tpl<F> GetNormal() const
	{
		return ((v1-v0) ^ (v2-v0)).GetNormalized();
	}
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct AABB
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct AABB {

	Vec3 min;
	Vec3 max;

	/// default AABB constructor (without initialisation)
	inline AABB() 
		{}
  enum type_reset { RESET };
  // AABB aabb(RESET) generates a reset aabb
  inline AABB(type_reset) 
		{ Reset(); }
	inline AABB( const Vec3& v ) 
		{ min=-v; max=v; }
	inline AABB( const Vec3 &vmin, const Vec3 &vmax ) 
		{ min=vmin; max=vmax; }
	inline AABB( const Vec3* points, uint32 num  )
	{
		Reset();
		for ( uint32 i=0; i<num; i++ )
			Add( points[i] );
	}

	inline void operator () ( const Vec3 &vmin, const Vec3 &vmax ) 
		{ min = vmin; max = vmax; }

	//! Reset Bounding box before calculating bounds.
	//! These values ensure that Add() functions work correctly for Reset bbs, without additional testing.
	inline void Reset()	
		{	min = Vec3(9999999.0f);	max = Vec3(-9999999.0f);	}

	inline bool IsReset() const 
		{ return min.x > max.x; }

	//! Check if bounding box is empty (Zero volume).
	ILINE bool IsEmpty() const 
		{ return min == max; }

	ILINE Vec3 GetCenter() const 
		{ return (min+max)*0.5f; }

	ILINE Vec3 GetSize() const 
		{ return IsReset() ? Vec3(ZERO) : max-min; }

	ILINE float GetRadius() const 
		{ return IsReset() ? 0.f : (max-min).len()*0.5f; }

	ILINE float GetVolume() const 
		{ return IsReset() ? 0.f : (max.x-min.x) * (max.y-min.y) * (max.z-min.z); }

	void Add( const Vec3 &v )
	{
		min.x = __min( min.x,v.x );	min.y = __min( min.y,v.y );	min.z = __min( min.z,v.z );
		max.x = __max( max.x,v.x );	max.y = __max( max.y,v.y );	max.z = __max( max.z,v.z );
	}

	inline void Add( const Vec3& v, float fRadius )
	{
		min.x = __min( min.x, v.x-fRadius );	min.y = __min( min.y, v.y-fRadius );	min.z = __min( min.z, v.z-fRadius );
		max.x = __max( max.x, v.x+fRadius );	max.y = __max( max.y, v.y+fRadius );	max.z = __max( max.z, v.z+fRadius );
	}

	inline void Add( const AABB& bb )
	{
		min.x = __min( min.x, bb.min.x );	min.y = __min( min.y, bb.min.y );	min.z = __min( min.z, bb.min.z );
		max.x = __max( max.x, bb.max.x );	max.y = __max( max.y, bb.max.y );	max.z = __max( max.z, bb.max.z );
	}

	inline void Move( const Vec3& v )
	{
		if (!IsReset())
		{
			min += v;
			max += v;
		}
	}

	inline void Expand( Vec3 const& v )
	{
		if (!IsReset())
		{
			min -= v;
			max += v;
		}
	}

	// Augment the box on all sides by a box.
	inline void Augment( AABB const& bb )
	{
		if (!IsReset() && !bb.IsReset())
		{
			Add( min + bb.min );
			Add( max + bb.max );
		}
	}

	void ClipToBox( AABB const& bb )
	{
		min.CheckMax(bb.min);
		max.CheckMin(bb.max);
	}

	void ClipMoveToBox( AABB const& bb )
	{
		for (int a = 0; a < 3; a++)
		{
			if (max[a] - min[a] > bb.max[a] - bb.min[a])
			{
				min[a] = bb.min[a];  
				max[a] = bb.max[a];
			}
			else if (min[a] < bb.min[a])
			{
				max[a] += bb.min[a] - min[a];
				min[a] = bb.min[a];
			}
			else if (max[a] > bb.max[a])
			{
				min[a] += bb.max[a] - max[a];
				max[a] = bb.max[a];
			}
		}
	}

	//! Check if this bounding box overlap with bounding box of sphere.
	bool IsOverlapSphereBounds( const Vec3 &pos,float radius ) const
	{
		assert( min.IsValid() );
		assert( max.IsValid() );
		assert( pos.IsValid() );

		if (pos.x > min.x && pos.x < max.x &&	pos.y > min.y && pos.y < max.y &&	pos.z > min.z && pos.z < max.z) 
			return true;

		if (pos.x+radius < min.x) return false;
		if (pos.y+radius < min.y) return false;
		if (pos.z+radius < min.z) return false;
		if (pos.x-radius > max.x) return false;
		if (pos.y-radius > max.y) return false;
		if (pos.z-radius > max.z) return false;
		return true;
	}

	//! Check if this bounding box contain sphere within itself.
	bool IsContainSphere( const Vec3 &pos,float radius ) const
	{
		assert( min.IsValid() );
		assert( max.IsValid() );
		assert( pos.IsValid() );
		if (pos.x-radius < min.x) return false;
		if (pos.y-radius < min.y) return false;
		if (pos.z-radius < min.z) return false;
		if (pos.x+radius > max.x) return false;
		if (pos.y+radius > max.y) return false;
		if (pos.z+radius > max.z) return false;
		return true;
	}

	//! Check if this bounding box contains a point within itself.
	bool IsContainPoint( const Vec3 &pos) const
	{
		assert( min.IsValid() );
		assert( max.IsValid() );
		assert( pos.IsValid() );
		if (pos.x < min.x) return false;
		if (pos.y < min.y) return false;
		if (pos.z < min.z) return false;
		if (pos.x > max.x) return false;
		if (pos.y > max.y) return false;
		if (pos.z > max.z) return false;
		return true;
	}

	float GetDistanceSqr( Vec3 const& v ) const
	{
		Vec3 vNear = v;
		vNear.CheckMax(min);
		vNear.CheckMin(max);
		return vNear.GetSquaredDistance(v);
	}

	float GetDistance( Vec3 const& v ) const
	{
		return sqrt(GetDistanceSqr(v));
	}

	bool ContainsBox( AABB const& b ) const
	{
		assert( min.IsValid() );
		assert( max.IsValid() );
		assert( b.min.IsValid() );
		assert( b.max.IsValid() );
		return min.x <= b.min.x && min.y <= b.min.y && min.z <= b.min.z
				&& max.x >= b.max.x && max.y >= b.max.y && max.z >= b.max.z;
	}


	// Check two bounding boxes for intersection.
	inline bool	IsIntersectBox( const AABB &b ) const	
	{
		assert( min.IsValid() );
		assert( max.IsValid() );
		assert( b.min.IsValid() );
		assert( b.max.IsValid() );
		// Check for intersection on X axis.
		if ((min.x > b.max.x)||(b.min.x > max.x)) return false;
		// Check for intersection on Y axis.
		if ((min.y > b.max.y)||(b.min.y > max.y)) return false;
		// Check for intersection on Z axis.
		if ((min.z > b.max.z)||(b.min.z > max.z)) return false;
		// Boxes overlap in all 3 axises.
		return true;
	}

	/*!
	* calculate the new bounds of a transformed AABB 
	*
	* Example:
	* AABB aabb = AABB::CreateAABBfromOBB(m34,aabb);
	*
	* return values:
	*  expanded AABB in world-space
	*/
	ILINE void SetTransformedAABB( const Matrix34& m34, const AABB& aabb ) 
	{

		if (aabb.IsReset())
			Reset();
		else
		{
			Matrix33 m33;
			m33.m00=fabs_tpl(m34.m00); m33.m01=fabs_tpl(m34.m01);	m33.m02=fabs_tpl(m34.m02); 
			m33.m10=fabs_tpl(m34.m10); m33.m11=fabs_tpl(m34.m11); m33.m12=fabs_tpl(m34.m12);
			m33.m20=fabs_tpl(m34.m20); m33.m21=fabs_tpl(m34.m21);	m33.m22=fabs_tpl(m34.m22);

			Vec3 sz		=	m33*((aabb.max-aabb.min)*0.5f);
			Vec3 pos	= m34*((aabb.max+aabb.min)*0.5f);
			min = pos-sz;	max = pos+sz;
		}
	}
	ILINE static AABB CreateTransformedAABB( const Matrix34& m34, const AABB& aabb ) 
		{ AABB taabb; taabb.SetTransformedAABB(m34,aabb); return taabb; 	}



	ILINE void SetTransformedAABB( const QuatT& qt, const AABB& aabb ) 
	{
		if (aabb.IsReset())
			Reset();
		else
		{
			Matrix33 m33=Matrix33(qt.q);
			m33.m00=fabs_tpl(m33.m00); m33.m01=fabs_tpl(m33.m01);	m33.m02=fabs_tpl(m33.m02); 
			m33.m10=fabs_tpl(m33.m10); m33.m11=fabs_tpl(m33.m11); m33.m12=fabs_tpl(m33.m12);
			m33.m20=fabs_tpl(m33.m20); m33.m21=fabs_tpl(m33.m21);	m33.m22=fabs_tpl(m33.m22);
			Vec3 sz		=	m33*((aabb.max-aabb.min)*0.5f);
			Vec3 pos	=  qt*((aabb.max+aabb.min)*0.5f);
			min = pos-sz;	max = pos+sz;
		}
	}
	ILINE static AABB CreateTransformedAABB( const QuatT& qt, const AABB& aabb ) 
	{ AABB taabb; taabb.SetTransformedAABB(qt,aabb); return taabb; 	}


	//create an AABB using just the extensions of the OBB and ignore the orientation. 
	template<typename F>
	ILINE void SetAABBfromOBB( const OBB_tpl<F>& obb ) 
		{ min=obb.c-obb.h; max=obb.c+obb.h;	}
	template<typename F>
	ILINE static AABB CreateAABBfromOBB( const OBB_tpl<F>& obb ) 
		{	return AABB(obb.c-obb.h,obb.c+obb.h);	}

	/*!
	* converts an OBB into an tight fitting AABB 
	*
	* Example:
	* AABB aabb = AABB::CreateAABBfromOBB(wposition,obb,1.0f);
	*
	* return values:
	*  expanded AABB in world-space
	*/
	template<typename F>
	ILINE void SetAABBfromOBB( const Vec3& wpos, const OBB_tpl<F>& obb, f32 scaling=1.0f ) 
	{
		Vec3 pos	= obb.m33*obb.c*scaling + wpos;
		Vec3 sz		=	obb.m33.GetFabs()*obb.h*scaling;
		min=pos-sz; max=pos+sz;
	}
	template<typename F>
	ILINE static AABB CreateAABBfromOBB( const Vec3& wpos, const OBB_tpl<F>& obb, f32 scaling=1.0f) 
		{ AABB taabb; taabb.SetAABBfromOBB(wpos,obb,scaling); return taabb; 	}

  AUTO_STRUCT_INFO
};

ILINE bool IsEquivalent( const AABB& a, const AABB& b, float epsilon=VEC_EPSILON )
{
	return IsEquivalent(a.min, b.min, epsilon) && IsEquivalent(a.max, b.max, epsilon);
}





///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct OBB
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename F> struct OBB_tpl {

	Matrix33 m33; //orientation vectors
	Vec3 h;				//half-length-vector
	Vec3 c;				//center of obb 

	//default OBB constructor (without initialisation)
	inline OBB_tpl() {}

	ILINE void SetOBB( const Matrix33& matrix, const Vec3& hlv, const Vec3& center  ) {  m33=matrix; h=hlv; c=center;	}
	ILINE static OBB_tpl<F> CreateOBB( const Matrix33& m33, const Vec3& hlv, const Vec3& center  ) {	OBB_tpl<f32> obb; obb.m33=m33; obb.h=hlv; obb.c=center; return obb;	}

	ILINE void SetOBBfromAABB( const Matrix33& mat33, const AABB& aabb ) {
		m33	=	mat33;
		h		=	(aabb.max-aabb.min)*0.5f;	//calculate the half-length-vectors
		c		=	(aabb.max+aabb.min)*0.5f;	//the center is relative to the PIVOT
	}
	ILINE void SetOBBfromAABB( const Quat& q, const AABB& aabb ) {
		m33	=	Matrix33(q);
		h		=	(aabb.max-aabb.min)*0.5f;	//calculate the half-length-vectors
		c		=	(aabb.max+aabb.min)*0.5f;	//the center is relative to the PIVOT
	}
	ILINE static OBB_tpl<F> CreateOBBfromAABB( const Matrix33& m33, const AABB& aabb ) { OBB_tpl<f32> obb; obb.SetOBBfromAABB(m33,aabb); return obb;	}
	ILINE static OBB_tpl<F> CreateOBBfromAABB( const Quat& q, const AABB& aabb ) { OBB_tpl<f32> obb; obb.SetOBBfromAABB(q,aabb); return obb;	}

	~OBB_tpl( void ) {};
};




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Sphere
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct Sphere {

	Vec3 center;
	float radius;

	//default Sphere constructor (without initialisation)
	inline Sphere( void ) {}
	inline Sphere( const Vec3 &c, const float &r ) {  center=c; radius=r; }
	inline void operator () (  const Vec3 &c, const float &r  ) {  center=c; radius=r; }

	~Sphere( void ) {};
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct AAEllipsoid
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct AAEllipsoid {

	Vec3 center;
	Vec3 radius_vec;

	//default AAEllipsoid constructor (without initialisation)
	inline AAEllipsoid( void ) {}
	inline AAEllipsoid( const Vec3 &c, const Vec3 &rv  ) {  radius_vec=rv; center=c; }
	inline void operator () ( const Vec3 &c, const Vec3 &rv ) {  radius_vec=rv; center=c; }

	~AAEllipsoid( void ) {};
};



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Ellipsoid
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct Ellipsoid {

	Matrix34 ExtensionPos;

	//default Ellipsoid constructor (without initialisation)
	inline Ellipsoid( void ) {}
	inline Ellipsoid( const Matrix34 &ep ) {  ExtensionPos=ep; }
	inline void operator () (  const Matrix34 &ep  ) {  ExtensionPos=ep; }

	~Ellipsoid( void ) {};
};






typedef Triangle_tpl<f32>	Triangle;
typedef Triangle_tpl<f64>	Triangle_f64;
typedef OBB_tpl<f32>	OBB;





#include "Cry_GeoDistance.h" 
#include "Cry_GeoOverlap.h" 
#include "Cry_GeoIntersect.h" 










/////////////////////////////////////////////////////////////////////////
//this is some special engine stuff, should be moved to a better location
/////////////////////////////////////////////////////////////////////////

// for bbox's checks and calculations
#define MAX_BB	+99999.0f
#define MIN_BB	-99999.0f

//! checks if this has been set to minBB
inline bool IsMinBB( const Vec3& v ) {
	if (v.x<=MIN_BB) return (true);
	if (v.y<=MIN_BB) return (true);
	if (v.z<=MIN_BB) return (true);
	return (false);
}

//! checks if this has been set to maxBB
inline bool IsMaxBB( const Vec3& v ) {
	if (v.x>=MAX_BB) return (true);
	if (v.y>=MAX_BB) return (true);
	if (v.z>=MAX_BB) return (true);
	return (false);
}

inline Vec3	SetMaxBB( void ) { return Vec3(MAX_BB,MAX_BB,MAX_BB); }
inline Vec3	SetMinBB( void ) { return Vec3(MIN_BB,MIN_BB,MIN_BB); }

inline void AddToBounds (const Vec3& v, Vec3& mins, Vec3& maxs) {
	if (v.x < mins.x)	mins.x = v.x;
	if (v.x > maxs.x)	maxs.x = v.x;
	if (v.y < mins.y)	mins.y = v.y;
	if (v.y > maxs.y)	maxs.y = v.y;
	if (v.z < mins.z)	mins.z = v.z;
	if (v.z > maxs.z)	maxs.z = v.z;
}


////////////////////////////////////////////////////////////////		
//! calc the area of a polygon giving a list of vertices and normal
inline float CalcArea(const Vec3 *vertices,int numvertices,const Vec3 &normal)
{	
	Vec3 csum(0,0,0);

	int n=numvertices;
	for (int i = 0, j = 1; i <= n-2; i++, j++)
	{
		csum.x += vertices[i].y*vertices[j].z-vertices[i].z*vertices[j].y;
		csum.y += vertices[i].z*vertices[j].x-vertices[i].x*vertices[j].z;
		csum.z += vertices[i].x*vertices[j].y-vertices[i].y*vertices[j].x;
	}

	csum.x += vertices[n-1].y*vertices[0].z-vertices[n-1].z*vertices[0].y;
	csum.y += vertices[n-1].z*vertices[0].x-vertices[n-1].x*vertices[0].z;
	csum.z += vertices[n-1].x*vertices[0].y-vertices[n-1].y*vertices[0].x;

	float area=0.5f*(float)fabs(normal|csum);
	return (area);
}



#endif //geostructs
