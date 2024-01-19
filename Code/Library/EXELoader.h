#pragma once

/**
 * Multiple EXEs loaded within a single process is normally impossible, and yet here we are.
 */
namespace EXELoader
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

	struct Result
	{
		void* exe;
		Error error;
		unsigned long sysError;  // GetLastError

		explicit Result(void* exe) : exe(exe), error(NO_ERROR), sysError(0) {}
		explicit Result(Error error, unsigned long sysError = 0) : exe(0), error(error), sysError(sysError) {}
	};

	Result Load(const char* name);

	extern const char* const errorNames[];
}
