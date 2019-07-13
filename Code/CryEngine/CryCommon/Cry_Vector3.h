//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File: Cry_Vector3.h
//	Description: Common vector class
//
//	History:
//	-Feb 27,2003: Created by Ivo Herzeg
//
//////////////////////////////////////////////////////////////////////

#ifndef VECTOR_H
#define VECTOR_H

#if _MSC_VER > 1000
# pragma once
#endif


// some constants
template<typename T> struct Vec3_tpl;

template<typename T> struct Vec3Constants
{
	static const Vec3_tpl<T> fVec3_Zero;
	static const Vec3_tpl<T> fVec3_OneX;
	static const Vec3_tpl<T> fVec3_OneY;
	static const Vec3_tpl<T> fVec3_OneZ;
  static const Vec3_tpl<T> fVec3_One;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// class Vec3_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <typename F> struct Vec3_tpl
{

	F x,y,z;

#ifdef _DEBUG
	ILINE Vec3_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&x;		p[0]=F32NAN;	p[1]=F32NAN; p[2]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&x;		p[0]=F64NAN;	p[1]=F64NAN; p[2]=F64NAN;
		}
	}
#else
	ILINE Vec3_tpl()	{};
#endif


	/*!
	* template specailization to initialize a vector 
	* 
	* Example:
	*  Vec3 v0=Vec3(ZERO);
	*  Vec3 v1=Vec3(MIN);
	*  Vec3 v2=Vec3(MAX);
	*/
	Vec3_tpl(type_zero) : x(0),y(0),z(0) {}
	Vec3_tpl(type_min);
	Vec3_tpl(type_max);

	/*!
	* constrctors and bracket-operator to initialize a vector 
	* 
	* Example:
	*  Vec3 v0=Vec3(1,2,3);
	*  Vec3 v1(1,2,3);
	*  v2.Set(1,2,3);
	*/
	ILINE Vec3_tpl( F vx, F vy, F vz ) : x(vx),y(vy),z(vz){ assert(this->IsValid()); }
	ILINE void operator () ( F vx, F vy, F vz ) { x=vx; y=vy; z=vz; assert(this->IsValid()); }
	ILINE Vec3_tpl<F>& Set(const F xval,const F yval, const F zval) { x=xval; y=yval; z=zval; assert(this->IsValid()); return *this; }

	explicit Vec3_tpl( F f ) : x(f),y(f),z(f) { assert(this->IsValid()); }

	/*!
	* the copy/casting/assignement constructor 
	* 
	* Example:
	*  Vec3 v0=v1;
	*/
	template <class T> ILINE  Vec3_tpl( const Vec3_tpl<T>& v ) : x((F)v.x), y((F)v.y), z((F)v.z) { assert(this->IsValid()); }
	explicit ILINE Vec3_tpl(const Ang3_tpl<F>& v) : x((F)v.x), y((F)v.y), z((F)v.z) { assert(this->IsValid()); }
	ILINE Vec3_tpl(const Vec2_tpl<F>& v) : x((F)v.x), y((F)v.y), z(0) { assert(this->IsValid()); }


	/*!
	* overloaded arithmetic operator  
	* 
	* Example:
	*  Vec3 v0=v1*4;
	*/
	ILINE Vec3_tpl<F> operator * (F k) const { return Vec3_tpl<F>(x*k,y*k,z*k); }
	ILINE Vec3_tpl<F> operator / (F k) const { k=(F)1.0/k; return Vec3_tpl<F>(x*k,y*k,z*k); }
	ILINE friend Vec3_tpl<F> operator * (f32 f, const Vec3_tpl &vec)	{ return Vec3_tpl((F)(f*vec.x), (F)(f*vec.y), (F)(f*vec.z)); }

	ILINE Vec3_tpl<F>& operator *= (F k) { x*=k;y*=k;z*=k; return *this; }
	ILINE Vec3_tpl<F>& operator /= (F k) { k=(F)1.0/k; x*=k;y*=k;z*=k; return *this; }

	ILINE Vec3_tpl<F> operator - ( void ) const { return Vec3_tpl<F>(-x,-y,-z); }
	ILINE Vec3_tpl<F>& Flip()   { x=-x; y=-y; z=-z; return *this; }


	/*!
	* bracked-operator   
	* 
	* Example:
	*  uint32 t=v[0];
	*/
	ILINE F &operator [] (int32 index)		  { assert(index>=0 && index<=2); return ((F*)this)[index]; }
	ILINE F operator [] (int32 index) const { assert(index>=0 && index<=2); return ((F*)this)[index]; }


	/*!
	* functions and operators to compare vector   
	* 
	* Example:
	*  if (v0==v1) dosomething;
	*/
	ILINE bool operator==(const Vec3_tpl<F> &vec) { return x == vec.x && y == vec.y && z == vec.z; }
	ILINE bool operator!=(const Vec3_tpl<F> &vec) { return !(*this == vec); }

	ILINE friend bool operator ==(const Vec3_tpl<F> &v0, const Vec3_tpl<F> &v1)	{
		return ((v0.x==v1.x) && (v0.y==v1.y) && (v0.z==v1.z));
	}
	ILINE friend bool operator !=(const Vec3_tpl<F> &v0, const Vec3_tpl<F> &v1)	{	return !(v0==v1);	}

	ILINE bool IsZero(F e=(F)0.0) const {	return  (fabs_tpl(x)<=e) && (fabs_tpl(y)<=e) && (fabs_tpl(z)<=e);	}

	ILINE bool IsEquivalent(const Vec3_tpl<F>& v1, F epsilon=VEC_EPSILON) const 
	{
		assert(v1.IsValid()); 
	  assert(this->IsValid()); 
		return  ((fabs_tpl(x-v1.x) <= epsilon) &&	(fabs_tpl(y-v1.y) <= epsilon)&&	(fabs_tpl(z-v1.z) <= epsilon));	
	}

	ILINE bool IsUnit(F epsilon=VEC_EPSILON) const 
	{
		return (fabs_tpl(1 - GetLengthSquared()) <= epsilon);
	}

	ILINE bool IsValid() const
	{
		if (!NumberValid(x)) return false;
		if (!NumberValid(y)) return false;
		if (!NumberValid(z)) return false;
		return true;
	}

	//! force vector length by normalizing it
	ILINE void SetLength(F fLen)
	{ 
		F fLenMe = GetLengthSquared();
		if(fLenMe<0.00001f*0.00001f)
			return;
		fLenMe = fLen * isqrt_tpl(fLenMe);
		x*=fLenMe; y*=fLenMe; z*=fLenMe;
	}

	ILINE void ClampLength(F maxLength)
	{
		F sqrLength = GetLengthSquared();
		if (sqrLength > (maxLength * maxLength))
		{
			F scale = maxLength * isqrt_tpl(sqrLength);
			x *= scale; y *= scale; z *= scale;
		}
	}

	//! calculate the length of the vector
	ILINE F	GetLength() const { return sqrt_tpl(x*x+y*y+z*z); }		

	ILINE F	GetLengthFast() const { return sqrt_fast_tpl(x*x+y*y+z*z); }		

	//! calculate the squared length of the vector
	ILINE F GetLengthSquared() const { return x*x+y*y+z*z; }

	//! calculate the length of the vector ignoring the z component
	ILINE F	GetLength2D() const { return sqrt_tpl(x*x+y*y); }		

	//! calculate the squared length of the vector ignoring the z component
	ILINE F	GetLengthSquared2D() const { return x*x+y*y; }		

	ILINE F GetDistance(const Vec3_tpl<F> vec1) const {
		return  sqrt_tpl((x-vec1.x)*(x-vec1.x)+(y-vec1.y)*(y-vec1.y)+(z-vec1.z)*(z-vec1.z)); 
	}
	ILINE F GetSquaredDistance ( const Vec3_tpl<F> &v) const {		
		return  (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z); 
	}
  ILINE F GetSquaredDistance2D ( const Vec3_tpl<F> &v) const {		
    return  (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y);
  }

	//! normalize the vector
	// this version is "safe", as zero vectors will remain zero without float errors.
	ILINE void	Normalize() 
	{ 
		assert(this->IsValid()); 
		F fInvLen = isqrt_safe_tpl( x*x+y*y+z*z );
		x*=fInvLen; y*=fInvLen; z*=fInvLen; 
	}

	//! may be faster and less accurate
	ILINE void NormalizeFast() 
	{
		assert(this->IsValid()); 
		F fInvLen = isqrt_fast_tpl( x*x+y*y+z*z );
		x*=fInvLen; y*=fInvLen; z*=fInvLen; 
	}

	//! normalize the vector
	// check for null vector - set to the passed in vector (which should be normalised!) if it is null vector
	// returns the original length of the vector
	ILINE F NormalizeSafe(const struct Vec3_tpl<F>& safe = Vec3Constants<F>::fVec3_Zero) 
	{ 
		assert(this->IsValid()); 
		F fLen2 = x*x+y*y+z*z;
		if (fLen2>0.0f)
		{
			F fInvLen = isqrt_tpl(fLen2);
			x*=fInvLen; y*=fInvLen; z*=fInvLen; 
			return F(1) / fInvLen;
		}
		else
		{
			*this = safe;
			return F(0);
		}
	}

	//! return a normalized vector
	ILINE Vec3_tpl GetNormalized() const 
	{ 
		F fInvLen = isqrt_safe_tpl( x*x+y*y+z*z );
		return *this * fInvLen;
	}

	//! return a safely normalized vector - returns safe vector (should be normalised) if original is zero length
	ILINE Vec3_tpl GetNormalizedSafe(const struct Vec3_tpl<F>& safe = Vec3Constants<F>::fVec3_OneX) const 
	{ 
		F fLen2 = x*x+y*y+z*z;	
		if (fLen2 > 0.0f)
		{
			F fInvLen = isqrt_tpl(fLen2);
			return *this * fInvLen;
		}
		else
		{
			return safe;
		}
	}



	// permutate coordinates so that z goes to new_z slot
	ILINE Vec3_tpl GetPermutated(int new_z) const { return Vec3_tpl(*(&x+inc_mod3[new_z]), *(&x+dec_mod3[new_z]), *(&x+new_z)); }

	// returns volume of a box with this vector as diagonal 
	ILINE F GetVolume() const { return x*y*z; }

	// returns a vector that consists of absolute values of this one's coordinates
	ILINE Vec3_tpl<F> abs() const { return Vec3_tpl(fabs_tpl(x),fabs_tpl(y),fabs_tpl(z)); }

	//! check for min bounds
	ILINE void CheckMin(const Vec3_tpl<F> &other)	{ 
		if (other.x<x) x=other.x;
		if (other.y<y) y=other.y;
		if (other.z<z) z=other.z;
	}			
	//! check for max bounds
	ILINE void CheckMax(const Vec3_tpl<F> &other)	{
		if (other.x>x) x=other.x;
		if (other.y>y) y=other.y;
		if (other.z>z) z=other.z;
	}


	/*!
	* sets a vector orthogonal to the input vector 
	* 
	* Example:
	*  Vec3 v;
	*  v.SetOrthogonal( i );
	*/
	ILINE void SetOrthogonal( const Vec3_tpl<F>& v ) {
		int i = isneg(square((F)0.9)*v.GetLengthSquared()-v.x*v.x);
		(*this)[i]=0; (*this)[incm3(i)]= v[decm3(i)];	(*this)[decm3(i)]=-v[incm3(i)];
	}
	// returns a vector orthogonal to this one
	ILINE Vec3_tpl GetOrthogonal() const {
		int i = isneg(square((F)0.9)*GetLengthSquared()-x*x);
		Vec3_tpl<F> res;
		res[i]=0; res[incm3(i)]=(*this)[decm3(i)]; res[decm3(i)]=-(*this)[incm3(i)];
		return res;
	}

	/*!
	* Project a point/vector on a (virtual) plane 
	* Consider we have a plane going through the origin. 
	* Because d=0 we need just the normal. The vector n is assumed to be a unit-vector.
	* 
	* Example:
	*  Vec3 result=Vec3::CreateProjection( incident, normal );
	*/
	ILINE void SetProjection( const Vec3_tpl& i, const Vec3_tpl& n ) { *this = i-n*(n|i); }
	ILINE static Vec3_tpl<F> CreateProjection( const Vec3_tpl& i, const Vec3_tpl& n ) { return i-n*(n|i); }

	/*!
	* Calculate a reflection vector. Vec3 n is assumed to be a unit-vector.
	* 
	* Example:
	*  Vec3 result=Vec3::CreateReflection( incident, normal );
	*/
	ILINE void SetReflection( const Vec3_tpl& i, const Vec3_tpl &n ) { *this=(n*(i|n)*2)-i; }
	ILINE static Vec3_tpl<F> CreateReflection( const Vec3_tpl& i, const Vec3_tpl &n ) {	return (n*(i|n)*2)-i; }

	/*!
	* Linear-Interpolation between Vec3 (lerp)
	* 
	* Example:
	*  Vec3 r=Vec3::CreateLerp( p, q, 0.345f );
	*/
	ILINE void SetLerp( const Vec3_tpl<F> &p, const Vec3_tpl<F> &q, F t ) {	*this = p*(1.0f-t) + q*t;}
	ILINE static Vec3_tpl<F> CreateLerp( const Vec3_tpl<F> &p, const Vec3_tpl<F> &q, F t ) {	return p*(1.0f-t) + q*t;}


	/*!
	* Spherical-Interpolation between 3d-vectors (geometrical slerp)
	* both vectors are assumed to be normalized.  
	*
	* Example:
	*  Vec3 r=Vec3::CreateSlerp( p, q, 0.5674f );
	*/
	ILINE void SetSlerp( const Vec3_tpl<F>& p, const Vec3_tpl<F>& q, F t ) {
		assert(p.IsUnit(0.005f));
		assert(q.IsUnit(0.005f));
		// calculate cosine using the "inner product" between two vectors: p*q=cos(radiant)
		F cosine = clamp_tpl((p|q),-1.0f,1.0f);
		//we explore the special cases where the both vectors are very close together, 
		//in which case we approximate using the more economical LERP and avoid "divisions by zero" since sin(Angle) = 0  as   Angle=0
		if(cosine>=(F)0.99) {
			SetLerp(p,q,t); //perform LERP:
			Normalize();
		}	else {
			//perform SLERP: because of the LERP-check above, a "division by zero" is not possible
			F rad				= acos_tpl(cosine);
			F scale_0   = sin_tpl((1-t)*rad);
			F scale_1   = sin_tpl(t*rad);
			*this=(p*scale_0 + q*scale_1) / sin_tpl(rad);
			Normalize();
		}
	}
	ILINE static Vec3_tpl<F> CreateSlerp( const Vec3_tpl<F>& p, const Vec3_tpl<F>& q, F t ) {
		Vec3_tpl<F> v;	v.SetSlerp(p,q,t); return v;	
	}

	/*!
	* set random normalized vector (=random position on unit sphere) 
	* 
	* Example:
	*  Vec3 v;
	*  v.SetRandomDirection(); 
	*/
	void SetRandomDirection( void )
	{ 
		int nMax = 5; // To prevent hanging with bad randoms.
		F Length2;
		do {
			x = 1.0f - 2.0f*cry_frand();
			y = 1.0f - 2.0f*cry_frand();
			z = 1.0f - 2.0f*cry_frand();
			Length2 = len2();
			nMax--;
		} while((Length2>1.0f || Length2<0.0001f) && nMax > 0);
		F InvScale = isqrt_tpl(Length2);				// dividion by 0 is prevented
		x *= InvScale; y *= InvScale; z *= InvScale;
	}	


	/*!
	* rotate a vector using angle&axis 
	* 
	* Example:
	*  Vec3 r=v.GetRotated(axis,angle);
	*/
	ILINE Vec3_tpl GetRotated(const Vec3_tpl &axis, F angle) const { 
		return GetRotated(axis,cos_tpl(angle),sin_tpl(angle)); 
	}
	ILINE Vec3_tpl GetRotated(const Vec3_tpl &axis, F cosa,F sina) const {
		Vec3_tpl zax = axis*(*this|axis); 
		Vec3_tpl xax = *this-zax; 
		Vec3_tpl yax = axis%xax;
		return xax*cosa + yax*sina + zax;
	}

	/*!
	* rotate a vector around a center using angle&axis 
	* 
	* Example:
	*  Vec3 r=v.GetRotated(axis,angle);
	*/
	ILINE Vec3_tpl GetRotated(const Vec3_tpl &center,const Vec3_tpl &axis, F angle) const { 
		return center+(*this-center).GetRotated(axis,angle); 
	}
	ILINE Vec3_tpl GetRotated(const Vec3_tpl &center,const Vec3_tpl &axis, F cosa,F sina) const { 
		return center+(*this-center).GetRotated(axis,cosa,sina); 
	}

	/*!
	* component wise multiplication of two vectors
	*/
	ILINE Vec3_tpl CompMul( const Vec3_tpl& rhs ) const { 
		return( Vec3_tpl( x * rhs.x, y * rhs.y, z * rhs.z ) ); 
	}

	//DEPRICATED ILINE friend F GetDistance(const Vec3_tpl<F> &vec1, const Vec3_tpl<F> &vec2) { 
		//return  sqrt_tpl((vec2.x-vec1.x)*(vec2.x-vec1.x)+(vec2.y-vec1.y)*(vec2.y-vec1.y)+(vec2.z-vec1.z)*(vec2.z-vec1.z)); 
	//}	
	//DEPRICATED ILINE friend F	GetSquaredDistance(const Vec3_tpl<F> &vec1, const Vec3_tpl<F> &vec2)	{		
		//return (vec2.x-vec1.x)*(vec2.x-vec1.x)+(vec2.y-vec1.y)*(vec2.y-vec1.y)+(vec2.z-vec1.z)*(vec2.z-vec1.z);
	//}
	//three methods for a "dot-product" operation
	ILINE F Dot (const Vec3_tpl<F> &vec2)	const	{ return x*vec2.x + y*vec2.y + z*vec2.z; }
	//two methods for a "cross-product" operation
	ILINE Vec3_tpl<F> Cross (const Vec3_tpl<F> &vec2) const	{	return Vec3_tpl<F>( y*vec2.z  -  z*vec2.y,     z*vec2.x -    x*vec2.z,   x*vec2.y  -  y*vec2.x); 	}	

	//f32* fptr=vec;
	DEPRICATED operator F* ()					{ return (F*)this; }
	template <class T>	explicit DEPRICATED Vec3_tpl(const T *src) { x=src[0]; y=src[1]; z=src[2]; }

	ILINE Vec3_tpl& zero() { x=y=z=0; return *this; }
	ILINE F len() const { return sqrt_tpl(x*x+y*y+z*z); }
	ILINE F len2() const { return x*x +y*y + z*z; }

	ILINE Vec3_tpl& normalize() { 
		F len2 = x*x+y*y+z*z;
		if (len2>0) 
		{ 
			F rlen = isqrt_tpl(len2);
			x*=rlen; y*=rlen; z*=rlen; 
		} 
		else 
			Set(0,0,1); 
		return *this; 
	}
	ILINE Vec3_tpl normalized() const { 
		F len2 = x*x+y*y+z*z;
		if (len2>0) 
		{ 
			F rlen = isqrt_tpl(len2); 
			return Vec3_tpl(x*rlen,y*rlen,z*rlen);
		}
		else 
			return Vec3_tpl(0,0,1);
	}

	AUTO_STRUCT_INFO
};

