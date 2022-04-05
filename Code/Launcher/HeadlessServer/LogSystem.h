#pragma once

#include <cstdarg>
#include <cstddef>
#include <string>
#include <vector>

#include "CryCommon/CrySystem/ILog.h"

#include "Library/File.h"

struct ICVar;

class Executor;

////////////////////////////////////////////////////////////////////////////////

class LogSystem : public ILog
{
	struct Message
	{
		enum Flags
		{
			FLAG_FILE    = (1 << 0),
			FLAG_CONSOLE = (1 << 1),
			FLAG_APPEND  = (1 << 2),
		};

		ELogType type;
		unsigned int flags;
		std::string prefix;
		std::string content;

		Message() : type(), flags(), prefix(), content()
		{
		}
	};

	int m_verbosity;

	File m_file;
	std::string m_fileName;
	std::string m_rootFolder;

	ICVar* m_verbosityCVar;
	ICVar* m_fileVerbosityCVar;
	ICVar* m_prefixCVar;

	Executor& m_executor;

	std::vector<ILogCallback*> m_callbacks;

public:
	////////////////////////////////////////////////////////////////////////////////

	static int GetDefaultVerbosity();
	static const char* GetDefaultFileName();
	static const char* GetDefaultPrefix();

	////////////////////////////////////////////////////////////////////////////////

	static void StdErr(const char* format, ...);
	static void StdErrV(const char* format, va_list args);

	////////////////////////////////////////////////////////////////////////////////

	LogSystem(Executor& executor, const std::string& rootFolder, int verbosity = GetDefaultVerbosity());
	~LogSystem();

	////////////////////////////////////////////////////////////////////////////////

	void LogV(ELogType type, const char* format, va_list args) override;

	void Log(const char* format, ...) override;
	void LogWarning(const char* format, ...) override;
	void LogError(const char* format, ...) override;

	void Release() override;

	bool SetFileName(const char* fileName) override;
	const char* GetFileName() override;

	void LogPlus(const char* format, ...) override;
	void LogToFile(const char* format, ...) override;
	void LogToFilePlus(const char* format, ...) override;
	void LogToConsole(const char* format, ...) override;
	void LogToConsolePlus(const char* format, ...) override;

	void UpdateLoadingScreen(const char* format, ...) override;

	void RegisterConsoleVariables() override;
	void UnregisterConsoleVariables() override;

	void SetVerbosity(int verbosity) override;
	int GetVerbosityLevel() override;

	void AddCallback(ILogCallback* pCallback) override;
	void RemoveCallback(ILogCallback* pCallback) override;

	////////////////////////////////////////////////////////////////////////////////

private:

	////////////////////////////////////////////////////////////////////////////////

	void PushMessage(ELogType type, unsigned int flags, const char* format, ...);
	void PushMessageV(ELogType type, unsigned int flags, const char* format, va_list args);

	int GetRequiredVerbosity(ELogType type);

	////////////////////////////////////////////////////////////////////////////////

	void BuildPrefix(Message& message);
	void BuildContent(Message& message, const char* format, va_list args);

	////////////////////////////////////////////////////////////////////////////////

	void WriteMessage(const Message& message);

	void WriteMessageToFile(const Message& message);
	void WriteMessageToConsole(const Message& message);

	////////////////////////////////////////////////////////////////////////////////

	void OpenLogFile();
	void BackupLogFile();

	static std::string ExtractBackupNameAttachment(const std::string& firstLine);
	static std::string CreateBackupFileName(const std::string& fileName, const std::string& backupNameAttachment);

	////////////////////////////////////////////////////////////////////////////////
};
