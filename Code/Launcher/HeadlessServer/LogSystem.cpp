#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/CryColorCode.h"

#include "Library/Format.h"
#include "Library/Path.h"
#include "Library/WinAPI.h"

#include "LogSystem.h"
#include "Executor.h"

////////////////////////////////////////////////////////////////////////////////

int LogSystem::GetDefaultVerbosity()
{
	return std::atoi(WinAPI::CmdLine::GetArgValue("-verbosity", "0"));
}

const char* LogSystem::GetDefaultFileName()
{
	return WinAPI::CmdLine::GetArgValue("-logfile", "Server.log");
}

const char* LogSystem::GetDefaultPrefix()
{
	return WinAPI::CmdLine::GetArgValue("-logprefix", "");
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::StdErr(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	StdErrV(format, args);
	va_end(args);
}

void LogSystem::StdErrV(const char* format, va_list args)
{
	std::vfprintf(stderr, format, args);
	std::fputc('\n', stderr);
	std::fflush(stderr);
}

////////////////////////////////////////////////////////////////////////////////

LogSystem::LogSystem(Executor& executor, const std::string& rootFolder, int verbosity)
: m_verbosity(verbosity),
  m_file(),
  m_fileName(),
  m_rootFolder(rootFolder),
  m_verbosityCVar(NULL),
  m_fileVerbosityCVar(NULL),
  m_prefixCVar(NULL),
  m_executor(executor),
  m_callbacks()
{
}

LogSystem::~LogSystem()
{
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::LogV(ELogType type, const char* format, va_list args)
{
	PushMessageV(type, Message::FLAG_FILE | Message::FLAG_CONSOLE, format, args);
}

void LogSystem::Log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eMessage, format, args);
	va_end(args);
}

void LogSystem::LogWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eWarning, format, args);
	va_end(args);
}

void LogSystem::LogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eError, format, args);
	va_end(args);
}

void LogSystem::Release()
{
}

bool LogSystem::SetFileName(const char* fileName)
{
	m_fileName = fileName;

	try
	{
		OpenLogFile();
	}
	catch (const std::exception& ex)
	{
		StdErr("Log file open error: %s", ex.what());
		return false;
	}

	return true;
}

const char* LogSystem::GetFileName()
{
	return m_fileName.c_str();
}

void LogSystem::LogPlus(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	PushMessageV(ILog::eMessage, Message::FLAG_FILE | Message::FLAG_CONSOLE | Message::FLAG_APPEND, format, args);
	va_end(args);
}

void LogSystem::LogToFile(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	PushMessageV(ILog::eMessage, Message::FLAG_FILE, format, args);
	va_end(args);
}

void LogSystem::LogToFilePlus(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	PushMessageV(ILog::eMessage, Message::FLAG_FILE | Message::FLAG_APPEND, format, args);
	va_end(args);
}

void LogSystem::LogToConsole(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	PushMessageV(ILog::eMessage, Message::FLAG_CONSOLE, format, args);
	va_end(args);
}

void LogSystem::LogToConsolePlus(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	PushMessageV(ILog::eMessage, Message::FLAG_CONSOLE | Message::FLAG_APPEND, format, args);
	va_end(args);
}

void LogSystem::UpdateLoadingScreen(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eMessage, format, args);
	va_end(args);

	// server has no loading screen
}

