#pragma once

#include <string>

namespace CrashLogger
{
	struct Sink
	{
		virtual void OnCrashData(const std::string& data) = 0;
	};

	void SetSink(Sink& sink);
}