#ifndef _SOURCES_TERMINAL_H
#define _SOURCES_TERMINAL_H

#include <source.h>

class TerminalSource: public AbstractSource
{
	channels_mask_t toggle_mode_channels_;

	void toggle (channels_mask_t mask);

public:
	TerminalSource();
	virtual ~TerminalSource();

	bool runs_in_main_thread() const;
	virtual void loop();
};

#endif // _SOURCES_TERMINAL_H