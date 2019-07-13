//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:Cry_GeoDistance.h
//	Description: Common distance-computations
//
//	History:
//	-March 15,2003: Created by Ivo Herzeg
//  -April  7,2005: Added Point_Lineseg, Point_Line
//  -June  27,2005: Added some Lineseg/Triangle functions
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYDISTANCE_H
#define CRYDISTANCE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <Cry_Geo.h>
#if defined(__GNUC__)
#include <limits>
#endif

#ifdef max
#undef max
#endif

namespace Overlap
{	
  bool Lineseg_Triangle( const Lineseg &lineseg, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2 );
}


namespace Distance {

  template<typename F>
  ILINE F Point_Point(const Vec3_tpl<F> & p1, const Vec3_tpl<F> & p2) 
  {
    return sqrt_tpl(square(p1.x - p2.x) + square(p1.y - p2.y) + square(p1.z - p2.z));
  }

  template<typename F>
  ILINE F Point_PointSq(const Vec3_tpl<F> & p1, const Vec3_tpl<F> & p2) 
  {
    return square(p1.x - p2.x) + square(p1.y - p2.y) + square(p1.z - p2.z);
  }

  template<typename F>
  ILINE F Point_Point2DSq(const Vec3_tpl<F> & p1, const Vec3_tpl<F> & p2) 
  {
    return square(p1.x - p2.x) + square(p1.y - p2.y);
  }

  template<typename F>
  ILINE F Point_Point2D(const Vec3_tpl<F> & p1, const Vec3_tpl<F> & p2) 
  {
    return sqrt_tpl(square(p1.x - p2.x) + square(p1.y - p2.y));
  }

  /*!
  * Distance: Origin_Triangle2D
  * Calculate the closest distance of a triangle in XY-plane to the coordinate origin.
  * it is assumed that the z-values of the triangle are all in the same plane. 
  * The function returns the 3d-position of the closest point on the triangle.
  *
  * Example:
  *  Vec3 result = Distance::Origin_Triangle2D( triangle );
  */
  template<typename F>
  ILINE Vec3_tpl<F> Origin_Triangle2D( const Triangle_tpl<F>& t ) {
    Vec3_tpl<F> a=t.v0;	
    Vec3_tpl<F> b=t.v1;
    Vec3_tpl<F> c=t.v2;
    //check if (0,0,0) is inside or in fron of any triangle sides.
    uint32 flag = ((a.x*(a.y-b.y)-a.y*(a.x-b.x))<0) | (((b.x*(b.y-c.y)-b.y*(b.x-c.x))<0)<<1) | (((c.x*(c.y-a.y)-c.y*(c.x-a.x))<0)<<2);
    switch (flag) {
      case 0:	return Vec3_tpl<F>(0,0,a.z); //center is inside of triangle
      case 1:	if ((a|(b-a))>0.0f) flag=5;	else if ((b|(a-b))>0.0f) flag=3;	break;
      case 2:	if ((b|(c-b))>0.0f) flag=3; else if ((c|(b-c))>0.0f) flag=6;	break;
      case 3:	return b; //vertex B is closed
      case 4:	if ((c|(a-c))>0.0f) flag=6; else if ((a|(c-a))>0.0f) flag=5;	break;
      case 5:	return a; //vertex A is closed 
      case 6:	return c; //vertex C is closed
    }
    //check again using expanded area
    switch (flag) {
      case 1: { Vec3_tpl<F> n=(b-a).GetNormalized(); return n*(-a|n)+a; }
      case 2:	{	Vec3_tpl<F> n=(c-b).GetNormalized(); return n*(-b|n)+b; }
      case 3:	return b;
      case 4:	{ Vec3_tpl<F> n=(a-c).GetNormalized(); return n*(-c|n)+c; }
      case 5:	return a;
      case 6:	return c;
    }
    return Vec3_tpl<F>(0,0,0);
  }


  /*!
  * Distance: Point_Triangle
  * Calculate the closest distance of a point to a triangle in 3d-space.
  * The function returns the squared distance. 
  *
  * Example:
  *  float result = Distance::Point_Triangle( pos, triangle );
  */
  template<typename F>
  ILINE F Point_TriangleSq( const Vec3_tpl<F>& p, const Triangle_tpl<F> &t ) {
    //translate triangle into origin
    Vec3_tpl<F> a=t.v0-p;	
    Vec3_tpl<F> b=t.v1-p;	
    Vec3_tpl<F> c=t.v2-p;
    //transform triangle into XY-plane to simplify the test
    Matrix33_tpl<F> r33=Matrix33_tpl<F>::CreateRotationV0V1( ((b-a)%(a-c)).GetNormalized(),Vec3(0,0,1) );
    Vec3_tpl<F> h = Origin_Triangle2D(Triangle_tpl<F>(r33*a,r33*b,r33*c));
    return (h|h); //return squared distance
  }

