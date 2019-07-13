// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __CONFIGURABLEVARIANT_H__
#define __CONFIGURABLEVARIANT_H__

#pragma once

#include "Typelist.h"
#include "ISerialize.h"
#include "CrySizer.h"

// Description
// these declarations belong "spiritually" as inner classes to 
// CConfigurableVariant, however I had some problems coaxing 
// Visual Studio 2003 to compile them when placed there without generating
// an internal compiler error - consequently after about five versions of
// trying to do so, I've placed them in a different namespace
namespace NConfigurableVariantHelpers
{

	template <typename T>
	struct SAbstractVisitDeclarator
	{
		virtual void AbstractVisit( T, void* ) = 0;
	};

	// Description
	// implementation of different methods pertaining to our "vtable" scheme
	// for manipulating types - basically a set of static functions that we
	// can use to manipulate our untyped buffer in the variant
	// - this is annotated with the bool TypeIsBiggerThanBufferSpace to
	//   allow the implementation to either allocate heap (if there is not
	//   enough space in the variant buffer for the type we are using), or to
	//   simply be placed inside of the variant (if we have a small space)
	// * NOTE: for this reason the variant must have BUFFERSIZE >= sizeof(void*) 
	//   iff we wish to place types that have sizeof(T) > BUFFERSIZE
	template <class T, bool TypeIsBiggerThanBufferSpace, class IVisitorClass, class IMutatingVisitorClass>
	struct VTableImpl;

	// Description
	// implementation of VTable methods for "small" objects - objects that
	// fit into CConfigurableVariant::m_buffer
	template <class T, class IVisitorClass, class IMutatingVisitorClass>
	struct VTableImpl< T, false, IVisitorClass, IMutatingVisitorClass >
	{
		// copy construct into buffer at ptr
		static ILINE void Construct( void * ptr, const T& value )
		{
			new (ptr) T(value);
		}
		// return a pointer to the object
		static ILINE T* Get( void * ptr )
		{
			return static_cast<T*>(ptr);
		}
		static ILINE const T* Get( const void * ptr )
		{
			return static_cast<const T*>(ptr);
		}

		// destruct an object
		static void Destruct( void * ptr )
		{
			static_cast<T*>(ptr)->~T();
		}
		// clone an object into a buffer of the same type
		static void Clone( void * pDst, const void * pSrc )
		{
			*static_cast<T*>(pDst) = *static_cast<const T*>(pSrc);
		}
		// clone an object into an uninitialized buffer
		static void ConstructClone( void * pDst, const void * pSrc )
		{
			new (pDst) T(*static_cast<const T*>(pSrc));
		}
		static ILINE void Visit( IVisitorClass * pVisitor, const void * pSrc, void * pImpl )
		{
			((SAbstractVisitDeclarator<const T&>*)pVisitor)->AbstractVisit( *static_cast<const T*>(pSrc), pImpl );
		}
		static ILINE void MutatingVisit( IMutatingVisitorClass * pVisitor, void * pSrc, void * pImpl )
		{
			((SAbstractVisitDeclarator<T&>*)pVisitor)->AbstractVisit( *static_cast<T*>(pSrc), pImpl );
		}
		static ILINE void GetMemoryStatistics(ICrySizer * s, const void *)
		{
		}
		static ILINE int  GetMemorySize(const void *)
		{
			return 0;
		}
	};

	template <class T, class IVisitorClass, class IMutatingVisitorClass>
	struct VTableImpl< T, true, IVisitorClass, IMutatingVisitorClass >
	{
		// copy construct into buffer at ptr
		static void Construct( void * ptr, const T& value )
		{
			*static_cast<T**>(ptr) = new T(value);
		}
		// return a pointer to the object
		static ILINE T* Get( void * ptr )
		{
			return *static_cast<T**>(ptr);
		}
		static ILINE const T* Get( const void * ptr )
		{
			return *static_cast<T*const*>(ptr);
		}

