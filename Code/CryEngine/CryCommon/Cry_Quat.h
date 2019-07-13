//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:Cry_Quat.h
//	Description: Common quaternion class
//
//	History
//	-Feb 27,2003: Created by Ivo Herzeg
//
//////////////////////////////////////////////////////////////////////

#ifndef _CRYQUAT_H
#define _CRYQUAT_H

//----------------------------------------------------------------------
// Quaternion
//----------------------------------------------------------------------

template <typename F> struct Quat_tpl
{

	Vec3_tpl<F> v;	F w;

	//-------------------------------
	//	ILINE Quat_tpl() : w(1),v(0,0,0) {}

	//constructors
#ifdef _DEBUG
	ILINE Quat_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&v.x; 		p[0]=F32NAN;	p[1]=F32NAN;	p[2]=F32NAN;	p[3]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&v.x;		p[0]=F64NAN;	p[1]=F64NAN;	p[2]=F64NAN;	p[3]=F64NAN;
		}
	}
#else
//	ILINE Quat_tpl() : w(1),v(0,0,0) {}
	ILINE Quat_tpl() {}
#endif



	ILINE Quat_tpl( F W,F X,F Y,F Z ) : w(W),v(X,Y,Z)	{}
	ILINE Quat_tpl( F angle, const Vec3_tpl<F> &axis) : w(angle),v(axis) {};
	ILINE Quat_tpl(type_identity) : w(1),v(0,0,0) {}


















	explicit ILINE Quat_tpl( const Ang3_tpl<F>& rad ) {
		assert(rad.IsValid());
		SetRotationXYZ(rad);
	}

	explicit Quat_tpl(const Matrix33_tpl<F>& m);
	explicit Quat_tpl(const Matrix34_tpl<F>& m);
	explicit Quat_tpl(const Matrix44_tpl<F>& m);

	//CONSTRUCTOR: implement the copy/casting/assignment constructor:	
	template <typename F1> ILINE Quat_tpl( const Quat_tpl<F1>& q ) {	w=q.w;  v.x=q.v.x; v.y=q.v.y;	v.z=q.v.z; }

	//multiplication by a scalar
	void operator *= (F op) {	w*=op; v*=op;	}

	// Exact compare of 2 quats.
	ILINE bool operator==(const Quat_tpl<F> &q) { return IsEquivalent(q,0.00001f); }
	ILINE bool operator!=(const Quat_tpl<F> &q) { return !(*this == q); }

	//flip quaternion. don't confuse this with quaternion-inversion.
	Quat_tpl<F>	operator - () const { return Quat_tpl<F>( -w,-v ); };

	Quat_tpl<F> operator ! () const;

	// Description:
	//    Check if identity quaternion.
	bool IsIdentity() const { return w == 1 && v.x == 0 && v.y == 0 && v.z == 0; }

	void	SetIdentity(void);
	static Quat_tpl<F> CreateIdentity(void);

	ILINE bool IsUnit(F e = VEC_EPSILON) const
	{
		return fabs_tpl( 1 - ((*this) | (*this)) ) < e;
	}

	ILINE bool IsValid(F e = VEC_EPSILON) const
	{
		if (!v.IsValid())	return false;
		if (!NumberValid(w)) return false;
		if (!IsUnit(e))	return false;
		return true;
	}


	void SetRotationAA(F rad, const Vec3_tpl<F> &axis);
	static Quat_tpl<F> CreateRotationAA(F rad, const Vec3_tpl<F> &axis);

	void SetRotationAA(F cosha, F sinha, const Vec3_tpl<F> &axis);
	static Quat_tpl<F> CreateRotationAA(F cosha, F sinha, const Vec3_tpl<F> &axis);


	void SetRotationXYZ(const Ang3 &a);
	static Quat_tpl<F> CreateRotationXYZ(const Ang3 &a);

	void SetRotationX( f32 r );
	static Quat_tpl<F> CreateRotationX( f32 r );
	void SetRotationY( f32 r );
	static Quat_tpl<F> CreateRotationY( f32 r );
	void SetRotationZ( f32 r );
	static Quat_tpl<F> CreateRotationZ( f32 r );

	void SetRotationV0V1(const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1);
	static Quat_tpl<F> CreateRotationV0V1(const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1);


	void SetRotationVDir( const Vec3_tpl<F>& vdir );
	static Quat_tpl<F> CreateRotationVDir( const Vec3_tpl<F>& vdir); 
	void SetRotationVDir( const Vec3_tpl<F>& vdir, F roll );
	static Quat_tpl<F> CreateRotationVDir( const Vec3_tpl<F>& vdir, F roll ); 

	//A quaternion is a compressed matrix. Thus there is no problem extracting the rows & columns. 
	Vec3_tpl<F> GetColumn0() const {return Vec3_tpl<F>(2*(v.x*v.x+w*w)-1,2*(v.y*v.x+v.z*w),2*(v.z*v.x-v.y*w));}
	Vec3_tpl<F> GetColumn1() const {return Vec3_tpl<F>(2*(v.x*v.y-v.z*w),2*(v.y*v.y+w*w)-1,2*(v.z*v.y+v.x*w));}
	Vec3_tpl<F> GetColumn2() const {return Vec3_tpl<F>(2*(v.x*v.z+v.y*w),2*(v.y*v.z-v.x*w),2*(v.z*v.z+w*w)-1);}
	Vec3_tpl<F> GetRow0() const {return Vec3_tpl<F>(2*(v.x*v.x+w*w)-1,2*(v.x*v.y-v.z*w),2*(v.x*v.z+v.y*w));}
	Vec3_tpl<F> GetRow1() const {return Vec3_tpl<F>(2*(v.y*v.x+v.z*w),2*(v.y*v.y+w*w)-1,2*(v.y*v.z-v.x*w));}
	Vec3_tpl<F> GetRow2() const	{return Vec3_tpl<F>(2*(v.z*v.x-v.y*w),2*(v.z*v.y+v.x*w),2*(v.z*v.z+w*w)-1);}

	// These are just copy & pasted components of the GetColumn1() above.
	ILINE F GetFwdX() const { return (2*(v.x*v.y-v.z*w)); }
	ILINE F GetFwdY() const { return (2*(v.y*v.y+w*w)-1); }
	ILINE F GetFwdZ() const { return (2*(v.z*v.y+v.x*w)); }
	ILINE F GetRotZ() const { return atan2_tpl(-GetFwdX(), GetFwdY()); }

	void Invert( void );
	Quat_tpl<F> GetInverted() const;

	void	Normalize(void);
	Quat_tpl<F> GetNormalized() const;

	void	NormalizeFast(void);
	Quat_tpl<F> GetNormalizedFast() const;

	F GetLength() const;

	ILINE bool IsEquivalent( const Quat_tpl<F>& q, F e=VEC_EPSILON) const {
		Quat_tpl<F> p=-q;
		bool t0= (fabs_tpl(v.x-q.v.x)<=e) && (fabs_tpl(v.y-q.v.y)<=e) && (fabs_tpl(v.z-q.v.z)<=e) && (fabs_tpl(w-q.w)<=e);	
		bool t1= (fabs_tpl(v.x-p.v.x)<=e) && (fabs_tpl(v.y-p.v.y)<=e) && (fabs_tpl(v.z-p.v.z)<=e) && (fabs_tpl(w-p.w)<=e);	
		t0 |= t1;
		return t0;
	}

	void SetNlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t  ); 
	static Quat_tpl<F> CreateNlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ); 

	void SetNlerp2( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t  ); 
	static Quat_tpl<F> CreateNlerp2( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ); 

	void SetSlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ); 
	static Quat_tpl<F> CreateSlerp( const Quat_tpl<F>& p, const Quat_tpl<F>& tq, F t );

	void SetExpSlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ); 
	static Quat_tpl<F> CreateExpSlerp( const Quat_tpl<F>& p, const Quat_tpl<F>& tq, F t );

	static Quat_tpl<F> CreateSquad( const Quat_tpl<F>& p,const Quat_tpl<F>& a,const Quat_tpl<F>& b,const Quat_tpl<F>& q, F t );
	static Quat_tpl<F> CreateSquadRev( F angle, const Vec3& axis, const Quat_tpl<F>& p, const Quat_tpl<F>& a, const Quat_tpl<F>& b, const Quat_tpl<F>& q, F t );

	Quat_tpl<F> GetScaled(F scale) const
	{
		return CreateNlerp(IDENTITY, *this, scale);
/*
		AngleAxis_tpl<F> aa = *this;
		aa.angle *= scale;
		return CreateRotationAA(aa.angle, aa.axis);
*/
	}

	void ClampAngle(F maxAngleDeg)
	{
		F maxAngleRad = DEG2RAD(maxAngleDeg);
		F wMax = cry_cosf((F)(2.0) * maxAngleRad);
		if (w < wMax)
		{
			w = wMax;
			Normalize();
		}
	}

	AUTO_STRUCT_INFO
};


