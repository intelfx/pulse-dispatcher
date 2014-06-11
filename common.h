#ifndef _COMMON_H
#define _COMMON_H

#include "log.h"

#include <mutex>
#include <condition_variable>

class semaphore
{
	std::mutex mutex_;
	std::condition_variable wait_;
	int count_;

	typedef std::unique_lock<decltype(mutex_)> lock_guard_t;

public:
	void inc()
	{
		lock_guard_t L (mutex_);
		++count_;
	}

	void dec()
	{
		lock_guard_t L (mutex_);
		--count_;
		wait_.notify_one();
	}

	void wait_for_zero()
	{
		lock_guard_t L (mutex_);
		wait_.wait (L, [this] () { return !(count_ > 0); });
	}

	class capture
	{
		semaphore& ref_;

	public:
		capture (semaphore& ref)
			: ref_ (ref)
			{ ref_.inc(); }

		~capture()
			{ ref_.dec(); }
	};
};

template <typename T>
bool bit_enabled (T value, size_t bit)
{
	return value & (1 << bit);
}

template <typename T>
void bit_set (T& value, size_t bit)
{
	value |= (1 << bit);
}

template <typename T>
void bit_clear (T& value, size_t bit)
{
	value &= ~static_cast<T> (1 << bit);
}

template <typename T>
bool check_in_mask (T value, T mask)
{
	return value == (value & mask);
}

template <typename T>
inline size_t find_highest_set_bit (T value)
{
	size_t result = 0;

	while (value >>= 1) {
		++result;
	}

	return result;
}

#endif // _COMMON_H