///////////////////////////////////////////////////////////////////////////////
// Typedefs                                                                  //
///////////////////////////////////////////////////////////////////////////////
typedef Vec3_tpl<f32>		Vec3;			//we will use only this throughout the project
typedef Vec3_tpl<f64>	  Vec3_f64; //for double-precision
typedef Vec3_tpl<real>	Vec3r;		//for systems with float precision higher then 64bit
typedef Vec3_tpl<int>	  Vec3i;		//for integers
template<> inline Vec3_tpl<f32>::Vec3_tpl(type_min) { x=y=z=-3.3E38f; }
template<> inline Vec3_tpl<f32>::Vec3_tpl(type_max) { x=y=z=3.3E38f; }
template<> inline Vec3_tpl<f64>::Vec3_tpl(type_min) { x=y=z=-1.7E308; }
template<> inline Vec3_tpl<f64>::Vec3_tpl(type_max) { x=y=z=1.7E308; }


// dot product (2 versions)
template<class F1,class F2> 
ILINE F1 operator * (const Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) { 
 	return (F1)(v0.x*v1.x+v0.y*v1.y+v0.z*v1.z); 
} 
template<class F1,class F2> 
ILINE F1 operator | (const Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) { 
 	return (F1)(v0.x*v1.x+v0.y*v1.y+v0.z*v1.z); 
} 

