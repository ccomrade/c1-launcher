//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File: GeomQuery.h
//	Description: Facility for efficiently generating random positions on geometry
//
//	History:
//		2006-05-24:		Created by J Scott Peter
//
//////////////////////////////////////////////////////////////////////

#ifndef GEOM_QUERY_H
#define GEOM_QUERY_H

#include "Cry_Geo.h"
#include "CryArray.h"

// Implementation for efficient repeated queries.
struct GeomQuery
{
	GeomQuery()
		: m_pOwner(0), m_nParts(0), m_fExtent(0.f)
	{}

	~GeomQuery()
	{
	}

	// Set owner context, return whether it's updated.
	bool SetOwner(void const* pOwner, EGeomForm eForm)
	{
		if (m_pOwner != pOwner || m_eForm != eForm)
		{
			m_pOwner = pOwner;
			m_eForm = eForm;
			m_fExtent = 0.f;
			m_aCumExtents.clear();
			m_aParts.clear();
			return true;
		}
		return false;
	}

	float GetExtent() const
	{
		return m_fExtent;
	}

	template<class T>
	float GetExtent(T* pOwner, EGeomForm eForm)
	{
		if (SetOwner(pOwner, eForm))
			m_fExtent = pOwner->ComputeExtent(*this, eForm);
		return m_fExtent;
	}

	float SetExtent(float fExtent)
	{
		return m_fExtent = fExtent;
	}
	void SetNumParts(int nParts)
	{
		m_nParts = nParts;
		m_aCumExtents.resize(m_nParts);
		m_fExtent = 0.f;
	}
	void SetPartExtent(int iPart, float fExt)
	{
		if (iPart >= m_nParts)
		{
			m_nParts = iPart+1;
			m_aCumExtents.resize(m_nParts);
		}
		m_fExtent += fExt;
		m_aCumExtents[iPart] = m_fExtent;
	}

	// Basic part extent caching.
	int GetNumParts() const
	{
		return m_nParts;
	}

	// Detailed part extent caching.
	void AllocParts(int nParts)
	{
		m_nParts = nParts;
		m_aParts.resize(m_nParts);
	}

	GeomQuery& GetPart(int nPart)
	{
		return m_aParts[nPart];
	}

	int GetRandomPart() const
	{
		if (m_aCumExtents.size())
		{
			// Binary search thru CumExtents.
			float f = Random(m_fExtent);
			int lo = 0, hi = m_aCumExtents.size()-1;
			while (lo < hi)
			{
				int i = (lo+hi)/2;
				if (f <= m_aCumExtents[i])
					hi = i;
				else
					lo = i+1;
			}
			return lo;
		}
		else if (m_aParts.size())
		{
			float fRan = Random(m_fExtent);
			for (uint32 i = 0, end = m_aParts.size()-1; i < end; i++)
			{
				fRan -= m_aParts[i].m_fExtent;
				if (fRan <= 0.f)
					return i;
			}
		}
		else if (m_nParts > 1)
			return Random(m_nParts);

		return m_nParts-1;
	}

	template<class T>
	int GetRandomPart(T* pOwner, EGeomForm eForm) const
	{
		return GetRandomPart();
	}

private:
	void const*					m_pOwner;
	EGeomForm						m_eForm;
	float								m_fExtent;						// Cached total extent.
	int									m_nParts;							// Number of parts.
	DynArray<float,FSystemAlloc>				m_aCumExtents;				// Per-part extent (optional).
	DynArray<GeomQuery,FSystemAlloc>		m_aParts;							// Per-part full info (optional).
};

// Other random/extent functions

inline float ScaleExtent(EGeomForm eForm, float fScale)
{
	switch (eForm)
	{
		default:
			return 1;
		case GeomForm_Edges:
			return fScale;
		case GeomForm_Surface:
			return fScale*fScale;
		case GeomForm_Volume:
			return fScale*fScale*fScale;
	}
}

inline float BoxExtent(EGeomForm eForm, Vec3 const& vSize)
{
	switch (eForm)
	{
		default:
			assert(0);
		case GeomForm_Vertices:
			return 8.f;
		case GeomForm_Edges:
			return (vSize.x + vSize.y + vSize.z) * 4.f;
		case GeomForm_Surface:
			return (vSize.x*vSize.y + vSize.x*vSize.z + vSize.y*vSize.z) * 2.f;
		case GeomForm_Volume:
			return vSize.x*vSize.y*vSize.z;
	}
}

// Utility functions.

template<class T> inline
const typename T::value_type& RandomElem(const T& array)
{
	int n = Random(int(array.size()));
	return array[n];
}

inline void Transform(RandomPos& ran, Matrix34 const& mx)
{
	ran.vPos = mx * ran.vPos;
	ran.vNorm = Matrix33(mx) * ran.vNorm;
}

inline void Transform(RandomPos& ran, QuatTS const& qts)
{
	ran.vPos = qts * ran.vPos;
	ran.vNorm = qts.q * ran.vNorm;
}

// Geometric primitive randomizing functions.

