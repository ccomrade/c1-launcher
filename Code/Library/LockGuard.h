#pragma once

template<class T>
class LockGuard
{
	T& m_lock;

public:
	explicit LockGuard(T& lock) : m_lock(lock)
	{
		m_lock.Lock();
	}

	~LockGuard()
	{
		m_lock.Unlock();
	}
};