// cross product
template<class F1,class F2> 
ILINE Vec3_tpl<F1> operator ^ (const Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.y*v1.z-v0.z*v1.y, v0.z*v1.x-v0.x*v1.z, v0.x*v1.y-v0.y*v1.x); 
}

template<class F1,class F2> 
Vec3_tpl<F1> operator % (const Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.y*v1.z-v0.z*v1.y, v0.z*v1.x-v0.x*v1.z, v0.x*v1.y-v0.y*v1.x); 
} 


template <class F> 
ILINE bool IsEquivalent(const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1, f32 epsilon=VEC_EPSILON )
{
	return  ((fabs_tpl(v0.x-v1.x) <= epsilon) &&	(fabs_tpl(v0.y-v1.y) <= epsilon)&&	(fabs_tpl(v0.z-v1.z) <= epsilon));	
}
//---------------------------------------------------------------------------

//vector addition
template<class F1,class F2>
ILINE Vec3_tpl<F1> operator + (const Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
}
//vector addition
template<class F1,class F2>
ILINE Vec3_tpl<F1> operator + (const Vec2_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.x+v1.x, v0.y+v1.y, v1.z);
}
//vector addition
template<class F1,class F2>
ILINE Vec3_tpl<F1> operator + (const Vec3_tpl<F1> &v0, const Vec2_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.x+v1.x, v0.y+v1.y, v0.z);
}