///////////////////////////////////////////////////////////////////////////////
// Typedefs                                                                  // 
///////////////////////////////////////////////////////////////////////////////

#ifndef MAX_API_NUM
typedef Quat_tpl<f32>	Quat;
typedef Quat_tpl<f64>	Quatr;
typedef Quat_tpl<real>	Quat_f64;
#endif

typedef Quat_tpl<f32>	CryQuat;
typedef Quat_tpl<f32> quaternionf;
typedef Quat_tpl<real>	quaternion;




//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------








/*!
*
* The "inner product" or "dot product" operation.
*
* calculate the "inner product" between two Quaternion.
* If both Quaternion are unit-quaternions, the result is the cosine: p*q=cos(angle)
* 
* Example:
*  Quat p(1,0,0,0),q(1,0,0,0);
*  f32 cosine = ( p | q );
*
*/
template<typename F1,typename F2> ILINE F1 operator | (const Quat_tpl<F1>& q, const Quat_tpl<F2>& p) 
{
	assert(q.v.IsValid());
	assert(p.v.IsValid());
	return (q.v.x*p.v.x + q.v.y*p.v.y + q.v.z*p.v.z + q.w*p.w); 
} 


/*!
*
*  Implements the multiplication operator: Qua=QuatA*QuatB
*
*  AxB = operation B followed by operation A.  
*  A multiplication takes 16 muls and 12 adds. 
*
*  Example 1:
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat result=p*q;
*
*  Example 2: (self-multiplication)
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat p*=q;
*/
template<class F1,class F2> ILINE Quat_tpl<F1> operator * (const Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	assert(q.IsValid());
	assert(p.IsValid());
	return Quat_tpl<F1>
		( 
		q.w*p.w  - (q.v.x*p.v.x + q.v.y*p.v.y + q.v.z*p.v.z),
		q.v.y*p.v.z-q.v.z*p.v.y + q.w*p.v.x + q.v.x*p.w,
		q.v.z*p.v.x-q.v.x*p.v.z + q.w*p.v.y + q.v.y*p.w,
		q.v.x*p.v.y-q.v.y*p.v.x + q.w*p.v.z + q.v.z*p.w  
		);
}
template<class F1,class F2> ILINE void operator *= (Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	assert(q.IsValid());
	assert(p.IsValid());
	F1 s0=q.w; q.w=q.w*p.w-(q.v|p.v); q.v=p.v*s0+q.v*p.w+(q.v%p.v); 
}

/*!
*  Implements the multiplication operator: QuatT=Quat*Quatpos
*
*  AxB = operation B followed by operation A.  
*  A multiplication takes 31 muls and 27 adds (=58 float operations). 
*
*  Example:
*   Quat    quat		=	Quat::CreateRotationX(1.94192f);;
*   QuatT quatpos	=	QuatT::CreateRotationZ(3.14192f,Vec3(11,22,33));
*	  QuatT qp			=	quat*quatpos;
*/
template<class F1,class F2> ILINE QuatT_tpl<F1> operator * (const Quat_tpl<F1> &q, const QuatT_tpl<F2> &p) { 
	return QuatT_tpl<F1>(q*p.q, q*p.t);
}



/*!
*  division operator
*
*  Example 1:
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat result=p/q;
*
*  Example 2: (self-division)
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat p/=q;
*/
template<class F1,class F2> ILINE Quat_tpl<F1> operator / (const Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	return (!p*q); 
}
template<class F1,class F2> ILINE void operator /= (Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	q=(!p*q);
}


/*!
*  addition operator
*
*  Example:
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat result=p+q;
*
*  Example:(self-addition operator)
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat p-=q;
*/
template<class F1,class F2> ILINE Quat_tpl<F1> operator + (const Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	return Quat_tpl<F1>( q.w+p.w, q.v+p.v ); 
}
template<class F1,class F2> ILINE void operator += (Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	q.w+=p.w; q.v+=p.v; 
}

template<class F1,class F2> ILINE Quat_tpl<F1> operator % (const Quat_tpl<F1> &q, const Quat_tpl<F2> &tp) { 
	Quat_tpl<F1> p=tp;
	if( (p|q) < 0 ) { p=-p;	} 
	return Quat_tpl<F1>( q.w+p.w, q.v+p.v ); 
}
template<class F1,class F2> ILINE void operator %= (Quat_tpl<F1> &q, const Quat_tpl<F2> &tp) { 
	Quat_tpl<F1> p=tp;
	if( (p|q) < 0 ) { p=-p;	} 
	q = Quat_tpl<F1>( q.w+p.w, q.v+p.v ); 
}


/*!
*  subtraction operator
*
*  Example:
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat result=p-q;
*
*  Example: (self-subtraction operator)
*   Quat p(1,0,0,0),q(1,0,0,0);
*   Quat p-=q;
*
*/
template<class F1,class F2> ILINE Quat_tpl<F1> operator - (const Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	return Quat_tpl<F1>( q.w-p.w, q.v-p.v); 
}
template<class F1,class F2> ILINE void operator -= (Quat_tpl<F1> &q, const Quat_tpl<F2> &p) { 
	q.w-=p.w; q.v-=p.v; 
}


//! Scale quaternion free function.
template <typename F> ILINE Quat_tpl<F>	operator * ( F t, const Quat_tpl<F> &q ) {
	return Quat_tpl<F>( t*q.w, t*q.v );
};


template <typename F1,typename F2> ILINE Quat_tpl<F1>	operator * ( const Quat_tpl<F1> &q, F2 t ) {
	return Quat_tpl<F1>( q.w*t, q.v*t );
};
template <typename F1,typename F2> ILINE Quat_tpl<F1>	operator / ( const Quat_tpl<F1> &q, F2 t ) {
	return Quat_tpl<F1>( q.w/t, q.v/t );
};


