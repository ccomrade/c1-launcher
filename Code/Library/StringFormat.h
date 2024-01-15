#pragma once

#include <cstdarg>
#include <stdexcept>
#include <string>

std::string StringFormat(const char* format, ...);
std::string StringFormatV(const char* format, va_list args);

void StringFormatTo(std::string& result, const char* format, ...);
void StringFormatToV(std::string& result, const char* format, va_list args);

std::runtime_error StringFormat_Error(const char* format, ...);

std::runtime_error StringFormat_SysError(const char* format, ...);
std::runtime_error StringFormat_SysError(unsigned long sysError, const char* format, ...);
std::runtime_error StringFormatV_SysError(unsigned long sysError, const char* format, va_list args);
