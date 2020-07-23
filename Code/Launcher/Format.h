#pragma once

#include <cstdarg>
#include <string>

// Beware of non-standard size prefixes in format string.
// For example, %zu and %llu are not supported. Use %Iu and %I64u instead.
std::string Format(const char *format, ...);
std::string FormatV(const char *format, va_list args);