  template<typename F>
  ILINE F Point_Triangle( const Vec3_tpl<F>& p, const Triangle_tpl<F> &t ) 
  {
    return sqrt_tpl(Point_TriangleSq(p, t));
  }

  /*!
  * Distance: Point_Triangle
  * Calculate the closest distance of a point to a triangle in 3d-space.
  * The function returns the squared distance and the 3d-position of the 
  * closest point on the triangle.
  *
  * Example:
  *  float result = Distance::Point_Triangle( pos, triangle, output );
  */
  template<typename F>
  ILINE F Point_TriangleSq( const Vec3_tpl<F>& p, const Triangle_tpl<F> &t, Vec3_tpl<F>& output ) {
    //translate triangle into origin
    Vec3_tpl<F> a=t.v0-p;	
    Vec3_tpl<F> b=t.v1-p;	
    Vec3_tpl<F> c=t.v2-p;
    //transform triangle into XY-plane to simplify the test
    Matrix33_tpl<F> r33=Matrix33_tpl<F>::CreateRotationV0( ((b-a)%(a-c)).GetNormalized() );
    Vec3_tpl<F> h = Origin_Triangle2D(Triangle_tpl<F>(r33*a,r33*b,r33*c));
    output=h*r33+p;
    return (h|h); //return squared distance
  }

  template<typename F>
  ILINE F Point_Triangle( const Vec3_tpl<F>& p, const Triangle_tpl<F> &t, Vec3_tpl<F>& output )
  {
    return sqrt_tpl(Point_TriangleSq(p, t, output));
  }

