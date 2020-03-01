#pragma once

#include <cstring>
#include <string>
#include <new>

using std::size_t;

template<size_t DefaultSize>
class StringBuffer
{
	char m_stackBuffer[DefaultSize];
	char *m_buffer;
	size_t m_bufferSize;
	size_t m_pos;

	// disable implicit copy constructor and copy assignment operator
	StringBuffer(const StringBuffer &);
	StringBuffer & operator=(const StringBuffer &);

public:
	StringBuffer()
	{
		m_stackBuffer[0] = '\0';
		m_buffer = m_stackBuffer;
		m_bufferSize = DefaultSize;
		m_pos = 0;
	}

	~StringBuffer()
	{
		if (isHeap())
		{
			delete [] m_buffer;
		}
	}

	bool isHeap() const
	{
		return m_buffer != m_stackBuffer;
	}

	bool isEmpty() const
	{
		return m_pos == 0;
	}

	const char *get() const
	{
		return m_buffer;
	}

	size_t getLength() const
	{
		return m_pos;
	}

	size_t getAvailableSpace() const
	{
		return m_bufferSize - m_pos - 1;
	}

	size_t getCapacity() const
	{
		return m_bufferSize;
	}

	char operator[](size_t index) const
	{
		return m_buffer[index];
	}

	char getLast() const
	{
		return (m_pos > 0) ? m_buffer[m_pos-1] : m_buffer[0];
	}

	std::string toString() const
	{
		return std::string(m_buffer, m_pos);
	}

	void clear()
	{
		m_pos = 0;
		m_buffer[0] = '\0';
	}

	void pop(size_t length = 1)
	{
		m_pos -= (length < m_pos) ? length : m_pos;
		m_buffer[m_pos] = '\0';
	}

	void append(char ch)
	{
		makeSpaceFor(1);

		m_buffer[m_pos] = ch;
		m_pos += 1;
		m_buffer[m_pos] = '\0';
	}

	void append(const char *string)
	{
		if (!string)
		{
			return;
		}

		append(string, std::strlen(string));
	}

	void append(const char *string, size_t length)
	{
		if (!string || length == 0)
		{
			return;
		}

		makeSpaceFor(length);

		std::memcpy(m_buffer + m_pos, string, length);
		m_pos += length;
		m_buffer[m_pos] = '\0';
	}

	void append(const std::string & string)
	{
		append(string.c_str(), string.length());
	}

	void appendNumberUnsigned(unsigned long number, int base = 10, bool upperCase = true)
	{
		if (base < 2 || base > 36)
		{
			return;
		}

		const char firstLetter = (upperCase) ? 'A' : 'a';
		const size_t beginPos = m_pos;

		do
		{
			const int digit = number % base;
			const char ch = (digit < 10) ? '0' + digit : firstLetter + (digit - 10);

			append(ch);

			number /= base;
		}
		while (number);

		// reverse the number
		for (size_t i = beginPos, j = m_pos-1; i < j; i++, j--)
		{
			const char temp = m_buffer[i];
			m_buffer[i] = m_buffer[j];
			m_buffer[j] = temp;
		}
	}

	void appendNumber(long number, int base = 10, bool upperCase = true)
	{
		if (number < 0)
		{
			append('-');
			appendNumberUnsigned(-number, base, upperCase);
		}
		else
		{
			appendNumberUnsigned(number, base, upperCase);
		}
	}

	// no append_f and append_vf here
	// MSVC below VS2015 provides only broken and non-standard implementation of printf functions

	void makeSpaceFor(size_t length)
	{
		if (length > getAvailableSpace())
		{
			size_t requiredSize = getLength() + length + 1;
			size_t minimalSize = getCapacity() * 2;
			size_t newSize = (requiredSize > minimalSize) ? requiredSize : minimalSize;

			resize(newSize);
		}
	}

	void resize(size_t size)
	{
		if (size == 0 || size == m_bufferSize)
		{
			return;
		}

		char *oldBuffer = m_buffer;
		char *newBuffer = (size > DefaultSize) ? new char[size] : m_stackBuffer;

		if (newBuffer == oldBuffer)
		{
			return;
		}

		if (m_pos >= size)
		{
			// truncate buffer content
			m_pos = size - 1;
			oldBuffer[m_pos] = '\0';
		}

		// copy content to the new buffer
		std::memcpy(newBuffer, oldBuffer, m_pos + 1);

		if (isHeap())
		{
			delete [] oldBuffer;
		}

		m_buffer = newBuffer;
		m_bufferSize = size;
	}
};

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, char ch)
{
	buffer.append(ch);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, const char *string)
{
	buffer.append(string);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, const std::string & string)
{
	buffer.append(string);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, short number)
{
	buffer.appendNumber(number);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, int number)
{
	buffer.appendNumber(number);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, long number)
{
	buffer.appendNumber(number);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, unsigned short number)
{
	buffer.appendNumberUnsigned(number);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, unsigned int number)
{
	buffer.appendNumberUnsigned(number);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, unsigned long number)
{
	buffer.appendNumberUnsigned(number);
	return buffer;
}
