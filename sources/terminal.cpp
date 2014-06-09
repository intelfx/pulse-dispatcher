#include "terminal.h"
#include <common.h>
#include <terminal.h>

namespace {

size_t instance_count = 0;

const std::chrono::milliseconds DEFAULT_PULSE (100);

} // anonymous namespace

TerminalSource::TerminalSource()
	: toggle_mode_channels_ (0)
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

void TerminalSource::toggle (channels_mask_t mask)
{
	mask &= channels();

	channels_mask_t to_disable =  toggle_mode_channels_ & mask,
	                to_enable  = ~toggle_mode_channels_ & mask;

	edge_m (to_enable, true);
	edge_m (to_disable, false);

	toggle_mode_channels_ ^= mask;
}

void TerminalSource::loop()
{
	char input;
	ssize_t channel;

	for (;;) {
		input = getchar();
		if (input == '`') {
			return;
		} else if (input == '-') {
			pulse (DEFAULT_PULSE);
		} else {
			channel = Core::symbol_to_channel (input);
			if (channel >= 0) {
				if (isupper (input)) {
					toggle (1 << channel);
				} else {
					pulse_m (1 << channel, DEFAULT_PULSE);
				}
			}
		}
	}
}