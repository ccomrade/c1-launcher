#pragma once

#include <cstdio>

struct StdFile
{
	std::FILE* handle;

	StdFile() : handle(NULL)
	{
	}

	StdFile(const char* path, const char* mode) : handle(std::fopen(path, mode))
	{
	}

private:
	// no copies
	StdFile(const StdFile&);
	StdFile& operator=(const StdFile&);

public:
	~StdFile()
	{
		this->Close();
	}

	bool IsOpen() const
	{
		return this->handle != NULL;
	}

	bool Open(const char* path, const char* mode)
	{
		this->Close();
		this->handle = std::fopen(path, mode);

		return this->IsOpen();
	}

	void Close()
	{
		if (this->handle)
		{
			std::fclose(this->handle);
			this->handle = NULL;
		}
	}

	std::size_t Read(char* buffer, std::size_t bufferSize)
	{
		return std::fread(buffer, 1, bufferSize, this->handle);
	}

	std::size_t Write(const char* buffer, std::size_t bufferSize)
	{
		return std::fwrite(buffer, 1, bufferSize, this->handle);
	}

	bool IsEndOfFile()
	{
		return std::feof(this->handle) != 0;
	}

	void Flush()
	{
		std::fflush(this->handle);
	}
};