		// destruct an object
		static void Destruct( void * ptr )
		{
			delete *static_cast<T**>(ptr);
		}
		// clone an object into a buffer of the same type
		static void Clone( void * pDst, const void * pSrc )
		{
			**static_cast<T**>(pDst) = **static_cast<T*const*>(pSrc);
		}
		// clone an object into an uninitialized buffer
		static void ConstructClone( void * pDst, const void * pSrc )
		{
			*static_cast<T**>(pDst) = new T(**static_cast<T*const*>(pSrc));
		}
		static void Visit( IVisitorClass * pVisitor, const void * pSrc, void * pImpl )
		{
			((SAbstractVisitDeclarator<const T&>*)pVisitor)->AbstractVisit( **static_cast<T*const*>(pSrc), pImpl );
		}
		static void MutatingVisit( IMutatingVisitorClass * pVisitor, void * pSrc, void * pImpl )
		{
			((SAbstractVisitDeclarator<T&>*)pVisitor)->AbstractVisit( **static_cast<T**>(pSrc), pImpl );
		}
		static ILINE void GetMemoryStatistics(ICrySizer * s, const void * p)
		{
			s->Add(**static_cast<const T*const*>(p));
		}
		static ILINE int GetMemorySize(const void * p)
		{
			return sizeof( **static_cast<const T*const*>(p) );
		}
	};

	template <class TL>
	struct IVisitor;
	template <class H, class T>
	struct IVisitor< NTypelist::CNode<H,T> > : 
		public SAbstractVisitDeclarator<const H&>, 
		public IVisitor<T>
	{
	};
	template <>
	struct IVisitor< NTypelist::CEnd >
	{
	};

	template <class TL>
	struct IMutatingVisitor;
	template <class H, class T>
	struct IMutatingVisitor< NTypelist::CNode<H,T> > : 
		public SAbstractVisitDeclarator<H&>,
		public IMutatingVisitor<T>
	{
	};
	template <>
	struct IMutatingVisitor< NTypelist::CEnd >
	{
	};

	template <class TL, class Impl, class Base>
	class CVisitor;

	template <class H, class T, class Impl, class Base>
	class CVisitor< NTypelist::CNode<H,T>, Impl, Base > : public CVisitor< T, Impl, Base >
	{
	public:
//		using NConfigurableVariantHelpers::CVisitor<T, Impl, Base>::AbstractVisit;
		using SAbstractVisitDeclarator<const H&>::AbstractVisit;
		void /*SAbstractVisitDeclarator<const H&>::*/AbstractVisit( const H& val, void * pImpl ) 
		{
			static_cast<Impl*>(pImpl)->Visit(val);
			//m_impl.Visit( val );
		}
	};
	template <class Impl, class Base>
	class CVisitor< NTypelist::CEnd, Impl, Base > : public Base
	{
	};

	template <class TL, class Impl, class TLL = TL>
	class CMutatingVisitor;

	template <class H, class T, class Impl, class TL>
	class CMutatingVisitor< NTypelist::CNode<H,T>, Impl, TL > : public CMutatingVisitor< T, Impl, TL >
	{
	public:
		using SAbstractVisitDeclarator<H&>::AbstractVisit;
		void /*SAbstractVisitDeclarator<H&>::*/AbstractVisit( H& val, void * pImpl ) 
		{
			static_cast<Impl*>(pImpl)->Visit(val);
			//m_impl.Visit( val );
		}
	};
	template <class Impl, class TL>
	class CMutatingVisitor< NTypelist::CEnd, Impl, TL > : public IMutatingVisitor<TL>
	{
	};

	template <class TL, int i>
	class CSetDefaultForTag;

	template <class H, class T, int i>
	class CSetDefaultForTag< NTypelist::CNode<H,T>, i >
	{
	public:
		template <class Callback>
		static ILINE bool f( int tag, Callback& cb )
		{
			if (tag == i)
			{
				cb( H() );
				return true;
			}
			else
				return CSetDefaultForTag<T,i+1>::f( tag, cb );
		}
	};

	template <int i>
	class CSetDefaultForTag< NTypelist::CEnd, i >
	{
	public:
		template <class Callback>
		static ILINE bool f( int tag, Callback& cb )
		{
			return false;
		}
	};

	template <class To, template <typename, typename> class T_Converter>
	class CConversionVisitor
	{
	public:
		CConversionVisitor( bool& ok, To& value ) : m_ok(ok), m_value(value) {}

		template <class From>
		ILINE void Visit( const From& value )
		{
			m_ok = T_Converter<From,To>::ConvertValue( value, m_value );
		}

	private:
		bool& m_ok;
		To& m_value;
	};

