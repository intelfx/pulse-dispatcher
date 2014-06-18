#ifndef _SOURCES_TERMINAL_H
#define _SOURCES_TERMINAL_H

#include <source.h>
#include <options.h>

class TerminalSource: public AbstractSource
{
	channels_mask_t toggle_mode_channels_;
	bool toggle_;
	std::chrono::milliseconds pulse_width_;

	void toggle();
	void toggle_m (channels_mask_t mask);
	void toggle_internal (channels_mask_t mask);

public:
	TerminalSource(const options_map_t& options);
	virtual ~TerminalSource();

	virtual bool runs_in_main_thread() const;

protected:
	virtual void loop();
};

#endif // _SOURCES_TERMINAL_H