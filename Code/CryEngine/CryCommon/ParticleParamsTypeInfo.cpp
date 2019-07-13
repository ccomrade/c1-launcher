////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ParticleParamsTypeInfo.h
// -------------------------------------------------------------------------
// Implements TypeInfo for ParticleParams. 
// Include only once per executable.
//
////////////////////////////////////////////////////////////////////////////

#include "TypeInfo_impl.h"
#include "ParticleParams_info.h"

// Implementation of TCurveSpline<T> functions.

	// Helper class for serialization.
	template<class T>
	struct SplineElem
	{
		float fTime;
		T			Value;
		int		nFlags;
		STRUCT_INFO
	};

	// Manually define type info.
	STRUCT_INFO_T_BEGIN(SplineElem, <T>, <class T>)
		STRUCT_VAR_INFO(fTime, TYPE_INFO(float))
		STRUCT_VAR_INFO(Value, TYPE_INFO(T))
		STRUCT_VAR_INFO(nFlags, TYPE_INFO(int))
	STRUCT_INFO_T_END(SplineElem, <T>, <class T>)


	template<class T>
	string TCurveSpline<T>::ToString(int flags) const
	{
		string str;
		for (int i = 0; i < this->num_keys(); i++)
		{
			if (i > 0)
				str += ";";
			SplineElem<T> elem = { this->time(i), this->value(i), this->key(i).flags };
			str += TypeInfo(&elem).ToString(&elem, flags);
		}
		return str;
	}

	template<class T>
	void TCurveSpline<T>::FromString( const char* str )
	{
		char strElem[4096];
		this->resize(0);
		int nStrlen = strlen(str);
		if (nStrlen >= sizeof(strElem)-1)
			nStrlen = sizeof(strElem)-1;
		int nCur = 0;
		while (nCur < nStrlen)
		{
			// Extract element string.
			int nEnd = nStrlen;
			const char *strend = strchr(str+nCur,';');
			if (strend)
				nEnd = strend - str;
			
			strncpy( strElem,str+nCur,nEnd-nCur );
			strElem[nEnd-nCur] = 0;

			nCur = nEnd+1;
			while (nCur < nStrlen && str[nCur] == ' ')
				nCur++;

			// Parse element.
			SplineElem<T> elem = { 0.f, T(0.f), 0 };
			TypeInfo(&elem).FromString(&elem, strElem);

			// Fix any values somehow erroneously serialised.
			Limit(elem.fTime, 0.f, 1.f);
			if (max(elem.Value, T(0.f)) != elem.Value
			|| min(elem.Value, T(1.f)) != elem.Value)
				elem.Value = T(0.f);
			if (elem.nFlags != SPLINE_KEY_NONCONTINUOUS_SLOPE)
				elem.nFlags = 0;

			int nKey = insert_key(elem.fTime, elem.Value);
			this->key(nKey).flags = elem.nFlags;
		};
		this->update();
	}

	template<class T>
	void TCurveSpline<T>::SerializeSpline( XmlNodeRef &node, bool bLoading )
	{
		if (bLoading)
			FromString( node->getAttr( "Keys" ) );
		else
			node->setAttr( "Keys", ToString(CTypeInfo::WRITE_SKIP_DEFAULT | CTypeInfo::WRITE_TRUNCATE_SUB) );
	}

// Implementation of TCurve<T>::TypeInfo()

template<class T>
struct TCurveTypeInfo: CTypeInfo
{
	typedef TCurve<T> STRUCTYPE;

	TCurveTypeInfo()
		: CTypeInfo("TCurve<>", sizeof(STRUCTYPE))
	{}

	virtual string ToString(const void* data, int flags = 0, const void* def_data = 0) const
	{
		// Ignore def_data, spline defaults are always zero.
		return ((const STRUCTYPE*)data)->ToString(flags);
	}
	virtual bool FromString(void* data, cstr str, int flags = 0) const
	{
		((STRUCTYPE*)data)->FromString(str);
		return true;
	}
	virtual size_t GetMemoryUsage(ICrySizer* pSizer, const void* data) const
	{



		return ((STRUCTYPE*)data)->GetMemoryUsage();

	}
};

template<class T>
const CTypeInfo& TCurve<T>::TypeInfo() 
{
	static TCurveTypeInfo<T> Info;
	return Info;
}
