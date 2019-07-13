// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IANIMATIONSTATENODE_H__
#define __IANIMATIONSTATENODE_H__

#pragma once

#include "IAnimationGraph.h"
#include "IAnimationGraphSystem.h"

struct AG_FILE
{
	FILE *file;
	
	DynArray<uint8> m_buffer;
	size_t m_curpos;
	DynArray<char> m_strBuffer;

	void SetFile( bool read,FILE *f )
	{
		file = f;
		if (read)
		{
			// Read all buffer.
			m_curpos = gEnv->pCryPak->FTell(file);
			size_t fileSize = gEnv->pCryPak->FGetSize(file);
			m_buffer.resize(fileSize);
			if (fileSize > 0)
				gEnv->pCryPak->FReadRaw( &m_buffer[0],fileSize,1,file);
		}
	}
};

//helper for AG serialization
struct FileSerializationHelper
{
	bool reading;
	bool swappingEndian;
	AG_FILE *ag_file;
	FILE *file;
	ICryPak *pak;

	FileSerializationHelper() : file(NULL), pak(NULL)
	{}

	FileSerializationHelper(bool read, AG_FILE* f)
	{
		Init(read, f);
	}

	void Init(bool read, AG_FILE* f)
	{
		reading = read;
		ag_file = f;
		file = f->file;
		pak = gEnv->pCryPak;
		swappingEndian = false;
#ifdef NEED_ENDIAN_SWAP
		swappingEndian = true;
#endif
	}
	template <class T>
	void ReadFromBuffer( T* data,int elements = 1 )
	{
		memcpy( data,&ag_file->m_buffer[ag_file->m_curpos],sizeof(T)*elements );
		SwapEndian(data,elements);
		ag_file->m_curpos += sizeof(T)*elements;
		//pak->FRead(&val, 1, file, swappingEndian);
	}

	void StringValue(string *s)
	{
		if(!file)
			return;
		if(reading)
		{
			int size = 0;
			ReadFromBuffer(&size);

			ag_file->m_strBuffer.resize(size+1);
			ReadFromBuffer(ag_file->m_strBuffer.begin(),size);
			ag_file->m_strBuffer[size] = 0; // zero terminate
			s->clear();
			s->append( ag_file->m_strBuffer.begin() );
			assert(s->length() == size-1);

			/*
			int size = 0;
			pak->FRead(&size, 1, file, swappingEndian);
			char *str = new char[size];
			pak->FRead(str, size, file, swappingEndian);
			s->clear();
			s->append(str);
			assert(s->length() == size-1);
			delete[] str;
			*/
		}
		else
		{
			int size = 1+ s->length();
			pak->FWrite(&size, 1, file);
			pak->FWrite(s->c_str(), size, file);
		}
	}

	template<class T>
	void Value(T* data, int elements = 1)
	{
		if(!file)
			return;
		if(reading)
		{
			ReadFromBuffer( data,elements );
		}
		else
			pak->FWrite(data, elements, file);
	}

	void Value(CCryName* data, int elements = 1)
	{
		if(!file)
			return;
		for (int i=0; i<elements; i++)
		{
			string temp = data[i].c_str();
			StringValue(&temp);
			data[i] = temp;
		}
	}

	void Value(uint64* data, int elements = 1)
	{
		assert( !"Binary serialization of size_t data types is not supported!" );
	}
};

struct IAnimationStateNodeFactory;

enum EHasEnteredState
{
	eHES_Entered,
	eHES_Waiting,
	eHES_Instant
};

enum EAnimationStateNodeFlags
{
	eASNF_Update = 1,
};

struct SAnimationMovement
{
	SAnimationMovement() : translation(ZERO), rotation(IDENTITY), duration(0.0f) {}
	SAnimationMovement( const Vec3& t, const Quat& r, CTimeValue d) : translation(t), rotation(r), duration(d) {}
	Vec3 translation;
	Quat rotation;
	CTimeValue duration;

