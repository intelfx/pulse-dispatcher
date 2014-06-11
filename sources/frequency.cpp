#include "frequency.h"
#include <common.h>

void FrequencySource::set_frequency (unsigned freq)
{
	dbg ("setting frequency to %u Hz", freq);
	frequency_ = freq;
}

void FrequencySource::loop()
{
	typedef std::chrono::steady_clock chrono_t;
	typedef chrono_t::rep             ticks_t;
	typedef chrono_t::duration        duration_t;
	typedef duration_t::period        period_t;

	const auto time_point = chrono_t::now();

	for (ticks_t samples = 1; !Core::instance.is_destroying(); ++samples) {
		/* do work */
		if (!single_iteration()) {
			break;
		}

		/* sleep until next tick */
		ticks_t ticks = (samples * period_t::den) / (frequency_ * period_t::num);
		std::this_thread::sleep_until (time_point + duration_t (ticks));
	}
}