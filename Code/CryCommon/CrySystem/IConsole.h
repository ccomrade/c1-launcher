// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include <cstddef>

class  ICrySizer;
struct ICVar;
class  ITexture;

enum
{
	CVAR_INT    = 1,
	CVAR_FLOAT  = 2,
	CVAR_STRING = 3,
};

enum EVarFlags
{
	VF_CHEAT                = 0x00000002,  // stays in the default state when cheats are disabled
	VF_NOT_NET_SYNCED       = 0x00000080,  // can be changed on client
	VF_DUMPTODISK           = 0x00000100,
	VF_SAVEGAME             = 0x00000200,  // stored when saving a savegame
	VF_NOHELP               = 0x00000400,
	VF_READONLY             = 0x00000800,  // can not be changed by the user
	VF_REQUIRE_LEVEL_RELOAD = 0x00001000,
	VF_REQUIRE_APP_RESTART  = 0x00002000,
	VF_WARNING_NOTUSED      = 0x00004000,  // shows warning that this var was not used in config file
	VF_COPYNAME             = 0x00008000,  // otherwise the const char* to the name will be stored without copying
	VF_MODIFIED             = 0x00010000,  // set when variable value modified
	VF_WASINCONFIG          = 0x00020000,  // set when variable was present in config file
	VF_BITFIELD             = 0x00040000,  // allow bitfield setting syntax
	VF_RESTRICTEDMODE       = 0x00080000,  // is visible and usable in restricted (normal user) console mode
};

struct ICVarDumpSink
{
	virtual void OnElementFound(ICVar* pCVar) = 0;
};

struct IKeyBindDumpSink
{
	virtual void OnKeyBindFound(const char* bind, const char* command) = 0;
};

struct IOutputPrintSink
{
	virtual void Print(const char* text) = 0;
};

struct IConsoleVarSink
{
	virtual bool OnBeforeVarChange(ICVar* pCVar, const char* newValue) = 0;
	virtual void OnAfterVarChange(ICVar* pCVar) = 0;
};

struct IConsoleCmdArgs
{
	virtual int GetArgCount() const = 0;
	virtual const char* GetArg(int index) const = 0;
	virtual const char* GetCommandLine() const = 0;
};

struct IConsoleArgumentAutoComplete
{
	virtual int GetCount() const = 0;
	virtual const char* GetValue(int index) const = 0;
};

typedef void (*ConsoleCommandFunc)(IConsoleCmdArgs*);
typedef void (*ConsoleVarFunc)(ICVar*);

/**
 * CryEngine in-game console interface.
 *
 * The engine console allow to manipulate the internal engine parameters and to invoke commands.
 * This interface allow external modules to integrate their functionalities into the console as commands or variables.
 */
struct IConsole
{
	virtual void Release() = 0;

	virtual ICVar* RegisterString(const char* name, const char* value, int flags,
	                              const char* help = "", ConsoleVarFunc pChangeFunc = NULL) = 0;

	virtual ICVar* RegisterInt(const char* name, int value, int flags,
	                           const char* help = "", ConsoleVarFunc pChangeFunc = NULL) = 0;

	virtual ICVar* RegisterFloat(const char* name, float value, int flags,
	                             const char* help = "", ConsoleVarFunc pChangeFunc = NULL) = 0;

	virtual ICVar* Register(const char* name, float* pValue, float defaultValue, int flags = 0,
	                        const char* help = "", ConsoleVarFunc pChangeFunc = NULL) = 0;

	virtual ICVar* Register(const char* name, int* pValue, int defaultValue, int flags = 0,
	                        const char* help = "", ConsoleVarFunc pChangeFunc = NULL) = 0;

	virtual void UnregisterVariable(const char* name, bool remove = false) = 0;

	virtual void SetScrollMax(int value) = 0;

	virtual void AddOutputPrintSink(IOutputPrintSink* pSink) = 0;
	virtual void RemoveOutputPrintSink(IOutputPrintSink* pSink) = 0;

	virtual void ShowConsole(bool show, const int requestScrollMax = -1) = 0;

