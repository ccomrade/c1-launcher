////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2006.
// -------------------------------------------------------------------------
//  File name:   IReadWriteXMLSink.h
//  Version:     v1.00
//  Created:     10-03-2005 by MichaelR 
//  Compilers:   Visual Studio 2005
//  Description: Moved Craig's ReadWriteXMLSink from CryAction to CrySystem
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __I_READWRITEXMLSINK_H__
#define __I_READWRITEXMLSINK_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <IXml.h>
#include "ConfigurableVariant.h"


struct IReadXMLSink;
struct IWriteXMLSource;

struct IReadWriteXMLSink
{
  virtual bool ReadXML( const char * definitionFile, const char * dataFile, IReadXMLSink * pSink ) = 0;
  virtual bool ReadXML( const char * definitionFile, XmlNodeRef node, IReadXMLSink * pSink ) = 0;
  virtual bool ReadXML( XmlNodeRef definition, const char* dataFile, IReadXMLSink * pSink ) = 0;
  virtual bool ReadXML( XmlNodeRef definition, XmlNodeRef node, IReadXMLSink * pSink ) = 0;

  virtual XmlNodeRef CreateXMLFromSource( const char * definitionFile, IWriteXMLSource * pSource ) = 0;
  virtual bool WriteXML( const char * definitionFile, const char * dataFile, IWriteXMLSource * pSource ) = 0;
};


struct SReadWriteXMLCommon
{
  typedef NTypelist::CConstruct<Vec3,int,float,const char*,bool>::TType TValueTypes;
  typedef CConfigurableVariant<TValueTypes, NTypelist::MaximumSize<TValueTypes>::value> TValue;
};


TYPEDEF_AUTOPTR(IReadXMLSink);
typedef IReadXMLSink_AutoPtr IReadXMLSinkPtr;

// this interface allows customization of the data read routines
struct IReadXMLSink : public SReadWriteXMLCommon
{
  // reference counting
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  virtual IReadXMLSinkPtr BeginTable( const char * name, const XmlNodeRef& definition ) = 0;
  virtual IReadXMLSinkPtr BeginTableAt( int elem, const XmlNodeRef& definition ) = 0;
  virtual bool SetValue( const char * name, const TValue& value, const XmlNodeRef& definition ) = 0;
  virtual bool EndTableAt( int elem ) = 0;
  virtual bool EndTable( const char * name ) = 0;

  virtual IReadXMLSinkPtr BeginArray( const char * name, const XmlNodeRef& definition ) = 0;
  virtual bool SetAt( int elem, const TValue& value, const XmlNodeRef& definition ) = 0;
  virtual bool EndArray( const char * name ) = 0;

  virtual bool Complete() = 0;

  virtual bool IsCreationMode() = 0;
  virtual XmlNodeRef GetCreationNode() = 0;  
  virtual void SetCreationNode(XmlNodeRef definition) = 0;

};


TYPEDEF_AUTOPTR(IWriteXMLSource);
typedef IWriteXMLSource_AutoPtr IWriteXMLSourcePtr;

// this interface allows customization of the data write routines
struct IWriteXMLSource : public SReadWriteXMLCommon
{
  // reference counting
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  virtual IWriteXMLSourcePtr BeginTable( const char * name ) = 0;
  virtual IWriteXMLSourcePtr BeginTableAt( int elem ) = 0;
  virtual bool HaveValue( const char * name ) = 0;
  virtual bool GetValue( const char * name, TValue& value, const XmlNodeRef& definition ) = 0;
  virtual bool EndTableAt( int elem ) = 0;
  virtual bool EndTable( const char * name ) = 0;

  virtual IWriteXMLSourcePtr BeginArray( const char * name, size_t * numElems, const XmlNodeRef& definition ) = 0;
  virtual bool HaveElemAt( int elem ) = 0;
  virtual bool GetAt( int elem, TValue& value, const XmlNodeRef& definition ) = 0;
  virtual bool EndArray( const char * name ) = 0;

  virtual bool Complete() = 0;
};

#endif // __I_READWRITEXMLSINK_H__