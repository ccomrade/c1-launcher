#pragma once

#include <deque>

#include "Mutex.h"

template<class T>
class ThreadSafeQueue
{
	std::deque<T> m_queue;
	Mutex m_mutex;

public:
	void Push(const T& value)
	{
		LockGuard<Mutex> lock(m_mutex);

		m_queue.push_back(value);
	}

	bool Pop(T& value)
	{
		LockGuard<Mutex> lock(m_mutex);

		if (m_queue.empty())
		{
			return false;
		}
		else
		{
			value = m_queue.front();
			m_queue.pop_front();
			return true;
		}
	}
};