//vector subtraction
template<class F1,class F2>
ILINE Vec3_tpl<F1> operator - (const Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.x-v1.x, v0.y-v1.y, v0.z-v1.z);
}
template<class F1,class F2>
ILINE Vec3_tpl<F1> operator - (const Vec2_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.x-v1.x, v0.y-v1.y, 0.0f-v1.z);
}
template<class F1,class F2>
ILINE Vec3_tpl<F1> operator - (const Vec3_tpl<F1> &v0, const Vec2_tpl<F2> &v1) {
	return Vec3_tpl<F1>(v0.x-v1.x, v0.y-v1.y, v0.z);
}


//---------------------------------------------------------------------------


//vector self-addition
template<class F1,class F2>
ILINE Vec3_tpl<F1>& operator += (Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	v0.x+=v1.x; v0.y+=v1.y; v0.z+=v1.z; return v0;
}

//vector self-subtraction
template<class F1,class F2>
ILINE Vec3_tpl<F1>& operator -= (Vec3_tpl<F1> &v0, const Vec3_tpl<F2> &v1) {
	v0.x-=v1.x; v0.y-=v1.y; v0.z-=v1.z; return v0;
}



//////////////////////////////////////////////////////////////////////////
// Random vector functions.

inline Vec3 Random(const Vec3& v)
{
	return Vec3( Random(v.x), Random(v.y), Random(v.z) );
}
inline Vec3 Random(const Vec3& a, const Vec3& b)
{
	return Vec3( Random(a.x,b.x), Random(a.y,b.y), Random(a.z,b.z) );
}

