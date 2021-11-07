#pragma once

#include <cstdarg>
#include <string>

std::string Format(const char *format, ...);
std::string FormatV(const char *format, va_list args);
