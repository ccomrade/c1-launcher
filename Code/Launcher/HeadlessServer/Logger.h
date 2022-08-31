#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "CryCommon/CrySystem/ILog.h"

#include "Library/Mutex.h"
#include "Library/OS.h"

struct ICVar;

class Logger : public ILog
{
	struct Message
	{
		enum Flags
		{
			FLAG_FILE    = (1 << 0),
			FLAG_CONSOLE = (1 << 1),
			FLAG_APPEND  = (1 << 2),
		};

		ILog::ELogType type;
		unsigned int flags;
		std::string prefix;
		std::string content;
	};

	int m_verbosity;
	OS::File m_file;
	std::string m_filePath;
	std::string m_prefix;

	struct CVars
	{
		ICVar* verbosity;
		ICVar* fileVerbosity;
		ICVar* prefix;
	};

	CVars m_cvars;

	Mutex m_mutex;
	unsigned long m_mainThreadID;
	std::vector<Message> m_messages;

	std::vector<ILogCallback*> m_callbacks;

public:
	Logger();
	~Logger();

	void OnUpdate();

	void OpenFile(const char* filePath);
	void CloseFile();
	std::FILE* ReleaseFile();

	void SetPrefix(const char* prefix);

	////////////////////////////////////////////////////////////////////////////////
	// ILog
	////////////////////////////////////////////////////////////////////////////////

	void LogV(ILog::ELogType type, const char* format, va_list args) override;

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
	void PushMessage(ILog::ELogType type, unsigned int flags, const char* format, ...);
	void PushMessageV(ILog::ELogType type, unsigned int flags, const char* format, va_list args);

	int GetRequiredVerbosity(ILog::ELogType type);

	void BuildMessagePrefix(Message& message);
	void BuildMessageContent(Message& message, const char* format, va_list args);

	void WriteMessage(const Message& message);

	void WriteMessageToFile(const Message& message);
	void WriteMessageToConsole(const Message& message);
};
