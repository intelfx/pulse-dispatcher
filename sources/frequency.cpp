#include "frequency.h"
#include <core.h>

FrequencySource::FrequencySource (const options_map_t& options)
	: AbstractSource (options)
{ }

void FrequencySource::set_frequency (unsigned freq)
{
	dbg ("setting frequency to %u Hz", freq);
	frequency_ = freq;
}

void FrequencySource::loop()
{
	using utils::clock;

	const clock::time_point start_point = clock::now();

	for (clock::rep samples = 1; !Core::instance.is_destroying(); ++samples) {
		/* do work */
		if (!single_iteration()) {
			break;
		}

		/* sleep until next tick */
		clock::rep ticks = (samples * clock::period::den) / (frequency_ * clock::period::num);
		std::this_thread::sleep_until (start_point + clock::duration (ticks));
	}
}