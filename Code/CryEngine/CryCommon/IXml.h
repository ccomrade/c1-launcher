////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ixml.h
//  Version:     v1.00
//  Created:     16/7/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ixml_h__
#define __ixml_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <platform.h>
#include <Cry_Math.h>
#include <vector>
#include <set>

#ifdef  SANDBOX_API
#include "Util\GuidUtil.h"
#endif //SANDBOX_API

class IXMLDataSink;
class IXMLBinarySerializer;
struct IReadWriteXMLSink;
struct ISerialize;

/**

This is wrapper arround expat library to provide DOM type of access for xml.
Do not use IXmlNode class directly instead always use XmlNodeRef wrapper that
takes care of memory managment issues.

Usage Example:
-------------------------------------------------------
void testXml()
{
XmlParser xml;
XmlNodeRef root = xml.parse( "test.xml" );

if (root) {
for (int i = 0; i < root->getChildCount(); i++) {
XmlNodeRef child = root->getChild(i);
if (child->isTag("world")) {
if (child->getAttr("name") == "blah") {
}
}
}
}
};
*/

// Special string wrapper for xml nodes.
class XmlString : public string
{
public:
	XmlString() {};
	XmlString( const char *str ) : string(str) {};
#ifdef  SANDBOX_API
	XmlString( const CString &str ) : string( (const char*)str ) {};
#endif // SANDBOX_API

	operator const char*() const { return c_str(); }
};

//////////////////////////////////////////////////////////////////////////
// XML string data.
//////////////////////////////////////////////////////////////////////////
struct IXmlStringData
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual const char* GetString() = 0;
	virtual size_t      GetStringLength() = 0;
};

class IXmlNode;

/**
******************************************************************************
* XmlNodeRef, wrapper class implementing reference counting for IXmlNode.
******************************************************************************
*/
class XmlNodeRef {
private:
	IXmlNode* p;
public:
	XmlNodeRef() : p(NULL) {}
	XmlNodeRef( int Null ) : p(NULL) {}
	XmlNodeRef( IXmlNode* p_ );
	XmlNodeRef( const XmlNodeRef &p_ );
	//explicit XmlNodeRef( const char *tag,IXmlNode *node );
	~XmlNodeRef();

	operator IXmlNode*() const { return p; }
	operator const IXmlNode*() const { return p; }
	IXmlNode& operator*() const { return *p; }
	IXmlNode* operator->(void) const { return p; }

	XmlNodeRef&  operator=( IXmlNode* newp );
	XmlNodeRef&  operator=( const XmlNodeRef &newp );

	operator bool() const { return p != NULL; };
	bool operator !() const { return p == NULL; };

	// Misc compare functions.
	bool  operator == ( const IXmlNode* p2 ) const { return p == p2; };
	bool  operator == ( IXmlNode* p2 ) const { return p == p2; };
	bool  operator != ( const IXmlNode* p2 ) const { return p != p2; };
	bool  operator != ( IXmlNode* p2 ) const { return p != p2; };
	bool  operator <  ( const IXmlNode* p2 ) const { return p < p2; };
	bool  operator >  ( const IXmlNode* p2 ) const { return p > p2; };

	bool  operator == ( const XmlNodeRef &n ) const { return p == n.p; };
	bool  operator != ( const XmlNodeRef &n ) const { return p != n.p; };
	bool  operator <  ( const XmlNodeRef &n ) const { return p < n.p; };
	bool  operator >  ( const XmlNodeRef &n ) const { return p > n.p; };

	friend bool operator == ( const XmlNodeRef &p1,int null );
	friend bool operator != ( const XmlNodeRef &p1,int null );
	friend bool operator == ( int null,const XmlNodeRef &p1 );
	friend bool operator != ( int null,const XmlNodeRef &p1 );
};

/**
******************************************************************************
* IXmlNode class
* Never use IXmlNode directly instead use reference counted XmlNodeRef.
******************************************************************************
*/

