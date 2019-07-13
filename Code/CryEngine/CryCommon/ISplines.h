////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2005.
// -------------------------------------------------------------------------
//  File name:   IFacialAnimation.h
//  Version:     v1.00
//  Created:     7/10/2005 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ISplines_h__
#define __ISplines_h__

enum ESplineType
{
	ESPLINE_LINEAR,
	ESPLINE_CATMULLROM,
	ESPLINE_HERMIT,
	ESPLINE_TCB
};

//////////////////////////////////////////////////////////////////////////
enum ESplineKeyFlags
{
	ESPLINE_KEY_UI_SELECTED  = 0x10000,
};

//////////////////////////////////////////////////////////////////////////
// This flags are mostly applicable for hermit based splines.
//////////////////////////////////////////////////////////////////////////
enum ESplineKeyTangentType
{
	SPLINE_KEY_TANGENT_CUSTOM  = 1,
	SPLINE_KEY_TANGENT_ZERO    = 2,
	SPLINE_KEY_TANGENT_STEP    = 3,
	SPLINE_KEY_TANGENT_LINEAR  = 4,
};

#define SPLINE_KEY_TANGENT_IN_SHIFT  (0)
#define SPLINE_KEY_TANGENT_IN_MASK   (0x07)
#define SPLINE_KEY_TANGENT_OUT_SHIFT (3)
#define SPLINE_KEY_TANGENT_OUT_MASK  (0x07<<(SPLINE_KEY_TANGENT_OUT_SHIFT))

// Flag relaxes continuity at key to zero order only. Default is 1st order
#define SPLINE_KEY_NONCONTINUOUS_SLOPE				(0x40)

// Return value closest to 0 if same sign, or 0 if opposite.
template<class T>
inline T minmag( T const& a, T const& b)
{
	if ( a*b <= T(0.f) )
		return T(0.f);
	else if (a < T(0.f))
		return max(a,b);
	else
		return min(a,b);
}

template<>
inline Vec3 minmag( Vec3 const& a, Vec3 const& b)
{
	Vec3 m;
	for (int i = 0; i < 3; i++)
		m[i] = minmag(a[i], b[i]);
	return m;
}

//////////////////////////////////////////////////////////////////////////
// Interface returned by backup methods of ISplineInterpolator.
//////////////////////////////////////////////////////////////////////////
struct ISplineBackup
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

//////////////////////////////////////////////////////////////////////////
// General Interpolation interface.
//////////////////////////////////////////////////////////////////////////
struct ISplineInterpolator
{
	typedef float ElemType;
	typedef ElemType ValueType[4];

	// Dimension of the spline from 0 to 3, number of parameters used in ValueType.
	virtual int GetNumDimensions() = 0;

	// Get Spline type 
	virtual ESplineType GetSplineType() = 0;

	// Insert`s a new key, returns index of the key.
	virtual int  InsertKey( float time,ValueType value ) = 0;
	virtual void RemoveKey( int key ) = 0;

	virtual void FindKeysInRange(float startTime, float endTime, int& firstFoundKey, int& numFoundKeys) = 0;
	virtual void RemoveKeysInRange(float startTime, float endTime) = 0;

	virtual int   GetKeyCount() = 0;
	virtual void  SetKeyTime( int key,float time ) = 0;
	virtual float GetKeyTime( int key ) = 0;
	virtual void  SetKeyValue( int key,ValueType value ) = 0;
	virtual bool  GetKeyValue( int key,ValueType &value ) = 0;

	virtual void  SetKeyTangents( int key,ValueType tin,ValueType tout ) = 0;
	virtual bool  GetKeyTangents( int key,ValueType &tin,ValueType &tout ) = 0;

	// Changes key flags, @see ESplineKeyFlags
	virtual void  SetKeyFlags( int key,int flags ) = 0;
	// Retrieve key flags, @see ESplineKeyFlags
	virtual int   GetKeyFlags( int key ) = 0;

	virtual void Interpolate( float time,ValueType &value ) = 0;

	virtual void SerializeSpline( XmlNodeRef &node,bool bLoading ) = 0;