	template <class From, template <typename, typename> class T_Converter>
	class CSetConversionVisitor
	{
	public:
		CSetConversionVisitor( bool& ok, const From& value ) : m_ok(ok), m_value(value) {}

		template <class To>
		ILINE void Visit( To& value )
		{
			m_ok = T_Converter<From,To>::ConvertValue( m_value, value );
		}

	private:
		bool& m_ok;
		const From& m_value;
	};

	template <int SLOT, class TL, template <typename, typename> class T_Converter>
	class CSetConversionFlexi
	{
	public:
		template <class V>
		ILINE static bool func( const V& from, V& to )
		{
			if (from.GetTag() == SLOT)
			{
				typedef typename NTypelist::TypeAt<SLOT, TL> TheTypeAt;
				typedef typename TheTypeAt::type SlotType;
				//SlotType * ptr = from.GetPtrworkarounded( (SlotType*)0 );
				const SlotType * ptr;
				from.GetPtr( &ptr );
				to.SetValueWithConversion( *ptr );
				return true;
			}
			else
				return CSetConversionFlexi<SLOT-1,TL,T_Converter>::func( from, to );
		}
	};
	template <class TL, template <typename, typename> class T_Converter>
	class CSetConversionFlexi<-1,TL,T_Converter>
	{
	public:
		template <class V>
		ILINE static bool func( const V& from, V& to )
		{
			return false;
		}
	};

	template <typename From, typename To>
	class CDefaultVariantConversion
	{
	public:
		static ILINE bool ConvertValue( const From& from, To& to )
		{
			return false;
		}
	};

	template <typename From>
	class CDefaultVariantConversion<From,From>
	{
	public:
		static ILINE bool ConvertValue( const From& from, From& to )
		{
			to = from;
			return true;
		}
	};

	template <typename T>
	class CDefaultVariantConstruction
	{
	public:
		static ILINE T ConstructedValue()
		{
			return T();
		}
	};
	template <>
	class CDefaultVariantConstruction<Vec3>
	{
	public:
		static ILINE Vec3 ConstructedValue()
		{
			return Vec3(ZERO);
		}
	};

	class CSerializeVisitor
	{
		TSerialize m_ser;
	public:
		CSerializeVisitor(TSerialize ser) : m_ser(ser)
		{ 
		}
		template <class T>
		void Visit( T& t )
		{
			m_ser.Value( "v", t );
		}
	};

}

// Description
// Configurable variant class - should be somewhat efficient, and easy to 
// extend
// TL should be a type-list containing the set of types we would like this
// variant to accept
template <
	class TL, 
	size_t BUFFERSIZE = sizeof(void*), 
	template <typename, typename> class T_Converter = NConfigurableVariantHelpers::CDefaultVariantConversion,
	template <typename> class T_Constructor = NConfigurableVariantHelpers::CDefaultVariantConstruction>
class CConfigurableVariant
{
	typedef NConfigurableVariantHelpers::IVisitor<TL> MyIVisitor;
	typedef NConfigurableVariantHelpers::IMutatingVisitor<TL> MyIMutatingVisitor;

	// Description
	// this class allows us to safely manipulate m_buffer, without knowing
	// what is stored there - see GetVTableForType() for details on how these
	// are created
	struct VTable
	{
		int tag;
		void (*Destruct)( void* );
		void (*Clone)( void*, const void* );
		void (*ConstructClone)( void*, const void* );
		void (*Visit)( MyIVisitor*, const void*, void* );
		void (*MutatingVisit)( MyIMutatingVisitor*, void*, void* );
		void (*GetMemoryStatistics)(ICrySizer*, const void*);
		int  (*GetMemorySize)(const void*);
	};

	// Description
	// we can store 2 flags (on a 32-bit platform) in the least-significant
	// bits of the vtable pointer; 
	// one of these we exploit to specify if this variant is "type-locked"
	// -- that is that it will only accept values of its current type
	static const INT_PTR FLAG_ISLOCKED = 1;
	// and the other we allow the user of this class to decide the semantics
	static const INT_PTR FLAG_USER = 2;
	static const INT_PTR FLAGS_MASK = FLAG_ISLOCKED | FLAG_USER;

	// this helper meta-function decides if T is bigger than the buffer for
	// values that we have allocated
	template <class T>
	struct IsBig
	{
		static const bool value = sizeof(T) > BUFFERSIZE;
	};

