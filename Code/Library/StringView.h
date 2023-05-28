#pragma once

#include <algorithm>
#include <cstring>
#include <string>

template<typename T>
struct CharTraitsNoCase
{
	static unsigned long to_lower(unsigned long ch)
	{
		return ch | ((ch >= 'A' && ch <= 'Z') << 5);
	}

	static bool eq(const T& ch_a, const T& ch_b)
	{
		return to_lower(ch_a) == to_lower(ch_b);
	}

	static bool lt(const T& ch_a, const T& ch_b)
	{
		return to_lower(ch_a) < to_lower(ch_b);
	}

	static int compare(const T* string_a, const T* string_b, std::size_t length)
	{
		for (std::size_t i = 0; i < length; i++)
		{
			const unsigned long ch_a = to_lower(string_a[i]);
			const unsigned long ch_b = to_lower(string_b[i]);

			if (ch_a != ch_b)
			{
				return (ch_a < ch_b) ? -1 : 1;
			}
		}

		return 0;
	}

	static const T* find(const T* string, std::size_t length, T ch)
	{
		const T ch_lower = to_lower(ch);

		for (std::size_t i = 0; i < length; i++)
		{
			if (to_lower(string[i]) == ch_lower)
			{
				return string + i;
			}
		}

		return NULL;
	}
};

template<typename T>
class BasicStringView
{
	const T* m_string;
	std::size_t m_length;

	static std::size_t safe_strlen(const T* string) { return (string) ? std::char_traits<T>::length(string) : 0; }

public:
	BasicStringView() : m_string(NULL), m_length(0) {}

	BasicStringView(const T* string) : m_string(string), m_length(safe_strlen(string)) {}
	BasicStringView(const T* string, std::size_t length) : m_string(string), m_length(length) {}

	BasicStringView(const std::basic_string<T>& string) : m_string(string.c_str()), m_length(string.length()) {}

	////////////////////////////////////////////////////////////////////////////////
	// Size
	////////////////////////////////////////////////////////////////////////////////

	std::size_t length() const
	{
		return m_length;
	}

	std::size_t size() const
	{
		return m_length;
	}

