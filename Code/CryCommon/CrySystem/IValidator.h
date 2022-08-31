// Copyright (C) 2002 Crytek GmbH

#include <cstddef>

#pragma once

enum EValidatorSeverity
{
	VALIDATOR_ERROR,
	VALIDATOR_WARNING,
	VALIDATOR_COMMENT,
};

enum EValidatorModule
{
	VALIDATOR_MODULE_UNKNOWN,
	VALIDATOR_MODULE_RENDERER,
	VALIDATOR_MODULE_3DENGINE,
	VALIDATOR_MODULE_AI,
	VALIDATOR_MODULE_ANIMATION,
	VALIDATOR_MODULE_ENTITYSYSTEM,
	VALIDATOR_MODULE_SCRIPTSYSTEM,
	VALIDATOR_MODULE_SYSTEM,
	VALIDATOR_MODULE_SOUNDSYSTEM,
	VALIDATOR_MODULE_GAME,
	VALIDATOR_MODULE_MOVIE,
	VALIDATOR_MODULE_EDITOR,
	VALIDATOR_MODULE_NETWORK,
	VALIDATOR_MODULE_PHYSICS,
	VALIDATOR_MODULE_FLOWGRAPH,
};

enum EValidatorFlags
{
	VALIDATOR_FLAG_FILE    = (1 << 0),  // Indicate that required file was not found or file was invalid.
	VALIDATOR_FLAG_TEXTURE = (1 << 1),  // Problem with texture.
	VALIDATOR_FLAG_SCRIPT  = (1 << 2),  // Problem with script.
	VALIDATOR_FLAG_SOUND   = (1 << 3),  // Problem with sound.
	VALIDATOR_FLAG_AI      = (1 << 4),  // Problem with AI.
};

struct SValidatorRecord
{
	EValidatorSeverity severity;
	EValidatorModule subsystem;
	const char* text;
	const char* file;
	const char* description;
	int flags;

	SValidatorRecord()
	: severity(VALIDATOR_WARNING),
	  subsystem(VALIDATOR_MODULE_UNKNOWN),
	  text(NULL),
	  file(NULL),
	  description(NULL),
	  flags(0)
	{
	}
};

/**
 * This interface will be given to Validate methods of the engine for resource and object validation.
 */
struct IValidator
{
	virtual void Report(SValidatorRecord& record) = 0;
};