	template <class T>
	struct SelectVTable
	{
		typedef NConfigurableVariantHelpers::VTableImpl< 
			T, 
			IsBig<T>::value, 
			MyIVisitor, 
			MyIMutatingVisitor > 
			Impl;
	};

	class CSetCallback
	{
	public:
		CSetCallback( CConfigurableVariant * pThis ) : m_pThis(pThis) {}

		template <class T>
		void operator()( const T& value )
		{
			*m_pThis = CConfigurableVariant(value);
		}

	private:
		CConfigurableVariant * m_pThis;
	};

public:
	template <class T>
	struct ConstructType
	{
	private:
		static const int index = NTypelist::IndexOf<T,TL>::value;
	public:
		typedef T_Constructor<T> Helper;
	};

	// default construction sets our value to be of the type of the first
	// element of TL
	CConfigurableVariant()
	{
		typedef typename TL::THead TType;
		Construct( ConstructType<TType>::Helper::ConstructedValue(), false );
	}
	template <class T>
	explicit CConfigurableVariant( const T& value, bool locked = false )
	{
		Construct( value, locked );
	}
	~CConfigurableVariant()
	{
		GetVTable()->Destruct( m_buffer );
	}
	CConfigurableVariant( const CConfigurableVariant& other )
	{
		m_VTableAndFlags = other.m_VTableAndFlags;
		GetVTable()->ConstructClone( m_buffer, other.m_buffer );
	}

	void GetMemoryStatistics(ICrySizer * s) const
	{
		s->Add(*this);
		GetVTable()->GetMemoryStatistics(s, m_buffer);
	}
	int GetMemorySize() const
	{
		return sizeof(*this) + GetVTable()->GetMemorySize(m_buffer);
	}

	// these functions return a pointer to the value of this variant
	// or NULL if the current type is not the one requested....
	// example usage:
	//   int * pInt = variant.GetPtr<int>();
	//   if (pInt)
	//     CryLogAlways("Int is %d", *pInt);
	//   else
	//     CryLogAlways("Not an int");
	template <class T>
	T* GetPtr()
	{
		using namespace NConfigurableVariantHelpers;
		if (Tag<T>() == GetVTable()->tag)
			return SelectVTable<T>::Impl::Get( m_buffer );
		else
			return 0;
	}
	template <class T>
	void GetPtr( T const** where ) const
	{
		using namespace NConfigurableVariantHelpers;
		if (Tag<T>() == GetVTable()->tag)
			*where = SelectVTable<T>::Impl::Get( m_buffer );
		else
			*where = 0;
	}
	template <class T>
	void GetPtr( T** where )
	{
		using namespace NConfigurableVariantHelpers;
		if (Tag<T>() == GetVTable()->tag)
			*where = SelectVTable<T>::Impl::Get( m_buffer );
		else
			*where = 0;
	}
	template <class T>
	const T* GetPtr() const
	{
		using namespace NConfigurableVariantHelpers;
		if (Tag<T>() == GetVTable()->tag)
			return SelectVTable<T>::Impl::Get( m_buffer );
		else
			return 0;
	}
	template <class T>
	bool GetValueWithConversion( T& value ) const
	{
		using namespace NConfigurableVariantHelpers;
		bool ok = false;
		if (Tag<T>() == GetVTable()->tag)
		{
			ok = true;
			value = *SelectVTable<T>::Impl::Get( m_buffer );
		}
		else
		{
			CConversionVisitor<T, T_Converter> visitor(ok, value);
			Visit( visitor );
		}
		return ok;
	}

	// "Visit" the value of this variant... the Visit() function will be called
	// on the passed in class, and this is expected to have an override that
	// is acceptable for each value type - useful for adding type-based
	// additional functionality
	template <class V>
	ILINE void Visit( V& v ) const
	{
		NConfigurableVariantHelpers::CVisitor<TL,V,MyIVisitor> visitor;
		GetVTable()->Visit( &visitor, m_buffer, &v );
	}
	template <class V>
	ILINE void Visit( V& v )
	{
		NConfigurableVariantHelpers::CMutatingVisitor<TL,V> visitor;
		GetVTable()->MutatingVisit( &visitor, m_buffer, &v );
	}

	template <class T>
	static int Tag()
	{
		return NTypelist::IndexOf<T, TL>::value;
	}

	int GetTag() const
	{
		return GetVTable()->tag;
	}

