#include "source.h"
#include "log.h"

#include <thread>

AbstractSource::AbstractSource()
	: channels_claimed_ (0)
	{ }

AbstractSource::~AbstractSource() = default;

bool AbstractSource::runs_in_main_thread() const
{
	return false;
}

void AbstractSource::run()
{
	thread_ = std::thread (&AbstractSource::loop, this);
}

void AbstractSource::join()
{
	if (thread_.joinable()) {
		thread_.join();
	}
}

void AbstractSource::set_channels (channels_mask_t channels)
{
	channels_claimed_ = channels;
}

void AbstractSource::edge (bool value)
{
	Core::instance.edge (channels_claimed_, value);
}

void AbstractSource::pulse (std::chrono::milliseconds duration)
{
	Core::instance.pulse (channels_claimed_, duration);
}

void AbstractSource::edge_m (channels_mask_t mask, bool value)
{
	Core::instance.edge (channels_claimed_ & mask, value);
}

void AbstractSource::pulse_m (channels_mask_t mask, std::chrono::milliseconds duration)
{
	Core::instance.pulse (channels_claimed_ & mask, duration);
}