// Random point in sphere.
inline Vec3 SphereRandom(float fRadius)
{
	Vec3 v;
	do {
		v( BiRandom(fRadius), BiRandom(fRadius), BiRandom(fRadius) );
	} while (v.GetLengthSquared() > fRadius*fRadius);
	return v;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// class Vec4_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <typename F> struct Vec4_tpl
{
	F x,y,z,w;

#ifdef _DEBUG
	ILINE Vec4_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&x;		p[0]=F32NAN;	p[1]=F32NAN; p[2]=F32NAN; p[3]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&x;		p[0]=F64NAN;	p[1]=F64NAN; p[2]=F64NAN; p[3]=F64NAN;
		}
	
	}
#else
	ILINE Vec4_tpl()	{};
#endif



	ILINE Vec4_tpl( F vx, F vy, F vz, F vw ) { x=vx; y=vy; z=vz; w=vw; };
	ILINE Vec4_tpl( const Vec3_tpl<F> &v, F vw ) {  x=v.x; y=v.y; z=v.z; w=vw; };

	ILINE void operator () ( F vx, F vy, F vz, F vw ) { x=vx; y=vy; z=vz; w=vw; };
	ILINE void operator () ( const Vec3_tpl<F> &v, F vw ) {  x=v.x; y=v.y; z=v.z; vw=vw; };

	ILINE F &operator [] (int index)		  { assert(index>=0 && index<=3); return ((F*)this)[index]; }
	ILINE F operator [] (int index) const { assert(index>=0 && index<=3); return ((F*)this)[index]; }













	ILINE Vec4_tpl<F> operator * (F k) const 
	{ 




		return Vec4_tpl<F>(x*k,y*k,z*k,w*k); 

	}
	ILINE Vec4_tpl<F> operator / (F k) const 
	{ 
		k=(F)1.0/k; 




		return Vec4_tpl<F>(x*k,y*k,z*k,w*k); 

	}

	ILINE Vec4_tpl<F>& operator *= (F k) 
	{ 



		x*=k;
		y*=k;
		z*=k;
		w*=k; 

		return *this; 
	}
	ILINE Vec4_tpl<F>& operator /= (F k) 
	{ 
		k=(F)1.0/k; 



		x*=k;		y*=k;		z*=k;		w*=k; 

		return *this; 
	}

  ILINE F Dot (const Vec4_tpl<F> &vec2)	const	
	{ 







		return x*vec2.x + y*vec2.y + z*vec2.z + w*vec2.w; 

	}
	ILINE F GetLength() const 
	{ 
		return sqrt_tpl(Dot(*this)); 
	}

	ILINE bool IsValid() const
	{
		if (!NumberValid(x)) return false;
		if (!NumberValid(y)) return false;
		if (!NumberValid(z)) return false;
		if (!NumberValid(w)) return false;
		return true;
	}

	AUTO_STRUCT_INFO
} 



;


typedef Vec4_tpl<f32>		Vec4;			//we will use only this throughout the project
//before enabling it: add size check into Vec4 impl. for PS3
//typedef Vec4_tpl<f64>	  Vec4_f64; //for double-precision
typedef Vec4_tpl<real>	Vec4r;		//for systems with float precision higher then 64bit
//typedef Vec4_tpl<int>	  Vec4i;		//for integers


//vector self-addition
template<class F1,class F2>
ILINE Vec4_tpl<F1>& operator += (Vec4_tpl<F1> &v0, const Vec4_tpl<F2> &v1) 
{



	v0.x+=v1.x; v0.y+=v1.y; v0.z+=v1.z; v0.w+=v1.w;

	return v0;
}

//vector addition
template<class F1,class F2>
ILINE Vec4_tpl<F1> operator + (const Vec4_tpl<F1> &v0, const Vec4_tpl<F2> &v1) 
{




	return Vec4_tpl<F1>(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z, v0.w+v1.w);

}
//vector subtraction
template<class F1,class F2>
ILINE Vec4_tpl<F1> operator - (const Vec4_tpl<F1> &v0, const Vec4_tpl<F2> &v1) 
{




	return Vec4_tpl<F1>(v0.x-v1.x, v0.y-v1.y, v0.z-v1.z, v0.w-v1.w);

}

//vector multiplication
template<class F1,class F2>
ILINE Vec4_tpl<F1> operator * (const Vec4_tpl<F1> &v0, const Vec4_tpl<F2> &v1) 
{




	return Vec4_tpl<F1>(v0.x*v1.x, v0.y*v1.y, v0.z*v1.z, v0.w*v1.w);

}

//vector division
template<class F1,class F2>
ILINE Vec4_tpl<F1> operator / (const Vec4_tpl<F1> &v0, const Vec4_tpl<F2> &v1) 
{




  return Vec4_tpl<F1>(v0.x/v1.x, v0.y/v1.y, v0.z/v1.z, v0.w/v1.w);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Ang3_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename F> struct Ang3_tpl
{
	F x,y,z;

#ifdef _DEBUG
	ILINE Ang3_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&x;		p[0]=F32NAN;	p[1]=F32NAN; p[2]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&x;		p[0]=F64NAN;	p[1]=F64NAN; p[2]=F64NAN;
		}
	}
