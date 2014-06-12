#ifndef _CORE_H
#define _CORE_H

#include "options.h"
#include "common.h"

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <vector>
#include <memory>
#include <mutex>

typedef uint32_t channels_mask_t;

static const size_t CHANNELS_MAX = sizeof (channels_mask_t) * 8;

class AbstractSource;
class AbstractSink;

class Core
{
	std::vector<std::unique_ptr<AbstractSource>> sources_;
	std::vector<std::unique_ptr<AbstractSink>> sinks_;

	std::mutex operation_mutex_;
	semaphore pulse_semaphore_;

	size_t channels_count_;
	channels_mask_t channels_;
	channels_mask_t channels_taken_;
	channels_mask_t channels_possible_;

	bool core_is_destroying_;

	void update_channels_count (size_t channels);

public:
	Core();
	~Core();

	size_t channels_count() const { return channels_count_; }
	bool is_destroying() const { return core_is_destroying_; }

	void add_source (std::unique_ptr<AbstractSource> source, channels_mask_t channels = 0);
	void add_sink (std::unique_ptr<AbstractSink> sink);

	void run_sources();
	void join_sources();
	void set_destroying();

	void edge (channels_mask_t mask, bool value);
	void pulse (channels_mask_t mask, std::chrono::milliseconds duration);

	static Core instance;

	static char channel_to_symbol (size_t channel);
	static ssize_t symbol_to_channel (char symbol);
	static channels_mask_t string_to_mask (const char* symbols);
};

#endif // _CORE_H