/*!
*
* post-multiply of a quaternion and a Vec3 (3D rotations with quaternions)
* 
* Example:
*  Quat q(1,0,0,0);
*  Vec3 v(33,44,55);
*	 Vec3 result = q*v;
*/
template<class F,class F2> ILINE Vec3_tpl<F> operator * (const Quat_tpl<F> &q, const Vec3_tpl<F2> &v) {
	assert(v.IsValid());
	assert(q.IsValid());
	//muls=15 / adds=15
	Vec3_tpl<F> out,r2;
	r2.x=(q.v.y*v.z-q.v.z*v.y)+q.w*v.x;
	r2.y=(q.v.z*v.x-q.v.x*v.z)+q.w*v.y;
	r2.z=(q.v.x*v.y-q.v.y*v.x)+q.w*v.z;
	out.x=(r2.z*q.v.y-r2.y*q.v.z); out.x+=out.x+v.x;
	out.y=(r2.x*q.v.z-r2.z*q.v.x); out.y+=out.y+v.y;
	out.z=(r2.y*q.v.x-r2.x*q.v.y); out.z+=out.z+v.z;
	return out;
}


/*!
* pre-multiply of a quaternion and a Vec3 (3D rotations with quaternions)
* 
* Example:
*  Quat q(1,0,0,0);
*  Vec3 v(33,44,55);
*	 Vec3 result = v*q;
*/
template<class F,class F2> ILINE Vec3_tpl<F2> operator * (const Vec3_tpl<F> &v, const Quat_tpl<F2> &q) {
	assert(v.IsValid());
	assert(q.IsValid());
	//muls=15 / adds=15
	Vec3_tpl<F> out,r2;
	r2.x=(q.v.z*v.y-q.v.y*v.z)+q.w*v.x;
	r2.y=(q.v.x*v.z-q.v.z*v.x)+q.w*v.y;
	r2.z=(q.v.y*v.x-q.v.x*v.y)+q.w*v.z;
	out.x=(r2.y*q.v.z-r2.z*q.v.y); out.x+=out.x+v.x;
	out.y=(r2.z*q.v.x-r2.x*q.v.z); out.y+=out.y+v.y;
	out.z=(r2.x*q.v.y-r2.y*q.v.x); out.z+=out.z+v.z;
	return out;
}








/*!
* invert quaternion.
* 
* Example 1:
*  Quat q=Quat::CreateRotationXYZ(Ang3(1,2,3));
*  Quat result = !q;
*  Quat result = GetInverted(q);
*  q.Invert();
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::operator ! () const { return Quat_tpl(w,-v); }
template<typename F> ILINE void Quat_tpl<F>::Invert( void ) { *this=!*this;	}
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::GetInverted() const { return !(*this); } 



/*!
* set identity quaternion
* 
* Example:
*		Quat q=Quat::CreateIdentity();
*   or
*		q.SetIdentity();
*   or
*		Quat p=Quat(IDENTITY);
*/
template <typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateIdentity(void)	{ return Quat_tpl(1,0,0,0); }
template <typename F> ILINE void	Quat_tpl<F>::SetIdentity(void) { w=1; v.x=0; v.y=0; v.z=0; }


template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationAA(F rad, const Vec3_tpl<F> &axis) { 	
	Quat_tpl<F> q;	q.SetRotationAA(rad,axis); 	return q;	
}
template<typename F> ILINE void Quat_tpl<F>::SetRotationAA(F rad, const Vec3_tpl<F> &axis) { 
	F s,c; sincos_tpl( rad*(F)0.5, &s,&c);	SetRotationAA(c,s, axis); 
}

template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationAA(F cosha, F sinha, const Vec3_tpl<F> &axis) { 
	Quat_tpl<F> q;	q.SetRotationAA(cosha,sinha,axis); 	return q;	
}
template<typename F> ILINE void Quat_tpl<F>::SetRotationAA(F cosha, F sinha, const Vec3_tpl<F> &axis)	{ 
	assert(axis.IsUnit(0.001f));
	w=cosha; v=axis*sinha; 
}

/*!
* Create rotation-quaternion that around the fixed coordinate axes.
*
* Example:
*		Quat q=Quat::CreateRotationXYZ( Ang3(1,2,3) );
*   or
*		q.SetRotationXYZ( Ang3(1,2,3) );
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationXYZ(const Ang3 &a) 
{	
	assert(a.IsValid());
	Quat_tpl<F> q;	q.SetRotationXYZ(a); 	return q;	
}
template<typename F> ILINE void Quat_tpl<F>::SetRotationXYZ(const Ang3 &a)	
{ 
	assert(a.IsValid());
	F sx,cx;  sincos_tpl(F(a.x*0.5),&sx,&cx);
	F sy,cy;  sincos_tpl(F(a.y*0.5),&sy,&cy);
	F sz,cz;  sincos_tpl(F(a.z*0.5),&sz,&cz);
	w		= cx*cy*cz + sx*sy*sz;
	v.x = cz*cy*sx - sz*sy*cx;
	v.y = cz*sy*cx + sz*cy*sx;
	v.z = sz*cy*cx - cz*sy*sx;
}

/*!
* Create rotation-quaternion that about the x-axis.
*
* Example:
*		Quat q=Quat::CreateRotationX( radiant );
*   or
*		q.SetRotationX( Ang3(1,2,3) );
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationX( f32 r ) 
{	
	Quat_tpl<F> q;	q.SetRotationX(r); 	return q;	
}
template<typename F> ILINE void Quat_tpl<F>::SetRotationX( f32 r ) 
{ 
	F s,c; 
	sincos_tpl(F(r*0.5),&s,&c); w=c; v.x=s; v.y=0; v.z=0;	
}


/*!
* Create rotation-quaternion that about the y-axis.
*
* Example:
*		Quat q=Quat::CreateRotationY( radiant );
*   or
*		q.SetRotationY( radiant );
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationY( f32 r ) 
{	
	Quat_tpl<F> q;	q.SetRotationY(r); 	return q;	
}
template<typename F> ILINE void Quat_tpl<F>::SetRotationY( f32 r ) 
{ 
	F s,c; sincos_tpl(F(r*0.5),&s,&c); w=c; v.x=0; v.y=s; v.z=0;	
}


/*!
* Create rotation-quaternion that about the z-axis.
*
* Example:
*		Quat q=Quat::CreateRotationZ( radiant );
*   or
*		q.SetRotationZ( radiant );
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationZ( f32 r ) 
{	
	Quat_tpl<F> q;	q.SetRotationZ(r); 	return q;	
}
template<typename F> ILINE void Quat_tpl<F>::SetRotationZ( f32 r ) 
{ 
	F s,c; sincos_tpl(F(r*0.5),&s,&c); w=c; v.x=0; v.y=0; v.z=s;	
}



/*!
*
* Create rotation-quaternion that rotates from one vector to another.
* Both vectors are assumed to be nomalised.
*
* Example:
*		Quat q=Quat::CreateRotationV0V1( v0,v1 );
*		q.SetRotationV0V1( v0,v1 );
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationV0V1(const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1) {	Quat_tpl<F> q;	q.SetRotationV0V1(v0,v1); 	return q;	}
template<typename F> void Quat_tpl<F>::SetRotationV0V1(const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1)	
{ 
	assert(v0.IsUnit(0.01f));
	assert(v1.IsUnit(0.01f));
	f64 dot = v0.x*v1.x+v0.y*v1.y+v0.z*v1.z+1.0;
	if (dot > 0.0001) 
	{
		f64 vx=v0.y*v1.z-v0.z*v1.y; 
		f64 vy=v0.z*v1.x-v0.x*v1.z; 
		f64 vz=v0.x*v1.y-v0.y*v1.x; 
		f64 d = 1.0/sqrt(dot*dot + vx*vx+vy*vy+vz*vz);
		w=F(dot*d);	v.x=F(vx*d); v.y=F(vy*d); v.z=F(vz*d);
		return;
	}
	w=0; v=v0.GetOrthogonal().GetNormalized();
}


/*!
*
* \param vdir  normalized view direction.
* \param roll  radiant to rotate about Y-axis.
*
*  Given a view-direction and a radiant to rotate about Y-axis, this function builds a 3x3 look-at quaternion 
*  using only simple vector arithmetic. This function is always using the implicit up-vector Vec3(0,0,1). 
*  The view-direction is always stored in column(1).
*  IMPORTANT: The view-vector is assumed to be normalized, because all trig-values for the orientation are beeing 
*  extracted  directly out of the vector. This function must NOT be called with a view-direction 
*  that is close to Vec3(0,0,1) or Vec3(0,0,-1). If one of these rules is broken, the function returns a quaternion 
*  with an undifined rotation about the Z-axis.
*
*  Rotation order for the look-at-quaternion is Z-X-Y. (Zaxis=YAW / Xaxis=PITCH / Yaxis=ROLL)
*
*  COORDINATE-SYSTEM       
*                                   
*  z-axis                                 
*    ^                               
*    |                               
*    |  y-axis                   
*    |  /                         
*    | /                           
*    |/                             
*    +--------------->   x-axis     
*                                  
*  Example:
*		Quat LookAtQuat=Quat::CreateRotationVDir( Vec3(0,1,0) );
*   or
*		Quat LookAtQuat=Quat::CreateRotationVDir( Vec3(0,1,0), 0.333f );
*/
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationVDir( const Vec3_tpl<F>& vdir ) {	Quat_tpl<F> q;	q.SetRotationVDir(vdir); return q;	}
template<typename F> ILINE void Quat_tpl<F>::SetRotationVDir( const Vec3_tpl<F>& vdir )
{
	assert(vdir.IsUnit(0.01f));
	//set default initialisation for up-vector	
	w=F(0.70710676908493042);	v.x=F((vdir.z+vdir.z)*0.35355338454246521);	v.y=F(0.0); 	v.z=F(0.0); 
	f64 l = sqrt(vdir.x*vdir.x+vdir.y*vdir.y);
	if (l>0.00001)	
	{
		//calculate LookAt quaternion
		Vec3r hv	=	Vec3r(vdir.x/l,vdir.y/l+1.0,l+1.0);
		f64 r = sqrt(hv.x*hv.x + hv.y*hv.y);
		f64 s	= sqrt(hv.z*hv.z + vdir.z*vdir.z);
		//generate the half-angle sine&cosine
		f64 hacos0=0.0;			f64 hasin0=-1.0;			
		if (r>0.00001) { hacos0=hv.y/r; hasin0=-hv.x/r; }	//yaw
		f64 hacos1=hv.z/s;	f64 hasin1=vdir.z/s;					//pitch
		w=F(hacos0*hacos1); v.x=F(hacos0*hasin1);	v.y=F(hasin0*hasin1);	v.z=F(hasin0*hacos1);  
	}
}