#else
	ILINE Ang3_tpl()	{};
#endif


	Ang3_tpl(type_zero) { x=y=z=0; }

	void operator () ( F vx, F vy,F vz ) { x=vx; y=vy; z=vz; };
	ILINE Ang3_tpl<F>( F vx, F vy, F vz )	{	x=vx; y=vy; z=vz;	}  

	explicit ILINE Ang3_tpl(const Vec3_tpl<F>& v) : x((F)v.x), y((F)v.y), z((F)v.z) { assert(this->IsValid()); }

	ILINE bool operator==(const Ang3_tpl<F> &vec) { return x == vec.x && y == vec.y && z == vec.z; }
	ILINE bool operator!=(const Ang3_tpl<F> &vec) { return !(*this == vec); }

	ILINE Ang3_tpl<F> operator * (F k) const { return Ang3_tpl<F>(x*k,y*k,z*k); }
	ILINE Ang3_tpl<F> operator / (F k) const { k=(F)1.0/k; return Ang3_tpl<F>(x*k,y*k,z*k); }


	ILINE Ang3_tpl<F>& operator *= (F k) { x*=k;y*=k;z*=k; return *this; }
	//explicit ILINE Ang3_tpl<F>& operator = (const Vec3_tpl<F>& v)  { x=v.x; y=v.y; z=v.z; return *this; 	}

	ILINE Ang3_tpl<F> operator - ( void ) const { return Ang3_tpl<F>(-x,-y,-z); }

	ILINE friend bool operator ==(const Ang3_tpl<F> &v0, const Ang3_tpl<F> &v1)	{
		return ((v0.x==v1.x) && (v0.y==v1.y) && (v0.z==v1.z));
	}
	ILINE void Set(F xval,F yval,F zval) { x=xval; y=yval; z=zval; }

	ILINE bool IsEquivalent( const Ang3_tpl<F>& v1, F epsilon=VEC_EPSILON) const {
		return  ((fabs_tpl(x-v1.x) <= epsilon) &&	(fabs_tpl(y-v1.y) <= epsilon)&&	(fabs_tpl(z-v1.z) <= epsilon));	
	}
	ILINE bool IsInRangePI() const {
		F pi=(F)(gf_PI+0.001); //we need this to fix fp-precision problem 
		return  (  (x>-pi)&&(x<pi) && (y>-pi)&&(y<pi) && (z>-pi)&&(z<pi) );	
	}
	//! normalize angle to -pi and +pi range 
	ILINE void RangePI() {
		if (x< (F)gf_PI) x+=(F)gf_PI2;
		if (x> (F)gf_PI) x-=(F)gf_PI2;
		if (y< (F)gf_PI) y+=(F)gf_PI2;
		if (y> (F)gf_PI) y-=(F)gf_PI2;
		if (z< (F)gf_PI) z+=(F)gf_PI2;
		if (z> (F)gf_PI) z-=(F)gf_PI2;
	}


	//Convert unit quaternion to angle (xyz).
	template<class F1> explicit ILINE Ang3_tpl( const Quat_tpl<F1>& q )
	{
		assert(q.IsValid());
		y = (F)asin_tpl(max((F)-1.0,min((F)1.0,-(q.v.x*q.v.z-q.w*q.v.y)*2)));
		if (fabs_tpl(fabs_tpl(y)-(F)(g_PI*0.5))<(F)0.01)	
		{
			x = (F)0;
			z = (F)atan2_tpl(-2*(q.v.x*q.v.y-q.w*q.v.z),1-(q.v.x*q.v.x+q.v.z*q.v.z)*2);
		} else {
			x = (F)atan2_tpl((q.v.y*q.v.z+q.w*q.v.x)*2, 1-(q.v.x*q.v.x+q.v.y*q.v.y)*2);
			z = (F)atan2_tpl((q.v.x*q.v.y+q.w*q.v.z)*2, 1-(q.v.z*q.v.z+q.v.y*q.v.y)*2);
		}
	}

	//Convert matrix33 to angle (xyz).
	template<class F1> explicit ILINE Ang3_tpl( const Matrix33_tpl<F1>& m )
	{
		assert( m.IsOrthonormalRH(0.001f) );
		y = (F)asin_tpl(max((F)-1.0,min((F)1.0,-m.m20)));
		if (fabs_tpl(fabs_tpl(y)-(F)(g_PI*0.5))<(F)0.01)	
		{
			x = (F)0;
			z = (F)atan2_tpl(-m.m01,m.m11);
		} else {
			x = (F)atan2_tpl(m.m21, m.m22);
			z = (F)atan2_tpl(m.m10, m.m00);
		}
	}

	//Convert matrix34 to angle (xyz).
	template<class F1> explicit ILINE Ang3_tpl( const Matrix34_tpl<F1>& m )
	{
		assert( m.IsOrthonormalRH(0.001f) );
		y = (F)asin_tpl(max((F)-1.0,min((F)1.0,-m.m20)));
		if (fabs_tpl(fabs_tpl(y)-(F)(g_PI*0.5))<(F)0.01)	
		{
			x = (F)0;
			z = (F)atan2_tpl(-m.m01,m.m11);
		} else {
			x = (F)atan2_tpl(m.m21, m.m22);
			z = (F)atan2_tpl(m.m10, m.m00);
		}
	}

	//Convert matrix34 to angle (xyz).
	template<class F1> explicit ILINE Ang3_tpl( const Matrix44_tpl<F1>& m )
	{
		assert( Matrix33(m).IsOrthonormalRH(0.001f) );
		y = (F)asin_tpl(max((F)-1.0,min((F)1.0,-m.m20)));
		if (fabs_tpl(fabs_tpl(y)-(F)(g_PI*0.5))<(F)0.01)	
		{
			x = (F)0;
			z = (F)atan2_tpl(-m.m01,m.m11);
		} else {
			x = (F)atan2_tpl(m.m21, m.m22);
			z = (F)atan2_tpl(m.m10, m.m00);
		}
	}

	template<typename F1>	ILINE static F CreateRadZ( const Vec2_tpl<F1>& v0, const Vec2_tpl<F1>& v1 )
	{
		F cz	= v0.x*v1.y-v0.y*v1.x; 
		F c		=	v0.x*v1.x+v0.y*v1.y;
		return F( atan2(cz,c) );
	}

	template<typename F1>	ILINE static F CreateRadZ( const Vec3_tpl<F1>& v0, const Vec3_tpl<F1>& v1 )
	{
		F cx	= v0.y*v1.z; 
		F cz	= v0.x*v1.y-v0.y*v1.x; 
		F s		=	sgn(cz)*sqrt(cx*cx+cz*cz);
		F c		=	v0.x*v1.x+v0.y*v1.y;
		return F( atan2(s,c) );
	}

	template<typename F1>	ILINE static Ang3_tpl<F> GetAnglesXYZ( const Quat_tpl<F1>& q ) {	return Ang3_tpl<F>(q); }
	template<typename F1>	ILINE void SetAnglesXYZ( const Quat_tpl<F1>& q )	{	*this=Ang3_tpl<F>(q);	}

	template<typename F1>	ILINE static Ang3_tpl<F> GetAnglesXYZ( const Matrix33_tpl<F1>& m ) {	return Ang3_tpl<F>(m); }
	template<typename F1>	ILINE void SetAnglesXYZ( const Matrix33_tpl<F1>& m )	{	*this=Ang3_tpl<F>(m);	}

	//---------------------------------------------------------------
	ILINE F &operator [] (int index)		  { assert(index>=0 && index<=2); return ((F*)this)[index]; }
	ILINE F operator [] (int index) const { assert(index>=0 && index<=2); return ((F*)this)[index]; }


	ILINE bool IsValid() const
	{
		if (!NumberValid(x)) return false;
		if (!NumberValid(y)) return false;
		if (!NumberValid(z)) return false;
		return true;
	}

	AUTO_STRUCT_INFO
};

