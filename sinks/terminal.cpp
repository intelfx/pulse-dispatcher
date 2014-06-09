#include "terminal.h"
#include <common.h>

size_t TerminalSink::channels() const
{
	return CHANNELS_MAX;
}

void TerminalSink::set (channels_mask_t values)
{
	for (size_t i = 0; i < Core::instance.channels_count(); ++i) {
		if (bit_enabled (values, i)) {
			putchar (toupper (Core::channel_to_symbol (i)));
		} else {
			putchar ('-');
		}
	}

	putchar ('\r');
	fflush (stdout);
}