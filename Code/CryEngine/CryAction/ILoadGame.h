// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __ILOADGAME_H__
#define __ILOADGAME_H__

#pragma once

struct ILoadGame
{
	// initialize - set name of game
	virtual bool Init( const char * name ) = 0;

	// get some basic meta-data
	virtual const char * GetMetadata( const char * tag ) = 0;
	virtual bool GetMetadata( const char * tag, int& value ) = 0;
	virtual bool HaveMetadata( const char * tag ) = 0;
	// serialize a console variable
	virtual bool GetConsoleVariable( ICVar * pVar ) = 0;
	// create a serializer for some data section
	virtual std::auto_ptr<TSerialize> GetSection( const char * section ) = 0;
	virtual bool HaveSection( const char * section ) = 0;

	// finish - indicate success (negative success *must* remove file)
	// also calls delete this;
	virtual void Complete() = 0;

	// returns the filename of this savegame
	virtual const char* GetFileName() const = 0;
};

#endif