typedef Ang3_tpl<f32>		Ang3;
typedef Ang3_tpl<real>	Ang3r;
typedef Ang3_tpl<f64>		Ang3_f64;

//---------------------------------------

//vector addition
template<class F1,class F2>
ILINE Ang3_tpl<F1> operator + (const Ang3_tpl<F1> &v0, const Ang3_tpl<F2> &v1) {
	return Ang3_tpl<F1>(v0.x+v1.x, v0.y+v1.y, v0.z+v1.z);
}
//vector subtraction
template<class F1,class F2>
ILINE Ang3_tpl<F1> operator - (const Ang3_tpl<F1> &v0, const Ang3_tpl<F2> &v1) {
	return Ang3_tpl<F1>(v0.x-v1.x, v0.y-v1.y, v0.z-v1.z);
}

//---------------------------------------

//vector self-addition
template<class F1,class F2>
ILINE Ang3_tpl<F1>& operator += (Ang3_tpl<F1> &v0, const Ang3_tpl<F2> &v1) {
	v0.x+=v1.x; v0.y+=v1.y; v0.z+=v1.z; return v0;
}
//vector self-subtraction
template<class F1,class F2>
ILINE Ang3_tpl<F1>& operator -= (Ang3_tpl<F1> &v0, const Ang3_tpl<F2> &v1) {
	v0.x-=v1.x; v0.y-=v1.y; v0.z-=v1.z; return v0;
}


//! normalize the val to 0-360 range 
/*
	ILINE f32 Snap_s360( f32 val ) {
	if( val < 0.0f )
		val =f32( 360.0f + cry_fmod(val,360.0f));
	else
		if(val >= 360.0f)
			val =f32(cry_fmod(val,360.0f));
	return val;
}

//! normalize the val to -180, 180 range 
ILINE f32 Snap_s180( f32 val ) {
	if( val > -180.0f && val < 180.0f)
		return val;
	val = Snap_s360( val );
	if( val>180.0f )
		return -(360.0f - val);
	return val;
}*/


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct CAngleAxis
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <typename F> struct AngleAxis_tpl {

	//! storage for the Angle&Axis coordinates.
	F angle; Vec3_tpl<F> axis;

	// default quaternion constructor
	AngleAxis_tpl( void ) { };
	AngleAxis_tpl( F a, F ax, F ay, F az ) {  angle=a; axis.x=ax; axis.y=ay; axis.z=az; }
	AngleAxis_tpl( F a, Vec3_tpl<F> &n ) { angle=a; axis=n; }
	void operator () ( F a, const Vec3_tpl<F> &n ) {  angle=a; axis=n; }
	AngleAxis_tpl( const AngleAxis_tpl<F>& aa ); //CAngleAxis aa=angleaxis
	const Vec3_tpl<F> operator * ( const Vec3_tpl<F>& v ) const;

	AngleAxis_tpl( const Quat_tpl<F>& q)
	{
		angle = acos_tpl(q.w)*2;
		axis	= q.v;
		axis.Normalize();
		F s = sin_tpl(angle * 0.5);
		if (s == 0)
		{
			angle = 0;
			axis.x = 0;
			axis.y = 0;
			axis.z = 1;
		}
	}

};

typedef AngleAxis_tpl<f32> AngleAxis;
typedef AngleAxis_tpl<f64> AngleAxis_f64;

template<typename F> 
ILINE const Vec3_tpl<F> AngleAxis_tpl<F>::operator * ( const Vec3_tpl<F> &v ) const {
	Vec3_tpl<F> origin 	= axis*(axis|v);
	return 	origin +  (v-origin)*cos_tpl(angle)  +  (axis % v)*sin_tpl(angle);
}

















