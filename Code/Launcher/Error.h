#pragma once

#include <string>
#include <exception>

#if defined(_MSC_VER) && _MSC_VER < 1900
#define NOEXCEPT throw()
#else
#define NOEXCEPT noexcept  // C++11
#endif

class Error : public std::exception
{
	int m_code;
	std::string m_message;
	std::string m_description;
	std::string m_what;

public:
	Error(const std::string & message)
	: m_code(0),
	  m_message(message),
	  m_description(),
	  m_what()
	{
	}

	Error(const std::string & message, int code, const std::string & description)
	: m_code(code),
	  m_message(message),
	  m_description(description),
	  m_what()
	{
		if (m_code != 0 || !m_description.empty())
		{
			m_what = CreateWhat(m_message, m_code, m_description);
		}
	}

	int getCode() const
	{
		return m_code;
	}

	const std::string & getMessage() const
	{
		return m_message;
	}

	const std::string & getDescription() const
	{
		return m_description;
	}

	const std::string & getWhat() const
	{
		return m_what.empty() ? m_message : m_what;
	}

	const char *what() const NOEXCEPT override
	{
		return getWhat().c_str();
	}

	static std::string CreateWhat(const std::string & message, int code, const std::string & description);
};

struct SystemError : public Error
{
	SystemError(const std::string & message, int code = GetCurrentCode())
	: Error(message, code, GetCodeDescription(code))
	{
	}

	static int GetCurrentCode();

	static std::string GetCodeDescription(int code);
};
