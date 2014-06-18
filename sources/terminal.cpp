#include "terminal.h"
#include <core.h>
#include <terminal.h>

namespace {

size_t instance_count = 0;

const long DEFAULT_PULSE_MSEC = 30;

} // anonymous namespace

TerminalSource::TerminalSource (const options_map_t& options)
	: toggle_mode_channels_ (0)
	, toggle_ (options.get_bool ("toggle"))
	, pulse_width_ (options.get_int ("pulse", DEFAULT_PULSE_MSEC))
{
	assert (!instance_count, "TerminalSource: creating more than one instance is meaningless");

	if (!instance_count++) {
		dbg ("Creating terminal source and setting up terminal");
		terminal_setup();
	}
}

TerminalSource::~TerminalSource()
{
	if (!--instance_count) {
		dbg ("Destroying terminal source and restoring terminal");
		terminal_restore();
	}
}

bool TerminalSource::runs_in_main_thread() const
{
	return true;
}

void TerminalSource::toggle()
{
	toggle_internal (channels());
}

void TerminalSource::toggle_m (channels_mask_t mask)
{
	toggle_internal (channels() & mask);
}

void TerminalSource::toggle_internal (channels_mask_t mask)
{
	channels_mask_t to_disable =  toggle_mode_channels_ & mask,
	                to_enable  = ~toggle_mode_channels_ & mask;

	edge_m (to_enable, true);
	edge_m (to_disable, false);

	toggle_mode_channels_ ^= mask;
}

void TerminalSource::loop()
{
	int input;
	ssize_t channel;

	for (;;) {
		input = getchar();

		switch (input) {
		case EOF:
		case '`':
			return;

		case '-':
			if (toggle_) {
				toggle();
			} else {
				pulse (pulse_width_);
			}
			break;

		default:
			channel = Core::symbol_to_channel (input);
			if (channel < 0) {
				break;
			}

			if (toggle_) {
				toggle_m (1 << channel);
			} else {
				pulse_m (1 << channel, pulse_width_);
			}
			break;
		}
	}
}