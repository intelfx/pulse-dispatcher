#include "source.h"
#include "core.h"

AbstractSource::AbstractSource()
	: channels_claimed_ (0)
	{ }

AbstractSource::~AbstractSource() = default;

bool AbstractSource::runs_in_main_thread() const
{
	return false;
}

void AbstractSource::run_thread()
{
	worker_.run();
	thread_ = std::thread (&AbstractSource::loop, this);
}

void AbstractSource::run_loop()
{
	worker_.run();
	loop();
	worker_.stop_and_join();
}

void AbstractSource::join()
{
	if (thread_.joinable()) {
		thread_.join();
	}
	worker_.stop_and_join();
}

void AbstractSource::set_channels (channels_mask_t channels)
{
	channels_claimed_ = channels;
	worker_.mask = channels;
}

void AbstractSource::edge (bool value)
{
	Core::instance.edge (channels_claimed_, value);
}

void AbstractSource::pulse (std::chrono::milliseconds duration)
{
	worker_.add_to_queue (utils::clock::now() + duration);
}

void AbstractSource::edge_m (channels_mask_t mask, bool value)
{
	Core::instance.edge (channels_claimed_ & mask, value);
}

void AbstractSource::pulse_m (channels_mask_t mask, std::chrono::milliseconds duration)
{
	Core::instance.pulse (channels_claimed_ & mask, duration);
}