	virtual ISplineBackup* Backup() = 0;
	virtual void Restore(ISplineBackup* pBackup) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Helper functions.
	//////////////////////////////////////////////////////////////////////////
	inline int InsertKeyFloat( float time,float val ) { ValueType v = {val,0,0,0}; return InsertKey(time,v); }
	inline int InsertKeyFloat3( float time,float *vals ) { ValueType v = {vals[0],vals[1],vals[2],0}; return InsertKey(time,v); }
	inline bool GetKeyValueFloat( int key,float &value ) { ValueType v = {value}; bool b = GetKeyValue(key,v); value = v[0]; return b; }
	inline void SetKeyValueFloat( int key,float value ) { ValueType v = {value,0,0,0}; SetKeyValue(key,v); }
	inline void SetKeyValueFloat3( int key,float *vals ) { ValueType v = {vals[0],vals[1],vals[2],0}; SetKeyValue(key,v); }
	inline void InterpolateFloat( float time,float &val ) { ValueType v = {val}; Interpolate(time,v); val=v[0]; }
	inline void InterpolateFloat3( float time,float *vals ) { ValueType v = {vals[0],vals[1],vals[2]}; Interpolate(time,v); vals[0]=v[0]; vals[1]=v[1]; vals[2]=v[2]; }

	// Return Key closest to the specified time.
	inline int FindKey( float fTime,float fEpsilon=0.01f)
	{
		int nKey = -1;
		// Find key.
		for (int k = 0; k < GetKeyCount(); k++)
		{
			if (fabs(GetKeyTime(k)-fTime) < fEpsilon)
			{
				nKey = k;
				break;
			}
		}
		return nKey;
	}

	// Force update.
	void Update() { ValueType val; Interpolate(0.f, val); }
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
namespace spline
{
	template	<int N>
	class	BasisFunction	{
	public:
		const float& operator[]( int i ) const { return m_f[i]; };
	protected:
		float	m_f[N];
	};

	// Special functions that makes parameter zero.
	template <class T>
	inline void Zero( T &val )
	{
		memset( &val,0,sizeof(val) );
	}
	inline float Concatenate( float left, float right) { return left + right; }
	inline Vec3 Concatenate(const Vec3& left, const Vec3& right) { return left + right; }
	inline Quat Concatenate(const Quat& left, const Quat& right) { return left * right; }
	inline float Subtract ( float left, float right) { return left - right; }
	inline Vec3 Subtract (const Vec3& left, const Vec3& right) { return left - right; }
	inline Quat Subtract(const Quat& left, const Quat& right) { return left / right; }

	///////////////////////////////////////////////////////////////////////////////
	// HermitBasis.
	class	HermitBasis : public BasisFunction<4>
	{
	public:
		HermitBasis( float t ) {
			float t2,t3,t2_3,t3_2,t3_t2;

			t2 = t*t;												// t2 = t^2;
			t3 = t2*t;											// t3 = t^3;

			t3_2 = t3 + t3;
			t2_3 = 3*t2;
			t3_t2 = t3 - t2;
			m_f[0] = t3_2 - t2_3 + 1;
			m_f[1] = -t3_2 + t2_3;
			m_f[2] = t3_t2 - t2 + t;
			m_f[3] = t3_t2;
		}
	};

	struct CoeffBasis
	{
		// Coefficients for a cubic polynomial.
		float	m_c[4];

		inline float eval(float t) const
		{
			return m_c[0] + t * (m_c[1] + t * (m_c[2] + t * m_c[3]));
		}

		// Compute coeffs based on 2 endpoints & slopes.
		void set( float t0, float v0, float s0, float t1, float v1, float s1 )
		{
			/*
				Solve cubic equation:
					v(u) = d t^3 + c t^2 + b t + a
				for
					v(0) = v0, v'(0) = s0, v(t1) = v1, v'(t1) = s1

				Solution:
					a	= v0
					b	=						s0
					c	= -3v0 +3v1 -2s0	-s1 
					d = +2v0 -2v1 +s0		+s1
			*/

			/*
				Polynomial will be evaluated on adjusted parameter u == t-t0. u0 = 0, u1 = t1-t0.
				The range is normalised to start at 0 to avoid extra terms in the coefficient
				computation that can compromise precision. However, the range is not normalised
				to length 1, because that would require a division at runtime. Instead, we perform
				the division on the coefficients.
			*/ 
			float dt = t1-t0;
			float idt = dt == 0.f ? 0.0f : 1.f / dt;

			m_c[0] = v0;
			m_c[1] = s0 * idt;
			m_c[2] = (-3.f*v0 + 3.f*v1 -2.f*s0 - s1) * idt*idt;
			m_c[3] = (2.f*v0 - 2.f*v1 + s0 + s1) * idt*idt*idt;

			// Correct coefficient for imprecision, to ensure value at 1 does not go outside of v0..v1 range.
			if (idt > 0.f)
			{
				for (int test = 0; test < 2; test++)
				{
					float cval = eval(t1-t0);
					float err = cval-v1;

					// Only correct for overshooting error.
					if (err * (v1-v0) <= 0.f || !NumberValid(err))
						return;

					// Second pass is just for testing.
					assert(test==0);

					// Apply correct to t^2 coefficient.
					float correct = -2.f*err*idt*idt;

					// Ensure applying LSB correction at minimum.
					if (correct > 0.f)
						correct = max(correct, fabsf(m_c[2]*4e-7f));
					else
						correct = min(correct, -fabsf(m_c[2]*4e-7f));
					m_c[2] += correct;
				}
			}
		}
	};