template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateRotationVDir( const Vec3_tpl<F>& vdir, F roll ) {	Quat_tpl<F> q; q.SetRotationVDir(vdir,roll);	return q;	}
template<typename F> ILINE void Quat_tpl<F>::SetRotationVDir( const Vec3_tpl<F>& vdir, F r )
{
	SetRotationVDir(vdir);
	f64 sy,cy;  sincos_tpl(r*0.5,&sy,&cy);
	f64 vx=v.x,vy=v.y;  
	v.x=F(vx*cy-v.z*sy); v.y=F(w*sy+vy*cy); v.z=F(v.z*cy+vx*sy); w=F(w*cy-vy*sy);
}



/*!
* normalize quaternion.
* 
* Example 1:
*  Quat q;
*  q.Normalize();
*
* Example 2:
*  Quat q=Quat(1,2,3,4);
*  Quat qn=q.GetNormalized();
*/
template <typename F> ILINE void	Quat_tpl<F>::Normalize(void)	
{
	F d = sqrt_tpl(w*w + v.x*v.x+v.y*v.y+v.z*v.z);
	assert(d>0.00001f); 
	d=(F)1/d;		
	w*=d;	v.x*=d; v.y*=d; v.z*=d;
}
template <typename F> ILINE Quat_tpl<F> Quat_tpl<F>::GetNormalized() const {  Quat_tpl<F> t=*this; t.Normalize(); return t;	}


/*!
* get length of quaternion.
* 
* Example 1:
*  f32 l=q.GetLength();
*/
template <typename F> ILINE F Quat_tpl<F>::GetLength() const 
{ 
	return sqrt_tpl(w*w + v.x*v.x+v.y*v.y+v.z*v.z); 
}





// Exponent of Quaternion.
template <typename F> ILINE Quat_tpl<F> exp(const Vec3_tpl<F>& v) 
{
	f64 length = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	f64 s,c; sincos_tpl(length,&s,&c); 
	if (length>0)	
		return Quat_tpl<F>( F(c),F(v.x*s/length),F(v.y*s/length),F(v.z*s/length));
	return Quat_tpl<F> (IDENTITY);
}

// logarithm of a quaternion, imaginary part (the real part of the logarithm is always 0)
template <typename F> ILINE Vec3_tpl<F> log (const Quat_tpl<F>& q) 
{
	assert(q.IsValid());
	f64 length	= sqrt(q.v.x*q.v.x+q.v.y*q.v.y+q.v.z*q.v.z);
	f64 angle		= atan2(length,(f64)q.w);
	if (length>0)	
		return Vec3_tpl<F>( F(q.v.x*angle/length),F(q.v.y*angle/length), F(q.v.z*angle/length) );
	return Vec3_tpl<F>(0,0,0);
}




/*!
* linear-interpolation between quaternions (lerp)
* 
* Example:
*  CQuaternion result,p,q;
*  result=qlerp( p, q, 0.5f );
*/
template<typename F> ILINE void Quat_tpl<F>::SetNlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ) 
{	
	Quat_tpl<F> q=tq;
	assert(p.IsValid());
	assert(q.IsValid());
	if( (p|q) < 0 ) { q=-q;	} 
	v.x = p.v.x*(1.0f-t) + q.v.x*t;
	v.y = p.v.y*(1.0f-t) + q.v.y*t;
	v.z = p.v.z*(1.0f-t) + q.v.z*t;
	w		= p.w  *(1.0f-t) + q.w*t;
	Normalize();
}

template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateNlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ) 
{
	Quat_tpl<F> d;  d.SetNlerp(p,tq,t); 	return d;
}



template<typename F> ILINE void Quat_tpl<F>::SetNlerp2( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ) 
{
	Quat_tpl<F> q=tq;
	assert(p.IsValid());
	assert(tq.IsValid());
	F cosine=(p|q);
	if(cosine<0) q=-q; 
	F k=(1-fabs_tpl(cosine))*0.4669269f;
	F s = 2*k*t*t*t - 3*k*t*t + (1+k)*t;
	v.x = p.v.x*(1.0f-s) + q.v.x*s;
	v.y = p.v.y*(1.0f-s) + q.v.y*s;
	v.z = p.v.z*(1.0f-s) + q.v.z*s;
	w		= p.w  *(1.0f-s) + q.w*s;
	Normalize();
}
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateNlerp2( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ) 
{
	Quat_tpl<F> d;  d.SetNlerp2(p,tq,t); 	return d;
}



