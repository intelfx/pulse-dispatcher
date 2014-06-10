#ifndef _SOURCE_H
#define _SOURCE_H

#include <stdint.h>
#include <thread>

#include "core.h"

class AbstractSource
{
	channels_mask_t channels_;
	std::thread thread_;

protected:
	void edge (bool value);
	void pulse (std::chrono::milliseconds duration);

	void edge_m (channels_mask_t mask, bool value);
	void pulse_m (channels_mask_t mask, std::chrono::milliseconds duration);

public:
	AbstractSource();
	virtual ~AbstractSource();

	virtual bool runs_in_main_thread() const;

	void set_channels (channels_mask_t channels);
	channels_mask_t channels() const { return channels_; }

	virtual void loop() = 0;
	void run();
	void join();
};

#endif // _SOURCE_H