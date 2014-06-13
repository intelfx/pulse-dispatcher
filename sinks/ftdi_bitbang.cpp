#include "ftdi_bitbang.h"

#include <ftdi.h>

#define CHK(call) \
	do { int ret = call; assert (ret >= 0, "%s: %d (%s)", #call, ret, ftdi_get_error_string (ctx_)); } while (false)

namespace {

const size_t ftdi_channels = 8;

} // anonymous namespace

FTDIBitbangSink::FTDIBitbangSink()
	: ctx_ (ftdi_new())
{
	assert (ctx_, "ftdi_new() failed to create context");

	CHK (ftdi_usb_open (ctx_, 0x0403, 0x6001));
	CHK (ftdi_set_bitmode (ctx_, (1 << ftdi_channels) - 1, BITMODE_BITBANG));
}

FTDIBitbangSink::~FTDIBitbangSink()
{
	CHK (ftdi_usb_close (ctx_));
	ftdi_free (ctx_);
}

size_t FTDIBitbangSink::channels() const
{
	return ftdi_channels;
}

void FTDIBitbangSink::set (channels_mask_t values)
{
	unsigned char buf[1];
	buf[0] = static_cast<unsigned char> (values);
	assert (buf[0] == values, "data to send does not fit in an unsigned char");

	CHK (ftdi_write_data (ctx_, buf, 1));
}