	template<class T>
	struct TCoeffBasis
	{
		static const int R = sizeof(T) / sizeof(float);

		static inline float* elems(T& t)							{ return (float*)&t; }
		static inline float const* elems(T const& t)	{ return (float const*)&t; }

		// Coefficients for a cubic polynomial.
		CoeffBasis	basis[R];

		inline void eval(T& val, float t) const
		{
			for (int r = 0; r < R; r++)
				elems(val)[r] = basis[r].eval(t);
		}

		void set( float t0, T const& v0, T const& s0, float t1, T const& v1, T const& s1 )
		{
			for (int r = 0; r < R; r++)
				basis[r].set(t0, elems(v0)[r], elems(s0)[r], t1, elems(v1)[r], elems(s1)[r]);
		}
	};

	inline float fast_fmod( float x,float y )
	{
		return cry_fmod( x,y );
		//int ival = ftoi(x/y);
		//return x - ival*y;
	}

	/****************************************************************************
	**                            Key classes																	 **
	****************************************************************************/
	template	<class T>
	struct	SplineKey
	{
		typedef	T	value_type;

		float				time;		//!< Key time.
		int					flags;	//!< Key flags.
		value_type	value;	//!< Key value.
		value_type	ds;			//!< Incoming tangent.
		value_type	dd;			//!< Outgoing tangent.
	};

	template	<class T>
		bool	operator ==( const SplineKey<T> &k1,const SplineKey<T> &k2 ) { return k1.time == k2.time; };
	template	<class T>
		bool	operator !=( const SplineKey<T> &k1,const SplineKey<T> &k2 ) { return k1.time != k2.time; };
	template	<class T>
		bool	operator < ( const SplineKey<T> &k1,const SplineKey<T> &k2 ) { return k1.time < k2.time; };
	template	<class T>
		bool	operator > ( const SplineKey<T> &k1,const SplineKey<T> &k2 ) { return k1.time > k2.time; };

	//////////////////////////////////////////////////////////////////////////
	// TCBSplineKey class
	//////////////////////////////////////////////////////////////////////////
	
	template	<class T>
	struct TCBSplineKey :  public SplineKey<T>
	{
		// Key controls.
		float tens;         //!< Key tension value.
		float cont;         //!< Key continuity value.
		float bias;         //!< Key bias value.
		float easeto;       //!< Key ease to value.
		float easefrom;     //!< Key ease from value.

		TCBSplineKey() { tens = 0, cont = 0, bias = 0, easeto = 0, easefrom = 0; };
	};

	//! TCB spline key used in quaternion spline with angle axis as input.
	struct TCBAngAxisKey :  public TCBSplineKey<CryQuat>
	{
		float angle;
		Vec3 axis;

		TCBAngAxisKey() : axis(0,0,0),angle(0) {};
	};

	//////////////////////////////////////////////////////////////////////////
	// General Spline class
	//////////////////////////////////////////////////////////////////////////
	template <class KeyType,class BasisType>
	class	TSpline
	{
	public:
		typedef	KeyType		key_type;
		typedef typename KeyType::value_type	value_type;
		typedef	BasisType	basis_type;

		// Out of range types.
		enum	{
			ORT_CONSTANT				=	0x0001,	// Constant track.
			ORT_CYCLE						=	0x0002,	// Cycle track
			ORT_LOOP						=	0x0003,	// Loop track.
			ORT_OSCILLATE				=	0x0004,	// Oscillate track.
			ORT_LINEAR					=	0x0005,	// Linear track.
			ORT_RELATIVE_REPEAT	=	0x0007	// Relative repeat track.
		};
		// Spline flags.
		enum	{
			MODIFIED		=	0x0001,	// Track modified.
			MUST_SORT		=	0x0002,	// Track modified and must be sorted.
		};

		/////////////////////////////////////////////////////////////////////////////
		// Methods.
		inline TSpline()
		{
			m_flags = MODIFIED;
			m_ORT = 0;
			m_curr = 0;
			m_rangeStart = 0;
			m_rangeEnd = 0;
		}

		virtual	~TSpline() {};

		ILINE void flag_set( int flag ) { m_flags |= flag; };
		ILINE void flag_clr( int flag ) { m_flags &= ~flag; };
		ILINE int  flag( int flag )  { return m_flags&flag; };

