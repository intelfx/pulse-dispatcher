#ifndef _SINK_H
#define _SINK_H

#include "core.h"

class AbstractSink
{
public:
	virtual ~AbstractSink();

	virtual size_t channels() const = 0;
	virtual void set (channels_mask_t values) = 0;
};

#endif // _SINK_H