	int GetType() const 
	{
		return GetTag();
	}

	bool SetDefaultForTag( int tag )
	{
		CSetCallback setCallback(this);
		return NConfigurableVariantHelpers::CSetDefaultForTag<TL,0>::f( tag, setCallback );
	}

	// set the value of this variant; returns false if T is a different
	// type to what we currently have and the type is locked
	template <class T>
	ILINE bool Set( const T& value )
	{
		using namespace NConfigurableVariantHelpers;
		bool bSameType = Tag<T>() == GetVTable()->tag;
		if (IsLocked() && !bSameType)
			return false;
		if (bSameType)
		{
			*SelectVTable<T>::Impl::Get( m_buffer ) = value;
		}
		else
		{
			SlowSet( value );
		}
		return true;
	}

	template <class T>
	bool SetValueWithConversion( const T& value )
	{
		using namespace NConfigurableVariantHelpers;
		bool bSameType = Tag<T>() == GetVTable()->tag;
		bool ok = true;
		if (IsLocked() && !bSameType)
		{
			CSetConversionVisitor<T, T_Converter> visitor(ok, value);
			Visit( visitor );
		}
		else if (bSameType)
		{
			*SelectVTable<T>::Impl::Get( m_buffer ) = value;
		}
		else
		{
			SlowSet( value );
		}
		return ok;
	}

	bool Set( const CConfigurableVariant& value )
	{
		using namespace NConfigurableVariantHelpers;
		bool bSameType = value.GetVTable()->tag == GetVTable()->tag;
		if (IsLocked() && !bSameType)
			return false;
		GetVTable()->Destruct(m_buffer);
		SetVTable( value.GetVTable() );
		GetVTable()->ConstructClone(m_buffer, value.m_buffer);
		return true;
	}

	bool SetValueWithConversion( const CConfigurableVariant& value )
	{
		using namespace NConfigurableVariantHelpers;
		bool bSameType = value.GetVTable()->tag == GetVTable()->tag;
		if (IsLocked() && !bSameType)
		{
			return CSetConversionFlexi<NTypelist::Length<TL>::value-1, TL, T_Converter>::func( value, *this );
		}
		else
		{
			GetVTable()->Destruct(m_buffer);
			SetVTable( value.GetVTable() );
			GetVTable()->ConstructClone(m_buffer, value.m_buffer);
			return true;
		}
	}

	// assignment copies all flags
	CConfigurableVariant& operator=( const CConfigurableVariant& rhs )
	{
		if (this != &rhs)
		{
			if (GetVTable() == rhs.GetVTable())
			{
				GetVTable()->Clone( m_buffer, rhs.m_buffer );
				m_VTableAndFlags = rhs.m_VTableAndFlags;
			}
			else
			{
				GetVTable()->Destruct( m_buffer );
				m_VTableAndFlags = rhs.m_VTableAndFlags;
				GetVTable()->ConstructClone( m_buffer, rhs.m_buffer );
			}
		}
		return *this;
	}

	ILINE bool IsLocked() const
	{
		return m_VTableAndFlags & FLAG_ISLOCKED;
	}
	ILINE void SetLocked()
	{
		m_VTableAndFlags |= FLAG_ISLOCKED;
	}
	ILINE void SetUnlocked()
	{
		m_VTableAndFlags &= ~FLAG_ISLOCKED;
	}
	ILINE bool IsUserFlagSet() const
	{
		return (m_VTableAndFlags & FLAG_USER) != 0;
	}
	ILINE void SetUserFlag( bool value )
	{
		if (value)
			m_VTableAndFlags |= FLAG_USER;
		else
			m_VTableAndFlags &= ~FLAG_USER;
	}
	ILINE void ToggleUserFlag()
	{
		m_VTableAndFlags ^= FLAG_USER;
	}
	ILINE void ClearUserFlag()
	{
		m_VTableAndFlags &= ~FLAG_USER;
	}