  /// Squared distance from point to triangle, optionally returning the triangle position in 
  /// parameteric form
  template<typename F>
  ILINE F Point_TriangleSq(const Vec3_tpl<F> & point, const Triangle_tpl<F> & triangle, F* pT0, F* pT1)
  {
    Vec3 diff = triangle.v0 - point;
    const Vec3 edge0 = triangle.v1 - triangle.v0;
    const Vec3 edge1 = triangle.v2 - triangle.v0;
    F fA00 = edge0.GetLengthSquared();
    F fA01 = edge0.Dot(edge1);
    F fA11 = edge1.GetLengthSquared();
    F fB0 = diff.Dot(edge0);
    F fB1 = diff.Dot(edge1);
    F fC = diff.GetLengthSquared();
    F fDet = abs(fA00*fA11-fA01*fA01);
    F fS = fA01*fB1-fA11*fB0;
    F fT = fA01*fB0-fA00*fB1;
    F fSqrDist;

    if ( fS + fT <= fDet )
    {
      if ( fS < (F)0.0 )
      {
        if ( fT < (F)0.0 )  // region 4
        {
          if ( fB0 < (F)0.0 )
          {
            fT = (F)0.0;
            if ( -fB0 >= fA00 )
            {
              fS = (F)1.0;
              fSqrDist = fA00+((F)2.0)*fB0+fC;
            }
            else
            {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
          else
          {
            fS = (F)0.0;
            if ( fB1 >= (F)0.0 )
            {
              fT = (F)0.0;
              fSqrDist = fC;
            }
            else if ( -fB1 >= fA11 )
            {
              fT = (F)1.0;
              fSqrDist = fA11+((F)2.0)*fB1+fC;
            }
            else
            {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
        }
        else  // region 3
        {
          fS = (F)0.0;
          if ( fB1 >= (F)0.0 )
          {
            fT = (F)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (F)1.0;
            fSqrDist = fA11+((F)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
      else if ( fT < (F)0.0 )  // region 5
      {
        fT = (F)0.0;
        if ( fB0 >= (F)0.0 )
        {
          fS = (F)0.0;
          fSqrDist = fC;
        }
        else if ( -fB0 >= fA00 )
        {
          fS = (F)1.0;
          fSqrDist = fA00+((F)2.0)*fB0+fC;
        }
        else
        {
          fS = -fB0/fA00;
          fSqrDist = fB0*fS+fC;
        }
      }
      else  // region 0
      {
        // minimum at interior point
        F fInvDet = ((F)1.0)/fDet;
        fS *= fInvDet;
        fT *= fInvDet;
        fSqrDist = fS*(fA00*fS+fA01*fT+((F)2.0)*fB0) +
          fT*(fA01*fS+fA11*fT+((F)2.0)*fB1)+fC;
      }
    }
    else
    {
      F fTmp0, fTmp1, fNumer, fDenom;

      if ( fS < (F)0.0 )  // region 2
      {
        fTmp0 = fA01 + fB0;
        fTmp1 = fA11 + fB1;
        if ( fTmp1 > fTmp0 )
        {
          fNumer = fTmp1 - fTmp0;
          fDenom = fA00-2.0f*fA01+fA11;
          if ( fNumer >= fDenom )
          {
            fS = (F)1.0;
            fT = (F)0.0;
            fSqrDist = fA00+((F)2.0)*fB0+fC;
          }
          else
          {
            fS = fNumer/fDenom;
            fT = (F)1.0 - fS;
            fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
              fT*(fA01*fS+fA11*fT+((F)2.0)*fB1)+fC;
          }
        }
        else
        {
          fS = (F)0.0;
          if ( fTmp1 <= (F)0.0 )
          {
            fT = (F)1.0;
            fSqrDist = fA11+((F)2.0)*fB1+fC;
          }
          else if ( fB1 >= (F)0.0 )
          {
            fT = (F)0.0;
            fSqrDist = fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
      else if ( fT < (F)0.0 )  // region 6
      {
        fTmp0 = fA01 + fB1;
        fTmp1 = fA00 + fB0;
        if ( fTmp1 > fTmp0 )
        {
          fNumer = fTmp1 - fTmp0;
          fDenom = fA00-((F)2.0)*fA01+fA11;
          if ( fNumer >= fDenom )
          {
            fT = (F)1.0;
            fS = (F)0.0;
            fSqrDist = fA11+((F)2.0)*fB1+fC;
          }
          else
          {
            fT = fNumer/fDenom;
            fS = (F)1.0 - fT;
            fSqrDist = fS*(fA00*fS+fA01*fT+((F)2.0)*fB0) +
              fT*(fA01*fS+fA11*fT+((F)2.0)*fB1)+fC;
          }
        }
        else
        {
          fT = (F)0.0;
          if ( fTmp1 <= (F)0.0 )
          {
            fS = (F)1.0;
            fSqrDist = fA00+((F)2.0)*fB0+fC;
          }
          else if ( fB0 >= (F)0.0 )
          {
            fS = (F)0.0;
            fSqrDist = fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
      }
      else  // region 1
      {
        fNumer = fA11 + fB1 - fA01 - fB0;
        if ( fNumer <= (F)0.0 )
        {
          fS = (F)0.0;
          fT = (F)1.0;
          fSqrDist = fA11+((F)2.0)*fB1+fC;
        }
        else
        {
          fDenom = fA00-2.0f*fA01+fA11;
          if ( fNumer >= fDenom )
          {
            fS = (F)1.0;
            fT = (F)0.0;
            fSqrDist = fA00+((F)2.0)*fB0+fC;
          }
          else
          {
            fS = fNumer/fDenom;
            fT = (F)1.0 - fS;
            fSqrDist = fS*(fA00*fS+fA01*fT+((F)2.0)*fB0) +
              fT*(fA01*fS+fA11*fT+((F)2.0)*fB1)+fC;
          }
        }
      }
    }

    if ( pT0 )
      *pT0 = fS;

    if ( pT1 )
      *pT1 = fT;

    return abs(fSqrDist);
  }

  /// Distance from point to triangle, optionally returning the triangle position in 
  /// parameteric form
  template<typename F>
  ILINE F Point_Triangle(const Vec3_tpl<F> & point, const Triangle_tpl<F> & triangle, F* pT0, F* pT1)
  {
    return sqrt_tpl(Point_TriangleSq(point, triangle, pT0, pT1));
  }

  //----------------------------------------------------------------------------------
  /// Returns squared distance from a point to a line segment and also the "t value" (from 0 to 1) of the
  /// closest point on the line segment
  //----------------------------------------------------------------------------------
  template<typename F>
  ILINE F Point_LinesegSq(const Vec3_tpl<F> & p, const Lineseg & lineseg, F & fT) 
  {
    Vec3_tpl<F> diff = p - lineseg.start;
    Vec3_tpl<F> dir = lineseg.end - lineseg.start;
    fT = diff.Dot(dir);

    if ( fT <= 0.0f )
    {
      fT = 0.0f;
    }
    else
    {
      F fSqrLen= dir.GetLengthSquared();
      if ( fT >= fSqrLen )
      {
        fT = 1.0f;
        diff -= dir;
      }
      else
      {
        fT /= fSqrLen;
        diff -= fT*dir;
      }
    }

    return diff.GetLengthSquared();
  }

  /// Returns distance from a point to a line segment and also the "t value" (from 0 to 1) of the
  /// closest point on the line segment
  template<typename F>
  ILINE F Point_Lineseg(const Vec3_tpl<F> & p, const Lineseg & lineseg, F & fT) 
  {
    return sqrt_tpl(Point_LinesegSq(p, lineseg, fT));
  }


  //----------------------------------------------------------------------------------
  /// Returns squared distance from a point to a line segment, ignoring the z coordinates
  // TODO optimise. 
  //----------------------------------------------------------------------------------
  template<typename F>
  ILINE F Point_Lineseg2DSq(Vec3_tpl<F> p, Lineseg lineseg, F & fT) 
  {
    p.z = 0.0f;
    lineseg.start.z = 0.0f;
    lineseg.end.z = 0.0f;
    return Point_LinesegSq(p, lineseg, fT);
  }

  /// Returns distance from a point to a line segment, ignoring the z coordinates
  template<typename F>
  ILINE F Point_Lineseg2D(const Vec3_tpl<F> & p, const Lineseg & lineseg, F & fT) 
  {
    return sqrt_tpl(Point_Lineseg2DSq(p, lineseg, fT));
  }

  /// Returns the squared distance from a point to a line as defined by two points (for accuracy
  /// in some situations), and also the closest position on the line
  template<typename F>
  ILINE F Point_LineSq(const Vec3_tpl<F> & vPoint, 
    const Vec3_tpl<F> & vLineStart, const Vec3_tpl<F> & vLineEnd, Vec3_tpl<F> &linePt)
  {
    Vec3_tpl<F> dir;
    Vec3_tpl<F> pointVector;

    if ((vPoint-vLineStart).GetLengthSquared() > (vPoint-vLineEnd).GetLengthSquared())
    {
      dir = vLineStart - vLineEnd;
      pointVector = vPoint - vLineEnd;
      linePt = vLineEnd;
    }
    else
    {
      dir = vLineEnd-vLineStart;
      pointVector = vPoint - vLineStart;
      linePt = vLineStart;
    }	

    F dirLen2 = dir.GetLengthSquared();
    if (dirLen2 <= 0.0f)
      return pointVector.GetLengthSquared();
    dir /= sqrt_tpl(dirLen2);

    F t0 = pointVector.Dot(dir);
    linePt += t0 * dir;
    return (vPoint - linePt).GetLengthSquared();
  }

  /// Returns the distance from a point to a line as defined by two points (for accuracy
  /// in some situations)
  template<typename F>
  ILINE F Point_Line(const Vec3_tpl<F> & vPoint, 
    const Vec3_tpl<F> & vLineStart, const Vec3_tpl<F> & vLineEnd, Vec3_tpl<F> &linePt)
  {
    return sqrt_tpl(Point_LineSq(vPoint, vLineStart, vLineEnd, linePt));
  }

  /// In 2D. The returned linePt will have 0 z value
  template<typename F>
  ILINE F Point_Line2DSq(Vec3_tpl<F> vPoint, 
    Vec3_tpl<F> vLineStart, Vec3_tpl<F> vLineEnd, Vec3_tpl<F> &linePt)
  {
    vPoint.z = 0.0f;
    vLineStart.z = 0.0f;
    vLineEnd.z = 0.0f;
    return Point_LineSq(vPoint, vLineStart, vLineEnd, linePt);
  }

  /// In 2D. The returned linePt will have 0 z value
  template<typename F>
  ILINE F Point_Line2D(const Vec3_tpl<F> & vPoint, 
    const Vec3_tpl<F> & vLineStart, const Vec3_tpl<F> & vLineEnd, Vec3_tpl<F> &linePt)
  {
    return sqrt_tpl(Point_Line2DSq(vPoint, vLineStart, vLineEnd, linePt));
  }


  /// Returns squared distance from a point to a polygon _edge_, together with the closest point on
  /// the edge of the polygon. Can use the same point in/out
  template<typename F, typename VecContainer>
  inline F Point_Polygon2DSq(const Vec3_tpl<F> p, const VecContainer & polygon, Vec3_tpl<F> & polyPos, Vec3_tpl<F>* pNormal=NULL)
  {
    typename VecContainer::const_iterator li,linext;
    typename VecContainer::const_iterator liend = polygon.end();
    float bestDist = std::numeric_limits<F>::max();
    for (li = polygon.begin(); li != liend ; ++li)
    {
      linext = li;
      ++linext;
      if (linext == liend)
        linext = polygon.begin();
      const Vec3_tpl<F>  & l0 = *li;
      const Vec3_tpl<F>  & l1 = *linext;

      float f;
      float thisDist = Distance::Point_Lineseg2DSq<F>(p, Lineseg(l0, l1), f);
      if (thisDist < bestDist)
      {
        bestDist = thisDist;
        polyPos = l0 + f * (l1 - l0);
        if(pNormal )
        {
          Vec3_tpl<F> vPolyseg =l1  - l0;
          Vec3_tpl<F> vIntSeg = (polyPos - p);
          pNormal->x = vPolyseg.y;
          pNormal->y = -vPolyseg.x;
          pNormal->z = 0;
          pNormal->NormalizeSafe();
          // returns the normal towards the start point of the intersecting segment
          if((vIntSeg.Dot(*pNormal))>0)
          {
            pNormal->x = -pNormal->x;
            pNormal->y = -pNormal->y;
          }
        }      
      }
    }
    return bestDist;
  }

  /// Returns distance from a point to a polygon _edge_, together with the closest point on
  /// the edge of the polygon
  template<typename F, typename VecContainer>
  inline F Point_Polygon2D(const Vec3_tpl<F> p, const VecContainer &polygon, Vec3_tpl<F> &polyPos, Vec3_tpl<F>* pNormal=NULL)
  {
    return sqrt_tpl(Point_Polygon2DSq(p, polygon, polyPos,pNormal));
  }

  //----------------------------------------------------------------------------------
  // Distance: Point_AABB
  //----------------------------------------------------------------------------------
  // Calculate the closest distance of a point to a AABB in 3d-space.
  // The function returns the squared distance. 
  // optionally the closest point on the hull is calculated
  //
  // Example:
  //  float result = Distance::Point_AABBSq( pos, aabb );
  //----------------------------------------------------------------------------------
  template<typename F>
  ILINE F Point_AABBSq( const Vec3_tpl<F> & vPoint, const AABB& aabb) 
  {	
    F fDist2 = 0;
    for(int i=0; i<3; ++i)
    {
      if(vPoint[i] < aabb.min[i])
      {
        fDist2 += sqr((F)vPoint[i]-(F)aabb.min[i]);
      }
      else if(vPoint[i] > aabb.max[i])
      {
        fDist2 += sqr((F)aabb.max[i]-(F)vPoint[i]);
      }
    }
    return fDist2;
  }

  template<typename F>
	ILINE F Point_AABBSq( const Vec3_tpl<F> & vPoint, const AABB& aabb, Vec3_tpl<F> & vClosest) 
	{
		F fDist2 = Point_AABBSq(vPoint, aabb);

		vClosest = vPoint;
		if (!iszero(fDist2))
		{
			// vPoint is outside the AABB
			vClosest.x = max(vClosest.x,aabb.min.x);
			vClosest.x = min(vClosest.x,aabb.max.x);
			vClosest.y = max(vClosest.y,aabb.min.y);
			vClosest.y = min(vClosest.y,aabb.max.y);
			vClosest.z = max(vClosest.z,aabb.min.z);
			vClosest.z = min(vClosest.z,aabb.max.z);
		}
		else
		{
			// vPoint is inside the AABB
			uint8 nSubBox = 0;
			F fHalf = 2;

			F fMiddleX = ((aabb.max.x - aabb.min.x) / fHalf) + aabb.min.x;
			F fMiddleY = ((aabb.max.y - aabb.min.y) / fHalf) + aabb.min.y;
			F fMiddleZ = ((aabb.max.z - aabb.min.z) / fHalf) + aabb.min.z;

			if (vPoint.x < fMiddleX)
				nSubBox |= 0x001; // Is Left

			if (vPoint.y < fMiddleY)
				nSubBox |= 0x010; // Is Rear

			if (vPoint.z < fMiddleZ)
				nSubBox |= 0x100; // Is Low

			F fDistanceToX;
			F fDistanceToY;
			F fDistanceToZ;

			F fNewX, fNewY, fNewZ;

			switch(nSubBox)
			{
			case 0x000:
				// Is Right/Front/Top
				fDistanceToX = aabb.max.x - vPoint.x;
				fDistanceToY = aabb.max.y - vPoint.y;
				fDistanceToZ = aabb.max.z - vPoint.z;

				fNewX = aabb.max.x;
				fNewY = aabb.max.y;
				fNewZ = aabb.max.z;

				break;
			case 0x001:
				// Is Left/Front/Top
				fDistanceToX = vPoint.x - aabb.min.x;
				fDistanceToY = aabb.max.y - vPoint.y;
				fDistanceToZ = aabb.max.z - vPoint.z;

				fNewX = aabb.min.x;
				fNewY = aabb.max.y;
				fNewZ = aabb.max.z;

				break;
			case 0x010:
				// Is Right/Rear/Top
				fDistanceToX = aabb.max.x - vPoint.x;
				fDistanceToY = vPoint.y - aabb.min.y;
				fDistanceToZ = aabb.max.z - vPoint.z;

				fNewX = aabb.max.x;
				fNewY = aabb.min.y;
				fNewZ = aabb.max.z;

				break;
			case 0x011:
				// Is Left/Rear/Top
				fDistanceToX = vPoint.x - aabb.min.x;
				fDistanceToY = vPoint.y - aabb.min.y;
				fDistanceToZ = aabb.max.z - vPoint.z;

				fNewX = aabb.min.x;
				fNewY = aabb.min.y;
				fNewZ = aabb.max.z;

				break;
			case 0x100:
				// Is Right/Front/Low
				fDistanceToX = aabb.max.x - vPoint.x;
				fDistanceToY = aabb.max.y - vPoint.y;
				fDistanceToZ = vPoint.z - aabb.min.z;

				fNewX = aabb.max.x;
				fNewY = aabb.max.y;
				fNewZ = aabb.min.z;

				break;
			case 0x101:
				// Is Left/Front/Low
				fDistanceToX = vPoint.x - aabb.min.x;
				fDistanceToY = aabb.max.y - vPoint.y;
				fDistanceToZ = vPoint.z - aabb.min.z;

				fNewX = aabb.min.x;
				fNewY = aabb.max.y;
				fNewZ = aabb.min.z;

				break;
			case 0x110:
				// Is Right/Rear/Low
				fDistanceToX = aabb.max.x - vPoint.x;
				fDistanceToY = vPoint.y - aabb.min.y;
				fDistanceToZ = vPoint.z - aabb.min.z;

				fNewX = aabb.max.x;
				fNewY = aabb.min.y;
				fNewZ = aabb.min.z;

				break;
			case 0x111:
				// Is Left/Rear/Low
				fDistanceToX = vPoint.x - aabb.min.x;
				fDistanceToY = vPoint.y - aabb.min.y;
				fDistanceToZ = vPoint.z - aabb.min.z;

				fNewX = aabb.min.x;
				fNewY = aabb.min.y;
				fNewZ = aabb.min.z;

				break;
			default:
				break;
			}

			if (fDistanceToX < fDistanceToY && fDistanceToX < fDistanceToZ)
				vClosest.x = fNewX;

			if (fDistanceToY < fDistanceToX && fDistanceToY < fDistanceToZ)
				vClosest.y = fNewY;

			if (fDistanceToZ < fDistanceToX && fDistanceToZ < fDistanceToY)
				vClosest.z = fNewZ;

			fDist2 = vClosest.GetSquaredDistance(vPoint);

			}
			return fDist2;
	}

  //----------------------------------------------------------------------------------
  // Distance: Sphere_Triangle
  //----------------------------------------------------------------------------------
  // Calculate the closest distance of a sphere to a triangle in 3d-space.
  // The function returns the squared distance. If sphere and triangle overlaps, 
  // the returned distance is 0
  //
  // Example:
  //  float result = Distance::Point_TriangleSq( pos, triangle );
  //----------------------------------------------------------------------------------
  template<typename F>
  ILINE F Sphere_TriangleSq( const Sphere &s, const Triangle_tpl<F> &t ) {
    F sqdistance =  Distance::Point_TriangleSq(s.center,t) - (s.radius*s.radius);
    if (sqdistance<0) sqdistance=0; 
    return sqdistance;
  }

  template<typename F>
  ILINE F Sphere_TriangleSq( const Sphere &s, const Triangle_tpl<F> &t, Vec3_tpl<F>& output ) {
    F sqdistance =  Distance::Point_TriangleSq(s.center,t,output) - (s.radius*s.radius);
    if (sqdistance<0) sqdistance=0; 
    return sqdistance;
  }

  //----------------------------------------------------------------------------------
  /// Calculate squared distance between two line segments, along with the optional
  /// parameters of the closest points
  //----------------------------------------------------------------------------------
  template<typename F>
  ILINE F Lineseg_LinesegSq(const Lineseg& seg0, const Lineseg seg1, F* t0, F* t1)
  {
    Vec3 diff = seg0.start - seg1.start;
    Vec3 delta0 = seg0.end - seg0.start;
    Vec3 delta1 = seg1.end - seg1.start;
    F fA00 = delta0.GetLengthSquared();
    F fA01 = -delta0.Dot(delta1);
    F fA11 = delta1.GetLengthSquared();
    F fB0 = diff.Dot(delta0);
    F fC = diff.GetLengthSquared();
    F fDet = abs(fA00*fA11-fA01*fA01);
    F fB1, fS, fT, fSqrDist, fTmp;

    if ( fDet > (F) 0.0 )
    {
      // line segments are not parallel
      fB1 = -diff.Dot(delta1);
      fS = fA01*fB1-fA11*fB0;
      fT = fA01*fB0-fA00*fB1;

      if ( fS >= (F)0.0 )
      {
        if ( fS <= fDet )
        {
          if ( fT >= (F)0.0 )
          {
            if ( fT <= fDet )  // region 0 (interior)
            {
              // minimum at two interior points of 3D lines
              F fInvDet = ((F)1.0)/fDet;
              fS *= fInvDet;
              fT *= fInvDet;
              fSqrDist = fS*(fA00*fS+fA01*fT+((F)2.0)*fB0) +
                fT*(fA01*fS+fA11*fT+((F)2.0)*fB1)+fC;
            }
            else  // region 3 (side)
            {
              fT = (F)1.0;
              fTmp = fA01+fB0;
              if ( fTmp >= (F)0.0 )
              {
                fS = (F)0.0;
                fSqrDist = fA11+((F)2.0)*fB1+fC;
              }
              else if ( -fTmp >= fA00 )
              {
                fS = (F)1.0;
                fSqrDist = fA00+fA11+fC+((F)2.0)*(fB1+fTmp);
              }
              else
              {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+((F)2.0)*fB1+fC;
              }
            }
          }
          else  // region 7 (side)
          {
            fT = (F)0.0;
            if ( fB0 >= (F)0.0 )
            {
              fS = (F)0.0;
              fSqrDist = fC;
            }
            else if ( -fB0 >= fA00 )
            {
              fS = (F)1.0;
              fSqrDist = fA00+((F)2.0)*fB0+fC;
            }
            else
            {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
        }
        else
        {
          if ( fT >= (F)0.0 )
          {
            if ( fT <= fDet )  // region 1 (side)
            {
              fS = (F)1.0;
              fTmp = fA01+fB1;
              if ( fTmp >= (F)0.0 )
              {
                fT = (F)0.0;
                fSqrDist = fA00+((F)2.0)*fB0+fC;
              }
              else if ( -fTmp >= fA11 )
              {
                fT = (F)1.0;
                fSqrDist = fA00+fA11+fC+((F)2.0)*(fB0+fTmp);
              }
              else
              {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+((F)2.0)*fB0+fC;
              }
            }
            else  // region 2 (corner)
            {
              fTmp = fA01+fB0;
              if ( -fTmp <= fA00 )
              {
                fT = (F)1.0;
                if ( fTmp >= (F)0.0 )
                {
                  fS = (F)0.0;
                  fSqrDist = fA11+((F)2.0)*fB1+fC;
                }
                else
                {
                  fS = -fTmp/fA00;
                  fSqrDist = fTmp*fS+fA11+((F)2.0)*fB1+fC;
                }
              }
              else
              {
                fS = (F)1.0;
                fTmp = fA01+fB1;
                if ( fTmp >= (F)0.0 )
                {
                  fT = (F)0.0;
                  fSqrDist = fA00+((F)2.0)*fB0+fC;
                }
                else if ( -fTmp >= fA11 )
                {
                  fT = (F)1.0;
                  fSqrDist = fA00+fA11+fC+
                    ((F)2.0)*(fB0+fTmp);
                }
                else
                {
                  fT = -fTmp/fA11;
                  fSqrDist = fTmp*fT+fA00+((F)2.0)*fB0+fC;
                }
              }
            }
          }
          else  // region 8 (corner)
          {
            if ( -fB0 < fA00 )
            {
              fT = (F)0.0;
              if ( fB0 >= (F)0.0 )
              {
                fS = (F)0.0;
                fSqrDist = fC;
              }
              else
              {
                fS = -fB0/fA00;
                fSqrDist = fB0*fS+fC;
              }
            }
            else
            {
              fS = (F)1.0;
              fTmp = fA01+fB1;
              if ( fTmp >= (F)0.0 )
              {
                fT = (F)0.0;
                fSqrDist = fA00+((F)2.0)*fB0+fC;
              }
              else if ( -fTmp >= fA11 )
              {
                fT = (F)1.0;
                fSqrDist = fA00+fA11+fC+((F)2.0)*(fB0+fTmp);
              }
              else
              {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+((F)2.0)*fB0+fC;
              }
            }
          }
        }
      }
      else 
      {
        if ( fT >= (F)0.0 )
        {
          if ( fT <= fDet )  // region 5 (side)
          {
            fS = (F)0.0;
            if ( fB1 >= (F)0.0 )
            {
              fT = (F)0.0;
              fSqrDist = fC;
            }
            else if ( -fB1 >= fA11 )
            {
              fT = (F)1.0;
              fSqrDist = fA11+((F)2.0)*fB1+fC;
            }
            else
            {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
          else  // region 4 (corner)
          {
            fTmp = fA01+fB0;
            if ( fTmp < (F)0.0 )
            {
              fT = (F)1.0;
              if ( -fTmp >= fA00 )
              {
                fS = (F)1.0;
                fSqrDist = fA00+fA11+fC+((F)2.0)*(fB1+fTmp);
              }
              else
              {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+((F)2.0)*fB1+fC;
              }
            }
            else
            {
              fS = (F)0.0;
              if ( fB1 >= (F)0.0 )
              {
                fT = (F)0.0;
                fSqrDist = fC;
              }
              else if ( -fB1 >= fA11 )
              {
                fT = (F)1.0;
                fSqrDist = fA11+((F)2.0)*fB1+fC;
              }
              else
              {
                fT = -fB1/fA11;
                fSqrDist = fB1*fT+fC;
              }
            }
          }
        }
        else   // region 6 (corner)
        {
          if ( fB0 < (F)0.0 )
          {
            fT = (F)0.0;
            if ( -fB0 >= fA00 )
            {
              fS = (F)1.0;
              fSqrDist = fA00+((F)2.0)*fB0+fC;
            }
            else
            {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
          else
          {
            fS = (F)0.0;
            if ( fB1 >= (F)0.0 )
            {
              fT = (F)0.0;
              fSqrDist = fC;
            }
            else if ( -fB1 >= fA11 )
            {
              fT = (F)1.0;
              fSqrDist = fA11+((F)2.0)*fB1+fC;
            }
            else
            {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
        }
      }
    }
    else
    {
      // line segments are parallel
      if ( fA01 > (F)0.0 )
      {
        // direction vectors form an obtuse angle
        if ( fB0 >= (F)0.0 )
        {
          fS = (F)0.0;
          fT = (F)0.0;
          fSqrDist = fC;
        }
        else if ( -fB0 <= fA00 )
        {
          fS = -fB0/fA00;
          fT = (F)0.0;
          fSqrDist = fB0*fS+fC;
        }
        else
        {
          fB1 = -diff.Dot(delta1);
          fS = (F)1.0;
          fTmp = fA00+fB0;
          if ( -fTmp >= fA01 )
          {
            fT = (F)1.0;
            fSqrDist = fA00+fA11+fC+((F)2.0)*(fA01+fB0+fB1);
          }
          else
          {
            fT = -fTmp/fA01;
            fSqrDist = fA00+((F)2.0)*fB0+fC+fT*(fA11*fT+
              ((F)2.0)*(fA01+fB1));
          }
        }
      }
      else
      {
        // direction vectors form an acute angle
        if ( -fB0 >= fA00 )
        {
          fS = (F)1.0;
          fT = (F)0.0;
          fSqrDist = fA00+((F)2.0)*fB0+fC;
        }
        else if ( fB0 <= (F)0.0 )
        {
          fS = -fB0/fA00;
          fT = (F)0.0;
          fSqrDist = fB0*fS+fC;
        }
        else
        {
          fB1 = -diff.Dot(delta1);
          fS = (F)0.0;
          if ( fB0 >= -fA01 )
          {
            fT = (F)1.0;
            fSqrDist = fA11+((F)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB0/fA01;
            fSqrDist = fC+fT*(((F)2.0)*fB1+fA11*fT);
          }
        }
      }
    }

    if ( t0 )
      *t0 = fS;

    if ( t1 )
      *t1 = fT;

    return abs(fSqrDist);
  }

  /// Calculate distance between two line segments, along with the optional
  /// parameters of the closest points
  template<typename F>
  ILINE F Lineseg_Lineseg(const Lineseg& seg0, const Lineseg seg1, F* s, F* t)
  {
    return sqrt_tpl(Lineseg_LinesegSq(seg0, seg1, s, t));
  }

  //----------------------------------------------------------------------------------
  /// Squared distance from line segment to triangle. Optionally returns the parameters
  /// describing the closest points 
  //----------------------------------------------------------------------------------
  template<typename F>
  ILINE F Lineseg_TriangleSq(const Lineseg& seg, const Triangle_tpl<F>& triangle, 
    F* segT, F* triT0, F* triT1)
  {
    if (Overlap::Lineseg_Triangle(seg, triangle.v0, triangle.v1, triangle.v2))
      return 0.0f;

    // compare segment to all three edges of the triangle
    F s, t, u;
    F distEdgeSq = Distance::Lineseg_LinesegSq(seg, Lineseg(triangle.v0, triangle.v1), &s, &t);
    F distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = 0.0f;

    distEdgeSq = Distance::Lineseg_LinesegSq(seg, Lineseg(triangle.v0, triangle.v2), &s, &t);
    if (distEdgeSq < distSq)
    {
      distSq = distEdgeSq;
      if (segT) *segT = s;
      if (triT0) *triT0 = 0.0f;
      if (triT1) *triT1 = t;
    }
    distEdgeSq = Distance::Lineseg_LinesegSq(seg, Lineseg(triangle.v1, triangle.v2), &s, &t);
    if (distEdgeSq < distSq)
    {
      distSq = distEdgeSq;
      if (segT) *segT = s;
      if (triT0) *triT0 = 1.0f - t;
      if (triT1) *triT1 = t;
    }

    // compare segment end points to triangle interior
    F startTriSq = Distance::Point_TriangleSq(seg.start, triangle, &t, &u);
    if (startTriSq < distSq)
    {
      distSq = startTriSq;
      if (segT) *segT = 0.0f;
      if (triT0) *triT0 = t;
      if (triT1) *triT1 = u;
    }
    F endTriSq = Distance::Point_TriangleSq(seg.end, triangle, &t, &u);
    if (endTriSq < distSq)
    {
      distSq = endTriSq;
      if (segT) *segT = 1.0f;
      if (triT0) *triT0 = t;
      if (triT1) *triT1 = u;
    }
    return distSq;
  }

  /// Distance from line segment to triangle. Optionally returns the parameters
  /// describing the closest points 
  template<typename F>
  ILINE F Lineseg_Triangle(const Lineseg& seg, const Triangle_tpl<F>& triangle, 
    F* segT, F* triT0, F* triT1)
  {
    return sqrt_tpl(Lineseg_TriangleSq(seg, triangle, segT, triT0, triT1));
  }

} //namespace Distance


#endif