/*!
* spherical-interpolation between quaternions (geometrical slerp)
* 
* Example:
*  Quat result,p,q;
*  result.SetSlerp( p, q, 0.5f );
*/
template<typename F> ILINE void Quat_tpl<F>::SetSlerp( const Quat_tpl<F> &tp, const Quat_tpl<F> &tq, F t ) 
{
	assert(tp.IsValid());
	assert(tq.IsUnit());
	Quat_tpl<F> p=tp,q=tq;
	Quat_tpl<double> q2;

	f64 cosine = (p|q);
	if (cosine < 0 ) { cosine=-cosine; q=-q;	} //take shortest arc
  if (cosine > 0.9999)
  {
    SetNlerp(p,q,t);
    return;
  }
	// from now on, a division by 0 is not possible any more
	q2.w		= q.w-p.w*cosine;
	q2.v.x	= q.v.x-p.v.x*cosine;
	q2.v.y	= q.v.y-p.v.y*cosine;
	q2.v.z	= q.v.z-p.v.z*cosine;
	f64 sine	= sqrt(q2|q2);
	f64 s,c;	sincos_tpl(atan2(sine,cosine)*t,&s,&c); 
	w =   F(p.w  *c + q2.w  *s/sine);
	v.x = F(p.v.x*c + q2.v.x*s/sine);
	v.y = F(p.v.y*c + q2.v.y*s/sine);
	v.z = F(p.v.z*c + q2.v.z*s/sine);
}

template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateSlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ) 
{
	Quat_tpl<F> d;  d.SetSlerp(p,tq,t); 	return d;
}





/*!
* spherical-interpolation between quaternions (algebraic slerp_a)
* I have included this function just for the sake of completeness, because 
* its the only useful application to check if exp & log really work. 
* Both slerp-functions are returning the same result.
*	
* Example:
*  Quat result,p,q;
*  result.SetExpSlerp( p,q,0.3345f );
*/
template <typename F> ILINE void Quat_tpl<F>::SetExpSlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &tq, F t ) 
{
	assert(p.IsValid());
	assert(tq.IsValid());
	Quat_tpl<F> q=tq;
	if((p|q)<0) { q=-q;	} 
	*this = p * exp( log(!p*q)*t );			//algebraic slerp (1)

	//...and some more exp-slerp-functions producing all the same result
	//*this = exp( log (p* !q) * (1-t)) * q;		    //algebraic slerp (2)
	//*this = exp( log (q* !p) * t) * p;			//algebraic slerp (3)
	//*this = q * exp( log (!q*p) * (1-t));	//algebraic slerp (4)
}
template<typename F> ILINE Quat_tpl<F> Quat_tpl<F>::CreateExpSlerp( const Quat_tpl<F> &p, const Quat_tpl<F> &q, F t ) 
{
	Quat_tpl<F> d;  d.SetExpSlerp(p,q,t); 	return d;
}









//! squad(p,a,b,q,t) = slerp( slerp(p,q,t),slerp(a,b,t), 2(1-t)t).
template <typename F>
ILINE Quat_tpl<F> Quat_tpl<F>::CreateSquad( const Quat_tpl<F>& p,const Quat_tpl<F>& a,const Quat_tpl<F>& b,const Quat_tpl<F>& q, F t )
{
	F k = 2.0f*(1.0f-t)*t;
	return CreateSlerp( CreateSlerp(p,q,t), CreateSlerp(a,b,t), k );
}




//! Quaternion interpolation for angles > 2PI.
template <typename F>
ILINE Quat_tpl<F> Quat_tpl<F>::CreateSquadRev(	F angle,				// angle of rotation 
																							const Vec3& axis,	// the axis of rotation 
																							const Quat_tpl<F>& p,		// start quaternion 
																							const Quat_tpl<F>& a, 	// start tangent quaternion 
																							const Quat_tpl<F>& b, 	// end tangent quaternion 
																							const Quat_tpl<F>& q,		// end quaternion 
																							F t	)						// Time parameter, in range [0,1]
{
	F s,v;
	F omega = 0.5f*angle;
	F nrevs = 0;
	Quat_tpl<F> r,pp,qq;

	if (omega < (gf_PI - 0.00001f)) { 
		return CreateSquad( p,a,b,q,t );
	}

	while (omega > (gf_PI - 0.00001f)) { 
		omega -= gf_PI;
		nrevs += 1.0f;
	}
	if (omega < 0) omega = 0;
	s = t*angle/gf_PI;		// 2t(omega+Npi)/pi

	if (s < 1.0f) {
		pp = p*Quat_tpl<F>(0.0f,axis);//pp = p.Orthog( axis );
		r = CreateSquad(p,a,pp,pp,s);	// in first 90 degrees.
	}	else {
		v = s + 1.0f - 2.0f*(nrevs+(omega/gf_PI));
		if (v <= 0.0f)  {
			// middle part, on great circle(p,q).
			while (s >= 2.0f) s -= 2.0f;
			pp = p*Quat_tpl<F>(0.0f,axis);//pp = p.Orthog(axis);
			r = CreateSlerp(p,pp,s);
		}	else {
			// in last 90 degrees.
			qq = -q*Quat_tpl<F>(0.0f,axis);
			r = CreateSquad(qq,qq,b,q,v);
		}
	}
	return r;
}








//////////////////////////////////////////////////////////////////////////
//! Logarithm of Quaternion difference.
template <typename F>
ILINE Quat_tpl<F> LnDif( const Quat_tpl<F> &q1,const Quat_tpl<F> &q2 ){
	return Quat(0,log(q2/q1));
}





/*!
*
* quaternion copy constructor; Quat q=mat33
* We take only the rotation of the 3x3 part 
* 
* Example 1:
* \code
*  Matrix33 mat33;
*  mat33.rotationXYZ33(0.5f, 2.5f, 1.5f);
*  Quat q=mat33;
* \endcode
* 
* Example 2:
* \code
*  CMatrix34 mat34;
*  mat34.rotationXYZ34(0.5f, 2.5f, 1.5f);
*  Quat q=Matrix33(mat34);
* \endcode
*/
template<typename F> ILINE Quat_tpl<F> GetQuatFromMat33(const Matrix33_tpl<F>& m)	{ 
	//check if we have an orthonormal-base (assuming we are using a right-handed coordinate system)
	//assert removed by ivo: it was impossible to load a level because of this assert!
	//but be warned, if you convert a non-uniform-scaled matrix into a quaternion 
	//you get a worthless quaternion!
#if !defined(PS3) && !defined(LINUX)
	assert( m.IsOrthonormalRH(0.1f) );
#endif
	F s,p,tr = m.m00 + m.m11 + m.m22;

	//check the diagonal
	if(tr > 0.0) {
		s=sqrt_tpl(tr+1.0f);	assert(s); p=0.5f/s;
		return Quat_tpl<F>( s*0.5f, (m.m21-m.m12)*p, (m.m02-m.m20)*p, (m.m10-m.m01)*p );
	}
	//diagonal is negative. now we have to find the biggest element on the diagonal
	//check if "m00" is the biggest element
	if	( (m.m00>=m.m11) && (m.m00>=m.m22) ) {
		s=sqrt_tpl(m.m00-m.m11-m.m22+1.0f);	assert(s);	p=0.5f/s;
		return Quat_tpl<F>( (m.m21-m.m12)*p, s*0.5f, (m.m10+m.m01)*p, (m.m20+m.m02)*p );
	}
	//check if "m11" is the biggest element
	if	( (m.m11>=m.m00) && (m.m11>=m.m22) ) {
		s=sqrt_tpl(m.m11-m.m22-m.m00+1.0f);	assert(s); p=0.5f/s;
		return Quat_tpl<F>( (m.m02-m.m20)*p, (m.m01+m.m10)*p, s*0.5f, (m.m21+m.m12)*p );
	}
	//check if "m22" is the biggest element
	if	( (m.m22>=m.m00) && (m.m22>=m.m11) ) {
		s=sqrt_tpl(m.m22-m.m00-m.m11+1.0f);	assert(s); p=0.5f/s;
		return Quat_tpl<F>( (m.m10-m.m01)*p, (m.m02+m.m20)*p, (m.m12+m.m21)*p, s*0.5f );
	}
	assert(0); return Quat(IDENTITY);//if it ends here, then we have no valid rotation matrix
}

