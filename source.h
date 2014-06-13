#ifndef _SOURCE_H
#define _SOURCE_H

#include "common.h"
#include "pulse_worker.h"

class AbstractSource
{
	channels_mask_t channels_claimed_;
	std::thread thread_;
	pulse_worker worker_;

protected:
	void edge (bool value);
	void pulse (std::chrono::milliseconds duration);

	void edge_m (channels_mask_t mask, bool value);
	void pulse_m (channels_mask_t mask, std::chrono::milliseconds duration);

	virtual void loop() = 0;

public:
	AbstractSource();
	virtual ~AbstractSource();

	virtual bool runs_in_main_thread() const;

	void set_channels (channels_mask_t channels);
	channels_mask_t channels() const { return channels_claimed_; }

	void run_thread();
	void run_loop();
	void join();
};

#endif // _SOURCE_H