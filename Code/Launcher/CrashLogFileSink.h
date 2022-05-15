#pragma once

#include <cstddef>

#include "Library/CrashLogger.h"

class CrashLogFileSink : public CrashLogger::Sink
{
	const char* m_filename;

public:
	CrashLogFileSink() : m_filename(NULL)
	{
	}

	void SetFileName(const char* filename)
	{
		m_filename = filename;
	}

	void OnCrashData(const std::string& data) override;
};