		ILINE void ORT( int ort ) { m_ORT = ort; };
		ILINE int  ORT() const { return m_ORT; };
		ILINE int  isORT( int o ) const { return (m_ORT == o); };

		ILINE void SetRange( float start,float end ) { m_rangeStart = start; m_rangeEnd = end; };
		ILINE float GetRangeStart() const { return m_rangeStart; };
		ILINE float GetRangeEnd() const { return m_rangeEnd; };

		// Keys access methods.
		ILINE void reserve_keys( int n ) { m_keys.reserve(n); };			// Reserve memory for more keys.
		ILINE void clear()               { m_keys.clear(); };
		ILINE void resize( int num )     { m_keys.resize(num); SetModified(true); };			// Set new key count.
		ILINE bool empty() const         { return m_keys.empty(); };		// Check if curve empty (no keys).
		ILINE int		num_keys() const     { return (int)m_keys.size(); };		// Return number of keys in curve.

		ILINE key_type&	key( int n )     { return m_keys[n]; };				// Return n key.
		ILINE float&	time( int n )      { return m_keys[n].time; };	// Shortcut to key n time.
		ILINE value_type&	value( int n ) { return m_keys[n].value; };	// Shortcut to key n value.
		ILINE value_type&	ds( int n )    { return m_keys[n].ds; };		// Shortcut to key n incoming tangent.
		ILINE value_type&	dd( int n )    { return m_keys[n].dd; };		// Shortcut to key n outgoing tangent.

		ILINE key_type const&	key( int n ) const			{ return m_keys[n]; };				// Return n key.
		ILINE float	time( int n ) const								{ return m_keys[n].time; };		// Shortcut to key n time.
		ILINE value_type const&	value( int n ) const	{ return m_keys[n].value; };	// Shortcut to key n value.
		ILINE value_type const&	ds( int n ) const			{ return m_keys[n].ds; };			// Shortcut to key n incoming tangent.
		ILINE value_type const&	dd( int n ) const			{ return m_keys[n].dd; };			// Shortcut to key n outgoing tangent.

		ILINE void erase( int key )						{ m_keys.erase( m_keys.begin() + key ); SetModified(true); };
		ILINE bool closed()										{ return (ORT() == ORT_LOOP); } // return True if curve closed.

		ILINE void SetModified( bool bOn,bool bSort=false )
		{
			if (bOn) m_flags |= MODIFIED; else m_flags &= ~(MODIFIED);
			if (bSort)
				m_flags |= MUST_SORT;
			m_curr = 0;
		}

		ILINE void sort_keys()
		{
			std::sort( m_keys.begin(),m_keys.end() );
			m_flags &= ~MUST_SORT;
		}

		ILINE void push_back( const key_type &k )
		{
			m_keys.push_back( k );
			SetModified(true);
		};
		ILINE int insert_key( const key_type &k )
		{
			int num = num_keys();
			for (int i = 0; i < num; i++)
			{
				if (m_keys[i].time > k.time)
				{
					m_keys.insert( m_keys.begin()+i,k );
					SetModified(true);
					return i;
				}
			}
			m_keys.push_back( k );
			SetModified(true);
			return num_keys()-1;
		};

		ILINE int insert_key( float t, value_type const& val )
		{
			key_type key;
			key.time = t;
			key.value = val;
			key.flags = 0;
			Zero(key.ds);
			Zero(key.dd);
			return insert_key( key );
		}

		ILINE void remove_keys_in_range(float startTime, float endTime)
		{
			int count = num_keys();
			int start = 0;
			int end = count;
			for (int i = 0; i < count; ++i)
			{
				float keyTime = m_keys[i].time;
				if (keyTime < startTime)
					start = i + 1;
				if (keyTime > endTime && end > i)
					end = i;
			}
			if (start < end)
				m_keys.erase(m_keys.begin() + start, m_keys.begin() + end);

			SetModified(true);
		}

		ILINE void find_keys_in_range(float startTime, float endTime, int& firstFoundKey, int& numFoundKeys)
		{
			int count = num_keys();
			int start = 0;
			int end = count;
			for (int i = 0; i < count; ++i)
			{
				float keyTime = m_keys[i].time;
				if (keyTime < startTime)
					start = i + 1;
				if (keyTime > endTime && end > i)
					end = i;
			}
			if (start < end)
			{
				firstFoundKey = start;
				numFoundKeys = end - start;
			}
			else
			{
				firstFoundKey = -1;
				numFoundKeys = 0;
			}

			SetModified(true);
		}

		inline void update()
		{
			if (m_flags&MODIFIED)
			{
				sort_keys();
				if (m_flags&MODIFIED)	
					comp_deriv();
			}
		}

