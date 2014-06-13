#include "core.h"
#include "common.h"
#include "sink.h"
#include "source.h"

#include <signal.h>
#include <string.h>

#include <initializer_list>
#include <thread>

namespace {

const char* const CHANNEL_SYMBOL_MAP = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void handle_terminate_signal (int sig)
{
	warn ("Exiting on receipt of signal %d (%s)", sig, sys_siglist[sig]);
	Core::instance.set_destroying();

	signal (sig, SIG_DFL);
}

void set_terminate_signal (sighandler_t handler)
{
	for (int sig: { SIGINT, SIGTERM, SIGABRT }) {
		signal (sig, handler);
	}
}

} // anonymous namespace

Core::Core()
	: channels_count_ (0)
	, channels_ (0)
	, channels_taken_ (0)
	, channels_possible_ (0)
	, core_is_destroying_ (false)
{
	set_terminate_signal (handle_terminate_signal);

	log ("Core initialized");
}

Core::~Core()
{
	log ("Core destroying");

	set_terminate_signal (SIG_DFL);
}

void Core::update_channels_count (size_t channels)
{
	if (channels == channels_count_) {
		return;
	}

	assert (channels < CHANNELS_MAX,
	        "Cannot change channel count to %zu: hard limit is %zu",
	        channels, CHANNELS_MAX);

	for (const std::unique_ptr<AbstractSink>& sink: sinks_) {
		assert (sink->channels() >= channels,
		        "Cannot change channel count to %zu: sink-imposed limit is %zu",
		        channels, sink->channels());
	}

	channels_count_ = channels;
	channels_possible_ = (1 << channels) - 1;

	assert (check_in_mask (channels_taken_, channels_possible_),
	        "Cannot change channel count to %zu: some sources claimed more channels",
	        channels);

	if (channels_workers_.size() < channels) {
		for (size_t i = channels_workers_.size(); i < channels; ++i) {
			channels_workers_.emplace_back (1 << i);
		}
		assert (channels_workers_.size() == channels,
		        "Logic error while resizing channel workers vector");
	} else {
		channels_workers_.resize (channels); // shrink only
	}

	dbg ("Channel count changed to %zu", channels);
}

void Core::add_sink (std::unique_ptr<AbstractSink> sink)
{
	lock_guard_t L (operation_mutex_);

	note ("Adding sink '%s' (%zu channels)",
	      typeid (*sink.get()).name(), sink->channels());

	update_channels_count (std::min (channels_count_, sink->channels()));

	sink->set (channels_);
	sinks_.push_back (std::move (sink));
}

void Core::add_source (std::unique_ptr<AbstractSource> source, channels_mask_t channels)
{
	lock_guard_t L (operation_mutex_);

	if (channels) {
		source->set_channels (channels);
	}

	size_t channels_required = find_highest_set_bit (source->channels()) + 1;

	note ("Adding source '%s' (%zu channels)",
	      typeid (*source.get()).name(),
	      channels_required);

	update_channels_count (std::max (channels_count_, channels_required));

	assert (check_in_mask (source->channels(), channels_possible_),
	        "Cannot add source: failed to change channel count",
	        channels_count_);

	assert (!(source->channels() & channels_taken_),
	        "Cannot add source: channel sets overlap");

	channels_taken_ |= source->channels();
	sources_.push_back (std::move (source));
}

void Core::run_sources()
{
	if (sources_.empty()) {
		err ("No sources registered -- nothing to do");
		return;
	}

	if (sinks_.empty()) {
		err ("No sinks registered -- nothing to do");
		return;
	}

	log ("Starting per-channel worker threads");

	for (pulse_worker& worker: channels_workers_) {
		worker.run();
	}

	log ("Starting source threads");

	AbstractSource* main_thread_source = nullptr;

	for (std::unique_ptr<AbstractSource>& source: sources_) {
		if (source->runs_in_main_thread()) {
			assert (!main_thread_source,
			        "More than one source wants to run in main thread");
			main_thread_source = source.get();
		} else {
			source->run_thread();
		}
	}

	if (main_thread_source) {
		main_thread_source->run_loop();
	}
}

void Core::join_sources()
{
	for (std::unique_ptr<AbstractSource>& source: sources_) {
		source->join();
	}

	for (pulse_worker& worker: channels_workers_) {
		worker.join();
	}
}

void Core::set_destroying()
{
	if (!core_is_destroying_) {
		log ("Telling threads to stop");
		core_is_destroying_ = true;
	}
}

void Core::edge_internal (channels_mask_t mask, bool value)
{
	if (value) {
		channels_ |= mask;
	} else {
		channels_ &= ~mask;
	}

	for (std::unique_ptr<AbstractSink>& sink: sinks_) {
		sink->set (channels_);
	}
}

void Core::edge (channels_mask_t mask, bool value)
{
	lock_guard_t L (operation_mutex_);

	assert (check_in_mask (mask, channels_taken_),
	        "Wrong channel set to activate: some channels are not taken");

	channels_mask_t actual_mask = 0;

	for (size_t i = 0; i < CHANNELS_MAX; ++i) {
		if (bit_enabled (mask, i)) {
			if (value) {
				if (!channels_raise_counts_[i]++) {
					bit_set (actual_mask, i);
				}
			} else {
				assert (channels_raise_counts_[i],
				        "Rise-fall imbalance: trying to put channel %zu while it's already down",
				        i);

				if (!--channels_raise_counts_[i]) {
					bit_set (actual_mask, i);
				}
			}
		}
	}

	if (actual_mask) {
		assert (check_in_mask (actual_mask, mask),
		        "Rise-fall logic failure: trying to raise/put channels which are not part of initial request");

		edge_internal (actual_mask, value);
	}
}

void Core::pulse (channels_mask_t mask, std::chrono::milliseconds duration)
{
	utils::clock::time_point pulse_end_time = utils::clock::now() + duration;

	for (size_t i = 0; i < CHANNELS_MAX; ++i) {
		if (bit_enabled (mask, i)) {
			 channels_workers_.at (i).add_to_queue (pulse_end_time);
		}
	}
}

char Core::channel_to_symbol (size_t channel)
{
	assert (channel < CHANNELS_MAX, "Invalid channel to convert to symbol: %zu",
	        channel);

	return CHANNEL_SYMBOL_MAP[channel];
}

ssize_t Core::symbol_to_channel (char symbol)
{
	symbol = toupper (symbol);

	const char* location = strchr (CHANNEL_SYMBOL_MAP, symbol);

	if (!location) {
		warn ("Invalid channel designator: '%c'", symbol);
		return -1;
	}

	ssize_t channel = location - CHANNEL_SYMBOL_MAP;

	if (channel >= CHANNELS_MAX) {
		warn ("Invalid channel designator: '%c'", symbol);
		return -1;
	}

	return channel;
}

channels_mask_t Core::string_to_mask (const char* symbols)
{
	channels_mask_t mask = 0;
	const char* ptr = symbols;

	while (char c = *ptr++) {
		bit_set (mask, symbol_to_channel (c));
	}

	return mask;
}

Core Core::instance;