class IXmlNode
{
protected:
	int m_nRefCount;

protected:
	virtual void DeleteThis() = 0;
	virtual ~IXmlNode() {};

public:
	//! Create new XML node.
	virtual XmlNodeRef createNode( const char *tag ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//! Reference counting.
	ILINE void AddRef() { m_nRefCount++; };
	//! When ref count reach zero XML node dies.
	ILINE void Release() { if (--m_nRefCount <= 0) DeleteThis(); };

	//////////////////////////////////////////////////////////////////////////
	//! Get XML node tag.
	virtual const char *getTag() const = 0;
	virtual void	setTag( const char *tag ) = 0;

	//! Return true if givven tag equal to node tag.
	virtual bool isTag( const char *tag ) const = 0;

	//! Get XML Node attributes.
	virtual int getNumAttributes() const = 0;
	//! Return attribute key and value by attribute index.
	virtual bool getAttributeByIndex( int index,const char **key,const char **value ) = 0;

	//! Copy attributes to this node from givven node.
	virtual void copyAttributes( XmlNodeRef fromNode ) = 0;

	//! Get XML Node attribute for specified key.
	virtual const char* getAttr( const char *key ) const = 0;
	//! Check if attributes with specified key exist.
	virtual bool haveAttr( const char *key ) const = 0;

	//! Adds new child node.
	virtual void addChild( const XmlNodeRef &node ) = 0;

	//! Creates new xml node and add it to childs list.
	virtual XmlNodeRef newChild( const char *tagName ) = 0;

	//! Remove child node.
	virtual void removeChild( const XmlNodeRef &node ) = 0;

	//! Remove all child nodes.
	virtual void removeAllChilds() = 0;

	//! Remove child node at known position.
	virtual void deleteChildAt( int nIndex ) = 0;

	//! Get number of child XML nodes.
	virtual int	getChildCount() const = 0;

	//! Get XML Node child nodes.
	virtual XmlNodeRef getChild( int i ) const = 0;

	//! Find node with specified tag.
	virtual XmlNodeRef findChild( const char *tag ) const = 0;

	//! Get parent XML node.
	virtual XmlNodeRef getParent() const = 0;

	//! Returns content of this node.
	virtual const char* getContent() const = 0;
	virtual void setContent( const char *str ) = 0;

	//! Deep clone of this and all child xml nodes.
	virtual XmlNodeRef clone() = 0;

	//! Returns line number for XML tag.
	virtual int getLine() const = 0;
	//! Set line number in xml.
	virtual void setLine( int line ) = 0;

	//! Returns XML of this node and sub nodes.
	// IXmlStringData pointer must be release when string is not needed anymore.
	virtual IXmlStringData* getXMLData( int nReserveMem=0 ) const = 0;
	//! Returns XML of this node and sub nodes.
	virtual XmlString getXML( int level=0 ) const = 0;
	virtual bool saveToFile( const char *fileName ) = 0;
	virtual bool saveToFile( const char *fileName, size_t chunkSizeBytes ) = 0; // save in small memory chunks

	//! Set new XML Node attribute (or override attribute with same key).
	virtual void setAttr( const char* key,const char* value ) = 0;
	virtual void setAttr( const char* key,int value ) = 0;
	virtual void setAttr( const char* key,unsigned int value ) = 0;
	virtual void setAttr( const char* key,int64 value ) = 0;
	virtual void setAttr( const char* key,uint64 value ) = 0;
	virtual void setAttr( const char* key,float value ) = 0;
	virtual void setAttr( const char* key,const Vec2& value ) = 0;
	virtual void setAttr( const char* key,const Ang3& value ) = 0;
	virtual void setAttr( const char* key,const Vec3& value ) = 0;
	virtual void setAttr( const char* key,const Quat &value ) = 0;
	//////////////////////////////////////////////////////////////////////////
	// Inline Helpers.
	void setAttr( const char* key,unsigned long value ) { setAttr( key,(unsigned int)value ); };
	void setAttr( const char* key,long value ) { setAttr( key,(int)value ); };
	void setAttr( const char* key,double value ) { setAttr( key,(float)value ); };
	//////////////////////////////////////////////////////////////////////////


	//! Delete attrbute.
	virtual void delAttr( const char* key ) = 0;
	//! Remove all node attributes.
	virtual void removeAllAttributes() = 0;

	//! Get attribute value of node.
	virtual bool getAttr( const char *key,int &value ) const = 0;
	virtual bool getAttr( const char *key,unsigned int &value ) const = 0;
	virtual bool getAttr( const char *key,int64 &value ) const = 0;
	virtual bool getAttr( const char *key,uint64 &value ) const = 0;
	virtual bool getAttr( const char *key,float &value ) const = 0;
	virtual bool getAttr( const char *key,Vec2& value ) const = 0;
	virtual bool getAttr( const char *key,Ang3& value ) const = 0;
	virtual bool getAttr( const char *key,Vec3& value ) const = 0;
	virtual bool getAttr( const char *key,Quat &value ) const = 0;
	virtual bool getAttr( const char *key,bool &value ) const = 0;
	virtual bool getAttr( const char *key,XmlString &value ) const = 0;
	//////////////////////////////////////////////////////////////////////////
	// Inline Helpers.
	bool getAttr( const char *key,long &value ) const { int v; if (getAttr(key,v)) { value = v; return true; } else return false; }
	bool getAttr( const char *key,unsigned long &value ) const { unsigned int v; if (getAttr(key,v)) { value = v; return true; } else return false; }
	bool getAttr( const char *key,unsigned short &value ) const { unsigned int v; if (getAttr(key,v)) { value = v; return true; } else return false; }
	bool getAttr( const char *key,unsigned char &value ) const { unsigned int v; if (getAttr(key,v)) { value = v; return true; } else return false; }
	bool getAttr( const char *key,short &value ) const { int v; if (getAttr(key,v)) { value = v; return true; } else return false; }
	bool getAttr( const char *key,char &value ) const { int v; if (getAttr(key,v)) { value = v; return true; } else return false; }
	bool getAttr( const char *key,double &value ) const { float v; if (getAttr(key,v)) { value = (double)v; return true; } else return false; }

#ifdef  SANDBOX_API
	//////////////////////////////////////////////////////////////////////////
	// Get CString attribute.
	//////////////////////////////////////////////////////////////////////////
	bool getAttr( const char *key,CString &value ) const
	{
		if (!haveAttr(key))
			return false;
		value = getAttr(key);
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Set GUID attribute.
	//////////////////////////////////////////////////////////////////////////
	void setAttr( const char* key,REFGUID value )
	{
		const char *str = GuidUtil::ToString(value);
		setAttr( key,str );
	};

	//////////////////////////////////////////////////////////////////////////
	// Get GUID from attribute.
	//////////////////////////////////////////////////////////////////////////
	bool getAttr( const char *key,GUID &value ) const
	{
		if (!haveAttr(key))
			return false;
		const char *guidStr = getAttr(key);
		value = GuidUtil::FromString( guidStr );
		if (value.Data1 == 0)
		{
			memset( &value,0,sizeof(value) );
			// If bad GUID, use old guid system.
			value.Data1 = atoi(guidStr);
		}
		return true;
	}
#endif //SANDBOX_API

	//! Lets be friendly to him.
	friend class XmlNodeRef;
};

/*
///////////////////////////////////////////////////////////////////////////////
// Inline Implementation of XmlNodeRef
inline XmlNodeRef::XmlNodeRef( const char *tag,IXmlNode *node )
{
if (node)
p = node->createNode( tag );
else
p = new XmlNode( tag );
p->AddRef();
}
*/

//////////////////////////////////////////////////////////////////////////
inline XmlNodeRef::XmlNodeRef( IXmlNode* p_ ) : p(p_)
{
	if (p) p->AddRef();
}

inline XmlNodeRef::XmlNodeRef( const XmlNodeRef &p_ ) : p(p_.p)
{
	if (p) p->AddRef();
}

inline XmlNodeRef::~XmlNodeRef()
{
	if (p) p->Release();
}

inline XmlNodeRef&  XmlNodeRef::operator=( IXmlNode* newp )
{
	if (newp) newp->AddRef();
	if (p) p->Release();
	p = newp;
	return *this;
}

inline XmlNodeRef&  XmlNodeRef::operator=( const XmlNodeRef &newp )
{
	if (newp.p) newp.p->AddRef();
	if (p) p->Release();
	p = newp.p;
	return *this;
}

inline bool operator == ( const XmlNodeRef &p1,int null )	{
	return p1.p == 0;
}

inline bool operator != ( const XmlNodeRef &p1,int null )	{
	return p1.p != 0;
}

inline bool operator == ( int null,const XmlNodeRef &p1 )	{
	return p1.p == 0;
}

inline bool operator != ( int null,const XmlNodeRef &p1 )	{
	return p1.p != 0;
}

//////////////////////////////////////////////////////////////////////////
struct IXmlSerializer
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual ISerialize* GetWriter( XmlNodeRef &node ) = 0;
	virtual ISerialize* GetReader( XmlNodeRef &node ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// XML Parser interface.
//////////////////////////////////////////////////////////////////////////
struct IXmlParser
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	//! Parse xml file.
	virtual XmlNodeRef ParseFile( const char *filename,bool bCleanPools ) = 0;

	//! Parse xml from memory buffer.
	virtual XmlNodeRef ParseBuffer( const char *buffer,int nBufLen,bool bCleanPools ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// IXmlUtils
//////////////////////////////////////////////////////////////////////////
struct IXmlUtils
{
	// Create XML Parser.
	// WARNING!!!
	// IXmlParser does not normally support recursive XML loading, all nodes loaded by this parser are invalidated on loading new file.
	// This is a specialized interface for fast loading of many xmls, 
	// After use it must be released with call to Release method.
	virtual IXmlParser* CreateXmlParser() = 0;

	// Load xml file, return 0 if load failed.
	virtual XmlNodeRef LoadXmlFile( const char *sFilename ) = 0;
	// Load xml from string, return 0 if load failed.
	virtual XmlNodeRef LoadXmlFromString( const char *sXmlString ) = 0;	

	// create an MD5 hash of an XML file
	virtual const char * HashXml( XmlNodeRef node ) = 0;

	// Get an object that can read a xml into a IReadXMLSink 
	// and write a xml from a IWriteXMLSource
	virtual IReadWriteXMLSink* GetIReadWriteXMLSink() = 0;

	// Creates XML Writer for ISerialize interface.
	virtual IXmlSerializer* CreateXmlSerializer() = 0;
};

#endif // __ixml_h__
