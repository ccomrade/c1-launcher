#pragma once

/**
 * Multiple EXEs loaded within a single process is normally impossible, and yet here we are.
 */
struct EXELoader
{
#define EXE_LOADER_ERRORS \
	X(NO_ERROR) \
	X(ERROR_OPEN) \
	X(ERROR_OPTIONAL_HEADER) \
	X(ERROR_IMPORT_TABLE_MISSING) \
	X(ERROR_IMPORT_LOAD_LIBRARY) \
	X(ERROR_IAT_MISSING) \
	X(ERROR_IAT_VIRTUAL_PROTECT) \
	X(ERROR_IAT_VIRTUAL_PROTECT_RESTORE) \
	X(ERROR_IAT_GET_PROC_ADDRESS) \
	X(ERROR_TEXT_SECTION_MISSING) \
	X(ERROR_RDATA_SECTION_MISSING) \
	X(ERROR_GLOBAL_CONSTRUCTOR)

	enum Error
	{
#define X(name) name,
		EXE_LOADER_ERRORS
#undef X
	};

	Error error;
	unsigned long sysError;
	const char* errorValue;

	EXELoader() : error(), sysError(), errorValue() {}

	void* Load(const char* name);

	const char* GetErrorName() const;
};
