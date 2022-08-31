#pragma once

#include <cstdarg>
#include <cstddef>
#include <stdexcept>
#include <string>

namespace StringTools
{
	std::string Format(const char* format, ...);
	std::string FormatV(const char* format, va_list args);

	std::size_t FormatTo(std::string& result, const char* format, ...);
	std::size_t FormatToV(std::string& result, const char* format, va_list args);

	std::size_t FormatTo(char* buffer, std::size_t bufferSize, const char* format, ...);
	std::size_t FormatToV(char* buffer, std::size_t bufferSize, const char* format, va_list args);

	std::runtime_error Error(const char* format, ...);
	std::runtime_error ErrorV(const char* format, va_list args);

	std::runtime_error OSError(const char* format, ...);
	std::runtime_error OSErrorV(const char* format, va_list args);
}