//////////////////////////////////////////////////////////////////////
template<typename F> struct Plane_tpl
{

	//plane-equation: n.x*x + n.y*y + n.z*z + d > 0 is in front of the plane 
	Vec3_tpl<F>	n;	//!< normal
	F	d;						//!< distance

	//----------------------------------------------------------------	 

#ifdef _DEBUG
	ILINE Plane_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&n.x;		p[0]=F32NAN;	p[1]=F32NAN; p[2]=F32NAN; p[3]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&n.x;		p[0]=F64NAN;	p[1]=F64NAN; p[2]=F64NAN; p[3]=F64NAN;
		}
	}
#else
	ILINE Plane_tpl()	{};
#endif


	ILINE Plane_tpl( const Plane_tpl<F> &p ) {	n=p.n; d=p.d; }
	ILINE Plane_tpl( const Vec3_tpl<F> &normal, const F &distance ) {  n=normal; d=distance; }

	//! set normal and dist for this plane and  then calculate plane type
	ILINE void Set(const Vec3_tpl<F> &vNormal,const F fDist)	{	
		n = vNormal; 
		d = fDist;
	}

	ILINE void SetPlane( const Vec3_tpl<F> &normal, const Vec3_tpl<F> &point ) { 
		n=normal; 
		d=-(point | normal); 
	}
	ILINE static Plane_tpl<F> CreatePlane(  const Vec3_tpl<F> &normal, const Vec3_tpl<F> &point ) {  
		return Plane_tpl<F>( normal,-(point|normal)  );
	}

	ILINE Plane_tpl<F> operator - ( void ) const { return Plane_tpl<F>(-n,-d); }

	/*!
	* Constructs the plane by tree given Vec3s (=triangle) with a right-hand (anti-clockwise) winding
	*
	* Example 1:
	*  Vec3 v0(1,2,3),v1(4,5,6),v2(6,5,6);
	*  Plane_tpl<F>  plane;
	*  plane.SetPlane(v0,v1,v2);
	*
	* Example 2:
	*  Vec3 v0(1,2,3),v1(4,5,6),v2(6,5,6);
	*  Plane_tpl<F>  plane=Plane_tpl<F>::CreatePlane(v0,v1,v2);
	*/
	ILINE void SetPlane( const Vec3_tpl<F> &v0, const Vec3_tpl<F> &v1, const Vec3_tpl<F> &v2 ) {  
		n = ((v1-v0)%(v2-v0)).GetNormalized();	//vector cross-product
		d	=	-(n | v0);				//calculate d-value
	}
	ILINE static Plane_tpl<F> CreatePlane( const Vec3_tpl<F> &v0, const Vec3_tpl<F> &v1, const Vec3_tpl<F> &v2 ) {  
		Plane_tpl<F> p;	p.SetPlane(v0,v1,v2);	return p;
	}

	/*!
	* Computes signed distance from point to plane.
	* This is the standart plane-equation: d=Ax*By*Cz+D.
	* The normal-vector is assumed to be normalized.
	* 
	* Example:
	*  Vec3 v(1,2,3);
	*  Plane_tpl<F>  plane=CalculatePlane(v0,v1,v2);
	*  f32 distance = plane|v;
	*/
	ILINE F operator | ( const Vec3_tpl<F> &point ) const { return (n | point) + d; }
	ILINE F	DistFromPlane(const Vec3_tpl<F> &vPoint) const	{	return (n*vPoint+d); }

	ILINE Plane_tpl<F> operator - ( const Plane_tpl<F> &p) const { return Plane_tpl<F>( n-p.n, d-p.d); }
	ILINE Plane_tpl<F> operator + ( const Plane_tpl<F> &p) const { return Plane_tpl<F>(n+p.n,d+p.d); }
	ILINE void operator -= (const Plane_tpl<F> &p) { d-=p.d; n-=p.n; }
	ILINE Plane_tpl<F> operator * ( F s ) const {	return Plane_tpl<F>(n*s,d*s);	}
	ILINE Plane_tpl<F> operator / ( F s ) const {	return Plane_tpl<F>(n/s,d/s); }

	//! check for equality between two planes
	ILINE  friend	bool operator ==(const Plane_tpl<F> &p1, const Plane_tpl<F> &p2) {
		if (fabsf(p1.n.x-p2.n.x)>0.0001f) return (false);
		if (fabsf(p1.n.y-p2.n.y)>0.0001f) return (false);
		if (fabsf(p1.n.z-p2.n.z)>0.0001f) return (false);
		if (fabsf(p1.d-p2.d)<0.01f) return(true);
		return (false);
	}

	Vec3_tpl<F> MirrorVector(const Vec3_tpl<F>& i)   {	return n*(2* (n|i))-i;	}
	Vec3_tpl<F> MirrorPosition(const Vec3_tpl<F>& i) {  return n*(2* ((n|i)-d))-i;	}

	AUTO_STRUCT_INFO
};

typedef Plane_tpl<f32>	Plane;
typedef Plane_tpl<real>	Planer;
typedef Plane_tpl<f64>	Plane_f64;

//-----------------------------------------------------------------
// define the constants

template <typename T> const Vec3_tpl<T> Vec3Constants<T>::fVec3_Zero(0, 0, 0);
template <typename T> const Vec3_tpl<T> Vec3Constants<T>::fVec3_OneX(1, 0, 0);
template <typename T> const Vec3_tpl<T> Vec3Constants<T>::fVec3_OneY(0, 1, 0);
template <typename T> const Vec3_tpl<T> Vec3Constants<T>::fVec3_OneZ(0, 0, 1);
template <typename T> const Vec3_tpl<T> Vec3Constants<T>::fVec3_One(1, 1, 1);





#endif //vector