template<class F> ILINE Quat_tpl<F>::Quat_tpl(const Matrix33_tpl<F>& m) { *this=GetQuatFromMat33(m); }
template<class F> ILINE Quat_tpl<F>::Quat_tpl(const Matrix34_tpl<F>& m) { *this=GetQuatFromMat33(Matrix33(m));	} 
template<class F> ILINE Quat_tpl<F>::Quat_tpl(const Matrix44_tpl<F>& m) { *this=GetQuatFromMat33(Matrix33_tpl<f32>(m));	} 

















//----------------------------------------------------------------------
// Quaternion with translation vector
//----------------------------------------------------------------------
template <typename F> struct QuatT_tpl
{
	Quat_tpl<F> q; //this is the quaternion  
	Vec3_tpl<F> t; //this is the translation vector and a scalar (for uniform scaling??)	

	//constructors
#ifdef _DEBUG
	ILINE QuatT_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&q.v.x; 		p[0]=F32NAN;p[1]=F32NAN;p[2]=F32NAN;p[3]=F32NAN; p[4]=F32NAN;p[5]=F32NAN;p[6]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&q.v.x;		p[0]=F64NAN;p[1]=F64NAN;p[2]=F64NAN;p[3]=F64NAN; p[4]=F64NAN;p[5]=F64NAN;p[6]=F64NAN;
		}
	}
#else
	ILINE QuatT_tpl()	{};
#endif


	ILINE QuatT_tpl(const Vec3_tpl<F>& _t, const Quat_tpl<F>& _q) { q = _q; t = _t; }
	ILINE QuatT_tpl(type_identity) { q.w = 1; q.v.x = 0; q.v.y = 0; q.v.z = 0; t.x=0; t.y=0; t.z=0; }

	//assignment operator is needed since default operator = has precedence over assignment constructor
	ILINE QuatT_tpl& operator = (const QuatT_tpl<F> &qt) 
	{ 
		q=qt.q; t=qt.t; 
		return *this;
	}
	//CONSTRUCTOR: implement the copy/casting/assignment constructor:	
	template <typename F1> ILINE QuatT_tpl( const QuatT_tpl<F1>& qt ) 
	{	
		q.w	=	qt.q.w; 
		q.v.x=qt.q.v.x; 
		q.v.y=qt.q.v.y; 
		q.v.z=qt.q.v.z; 
		t.x	=	qt.t.x; 
		t.y	=	qt.t.y;			
		t.z	=	qt.t.z; 
	}


	explicit ILINE QuatT_tpl(const QuatTS_tpl<F>& qts )	: q(qts.q), t(qts.t)
	{ 
//		q=qts.q; t=qts.t; 
	}

	explicit ILINE QuatT_tpl(	const Matrix34_tpl<F>& m)	
	{ 
		q=GetQuatFromMat33( Matrix33(m) );	t=m.GetTranslation();
	}

	ILINE QuatT_tpl(const Quat_tpl<F>& quat, const Vec3_tpl<F>& trans) 
	{
		q=quat;	t=trans; 
	}

	ILINE void SetIdentity() 
	{
		q.w=1; q.v.x=0; q.v.y=0; q.v.z=0;
		t.x = 0; t.y = 0; t.z = 0;
	}

	ILINE bool IsIdentity() const
	{
		return (q.IsIdentity() && t.IsZero());
	}

	/*!
	* Convert three Euler angle to mat33 (rotation order:XYZ)
	* The Euler angles are assumed to be in radians. 
	* The translation-vector is set to zero by default.  
	*
	*  Example 1:
	*		QuatT qp;
	*		qp.SetRotationXYZ( Ang3(0.5f,0.2f,0.9f), translation );
	*
	*  Example 2:
	*		QuatT qp=QuatT::CreateRotationXYZ( Ang3(0.5f,0.2f,0.9f), translation );
	*/
	ILINE void SetRotationXYZ( const Ang3_tpl<F>& rad, const Vec3_tpl<F>& trans=Vec3(ZERO) ) 
	{
		assert(rad.IsValid());
		assert(trans.IsValid());
		q.SetRotationXYZ(rad); t=trans;
	}
	ILINE static QuatT_tpl<F> CreateRotationXYZ( const Ang3_tpl<F>& rad, const Vec3_tpl<F>& trans=Vec3(ZERO) ) 
	{	
		assert(rad.IsValid());
		assert(trans.IsValid());
		QuatT_tpl<F> qp;	qp.SetRotationXYZ(rad,trans); return qp;	
	}



	ILINE void SetRotationAA(F cosha, F sinha, const Vec3_tpl<F> axis, const Vec3_tpl<F>& trans=Vec3(ZERO) )	
	{
		q.SetRotationAA(cosha,sinha,axis); 	
		t=trans; 	
	}
	ILINE static QuatT_tpl<F> CreateRotationAA( F cosha, F sinha, const Vec3_tpl<F> axis, const Vec3_tpl<F>& trans=Vec3(ZERO) ) 
	{	
		QuatT_tpl<F> qt;  qt.SetRotationAA(cosha,sinha,axis,trans);	return qt;	
	}


	ILINE void Invert() { // in-place transposition
		assert(q.IsValid());
		t=-t*q; q=!q; 	
	}
	ILINE QuatT_tpl<F> GetInverted() const {
		assert(q.IsValid());
		QuatT_tpl<F> qpos;
		qpos.q=!q; 	qpos.t=-t*q; 	
		return qpos;
	}

	ILINE void SetTranslation( const Vec3_tpl<F>& trans )	{	t=trans;	}

	ILINE Vec3_tpl<F> GetColumn0() const {return q.GetColumn0();}
	ILINE Vec3_tpl<F> GetColumn1() const {return q.GetColumn1();}
	ILINE Vec3_tpl<F> GetColumn2() const {return q.GetColumn2();}
	ILINE Vec3_tpl<F> GetColumn3() const {return t;}
	ILINE Vec3_tpl<F> GetRow0() const {	return q.GetRow0(); }
	ILINE Vec3_tpl<F> GetRow1() const {	return q.GetRow1(); }
	ILINE Vec3_tpl<F> GetRow2() const {	return q.GetRow2(); }

	ILINE bool IsEquivalent( const QuatT_tpl<F>& p, F e=VEC_EPSILON) const {
		Quat q0= p.q;
		Quat q1=-p.q;
		uint32 t0= (fabs_tpl(q.v.x-q0.v.x)<=e) &&	(fabs_tpl(q.v.y-q0.v.y)<=e) && (fabs_tpl(q.v.z-q0.v.z)<=e) && (fabs_tpl(q.w-q0.w)<=e);	
		uint32 t1= (fabs_tpl(q.v.x-q1.v.x)<=e) &&	(fabs_tpl(q.v.y-q1.v.y)<=e) && (fabs_tpl(q.v.z-q1.v.z)<=e) && (fabs_tpl(q.w-q1.w)<=e);	
		return  (	(t0|t1) &&	(fabs_tpl(t.x-p.t.x)<=e) &&	(fabs_tpl(t.y-p.t.y)<=e) &&	(fabs_tpl(t.z-p.t.z)<=e) );	
	}

	ILINE bool IsValid() const
	{
		if (!t.IsValid())	return false;
		if (!q.IsValid())	return false;
		return true;
	}

	ILINE void SetNLerp( const QuatT_tpl<F> &p, const QuatT_tpl<F> &q, F t ); 
	static QuatT_tpl<F> CreateNLerp( const QuatT_tpl<F> &p, const QuatT_tpl<F> &q, F t ); 

	//NOTE: all vectors are stored in columns
	ILINE void SetFromVectors(const Vec3& vx, const Vec3& vy, const Vec3& vz, const Vec3& pos)	
	{
		Matrix34 m34;
		m34.m00=vx.x;		m34.m01=vy.x;		m34.m02=vz.x;		m34.m03 = pos.x;
		m34.m10=vx.y;		m34.m11=vy.y;		m34.m12=vz.y;		m34.m13 = pos.y;
		m34.m20=vx.z;		m34.m21=vy.z;		m34.m22=vz.z;		m34.m23 = pos.z;
		*this = QuatT_tpl<F>(m34);
	}
	ILINE static QuatT_tpl<F> CreateFromVectors(const Vec3_tpl<F>& vx, const Vec3_tpl<F>& vy, const Vec3_tpl<F>& vz, const Vec3_tpl<F>& pos) {
		QuatT_tpl<F> qt; qt.SetFromVectors(vx,vy,vz,pos); return qt;
	}

	void ClampLengthAngle(F maxLength, F maxAngleDeg)
	{
		t.ClampLength(maxLength);
		q.ClampAngle(maxAngleDeg);
	}

	QuatT_tpl<F> GetScaled(F scale)
	{
		return QuatT_tpl<F>(t * scale, q.GetScaled(scale));
	}

	AUTO_STRUCT_INFO
};

