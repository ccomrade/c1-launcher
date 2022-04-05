#pragma once

#include <cstdarg>
#include <cstddef>
#include <string>

std::string Format(const char* format, ...);
std::string FormatV(const char* format, va_list args);

std::size_t FormatTo(std::string& result, const char* format, ...);
std::size_t FormatToV(std::string& result, const char* format, va_list args);

std::size_t FormatTo(char* buffer, std::size_t bufferSize, const char* format, ...);
std::size_t FormatToV(char* buffer, std::size_t bufferSize, const char* format, va_list args);
