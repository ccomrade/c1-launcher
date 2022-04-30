#pragma once

#include "CryCommon/CrySystem/IValidator.h"

struct NullValidator : public IValidator
{
	void Report(SValidatorRecord&) override
	{
	}
};