	void Serialize( TSerialize ser )
	{
//		ser.BeginGroup("Variant");
		if (ser.IsWriting())
		{
			// serialize data type tag
			int tag = GetTag();
			ser.Value("tag", tag);
			// serialize user data flag
			bool bUserDataFlagSet = IsUserFlagSet();
			ser.Value("ud", bUserDataFlagSet);
		}
		else
		{
			int tag = -2; // should be safe
			ser.Value("tag", tag);
			CConfigurableVariant temp;
			temp.SetDefaultForTag(tag);
			SetValueWithConversion( temp );
			// serialize user data flag
			bool bUserDataFlagSet = false;
			ser.Value("ud", bUserDataFlagSet);
			SetUserFlag(bUserDataFlagSet);
		}
		NConfigurableVariantHelpers::CSerializeVisitor visitor(ser);
		Visit( visitor );
//		ser.EndGroup();
	}

private:
	// perform steps necessary to initialize this class with a given
	// type and value
	template <class T>
	ILINE void Construct( const T& value, bool isLocked )
	{
		using namespace NConfigurableVariantHelpers;
		m_VTableAndFlags = 0;
		SelectVTable<T>::Impl::Construct( m_buffer, value );
		SetVTable<T>();
		if (isLocked)
			SetLocked();
	}

	// slow path for Set() function
	template <class T>
	void SlowSet( const T& value )
	{
		using namespace NConfigurableVariantHelpers;
		GetVTable()->Destruct( m_buffer );
		SetVTable( GetVTableForType<T>() );
		SelectVTable<T>::Impl::Construct( m_buffer, value );
	}

	// this function allows access to the VTable pointer for a given type...
	// ie: GetVTableForType<float>() - it translates into a mov typically,
	// with type getting compiled into the applications constant data segment
	template <class T>
	ILINE VTable * GetVTableForType()
	{
		using namespace NConfigurableVariantHelpers;
		typedef typename SelectVTable<T>::Impl Impl;
		static VTable type = {
			NTypelist::IndexOf<T, TL>::value,
			&Impl::Destruct,
			&Impl::Clone,
			&Impl::ConstructClone,
			&Impl::Visit,
			&Impl::MutatingVisit,
			&Impl::GetMemoryStatistics,
			&Impl::GetMemorySize
		};
		return &type;
	}

	// trivial helper function to clean up more complicated code
	template <class T>
	ILINE void SetVTable()
	{
		SetVTable( GetVTableForType<T>() );
	}

	// access the vtable, respecting the fact that we stash flags into this
	// pointer also
	ILINE const VTable * GetVTable() const
	{
		return reinterpret_cast<VTable*>( m_VTableAndFlags & ~FLAGS_MASK );
	}
	// set the vtable, respecting the fact that we stash flags into this
	// pointer also
	ILINE void SetVTable( const VTable * pVTable )
	{
		m_VTableAndFlags = 
			reinterpret_cast<INT_PTR>(pVTable) | 
			(m_VTableAndFlags & FLAGS_MASK);
	}

	// this value contains both the vtable pointer and both of our flags
	INT_PTR m_VTableAndFlags;
	// this untyped buffer stores our value
	// we make it a union with a void* to guarantee at least pointer alignment
	// TODO: perhaps we should do alignment based upon members of the typelist
	// see: http://www.cuj.com/documents/s=7982/cujcexp2006alexandr/ for inspiration
	union
	{
		char m_buffer[BUFFERSIZE];
		void * m_dummy; // not used
	};
};

namespace std
{

	template <class TL, size_t BS>
	struct less< CConfigurableVariant<TL,BS> > :
		public binary_function<CConfigurableVariant<TL,BS>, CConfigurableVariant<TL,BS>, bool>
	{
		class CComparatorVisitor
		{
		public:
			ILINE CComparatorVisitor( const CConfigurableVariant<TL,BS>& b ) : m_b(b) {}
			template <class T> ILINE void Visit( const T& value )
			{
				const T * ptrB = 0;
				m_b.GetPtr(&ptrB);
				assert( ptrB );
				m_result = std::less<T>()( value, *ptrB );
			}
			ILINE bool result() { return m_result; }

		private:
			bool m_result;
			const CConfigurableVariant<TL,BS>& m_b;
		};

		bool operator()( const CConfigurableVariant<TL,BS>& a, const CConfigurableVariant<TL,BS>& b ) const
		{
			int aTag = a.GetTag();
			int bTag = b.GetTag();
			if (aTag < bTag)
				return true;
			else if (aTag == bTag)
			{
				CComparatorVisitor visitor(b);
				a.Visit( visitor );
				return visitor.result();
			}
			else // aTag > bTag
				return false;
		}
	};

}

#endif
