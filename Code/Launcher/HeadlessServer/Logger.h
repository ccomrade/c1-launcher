#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "CryCommon/CrySystem/ILog.h"

#include "Library/OS.h"
#include "Library/StdFile.h"

struct ICVar;

class Logger : public ILog
{
	struct Message
	{
		ILog::ELogType type;
		bool isFile;
		bool isConsole;
		std::string prefix;
		std::string content;
	};

	int m_verbosity;
	StdFile m_file;
	std::string m_filePath;
	std::string m_prefix;

	struct CVars
	{
		ICVar* verbosity;
		ICVar* fileVerbosity;
		ICVar* prefix;
	};

	CVars m_cvars;

	OS::Mutex m_mutex;
	unsigned long m_mainThreadID;
	std::vector<Message> m_messages;

	std::vector<ILogCallback*> m_callbacks;

public:
	Logger();
	~Logger();

	void OnUpdate();

	void OpenFile(const char* logPath);
	void CloseFile();

	std::FILE* GetFileHandle() { return m_file.handle; }

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
	void PushMessageV(ILog::ELogType type, bool isFile, bool isConsole, const char* format, va_list args);

	int GetRequiredVerbosity(ILog::ELogType type);

	void BuildMessagePrefix(Message& message);
	void BuildMessageContent(Message& message, const char* format, va_list args);

	void WriteMessage(const Message& message);

	void WriteMessageToFile(const Message& message);
	void WriteMessageToConsole(const Message& message);
};