void LogSystem::RegisterConsoleVariables()
{
	if (!gEnv)
		return;

	IConsole* pConsole = gEnv->pConsole;

	if (!pConsole)
		return;

	m_verbosityCVar = pConsole->RegisterInt("log_Verbosity", m_verbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for console log messages (use log_FileVerbosity for file logging).\n"
	  "Usage: log_Verbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	m_fileVerbosityCVar = pConsole->RegisterInt("log_FileVerbosity", m_verbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for file log messages (if log_Verbosity is higher, this one is used).\n"
	  "Usage: log_FileVerbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	m_prefixCVar = pConsole->RegisterString("log_Prefix", GetDefaultPrefix(), VF_NOT_NET_SYNCED,
	  "Defines prefix of each message written to the log file.\n"
	  "Usage: log_Prefix FORMAT\n"
	  "The format string consists of normal characters and the following conversion specifiers:\n"
	  "  %% = %\n"
	  "  %d = Day of the month (01..31)\n"
	  "  %m = Month (01..12)\n"
	  "  %Y = Year (e.g. 2007)\n"
	  "  %H = Hour (00..23)\n"
	  "  %M = Minute (00..59)\n"
	  "  %S = Second (00..60)\n"
	  "  %N = Millisecond (000..999)\n"
	  "  %z = Offset from UTC (time zone) in the ISO 8601 format (e.g. +0100)\n"
	  "  %F = Equivalent to \"%Y-%m-%d\" (the ISO 8601 date format)\n"
	  "  %T = Equivalent to \"%H:%M:%S\" (the ISO 8601 time format)\n"
	  "  %t = Thread ID where the message was logged"
	);
}

void LogSystem::UnregisterConsoleVariables()
{
}

void LogSystem::SetVerbosity(int verbosity)
{
	if (m_verbosityCVar)
		m_verbosityCVar->Set(verbosity);
	else
		m_verbosity = verbosity;
}

int LogSystem::GetVerbosityLevel()
{
	if (m_verbosityCVar)
		return m_verbosityCVar->GetIVal();
	else
		return m_verbosity;
}

void LogSystem::AddCallback(ILogCallback* pCallback)
{
	if (pCallback && std::count(m_callbacks.begin(), m_callbacks.end(), pCallback) == 0)
	{
		m_callbacks.push_back(pCallback);
	}
}

