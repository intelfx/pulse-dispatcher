#ifndef _CORE_H
#define _CORE_H

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
	std::array<size_t, CHANNELS_MAX> channels_raise_counts_;

	std::mutex operation_mutex_;

	typedef std::lock_guard<decltype(operation_mutex_)> lock_guard_t;

	size_t channels_count_;
	channels_mask_t channels_;
	channels_mask_t channels_taken_;
	channels_mask_t channels_possible_;

	void update_channels_count (size_t channels);
	void edge_internal (channels_mask_t mask, bool value);

public:
	Core();
	~Core();

	size_t channels_count() const { return channels_count_; }

	void add_source (std::unique_ptr<AbstractSource> source, channels_mask_t channels = 0);
	void add_sink (std::unique_ptr<AbstractSink> sink);

	void run_sources();

	void edge (channels_mask_t mask, bool value);
	void pulse (channels_mask_t mask, std::chrono::milliseconds duration);

	static Core instance;

	static char channel_to_symbol (size_t channel);
	static ssize_t symbol_to_channel (char symbol);
	static channels_mask_t string_to_mask (const char* symbols);
};

#endif // _CORE_H