	bool empty() const
	{
		return m_length == 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Access
	////////////////////////////////////////////////////////////////////////////////

	const T* data() const
	{
		return m_string;
	}

	const T& front() const
	{
		return m_string[0];
	}

	const T& back() const
	{
		return m_string[m_length - 1];
	}

	const T& operator[](std::size_t index) const
	{
		return m_string[index];
	}

	////////////////////////////////////////////////////////////////////////////////
	// Modification
	////////////////////////////////////////////////////////////////////////////////

	void remove_prefix(std::size_t length)
	{
		m_string += length;
		m_length -= length;
	}

	void remove_suffix(std::size_t length)
	{
		m_length -= length;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Comparison
	////////////////////////////////////////////////////////////////////////////////

	template<typename CharTraits>
	int generic_compare(BasicStringView other) const
	{
		const int diff = CharTraits::compare(m_string, other.m_string, std::min(m_length, other.m_length));
		if (diff)
		{
			return diff;
		}

		if (m_length != other.m_length)
		{
			return (m_length < other.m_length) ? -1 : 1;
		}

		return 0;
	}

	int compare(BasicStringView other) const
	{
		return this->generic_compare<std::char_traits<T>>(other);
	}

	int compare_no_case(BasicStringView other) const
	{
		return this->generic_compare<CharTraitsNoCase<T>>(other);
	}

	bool starts_with(T ch) const
	{
		return !this->empty() && std::char_traits<T>::eq(this->front(), ch);
	}

	bool starts_with_no_case(T ch) const
	{
		return !this->empty() && CharTraitsNoCase<T>::eq(this->front(), ch);
	}

	bool ends_with(T ch) const
	{
		return !this->empty() && std::char_traits<T>::eq(this->back(), ch);
	}

	bool ends_with_no_case(T ch) const
	{
		return !this->empty() && CharTraitsNoCase<T>::eq(this->back(), ch);
	}

	bool starts_with(BasicStringView prefix) const
	{
		if (prefix.m_length > m_length)
		{
			return false;
		}

		return std::char_traits<T>::compare(m_string, prefix.m_string, prefix.m_length) == 0;
	}

	bool starts_with_no_case(BasicStringView prefix) const
	{
		if (prefix.m_length > m_length)
		{
			return false;
		}

		return CharTraitsNoCase<T>::compare(m_string, prefix.m_string, prefix.m_length) == 0;
	}

	bool ends_with(BasicStringView suffix) const
	{
		if (suffix.m_length > m_length)
		{
			return false;
		}

		const std::size_t offset = m_length - suffix.m_length;

		return std::char_traits<T>::compare(m_string + offset, suffix.m_string, suffix.m_length) == 0;
	}

	bool ends_with_no_case(BasicStringView suffix) const
	{
		if (suffix.m_length > m_length)
		{
			return false;
		}

		const std::size_t offset = m_length - suffix.m_length;

		return CharTraitsNoCase<T>::compare(m_string + offset, suffix.m_string, suffix.m_length) == 0;
	}

	bool operator==(BasicStringView other) const
	{
		const std::size_t length = m_length;

		if (length != other.m_length)
		{
			return false;
		}

		return std::char_traits<T>::compare(m_string, other.m_string, length) == 0;
	}

	bool operator!=(BasicStringView other) const
	{
		return !this->operator==(other);
	}

	bool operator<=(BasicStringView other) const { return this->compare(other) <= 0; }
	bool operator>=(BasicStringView other) const { return this->compare(other) >= 0; }

	bool operator<(BasicStringView other) const { return this->compare(other) < 0; }
	bool operator>(BasicStringView other) const { return this->compare(other) > 0; }

	////////////////////////////////////////////////////////////////////////////////
	// Search
	////////////////////////////////////////////////////////////////////////////////

	static const std::size_t npos;

	template<typename CharTraits>
	std::size_t generic_find(T ch, std::size_t pos = 0) const
	{
		if (pos >= m_length)
		{
			return npos;
		}

		const T* found = CharTraits::find(m_string + pos, m_length - pos, ch);
		if (!found)
		{
			return npos;
		}

		return found - m_string;
	}

	template<typename CharTraits>
	std::size_t generic_find(BasicStringView view, std::size_t pos = 0) const
	{
		if (view.empty())
		{
			return (pos <= m_length) ? pos : npos;
		}

		if (pos >= m_length)
		{
			return npos;
		}

		const T first_char = view[0];
		const T* begin = m_string + pos;
		const T* const end = m_string + m_length;

		for (std::size_t length = m_length - pos; length >= view.m_length; length = end - ++begin)
		{
			begin = CharTraits::find(begin, (length - view.m_length) + 1, first_char);
			if (!begin)
			{
				return npos;
			}

			if (CharTraits::compare(begin, view.m_string, view.m_length) == 0)
			{
				return begin - m_string;
			}
		}

		return npos;
	}

	std::size_t find(T ch, std::size_t pos = 0) const
	{
		return this->generic_find<std::char_traits<T>>(ch, pos);
	}

	std::size_t find_no_case(T ch, std::size_t pos = 0) const
	{
		return this->generic_find<CharTraitsNoCase<T>>(ch, pos);
	}

	std::size_t find(BasicStringView view, std::size_t pos = 0) const
	{
		return this->generic_find<std::char_traits<T>>(view, pos);
	}

	std::size_t find_no_case(BasicStringView view, std::size_t pos = 0) const
	{
		return this->generic_find<CharTraitsNoCase<T>>(view, pos);
	}

	template<typename CharTraits>
	std::size_t generic_rfind(T ch, std::size_t pos = npos) const
	{
		if (this->empty())
		{
			return npos;
		}

		pos = std::min(pos, m_length - 1);

		do
		{
			if (CharTraits::eq(m_string[pos], ch))
			{
				return pos;
			}
		}
		while (pos-- > 0);

		return npos;
	}

	template<typename CharTraits>
	std::size_t generic_rfind(BasicStringView view, std::size_t pos = npos) const
	{
		if (view.m_length > m_length)
		{
			return npos;
		}

		pos = std::min(pos, m_length - view.m_length);

		do
		{
			if (CharTraits::compare(m_string + pos, view.m_string, view.m_length) == 0)
			{
				return pos;
			}
		}
		while (pos-- > 0);

		return npos;
	}

	std::size_t rfind(T ch, std::size_t pos = npos) const
	{
		return this->generic_rfind<std::char_traits<T>>(ch, pos);
	}

	std::size_t rfind_no_case(T ch, std::size_t pos = npos) const
	{
		return this->generic_rfind<CharTraitsNoCase<T>>(ch, pos);
	}

	std::size_t rfind(BasicStringView view, std::size_t pos = npos) const
	{
		return this->generic_rfind<std::char_traits<T>>(view, pos);
	}

	std::size_t rfind_no_case(BasicStringView view, std::size_t pos = npos) const
	{
		return this->generic_rfind<CharTraitsNoCase<T>>(view, pos);
	}

	template<typename CharTraits>
	std::size_t generic_find_first_of(BasicStringView view, std::size_t pos = 0) const
	{
		for (; pos < m_length; pos++)
		{
			if (CharTraits::find(view.m_string, view.m_length, m_string[pos]))
			{
				return pos;
			}
		}

		return npos;
	}

	std::size_t find_first_of(T ch, std::size_t pos = 0) const
	{
		return this->find(ch, pos);
	}

	std::size_t find_first_of_no_case(T ch, std::size_t pos = 0) const
	{
		return this->find_no_case(ch, pos);
	}

	std::size_t find_first_of(BasicStringView view, std::size_t pos = 0) const
	{
		return this->generic_find_first_of<std::char_traits<T>>(view, pos);
	}

	std::size_t find_first_of_no_case(BasicStringView view, std::size_t pos = 0) const
	{
		return this->generic_find_first_of<CharTraitsNoCase<T>>(view, pos);
	}

	template<typename CharTraits>
	std::size_t generic_find_last_of(BasicStringView view, std::size_t pos = npos) const
	{
		if (this->empty())
		{
			return npos;
		}

		pos = std::min(pos, m_length - 1);

		do
		{
			if (CharTraits::find(view.m_string, view.m_length, m_string[pos]))
			{
				return pos;
			}
		}
		while (pos-- > 0);

		return npos;
	}

	std::size_t find_last_of(T ch, std::size_t pos = npos) const
	{
		return this->rfind(ch, pos);
	}

	std::size_t find_last_of_no_case(T ch, std::size_t pos = npos) const
	{
		return this->rfind_no_case(ch, pos);
	}

	std::size_t find_last_of(BasicStringView view, std::size_t pos = npos) const
	{
		return this->generic_find_last_of<std::char_traits<T>>(view, pos);
	}

	std::size_t find_last_of_no_case(BasicStringView view, std::size_t pos = npos) const
	{
		return this->generic_find_last_of<CharTraitsNoCase<T>>(view, pos);
	}

	template<typename CharTraits>
	std::size_t generic_find_first_not_of(T ch, std::size_t pos = 0) const
	{
		for (; pos < m_length; pos++)
		{
			if (!CharTraits::eq(ch, m_string[pos]))
			{
				return pos;
			}
		}

		return npos;
	}

	template<typename CharTraits>
	std::size_t generic_find_first_not_of(BasicStringView view, std::size_t pos = 0) const
	{
		for (; pos < m_length; pos++)
		{
			if (!CharTraits::find(view.m_string, view.m_length, m_string[pos]))
			{
				return pos;
			}
		}

		return npos;
	}

	std::size_t find_first_not_of(T ch, std::size_t pos = 0) const
	{
		return this->generic_find_first_not_of<std::char_traits<T>>(ch, pos);
	}

	std::size_t find_first_not_of_no_case(T ch, std::size_t pos = 0) const
	{
		return this->generic_find_first_not_of<CharTraitsNoCase<T>>(ch, pos);
	}

	std::size_t find_first_not_of(BasicStringView view, std::size_t pos = 0) const
	{
		return this->generic_find_first_not_of<std::char_traits<T>>(view, pos);
	}

	std::size_t find_first_not_of_no_case(BasicStringView view, std::size_t pos = 0) const
	{
		return this->generic_find_first_not_of<CharTraitsNoCase<T>>(view, pos);
	}

	template<typename CharTraits>
	std::size_t generic_find_last_not_of(T ch, std::size_t pos = npos) const
	{
		if (this->empty())
		{
			return npos;
		}

		pos = std::min(pos, m_length - 1);

		do
		{
			if (!CharTraits::eq(ch, m_string[pos]))
			{
				return pos;
			}
		}
		while (pos-- > 0);

		return npos;
	}

	template<typename CharTraits>
	std::size_t generic_find_last_not_of(BasicStringView view, std::size_t pos = npos) const
	{
		if (this->empty())
		{
			return npos;
		}

		pos = std::min(pos, m_length - 1);

		do
		{
			if (!CharTraits::find(view.m_string, view.m_length, m_string[pos]))
			{
				return pos;
			}
		}
		while (pos-- > 0);

		return npos;
	}

	std::size_t find_last_not_of(T ch, std::size_t pos = npos) const
	{
		return this->generic_find_last_not_of<std::char_traits<T>>(ch, pos);
	}

	std::size_t find_last_not_of_no_case(T ch, std::size_t pos = npos) const
	{
		return this->generic_find_last_not_of<CharTraitsNoCase<T>>(ch, pos);
	}

	std::size_t find_last_not_of(BasicStringView view, std::size_t pos = npos) const
	{
		return this->generic_find_last_not_of<std::char_traits<T>>(view, pos);
	}

	std::size_t find_last_not_of_no_case(BasicStringView view, std::size_t pos = npos) const
	{
		return this->generic_find_last_not_of<CharTraitsNoCase<T>>(view, pos);
	}

	bool contains(T ch) const
	{
		return this->find(ch) != npos;
	}

	bool contains_no_case(T ch) const
	{
		return this->find_no_case(ch) != npos;
	}

	bool contains(BasicStringView view) const
	{
		return this->find(view) != npos;
	}

	bool contains_no_case(BasicStringView view) const
	{
		return this->find_no_case(view) != npos;
	}
};

template<typename T>
const std::size_t BasicStringView<T>::npos = static_cast<std::size_t>(-1);

template<typename T>
std::basic_string<T>& operator+=(std::basic_string<T>& string, BasicStringView<T> view)
{
	return string.append(view.data(), view.length());
}

typedef BasicStringView<char> StringView;
typedef BasicStringView<wchar_t> WStringView;
