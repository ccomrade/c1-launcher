#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <stdexcept>

namespace WinAPI
{
	//////////////////
	// Command line //
	//////////////////

	const char *CommandLine();

	////////////
	// Errors //
	////////////

	int CurrentErrorCode();

	std::string GetErrorCodeDescription(int code);

	// adds the current error code and its description
	std::runtime_error MakeError(const char *format, ...);

	/////////////
	// Modules //
	/////////////

	void *DLL_Get(const char *name);
	void *DLL_Load(const char *name);
	void *DLL_GetSymbol(void *pDLL, const char *name);
	void DLL_Unload(void *pDLL);

	/////////////////
	// Message box //
	/////////////////

	void ErrorBox(const char *message);

	///////////////
	// Resources //
	///////////////

	int GetCrysisGameBuild(void *pDLL);
	bool IsVistaOrLater();

	///////////
	// Hacks //
	///////////

	int FillNOP(void *address, std::size_t length);
	int FillMem(void *address, const void *data, std::size_t length);
	double GetTSCTicksPerNanosecond();
}