typedef QuatT_tpl<f32>	QuatT;
typedef QuatT_tpl<f64>	QuatTr;
typedef QuatT_tpl<f64>	QuatT_f64;

/*!
* linear-interpolation between quaternions (lerp)
* 
* Example:
*  CQuaternion result,p,q;
*  result=qlerp( p, q, 0.5f );
*/
/*!
* linear-interpolation between quaternions (lerp)
* 
* Example:
*  CQuaternion result,p,q;
*  result=qlerp( p, q, 0.5f );
*/
template<typename F> ILINE void QuatT_tpl<F>::SetNLerp( const QuatT_tpl<F> &p, const QuatT_tpl<F> &tq, F ti ) 
{	
	assert(p.q.IsValid());
	assert(tq.q.IsValid());
	Quat_tpl<F> d=tq.q;
	if( (p.q|d) < 0 ) { d=-d;	} 

	q.v.x = p.q.v.x*(1-ti) + d.v.x*ti;
	q.v.y = p.q.v.y*(1-ti) + d.v.y*ti;
	q.v.z = p.q.v.z*(1-ti) + d.v.z*ti;
	q.w		= p.q.w  *(1-ti) + d.w  *ti;
	q.Normalize();

	t.x   = p.t.x*(1-ti)   + tq.t.x*ti;
	t.y   = p.t.y*(1-ti)   + tq.t.y*ti;
	t.z   = p.t.z*(1-ti)   + tq.t.z*ti;
}

template<typename F> ILINE QuatT_tpl<F> QuatT_tpl<F>::CreateNLerp( const QuatT_tpl<F> &p, const QuatT_tpl<F> &q, F t ) 
{
	QuatT_tpl<F> d;  d.SetNLerp(p,q,t); 	return d;
}

/*!
*
*  Implements the multiplication operator: QuatT=Quatpos*Quat
*
*  AxB = operation B followed by operation A.  
*  A multiplication takes 16 muls and 12 adds (=28 float operations). 
*
*  Example:
*   Quat    quat		=	Quat::CreateRotationX(1.94192f);;
*   QuatT quatpos	=	QuatT::CreateRotationZ(3.14192f,Vec3(11,22,33));
*	  QuatT qp			=	quatpos*quat;
*/
template<class F1,class F2> ILINE QuatT_tpl<F1> operator * ( const QuatT_tpl<F1> &p, const Quat_tpl<F2> &q) 
{ 
	assert(p.IsValid());
	assert(q.IsValid());
	return QuatT_tpl<F1>(p.q*q,p.t);
}

/*!
*  Implements the multiplication operator: QuatT=QuatposA*QuatposB
*
*  AxB = operation B followed by operation A.  
*  A multiplication takes 31 muls and 30 adds  (=61 float operations). 
*
*  Example:
*   QuatT quatposA	=	QuatT::CreateRotationX(1.94192f,Vec3(77,55,44));
*   QuatT quatposB	=	QuatT::CreateRotationZ(3.14192f,Vec3(11,22,33));
*	  QuatT qp				=	quatposA*quatposB;
*/
template<class F1,class F2> ILINE QuatT_tpl<F1> operator * ( const QuatT_tpl<F1> &q, const QuatT_tpl<F2> &p) 
{ 
	assert(q.IsValid());
	assert(p.IsValid());
	return QuatT_tpl<F1>( q.q*p.q, q.q*p.t+q.t );
}



/*!
* post-multiply of a QuatT and a Vec3 (3D rotations with quaternions)
* 
* Example:
*  Quat q(1,0,0,0);
*  Vec3 v(33,44,55);
*	 Vec3 result = q*v;
*/
template<class F,class F2> ILINE Vec3_tpl<F> operator * (const QuatT_tpl<F> &q, const Vec3_tpl<F2> &v) {
	assert(v.IsValid());
	assert(q.IsValid());
	//muls=15 / adds=15+3
	Vec3_tpl<F> out,r2;
 	r2.x=(q.q.v.y*v.z-q.q.v.z*v.y)+q.q.w*v.x;
 	r2.y=(q.q.v.z*v.x-q.q.v.x*v.z)+q.q.w*v.y;
	r2.z=(q.q.v.x*v.y-q.q.v.y*v.x)+q.q.w*v.z;
	out.x =(r2.z*q.q.v.y-r2.y*q.q.v.z); out.x+=out.x+v.x+q.t.x;
	out.y =(r2.x*q.q.v.z-r2.z*q.q.v.x); out.y+=out.y+v.y+q.t.y;
  out.z =(r2.y*q.q.v.x-r2.x*q.q.v.y); out.z+=out.z+v.z+q.t.z;
  return out;
}


//----------------------------------------------------------------------
// Quaternion with translation vector and scale
// Similar to QuatT, but s is not ignored. 
// Most functions then differ, so we don't inherit.
//----------------------------------------------------------------------
template <typename F> struct QuatTS_tpl
{
	Quat_tpl<F>	q;
	Vec3_tpl<F>	t; 
	F						s;

	//constructors
#ifdef _DEBUG
	ILINE QuatTS_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&q.v.x; 		p[0]=F32NAN;p[1]=F32NAN;p[2]=F32NAN;p[3]=F32NAN; p[4]=F32NAN;p[5]=F32NAN;p[6]=F32NAN;p[7]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&q.v.x;		p[0]=F64NAN;p[1]=F64NAN;p[2]=F64NAN;p[3]=F64NAN; p[4]=F64NAN;p[5]=F64NAN;p[6]=F64NAN;p[7]=F64NAN;
		}
	}
#else
	ILINE QuatTS_tpl()	{};
