#ifndef _SINK_TERMINAL_H
#define _SINK_TERMINAL_H

#include <sink.h>

class TerminalSink: public AbstractSink
{
public:
	virtual size_t channels() const;
	virtual void set (channels_mask_t values);
};

#endif // _SINK_TERMINAL_H