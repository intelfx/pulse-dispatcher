#ifndef _COMMON_H
#define _COMMON_H

#include "log.h"

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

typedef uint32_t channels_mask_t;

static const size_t CHANNELS_MAX = sizeof (channels_mask_t) * 8;

namespace utils {

typedef std::chrono::steady_clock clock;
typedef clock::duration duration;
typedef clock::period period;
typedef clock::rep ticks;

} // namespace utils

typedef std::unique_lock<std::mutex> lock_guard_t;

class semaphore
{
	std::mutex mutex_;
	std::condition_variable wait_;
	int count_;

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