		inline bool is_updated() const
		{
			return (m_flags&MODIFIED) == 0;
		}

		// Interpolate the value along the spline.
		inline void interpolate( float t, value_type& val)
		{
			update();

			if (empty()) 
				return;

			if (t < time(0))
			{
				val = value(0);
				return;
			}

			adjust_time(t);
	
			int curr = seek_key(t);
			if (curr < num_keys()-1)
			{
				assert(t >= time(curr));
				float u = (t - time(curr)) / (time(curr+1) - time(curr));
				interp_keys( curr, curr+1, u, val );
			}	
			else
			{
				val = value(num_keys()-1);
			}
		}

		size_t mem_size() const
		{
			return this->m_keys.capacity() * sizeof(this->m_keys[0]);
		}

		size_t sizeofThis() const
		{
			return sizeof(*this) + mem_size();
		}

		//////////////////////////////////////////////////////////////////////////
		// This two functions must be overridden in derived spline classes.
		//////////////////////////////////////////////////////////////////////////
		// Pre-compute spline tangents.
		virtual void comp_deriv() = 0;
		// Interpolate value between two keys.
		virtual	void interp_keys( int key1, int key2, float u, value_type& val ) = 0;
		//////////////////////////////////////////////////////////////////////////

	protected:
		std::vector<key_type>	m_keys;	// List of keys.
		uint8			m_flags;
		uint8			m_ORT;							// Out-Of-Range type.
		int16			m_curr;							// Current key in track.

		float			m_rangeStart;
		float			m_rangeEnd;

		// Return key before or equal to this time.
		inline int seek_key( float t )
		{
			assert(num_keys() < (1<<15));
			if ((m_curr >= num_keys()) || (time(m_curr) > t))
				// Search from begining.
				m_curr = 0;
			while ((m_curr < num_keys()-1) && (time(m_curr+1) <= t)) 
				++m_curr;
			return m_curr;
		}

		inline void adjust_time( float &t )
		{
			if (isORT(ORT_CYCLE) || isORT(ORT_LOOP))
			{
				if (num_keys() > 0)
				{
					float endtime = time(num_keys()-1);
					if (t > endtime)
					{
						// Warp time.
						t = fast_fmod( t, endtime );
					}
				}
			}
		}

	};

	//////////////////////////////////////////////////////////////////////////
	// CatmullRomSpline class implementation
	//////////////////////////////////////////////////////////////////////////
	template <class T,class Key = SplineKey<T> >
	class	TSplineSlopes : public TSpline< Key,TCoeffBasis<T> >
	{
	protected:
		typedef typename TSpline<Key,HermitBasis>::key_type key_type;
		typedef typename TSpline<Key,HermitBasis>::value_type value_type;