#endif

	ILINE QuatTS_tpl( const Quat_tpl<F>& quat, const Vec3_tpl<F>& trans, F scale=1 )	{ q=quat; t=trans; s=scale; }
	ILINE QuatTS_tpl( type_identity ) { SetIdentity(); }

	//CONSTRUCTOR: implement the copy/casting/assignment constructor:	
	template <typename F1> ILINE QuatTS_tpl( const QuatTS_tpl<F1>& qts ) : 	q(qts.q), t(qts.t), s(qts.s)
	{	
		//q.w	=	qts.q.w; 
		//q.v.x=qts.q.v.x; 
		//q.v.y=qts.q.v.y; 
		//q.v.z=qts.q.v.z; 

		//t.x	=	qts.t.x; 
		//t.y	=	qts.t.y;			
		//t.z	=	qts.t.z; 

		//s		=	qts.s;
	}

	ILINE QuatTS_tpl& operator = (const QuatT_tpl<F> &qt) { 
		q=qt.q; t=qt.t; s=1.0f;	return *this;
	}
	ILINE QuatTS_tpl( const QuatT_tpl<F>& qp )	{ q=qp.q; t=qp.t; s=1.0f; }


	ILINE void SetIdentity() 
	{
		q.SetIdentity(); t=Vec3(ZERO); s=1;
	}

	explicit ILINE QuatTS_tpl(const Matrix34_tpl<F>& m)	
	{	
		t = m.GetTranslation();

		Matrix33_tpl<F> m3(m);
		s = m.GetRow(0).GetLengthSquared();
		if (abs(s-1) > 0.001f)
		{
			s = sqrt(s);
			m3 /= s;
		}
		else
		{
			s = 1;
		}
		q = GetQuatFromMat33(m3);
	}

	void Invert() 
	{
		s = 1/s;
		q = !q; 	
		t = q * t * -s; 
	}
	QuatTS_tpl<F> GetInverted() const 
	{
		QuatTS_tpl<F> inv;
		inv.s = 1/s;
		inv.q = !q; 	
		inv.t = inv.q * t * -inv.s;
		return inv;
	}

	ILINE void SetNLerp( const QuatTS_tpl<F> &p, const QuatTS_tpl<F> &q, F t ); 
	static QuatTS_tpl<F> CreateNLerp( const QuatTS_tpl<F> &p, const QuatTS_tpl<F> &q, F t ); 

	ILINE bool IsEquivalent( const QuatTS_tpl<F>& p, F e=VEC_EPSILON) const {
		Quat q0= p.q;
		Quat q1=-p.q;
		uint32 t0= (fabs_tpl(q.v.x-q0.v.x)<=e) &&	(fabs_tpl(q.v.y-q0.v.y)<=e) && (fabs_tpl(q.v.z-q0.v.z)<=e) && (fabs_tpl(q.w-q0.w)<=e);	
		uint32 t1= (fabs_tpl(q.v.x-q1.v.x)<=e) &&	(fabs_tpl(q.v.y-q1.v.y)<=e) && (fabs_tpl(q.v.z-q1.v.z)<=e) && (fabs_tpl(q.w-q1.w)<=e);	
		return  (	(t0|t1) &&	(fabs_tpl(t.x-p.t.x)<=e) &&	(fabs_tpl(t.y-p.t.y)<=e) &&	(fabs_tpl(t.z-p.t.z)<=e) &&	(fabs_tpl(s-p.s)<=e) );	
	}

	ILINE bool IsValid(F e = VEC_EPSILON) const
	{
		if (!q.v.IsValid())	return false;
		if (!NumberValid(q.w)) return false;
		if (!q.IsUnit(e))	return false;
		if (!t.IsValid())	return false;
		if (!NumberValid(s)) return false;
		return true;
	}

	ILINE Vec3_tpl<F> GetColumn0() const {return q.GetColumn0();}
	ILINE Vec3_tpl<F> GetColumn1() const {return q.GetColumn1();}
	ILINE Vec3_tpl<F> GetColumn2() const {return q.GetColumn2();}
	ILINE Vec3_tpl<F> GetColumn3() const {return t;}
	ILINE Vec3_tpl<F> GetRow0() const {	return q.GetRow0(); }
	ILINE Vec3_tpl<F> GetRow1() const {	return q.GetRow1(); }
	ILINE Vec3_tpl<F> GetRow2() const {	return q.GetRow2(); }

	AUTO_STRUCT_INFO
};

typedef QuatTS_tpl<f32> QuatTS;
typedef QuatTS_tpl<f64> QuatTSr;
typedef QuatTS_tpl<f64> QuatTS_f64;


template<class F1,class F2> ILINE QuatTS_tpl<F1> operator * ( const QuatTS_tpl<F1> &a, const Quat_tpl<F2> &b ) 
{ 
	return QuatTS_tpl<F1>( a.q*b, a.t, a.s );
}


template<class F1,class F2> ILINE QuatTS_tpl<F1> operator * ( const QuatTS_tpl<F1> &a, const QuatT_tpl<F2> &b ) 
{ 
	return QuatTS_tpl<F1>( a.q*b.q, a.q*b.t*a.s + a.t, a.s );
}
template<class F1,class F2> ILINE QuatTS_tpl<F1> operator * ( const QuatT_tpl<F1> &a, const QuatTS_tpl<F2> &b ) 
{ 
	return QuatTS_tpl<F1>( a.q*b.q, a.q*b.t + a.t, b.s );
}

/*!
*  Implements the multiplication operator: QuatTS=QuatTS*QuatTS
*/
template<class F1,class F2> ILINE QuatTS_tpl<F1> operator * ( const QuatTS_tpl<F1> &a, const QuatTS_tpl<F2> &b ) 
{ 
	assert(a.IsValid());
	assert(b.IsValid());
	return QuatTS_tpl<F1>( a.q*b.q, a.q*b.t*a.s + a.t, a.s*b.s );
}

/*!
* post-multiply of a QuatT and a Vec3 (3D rotations with quaternions)
*/
template<class F,class F2> ILINE Vec3_tpl<F> operator * ( const QuatTS_tpl<F> &q, const Vec3_tpl<F2> &v ) 
{
	assert(q.IsValid());
	assert(v.IsValid());
	return q.q * v * q.s + q.t;
}

/*!
* linear-interpolation between quaternions (lerp)
* 
* Example:
*  CQuaternion result,p,q;
*  result=qlerp( p, q, 0.5f );
*/
/*!
* linear-interpolation between quaternions (lerp)
* 
* Example:
*  CQuaternion result,p,q;
*  result=qlerp( p, q, 0.5f );
*/
template<typename F> ILINE void QuatTS_tpl<F>::SetNLerp( const QuatTS_tpl<F> &p, const QuatTS_tpl<F> &tq, F ti ) 
{	
	assert(p.q.IsValid());
	assert(tq.q.IsValid());
	Quat_tpl<F> d=tq.q;
	if( (p.q|d) < 0 ) { d=-d;	} 

	q.v.x = p.q.v.x*(1-ti) + d.v.x*ti;
	q.v.y = p.q.v.y*(1-ti) + d.v.y*ti;
	q.v.z = p.q.v.z*(1-ti) + d.v.z*ti;
	q.w		= p.q.w  *(1-ti) + d.w  *ti;
	q.Normalize();

	t.x   = p.t.x*(1-ti)   + tq.t.x*ti;
	t.y   = p.t.y*(1-ti)   + tq.t.y*ti;
	t.z   = p.t.z*(1-ti)   + tq.t.z*ti;

	s     = p.s*(1-ti)     + tq.s*ti;
}

template<typename F> ILINE QuatTS_tpl<F> QuatTS_tpl<F>::CreateNLerp( const QuatTS_tpl<F> &p, const QuatTS_tpl<F> &q, F t ) 
{
	QuatTS_tpl<F> d;  d.SetNLerp(p,q,t); 	return d;
}


#endif // _Quaternion_tpl_H