inline void BoxRandomPos(RandomPos& ran, EGeomForm eForm, Vec3 const& vSize)
{
	ran.vPos = ran.vNorm = BiRandom(vSize);

	if (eForm != GeomForm_Volume)
	{
		// Generate a random corner, for collapsing random point.
		int nCorner = Random(8);
		ran.vNorm.x = (((nCorner&1)<<1)-1) * vSize.x;
		ran.vNorm.y = (((nCorner&2))   -1) * vSize.y;
		ran.vNorm.z = (((nCorner&4)>>1)-1) * vSize.z;

		if (eForm == GeomForm_Vertices)
		{
			ran.vPos = ran.vNorm;
		}
		else if (eForm == GeomForm_Surface)
		{
			// Collapse one axis.
			float fAxis = Random(vSize.x*vSize.y + vSize.y*vSize.z + vSize.z*vSize.x);
			if ((fAxis -= vSize.y*vSize.z) < 0.f)
			{
				ran.vPos.x = ran.vNorm.x;
				ran.vNorm.y = ran.vNorm.z = 0.f;
			}
			else if ((fAxis -= vSize.z*vSize.x) < 0.f)
			{
				ran.vPos.y = ran.vNorm.y;
				ran.vNorm.x = ran.vNorm.z = 0.f;
			}
			else
			{
				ran.vPos.z = ran.vNorm.z;
				ran.vNorm.x = ran.vNorm.y = 0.f;
			}
		}
		else if (eForm == GeomForm_Edges)
		{
			// Collapse 2 axes.
			float fAxis = Random(vSize.x + vSize.y + vSize.z);
			if ((fAxis -= vSize.x) < 0.f)
			{
				ran.vPos.y = ran.vNorm.y;
				ran.vPos.z = ran.vNorm.z;
				ran.vNorm.x = 0.f;
			}
			else if ((fAxis -= vSize.y) < 0.f)
			{
				ran.vPos.x = ran.vNorm.x;
				ran.vPos.z = ran.vNorm.z;
				ran.vNorm.y = 0.f;
			}
			else
			{
				ran.vPos.x = ran.vNorm.x;
				ran.vPos.y = ran.vNorm.y;
				ran.vNorm.z = 0.f;
			}
		}
	}

	ran.vNorm.Normalize();
}

inline float CircleExtent(EGeomForm eForm, float fRadius)
{
	switch (eForm)
	{
		case GeomForm_Edges:
			return gf_PI2 * fRadius;
		case GeomForm_Surface:
			return gf_PI*square(fRadius);
		default:
			return 0.f;
	}
}

inline Vec2 CircleRandomPoint(EGeomForm eForm, float fRadius)
{
	Vec2 vPt;
	switch (eForm)
	{
		case GeomForm_Edges:
			// Generate random angle.
		//	sincos_tpl(Random(gf_PI2), &vPt.x);
			sincos_tpl(Random(gf_PI2), &vPt.y,&vPt.x);
			vPt *= fRadius;
			break;
		case GeomForm_Surface:
			// Generate random angle, and radius, adjusted for even distribution.
		//	sincos_tpl(Random(gf_PI2), &vPt.x);
			sincos_tpl(Random(gf_PI2), &vPt.y,&vPt.x);
			vPt *= sqrt(Random(1.f)) * fRadius;
			break;
		default:
			vPt.x = vPt.y = 0.f;
	}
	return vPt;
}

inline float TriExtent(EGeomForm eForm, Vec3 const& v0, Vec3 const& v1, Vec3 const& v2)
{
	switch (eForm)
	{
		default:
			assert(0);
		case GeomForm_Edges:
			return (v1-v0).GetLength() + (v2-v1).GetLength() + (v0-v2).GetLength();
		case GeomForm_Surface:
			return ((v1-v0) % (v2-v0)).GetLength() * 0.5f;
		case GeomForm_Volume:
			return ((vector2df(v1)-vector2df(v0)) ^ ((vector2df(v2)-vector2df(v0))))
					* (v0.z+v1.z+v2.z) * 1.f/6.f;
	}
}

inline void TriRandomPoint(float t[3], EGeomForm eForm)
{
	// Generate interpolators for verts.
	switch (eForm)
	{
		case GeomForm_Vertices:
		{
			int e = Random(3);
			t[e] = 1.f;
			t[(e+1)%3] = t[(e+2)%3] = 0.f;
			break;
		}
		case GeomForm_Edges:
		{
			// Approx: give each edge equal chance.
			int e = Random(3);
			t[e] = Random(1.f);
			t[(e+1)%3] = 1.f - t[e];
			t[(e+2)%3] = 0.f;
			break;
		}
		case GeomForm_Surface:
		case GeomForm_Volume:			// Volume generation currently not supported.
		{
			float r0 = Random(1.f), r1 = Random(1.f);
			if (r0 > r1)
				std::swap(r0, r1);
			t[0] = r0;
			t[1] = r1-r0;
			t[2] = 1.f-r1;
			break;
		}
	}
}

#endif // GEOM_QUERY_H
