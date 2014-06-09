#ifndef _SINK_FTDI_BITBANG_H
#define _SINK_FTDI_BITBANG_H

#include <sink.h>

struct ftdi_context;

class FTDIBitbangSink: public AbstractSink
{
	ftdi_context* ctx_;

public:
	FTDIBitbangSink();
	virtual ~FTDIBitbangSink();

	virtual size_t channels() const;
	virtual void set (channels_mask_t values);
};

#endif // _SINK_FTDI_BITBANG_H