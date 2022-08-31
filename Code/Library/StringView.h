#pragma once

#include <cstring>
#include <string>

struct StringView
{
	const char* string;
	std::size_t length;

	StringView() : string(NULL), length(0)
	{
	}

	StringView(const char* string) : string(string), length(string ? std::strlen(string) : 0)
	{
	}

	StringView(const char* string, std::size_t length) : string(string), length(length)
	{
	}

	StringView(const std::string& string) : string(string.c_str()), length(string.length())
	{
	}

	bool IsEmpty() const
	{
		return this->length == 0;
	}

	bool IsNotEmpty() const
	{
		return this->length != 0;
	}

	const char& Front() const
	{
		return this->string[0];
	}

	const char& Back() const
	{
		return this->string[this->length - 1];
	}

	const char& operator[](std::size_t index) const
	{
		return this->string[index];
	}

	void RemovePrefix(std::size_t length)
	{
		this->string += length;
		this->length -= length;
	}

	void RemoveSuffix(std::size_t length)
	{
		this->length -= length;
	}

	void PopFront()
	{
		RemovePrefix(1);
	}

	void PopBack()
	{
		RemoveSuffix(1);
	}

	std::string ToStdString() const
	{
		return std::string(this->string, this->length);
	}

	int Compare(const StringView& other) const;
	int CompareNoCase(const StringView& other) const;

	bool IsEqual(const StringView& other) const
	{
		return this->Compare(other) == 0;
	}

	bool IsEqualNoCase(const StringView& other) const
	{
		return this->CompareNoCase(other) == 0;
	}

	bool StartsWith(const StringView& prefix) const
	{
		if (this->length < prefix.length)
		{
			return false;
		}
		else
		{
			return StringView(this->string, prefix.length).IsEqual(prefix);
		}
	}

	bool StartsWithNoCase(const StringView& prefix) const
	{
		if (this->length < prefix.length)
		{
			return false;
		}
		else
		{
			return StringView(this->string, prefix.length).IsEqualNoCase(prefix);
		}
	}

	bool Find(char ch, std::size_t& pos) const;
};

inline bool operator==(const StringView& a, const StringView& b) { return a.Compare(b) == 0; }
inline bool operator!=(const StringView& a, const StringView& b) { return a.Compare(b) != 0; }
inline bool operator<=(const StringView& a, const StringView& b) { return a.Compare(b) <= 0; }
inline bool operator< (const StringView& a, const StringView& b) { return a.Compare(b) <  0; }
inline bool operator>=(const StringView& a, const StringView& b) { return a.Compare(b) >= 0; }
inline bool operator> (const StringView& a, const StringView& b) { return a.Compare(b) >  0; }

inline std::string& operator+=(std::string& a, const StringView& b) { return a.append(b.string, b.length); }