void LogSystem::RemoveCallback(ILogCallback* pCallback)
{
	m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), pCallback), m_callbacks.end());
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::OnCrashData(const std::string& data)
{
	if (m_file.IsOpen())
	{
		m_file.Write(data);
	}
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::PushMessage(ELogType type, unsigned int flags, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	PushMessageV(type, flags, format, args);
	va_end(args);
}

void LogSystem::PushMessageV(ELogType type, unsigned int flags, const char* format, va_list args)
{
	if (!format)
	{
		// drop messages with null format
		return;
	}

	const int currentVerbosity = GetVerbosityLevel();
	const int requiredVerbosity = GetRequiredVerbosity(type);

	if (currentVerbosity < requiredVerbosity)
	{
		// drop messages above the current verbosity level
		return;
	}

	const int currentFileVerbosity = (m_fileVerbosityCVar) ? m_fileVerbosityCVar->GetIVal() : currentVerbosity;

	if (currentVerbosity > currentFileVerbosity && currentFileVerbosity < requiredVerbosity)
	{
		flags &= ~Message::FLAG_FILE;
	}

	if (m_executor.IsMainThread())
	{
		Message message;
		message.type = type;
		message.flags = flags;

		BuildPrefix(message);
		BuildContent(message, format, args);

		WriteMessage(message);
	}
	else
	{
		struct PushMessageTask : public IExecutorMainThreadTask
		{
			LogSystem* self;
			Message message;

			PushMessageTask() : self(), message()
			{
			}

			void RunOnMainThread() override
			{
				self->WriteMessage(message);
			}
		};

		PushMessageTask* task = new PushMessageTask;
		task->self = this;
		task->message.type = type;
		task->message.flags = flags;

		BuildPrefix(task->message);
		BuildContent(task->message, format, args);

		m_executor.SubmitTask(task);
	}
}

int LogSystem::GetRequiredVerbosity(ELogType type)
{
	switch (type)
	{
		case ILog::eAlways:
		case ILog::eWarningAlways:
		case ILog::eErrorAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			return 0;
		}
		case ILog::eError:
		{
			return 1;
		}
		case ILog::eWarning:
		{
			return 2;
		}
		case ILog::eMessage:
		{
			return 3;
		}
		case ILog::eComment:
		{
			return 4;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::BuildPrefix(Message& message)
{
	if (!m_prefixCVar)
		return;

	const char* prefix = m_prefixCVar->GetString();

	// empty string or "0" means log prefix is disabled
	if (!prefix || !prefix[0] || (prefix[0] == '0' && !prefix[1]))
		return;

	const WinAPI::DateTime dateTime = WinAPI::DateTime::GetCurrentLocal();

	for (std::size_t i = 0; prefix[i]; i++)
	{
		if (prefix[i] == '%')
		{
			switch (prefix[i+1])
			{
				case '%':
				{
					message.prefix += '%';
					break;
				}
				case 't':
				{
					FormatTo(message.prefix, "%04lX", WinAPI::GetCurrentThreadID());
					break;
				}
				case 'd':
				{
					FormatTo(message.prefix, "%02u", dateTime.day);
					break;
				}
				case 'm':
				{
					FormatTo(message.prefix, "%02u", dateTime.month);
					break;
				}
				case 'Y':
				{
					FormatTo(message.prefix, "%04u", dateTime.year);
					break;
				}
				case 'F':
				{
					FormatTo(message.prefix, "%04u", dateTime.year);
					message.prefix += '-';
					FormatTo(message.prefix, "%02u", dateTime.month);
					message.prefix += '-';
					FormatTo(message.prefix, "%02u", dateTime.day);
					break;
				}
				case 'H':
				{
					FormatTo(message.prefix, "%02u", dateTime.hour);
					break;
				}
				case 'M':
				{
					FormatTo(message.prefix, "%02u", dateTime.minute);
					break;
				}
				case 'S':
				{
					FormatTo(message.prefix, "%02u", dateTime.second);
					break;
				}
				case 'T':
				{
					FormatTo(message.prefix, "%02u", dateTime.hour);
					message.prefix += ':';
					FormatTo(message.prefix, "%02u", dateTime.minute);
					message.prefix += ':';
					FormatTo(message.prefix, "%02u", dateTime.second);
					break;
				}
				case 'N':
				{
					FormatTo(message.prefix, "%03u", dateTime.millisecond);
					break;
				}
				case 'z':
				{
					WinAPI::DateTime::AddTimeZoneOffset(message.prefix);
					break;
				}
			}

			if (prefix[i+1])
			{
				i++;
			}
		}
		else
		{
			message.prefix += prefix[i];
		}
	}

	if (!message.prefix.empty())
	{
		// space after the prefix
		message.prefix += ' ';
	}
}

void LogSystem::BuildContent(Message& message, const char* format, va_list args)
{
	switch (message.type)
	{
		case ILog::eWarning:
		case ILog::eWarningAlways:
		{
			message.content += CRY_COLOR_CODE_YELLOW "[Warning] ";
			break;
		}
		case ILog::eError:
		case ILog::eErrorAlways:
		{
			message.content += CRY_COLOR_CODE_RED "[Error] ";
			break;
		}
		case ILog::eComment:
		{
			message.content += CRY_COLOR_CODE_GRAY;
			break;
		}
		case ILog::eMessage:
		case ILog::eAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			break;
		}
	}

	FormatToV(message.content, format, args);
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::WriteMessage(const Message& message)
{
	if (message.flags & Message::FLAG_FILE)
	{
		WriteMessageToFile(message);
	}

	if (message.flags & Message::FLAG_CONSOLE)
	{
		WriteMessageToConsole(message);
	}
}

void LogSystem::WriteMessageToFile(const Message& message)
{
	if (!m_file.IsOpen())
		return;

	const bool isAppend = (message.flags & Message::FLAG_APPEND);

	std::string buffer;

	if (isAppend)
	{
		buffer.reserve(message.content.length() + 2);
	}
	else
	{
		buffer.reserve(message.prefix.length() + message.content.length() + 2);

		// the prefix is used only in non-append mode
		buffer += message.prefix;
	}

	for (std::size_t i = 0; i < message.content.length(); i++)
	{
		if (message.content[i] < 32 || message.content[i] == 127)
		{
			// drop control characters
		}
		else if (message.content[i] == '$')
		{
			// convert "$$" to "$"
			if (message.content[i+1] == '$')
			{
				buffer += '$';
			}

			// drop color codes
			i++;
		}
		else
		{
			buffer += message.content[i];
		}
	}

	// newline
	buffer += "\r\n";

	// workaround for CPU detection messages
	if (message.content.length() > 2
	 && message.content[message.content.length()-2] == '\n'
	 && message.content[message.content.length()-1] == '\n')
	{
		buffer += "\r\n";
	}

	try
	{
		if (isAppend)
		{
			// move the file pointer before the last newline character
			m_file.Seek(File::END, -2);
		}

		m_file.Write(buffer);
	}
	catch (const std::exception& ex)
	{
		StdErr("Log file write error: %s", ex.what());
	}

	for (std::vector<ILogCallback*>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
	{
		(*it)->OnWriteToFile(message.content.c_str(), !isAppend);
	}
}

void LogSystem::WriteMessageToConsole(const Message& message)
{
	if (!gEnv)
		return;

	IConsole* pConsole = gEnv->pConsole;

	if (!pConsole)
		return;

	const bool isAppend = (message.flags & Message::FLAG_APPEND);

	if (isAppend)
	{
		pConsole->PrintLinePlus(message.content.c_str());
	}
	else
	{
		pConsole->PrintLine(message.content.c_str());
	}

	for (std::vector<ILogCallback*>::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
	{
		(*it)->OnWriteToConsole(message.content.c_str(), !isAppend);
	}
}

////////////////////////////////////////////////////////////////////////////////

void LogSystem::OpenLogFile()
{
	bool created = false;

	m_file.Open(Path::Join(m_rootFolder, m_fileName), File::READ_WRITE_CREATE, &created);

	if (!created)
	{
		BackupLogFile();

		// clear the existing file
		m_file.Resize(0);
	}
}

void LogSystem::BackupLogFile()
{
	std::string firstLine = m_file.Read(100);

	if (firstLine.empty())
	{
		// the existing log file is empty, so no backup is needed
		return;
	}

	const std::string backupNameAttachment = ExtractBackupNameAttachment(firstLine);
	const std::string backupFileName = CreateBackupFileName(m_fileName, backupNameAttachment);

	const std::string existingFilePath = Path::Join(m_rootFolder, m_fileName);
	const std::string backupFolderPath = Path::Join(m_rootFolder, "LogBackups");
	const std::string backupFilePath = Path::Join(backupFolderPath, backupFileName);

	File::CreateDirectory(backupFolderPath);
	File::Copy(existingFilePath, backupFilePath);
}

std::string LogSystem::ExtractBackupNameAttachment(const std::string& firstLine)
{
	const std::string prefix = "BackupNameAttachment=";

	// the line must begin with the prefix
	if (firstLine.substr(0, prefix.length()) == prefix)
	{
		const std::size_t beginQuotePos = firstLine.find('"', prefix.length());

		if (beginQuotePos != std::string::npos)
		{
			const std::size_t endQuotePos = firstLine.find('"', beginQuotePos + 1);

			return firstLine.substr(beginQuotePos + 1, endQuotePos - (beginQuotePos + 1));
		}
	}

	return std::string();
}

std::string LogSystem::CreateBackupFileName(const std::string& fileName, const std::string& backupNameAttachment)
{
	const std::size_t lastDotPos = fileName.find_last_of('.');

	std::string result;
	result += fileName.substr(0, lastDotPos);  // file name without extension
	result += backupNameAttachment;

	if (lastDotPos != std::string::npos)
	{
		result += fileName.substr(lastDotPos);  // file extension
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
