#include <cctype>

#include "StringView.h"

int StringView::Compare(const StringView& other) const
{
	const std::size_t commonLength = (this->length <= other.length) ? this->length : other.length;

	const int commonDiff = std::memcmp(this->string, other.string, commonLength);
	if (commonDiff != 0)
	{
		return commonDiff;
	}

	if (this->length != other.length)
	{
		return (this->length < other.length) ? -1 : 1;
	}

	return 0;
}

int StringView::CompareNoCase(const StringView& other) const
{
	const std::size_t commonLength = (this->length <= other.length) ? this->length : other.length;

	for (std::size_t i = 0; i < commonLength; i++)
	{
		char chA = std::toupper(this->string[i]);
		char chB = std::toupper(other.string[i]);

		if (chA != chB)
		{
			return (chA < chB) ? -1 : 1;
		}
	}

	if (this->length != other.length)
	{
		return (this->length < other.length) ? -1 : 1;
	}

	return 0;
}

bool StringView::Find(char ch, std::size_t& pos) const
{
	if (pos >= this->length)
	{
		return false;
	}

	const void* location = std::memchr(this->string + pos, ch, this->length - pos);

	if (!location)
	{
		return false;
	}

	pos = static_cast<const char*>(location) - this->string;

	return true;
}
