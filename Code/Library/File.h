#pragma once

#include "WinAPI.h"

class File : public FileBase
{
	void* m_handle;

	// no copies
	File(const File&);
	File& operator=(const File&);

public:
	File() : m_handle(NULL)
	{
	}

	explicit File(const std::string& path, FileBase::Access access, bool *pCreated = NULL)
	{
		Open(path, access, pCreated);
	}

	explicit File(const char* path, FileBase::Access access, bool *pCreated = NULL)
	{
		Open(path, access, pCreated);
	}

	~File()
	{
		Close();
	}

	void Open(const std::string& path, FileBase::Access access, bool *pCreated = NULL)
	{
		Open(path.c_str(), access, pCreated);
	}

	void Open(const char* path, FileBase::Access access, bool *pCreated = NULL)
	{
		Close();

		m_handle = WinAPI::File::Open(path, access, pCreated);

		if (!m_handle)
		{
			throw WinAPI::CurrentError("Failed to open file '%s'", path);
		}
	}

	void* GetHandle() const
	{
		return m_handle;
	}

	bool IsOpen() const
	{
		return m_handle != NULL;
	}

	std::string Read(std::size_t maxLength = 0)
	{
		std::string buffer;

		if (!WinAPI::File::Read(m_handle, buffer, maxLength))
		{
			throw WinAPI::CurrentError("File read of %Iu bytes failed", maxLength);
		}

		return buffer;
	}

	void Write(const std::string& text)
	{
		Write(text.c_str(), text.length());
	}

	void Write(const char* text)
	{
		Write(text, std::strlen(text));
	}

	void Write(const void* data, std::size_t dataLength)
	{
		if (!WinAPI::File::Write(m_handle, data, dataLength))
		{
			throw WinAPI::CurrentError("File write of %Iu bytes failed", dataLength);
		}
	}

	unsigned __int64 Seek(FileBase::SeekBase base, __int64 offset = 0)
	{
		unsigned __int64 newPos = 0;

		if (!WinAPI::File::Seek(m_handle, base, offset, &newPos))
		{
			throw WinAPI::CurrentError("File seek with base %d to offset %I64d failed", base, offset);
		}

		return newPos;
	}

	void Resize(unsigned __int64 size)
	{
		if (!WinAPI::File::Resize(m_handle, size))
		{
			throw WinAPI::CurrentError("File resize to %I64u bytes failed", size);
		}
	}

	void Close()
	{
		if (m_handle)
		{
			WinAPI::File::Close(m_handle);
			m_handle = NULL;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	static void CreateDirectory(const std::string& path, bool *pCreated = NULL)
	{
		CreateDirectory(path.c_str(), pCreated);
	}

	static void CreateDirectory(const char* path, bool *pCreated = NULL)
	{
		if (!WinAPI::Directory::Create(path, pCreated))
		{
			throw WinAPI::CurrentError("Failed to create directory '%s'", path);
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	static void Copy(const std::string& sourcePath, const std::string& destinationPath)
	{
		Copy(sourcePath.c_str(), destinationPath.c_str());
	}

	static void Copy(const std::string& sourcePath, const char* destinationPath)
	{
		Copy(sourcePath.c_str(), destinationPath);
	}

	static void Copy(const char* sourcePath, const std::string& destinationPath)
	{
		Copy(sourcePath, destinationPath.c_str());
	}

	static void Copy(const char* sourcePath, const char* destinationPath)
	{
		if (!WinAPI::File::Copy(sourcePath, destinationPath))
		{
			throw WinAPI::CurrentError("Failed to copy file '%s' to '%s'", sourcePath, destinationPath);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
};
