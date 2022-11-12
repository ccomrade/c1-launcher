// Copyright (C) 2002 Crytek GmbH

#pragma once

struct SValidatorRecord;

/**
 * This interface will be given to Validate methods of the engine for resource and object validation.
 */
struct IValidator
{
	virtual void Report(SValidatorRecord& record) = 0;
};