	SAnimationMovement& operator+=( const SAnimationMovement& rhs )
	{
		translation += rotation * rhs.translation;
		rotation *= rhs.rotation;
		duration += rhs.duration;
		return *this;
	}
};

struct SAnimationDesc
{
	const SAnimationSelectionProperties* properties;
	SAnimationMovement movement;
	//CTimeValue duration;
	QuatT startLocation;
	bool initialized;

	SAnimationDesc() : initialized(false), properties(NULL), startLocation(IDENTITY) {}
};

struct IAnimationStateNode
{
	IAnimationStateNode( uint32 f = 0 ) : flags(f) {}

	// Description:
	//   Called after instantiation, the state should perform initialization based on data
	virtual void EnterState( SAnimationStateData& data, bool dueToRollback ) = 0;
	// Description:
	//   Called after EnterState... determines when the state has really entered
	virtual EHasEnteredState HasEnteredState( SAnimationStateData& data ) = 0;
	// Description:
	//   Called after EnterState, when at least one state node has returned "Entered"
	virtual void EnteredState( SAnimationStateData& data ) = 0;
	// Description:
	//   Verify that this state can actually leave yet
	virtual bool CanLeaveState( SAnimationStateData& data ) = 0;
	// Description:
	//   Called when the state has completed, this function should cleanup anything in data
	virtual void LeaveState( SAnimationStateData& data ) = 0;
	// Description:
	//   Called after some state has had EnteredState called on it - this state node is then no longer needed (call delete this; and cleanup)
	virtual void LeftState( SAnimationStateData& data, bool wasEntered ) = 0;

	// Description:
	//   If we have eASNF_Update in flags, this will be called every frame
	virtual void Update( SAnimationStateData& data ) {assert(false);}
	// Description:
	//   What is the first time that this node will allow transitioning to another state (or 0)
	virtual void GetCompletionTimes( SAnimationStateData& data, CTimeValue stateStartTime, CTimeValue& mustRunToTime, CTimeValue& requeryTime ) = 0;

	virtual void DebugDraw( SAnimationStateData& data, IRenderer * pRenderer, int x, int& y, int yIncrement ) = 0;

	virtual IAnimationStateNodeFactory * GetFactory() = 0;

	virtual void GetStateMemoryStatistics( ICrySizer * s ) = 0;

	uint32 flags;

protected:

	void SerializeAsFile_NodeBase(bool reading, AG_FILE *file)
	{
		FileSerializationHelper h(reading, file);
		h.Value(&flags);
	}

};

struct IAnimationStateNodeFactory
{
	typedef IAnimationGraphStateFactoryIterator::SStateFactoryParameter Params;

	virtual bool Init( const XmlNodeRef& node, IAnimationGraphPtr pGraph ) = 0;
	virtual IAnimationStateNode * Create() = 0;
	virtual const char * GetCategory() = 0;
	virtual void Release() = 0;
	virtual const char * GetName() = 0;
	virtual const Params * GetParameters() = 0;
	virtual void SerializeAsFile(bool reading, AG_FILE *file) = 0;

	virtual void GetFactoryMemoryStatistics( ICrySizer * s ) = 0;

	virtual bool IsLessThan( IAnimationStateNodeFactory * pFactory ) = 0;
	virtual bool GetForceReentering() const { return false; }
};

#define AG_LT_BEGIN(cls) cls& r = *static_cast<cls*>(pFactory); if (false) 
#define AG_LT_BEGIN_FACTORY(cls) cls& r = *static_cast<cls*>(pFactory); if (flags < r.flags) return true; else if (flags > r.flags) return false
#define AG_LT_BEGIN_PARENT(cls,par) cls& r = *static_cast<cls*>(pFactory); if (par::IsLessThan(pFactory)) return true
#define AG_LT_ELEM(mem) else if (mem < r.mem) return true; else if (mem > r.mem) return false
#define AG_LT_END() return false

#endif
