#ifndef _PULSE_H
#define _PULSE_H

#include "common.h"

#include <queue>

struct pulse_worker
{
	channels_mask_t mask;
	std::thread thread;
	std::mutex mutex;
	std::condition_variable condvar;
	std::queue<utils::clock::time_point> queue;
	bool destroying;

	enum pulse_mode {
		MODE_DEFAULT,
		MODE_SINGLE_PULSE /* ignores all pulse continuations, so only one pulse will be emitted per any overlapping pulse series */
	} mode;

	pulse_worker (channels_mask_t channels = 0, pulse_mode m = MODE_DEFAULT);
	pulse_worker (pulse_worker&& rhs); // WARNING: moves only mask, everything else is expected to be empty

	void add_to_queue (utils::clock::time_point pulse_end);

	void loop();
	void run();
	void stop_and_join();
};

#endif // _PULSE_H