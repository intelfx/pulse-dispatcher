#ifndef _COMMON_H
#define _COMMON_H

#include "log.h"

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