		void comp_derivs(bool bRangeLimit)
		{
			this->SetModified( false );

			int last = this->num_keys()-1;
			if (last <= 0)
				return;

			if (bRangeLimit)
			{
				// Compute slopes to ensure interpolation never exceeds 0..1 bounds.
				// Also examine NonContinuous flag to customise slopes.
				// Compute continuous slopes first.
				for (int i = 0; i <= last; ++i)
				{
					key_type &key = this->key(i);
					if (!(key.flags & SPLINE_KEY_NONCONTINUOUS_SLOPE))
					{
						// In/out slopes equal, and computed automatically.
						if (i==0 || i==last)
							// Slope 0 at endpoints.
							key.ds = key.dd = value_type(0.f);
						else
						{
							value_type s0 = this->value(i) - this->value(i-1),
												 s1 = this->value(i+1) - this->value(i);
							key.ds = key.dd = minmag(s0,s1);
						}
					}
				}

				// Then discontinuous slopes.
				for (int i = 0; i <= last; ++i)
				{
					key_type &key = this->key(i);
					if (key.flags & SPLINE_KEY_NONCONTINUOUS_SLOPE)
					{
						// Left slope.
						if (i>0)
						{
							if (this->key(i-1).flags & SPLINE_KEY_NONCONTINUOUS_SLOPE)
								// Linear between these points.
								key.ds = this->value(i) - this->value(i-1);
							else
								// Match continuous slope on left.
								key.ds = 2.0f * (this->value(i) - this->value(i-1)) - this->dd(i-1);
						}
						else
							Zero(key.ds);

						// Right slope.
						if (i<last)
						{
							if (this->key(i+1).flags & SPLINE_KEY_NONCONTINUOUS_SLOPE)
								// Linear between these points.
								key.dd = this->value(i+1) - this->value(i);
							else
								// Match continuous slope on right.
								key.dd = 2.0f * (this->value(i+1) - this->value(i)) - this->ds(i+1);
						}
						else
							Zero(key.dd);
					}
				}
			}
			else
			{
				key_type &k0 = this->key(0);
				key_type &k1 = this->key(last);
				Zero(k0.ds);
				Zero(k0.dd);
				Zero(k1.ds);
				Zero(k1.dd);

				Zero(k0.ds);
				k0.dd = (0.5f)*(this->value(1) - this->value(0));
				k1.ds = (0.5f)*(this->value(last) - this->value(last-1));
				Zero(k1.dd);

				for (int i = 1; i < (this->num_keys()-1); ++i)
				{
					key_type &key = this->key(i);
					key.ds = 0.5f*(this->value(i+1) - this->value(i-1));
					key.dd = key.ds;
				}
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// CatmullRomSpline class implementation
	//////////////////////////////////////////////////////////////////////////
	template <class T, class Key = SplineKey<T>, bool bRangeLimit = false >
	class	CatmullRomSpline : public TSplineSlopes< T,Key >
	{
	protected:

		typedef TSplineSlopes< T,Key > Super;

		std::vector<typename TSpline< Key, TCoeffBasis<T> >::basis_type> m_coeffs;

		virtual void comp_deriv()
		{
			this->comp_derivs( bRangeLimit );

			// Store coeffs for each segment.
			m_coeffs.resize(this->num_keys());

			if (this->num_keys() > 0)
			{
        unsigned i;
				for (i = 0; i < m_coeffs.size()-1; i++)
				{
					m_coeffs[i].set(this->time(i), this->value(i), this->dd(i), this->time(i+1), this->value(i+1), this->ds(i+1));
				}

				// Last segment is just constant value.
				m_coeffs[i].set(this->time(i), this->value(i), T(0.f), this->time(i)+1.f, this->value(i), T(0.f));
			}
		}

		virtual	void interp_keys( int key1, int key2, float u, typename TSpline<Key,HermitBasis>::value_type& val )
		{
			u *= this->time(key2) - this->time(key1);
			m_coeffs[key1].eval(val, u);
		}

	public:

		// Quick version that skips update() and adjust_time(), and interpolates inline.
		inline void fast_interpolate( float t, typename TSpline<Key,HermitBasis>::value_type& val )
		{
			if (this->empty())
				return;

			assert(this->is_updated());

			if (t < this->time(0))
			{
				val = this->value(0);
				return;
			}

			// Inline seek to proper key.
			int curr;
			for (curr = 0; curr < this->num_keys()-1; curr++)
				if (this->time(curr+1) >= t)
					break;
			m_coeffs[curr].eval(val, t - this->time(curr));
		}

		size_t mem_size() const
		{
			return Super::mem_size() + this->m_coeffs.capacity() * sizeof(this->m_coeffs[0]);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// LookupTableSpline class implementation
	//////////////////////////////////////////////////////////////////////////
	template <class T, class Storage, class Key = SplineKey<T> >
	class	LookupTableSpline: public TSplineSlopes< T,Key >
	{
	public:
		typedef typename TSpline<Key,HermitBasis>::key_type key_type;
		typedef typename TSpline<Key,HermitBasis>::value_type value_type;

		LookupTableSpline()
			: m_indexScale(0)
		{
			this->SetModified(false);
			create_table();
		}
		inline void fast_interpolate( float t, value_type& val )
		{
			assert(this->is_updated());
			t = clamp(t, this->m_rangeStart, this->m_rangeEnd);
			t -= this->m_rangeStart;
			t *= m_indexScale;
			int i = int(t);
			t -= float(i);
			Storage::InterpFromStorage(val, m_table[i], m_table[i+1], t);
		}
		inline void interpolate( float t, value_type& val )
		{
			this->update();
			fast_interpolate( t, val );
		}

		size_t mem_size() const
		{
			return Super::mem_size() + this->m_table.capacity() * sizeof(this->m_table[0]);
		}

	protected:

		typedef TSplineSlopes< T,Key > Super;
		typedef typename Storage::TStorage storage_type;

		enum { nMAX_ENTRIES = 128 };
		enum { nENTRIES_PER_KEY = 32 };

		void create_table()
		{
			// Construct lookup table.
			m_indexScale = 0.f;
			if (this->m_keys.size() > 1)
			{
				// If active spline range less than 1, reduce table entries.
				int table_size = min<int>(nENTRIES_PER_KEY * (this->m_keys.size()-1), nMAX_ENTRIES);
				SetRange( this->m_keys[0].time, this->m_keys.end()[-1].time * 0.999f );
				float active_range = this->m_keys.end()[-1].time - this->m_keys[0].time;
				m_indexScale = (table_size-1) / active_range;
				m_table.resize(table_size);
				float time_scale = active_range / (table_size-1);
				for (int i = 0; i < table_size; i++)
				{
					value_type val(1.f);
					Super::interpolate(this->m_rangeStart + i*time_scale, val);
					m_table[i] = Storage::ToStorage(val);
				}
			}
			else
			{
				this->SetRange(0.f, 0.f);
				m_table.resize(2);
				value_type val(1.f);
				Super::interpolate(0.f, val);
				m_table[0] = Storage::ToStorage(val);
				m_table[1] = m_table[0];
			}
		}

		virtual void comp_deriv()
		{
			this->comp_derivs(true);
			create_table();
		}

		virtual	void interp_keys( int key1, int key2, float u, value_type& val )
		{
			u *= this->time(key2) - this->time(key1);

			// Compute coeffs dynamically.
			TCoeffBasis<T> coeff;
			coeff.set(this->time(key1), this->value(key1), this->dd(key1), this->time(key2), this->value(key2), this->ds(key2));
			coeff.eval(val, u);
		}

		std::vector<storage_type>	m_table;
		float											m_indexScale;
	};

	//////////////////////////////////////////////////////////////////////////
	// Extended version of Hermit Spline.
	// Provides more control on key tangents.
	//////////////////////////////////////////////////////////////////////////
	template <class T,class Key = SplineKey<T> >
	class	HermitSplineEx : public TSpline< Key,HermitBasis >
	{
	public:
		typedef typename TSpline<Key,HermitBasis>::key_type key_type;
		typedef typename TSpline<Key,HermitBasis>::value_type value_type;

		int GetInTangentType( int nkey ) { return (this->key(nkey).flags & SPLINE_KEY_TANGENT_IN_MASK)>>SPLINE_KEY_TANGENT_IN_SHIFT; }
		int GetOutTangentType( int nkey ) { return (this->key(nkey).flags & SPLINE_KEY_TANGENT_OUT_MASK)>>SPLINE_KEY_TANGENT_OUT_SHIFT; }

		virtual void comp_deriv()
		{
			this->SetModified( false );
			if (this->num_keys() > 1)
			{
				int last = this->num_keys()-1;
				key_type &k0 = this->key(0);
				key_type &k1 = this->key(last);
				Zero(k0.ds);
				Zero(k0.dd);
				Zero(k1.ds);
				Zero(k1.dd);

				Zero(k0.ds);
				k0.dd = (0.5f)*(this->value(1) - this->value(0));
				k1.ds = (0.5f)*(this->value(last) - this->value(last-1));
				Zero(k1.dd);

				for (int i = 1; i < (this->num_keys()-1); ++i)
				{
					key_type &key = this->key(i);
					key.ds = 0.5f*(this->value(i+1) - this->value(i-1));
					key.dd = key.ds;
					switch (GetInTangentType(i))
					{
					case SPLINE_KEY_TANGENT_STEP:
						key.ds = value_type();
						break;
					case SPLINE_KEY_TANGENT_ZERO:
						key.ds = value_type();
						break;
					case SPLINE_KEY_TANGENT_LINEAR:
						key.ds = this->value(i) - this->value(i-1);
						break;
					}
					switch (GetOutTangentType(i))
					{
					case SPLINE_KEY_TANGENT_STEP:
						key.dd = value_type();
						break;
					case SPLINE_KEY_TANGENT_ZERO:
						key.dd = value_type();
						break;
					case SPLINE_KEY_TANGENT_LINEAR:
						key.dd = this->value(i+1) - this->value(i);
						break;
					}
				}
			}
		}

	protected:
		virtual	void interp_keys( int from, int to, float u, T& val )
		{
			if (GetInTangentType(to) == SPLINE_KEY_TANGENT_STEP || GetOutTangentType(from) == SPLINE_KEY_TANGENT_STEP)
			{
				val = this->value(from);
				return;
			}
			typename TSpline<Key,HermitBasis >::basis_type basis( u );
			val = (basis[0] * this->value(from)) + (basis[1] * this->value(to)) + (basis[2] * this->dd(from)) + (basis[3] * this->ds(to));
		}
	};


	//////////////////////////////////////////////////////////////////////////
	// Base class for spline interpolators.
	//////////////////////////////////////////////////////////////////////////

	template <typename Key> struct SSplineBackup : public ISplineBackup
	{
		typedef Key key_type;

		SSplineBackup(): refCount(0) {}
		virtual void AddRef() {++refCount;}
		virtual void Release() {if (--refCount <= 0) delete this;}
		int refCount;

		int m_flags;
		int m_ORT;
		std::vector<key_type>	m_keys;
		int m_curr;

		float m_rangeStart;
		float m_rangeEnd;
	};

	template <class value_type,class spline_type>
	class CBaseSplineInterpolator : public ISplineInterpolator, public spline_type
	{
	public:
		typedef typename spline_type::key_type key_type;
		static const int DIM = sizeof(value_type) / sizeof(ElemType);

		//////////////////////////////////////////////////////////////////////////
		inline void ToValueType( const value_type& t, ValueType &v ) { *(value_type*)v = t; }
		inline void FromValueType( ValueType v, value_type& t ) { t = *(value_type*)v; }
		//////////////////////////////////////////////////////////////////////////

		virtual ESplineType GetSplineType() 
		{ 
			return ESPLINE_CATMULLROM; 
		}
		virtual int GetNumDimensions()
		{
			assert(sizeof(value_type) % sizeof(ElemType) == 0);
			return DIM;
		}

		virtual int InsertKey( float t,ValueType val )
		{
			value_type value;
			FromValueType( val, value );
			return insert_key( t, value );
		}
		virtual void RemoveKey( int key )
		{
			if (key >= 0 && key < this->num_keys())
				this->erase( key );
		}
		virtual void FindKeysInRange(float startTime, float endTime, int& firstFoundKey, int& numFoundKeys)
		{
			this->find_keys_in_range(startTime, endTime, firstFoundKey, numFoundKeys);
		}
		virtual void RemoveKeysInRange(float startTime, float endTime)
		{
			this->remove_keys_in_range(startTime, endTime);
		}
		virtual int GetKeyCount() 
		{ 
			return this->num_keys(); 
		};
		virtual float GetKeyTime( int key )
		{
			if (key >= 0 && key < this->num_keys())
				return this->time( key );
			return 0;
		}
		virtual bool GetKeyValue( int key,ValueType &val )
		{
			if (key >= 0 && key < this->num_keys())
			{
				ToValueType( this->value(key),val );
				return true;
			}
			return false;
		}
		virtual void SetKeyValue( int k,ValueType val )
		{
			if (k >= 0 && k < this->num_keys())
			{
				FromValueType( val,this->key(k).value );
				this->SetModified(true);
			}
		}
		virtual void SetKeyTime( int k,float fTime )
		{
			if (k >= 0 && k < this->num_keys())
			{
				this->key(k).time = fTime;
				this->SetModified(true,true);
			}
		}
		virtual void  SetKeyTangents( int k,ValueType tin,ValueType tout )
		{
			if (k >= 0 && k < this->num_keys())
			{
				FromValueType( tin,this->key(k).ds );
				FromValueType( tout,this->key(k).dd );
				this->SetModified(true);
			}
		}
		virtual bool GetKeyTangents( int k,ValueType &tin,ValueType &tout )
		{
			if (k >= 0 && k < this->num_keys())
			{
				ToValueType( this->key(k).ds,tin );
				ToValueType( this->key(k).dd,tout );
				return true;
			}
			else
				return false;
		}
		virtual void  SetKeyFlags( int k,int flags )
		{
			if (k >= 0 && k < this->num_keys())
			{
				this->key(k).flags = flags;
				this->SetModified(true);
			}
		}
		virtual int   GetKeyFlags( int k )
		{
			if (k >= 0 && k < this->num_keys())
			{
				return this->key(k).flags;
			}
			return 0;
		}

		virtual void Interpolate( float time,ValueType &value )
		{
			value_type v;
			interpolate( time,v );
			ToValueType(v,value);
		}

		virtual ISplineBackup* Backup()
		{
			SSplineBackup<key_type>* pBackup = new SSplineBackup<key_type>();
			pBackup->m_flags = this->m_flags;
			pBackup->m_ORT = this->m_ORT;
			pBackup->m_keys = this->m_keys;
			pBackup->m_curr = this->m_curr;
			pBackup->m_rangeStart = this->m_rangeStart;
			pBackup->m_rangeEnd = this->m_rangeEnd;
			return pBackup;
		}

		virtual void Restore(ISplineBackup* p)
		{
			SSplineBackup<key_type>* pBackup = static_cast<SSplineBackup<key_type>*>(p);
			this->m_flags = pBackup->m_flags;
			this->m_ORT = pBackup->m_ORT;
			this->m_keys = pBackup->m_keys;
			this->m_curr = pBackup->m_curr;
			this->m_rangeStart = pBackup->m_rangeStart;
			this->m_rangeEnd = pBackup->m_rangeEnd;
		}
	};

} //namespace spline

#endif // __ISplines_h__