	virtual void DumpCVars(ICVarDumpSink* pCallback, unsigned int flagsFilter = 0) = 0;

	virtual void CreateKeyBind(const char* cmd,const char* key) = 0;

	virtual void SetImage(ITexture* pImage, bool removeCurrent) = 0;
	virtual ITexture* GetImage() = 0;

	virtual void StaticBackground(bool isStatic) = 0;

	virtual void SetLoadingImage(const char* filename) = 0;

	virtual bool GetLineNo(const int lineNo, char* buffer, const int bufferSize) const = 0;
	virtual int GetLineCount() const = 0;

	virtual ICVar* GetCVar(const char* name) = 0;
	virtual char* GetVariable(const char* name, const char* filename, const char* defaultValue) = 0;
	virtual float GetVariable(const char* name, const char* filename, float defaultValue) = 0;

	virtual void PrintLine(const char* text) = 0;
	virtual void PrintLinePlus(const char* text) = 0;

	virtual bool GetStatus() = 0;

	virtual void Clear() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void AddCommand(const char* name, ConsoleCommandFunc func, int flags = 0, const char* help = NULL) = 0;
	virtual void AddCommand(const char* name, const char* scriptFunc, int flags = 0, const char* help = NULL) = 0;
	virtual void RemoveCommand(const char* name) = 0;

	virtual void ExecuteString(const char* command) = 0;

	virtual void Exit(const char* command, ...) = 0;

	virtual bool IsOpened() = 0;

	virtual int GetNumVars() = 0;
	virtual std::size_t GetSortedVars(const char** buffer, std::size_t bufferSize, const char* prefix = NULL) = 0;
	virtual const char* AutoComplete(const char* substr) = 0;
	virtual const char* AutoCompletePrev(const char* substr) = 0;
	virtual char* ProcessCompletion(const char* input) = 0;
	virtual void RegisterAutoComplete(const char* name, IConsoleArgumentAutoComplete* pArgAutoComplete) = 0;
	virtual void ResetAutoCompletion() = 0;

	virtual void GetMemoryUsage(ICrySizer* pSizer) = 0;

	virtual void ResetProgressBar(int progressRange) = 0;
	virtual void TickProgressBar() = 0;

	virtual void SetInputLine(const char* line) = 0;

	virtual void DumpKeyBinds(IKeyBindDumpSink* pCallback) = 0;
	virtual const char* FindKeyBind(const char* cmd) const = 0;

	virtual void AddConsoleVarSink(IConsoleVarSink* pSink) = 0;
	virtual void RemoveConsoleVarSink(IConsoleVarSink* pSink) = 0;

	virtual const char* GetHistoryElement(const bool isUpOrDown) = 0;
	virtual void AddCommandToHistory(const char* command) = 0;

	virtual void LoadConfigVar(const char* var, const char* value) = 0;

	virtual void EnableActivationKey(bool enable) = 0;
};

struct ICVar
{
	enum EConsoleLogMode
	{
		eCLM_Off,             // off
		eCLM_ConsoleAndFile,  // normal info to console and file
		eCLM_FileOnly,        // normal info to file only
		eCLM_FullInfo,        // full info to file only
	};

	virtual void Release() = 0;

	virtual int GetIVal() const = 0;
	virtual float GetFVal() const = 0;
	virtual const char* GetString() = 0;

	virtual void Set(const char* value) = 0;
	virtual void ForceSet(const char* value) = 0;
	virtual void Set(const float value) = 0;
	virtual void Set(const int value) = 0;

	virtual void ClearFlags(int flags) = 0;
	virtual int GetFlags() = 0;
	virtual int SetFlags(int flags) = 0;

	virtual int GetType() = 0;  // CVAR_INT, CVAR_FLOAT, CVAR_STRING

	virtual const char* GetName() const = 0;

	virtual const char* GetHelp() = 0;

	virtual void SetOnChangeCallback(ConsoleVarFunc pChangeFunc) = 0;

	virtual void GetMemoryUsage(ICrySizer* pSizer) = 0;

	virtual int GetRealIVal() const = 0;

	virtual void DebugLog(const int expectedValue, const EConsoleLogMode mode) const = 0;
};
