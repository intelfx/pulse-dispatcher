#ifndef _SOURCE_SOUNDFILE_H
#define _SOURCE_SOUNDFILE_H

#include "frequency.h"
#include <options.h>

#include <sndfile.hh>

class SoundFileSource: public FrequencySource
{
	typedef int sample_t;

	std::string filename_;
	SndfileHandle handle_;
	sample_t threshold_low_;
	sample_t threshold_high_;
	std::chrono::milliseconds pulse_width_;
	bool play_file_;

	bool check_sample (sample_t sample);

public:
	SoundFileSource (const options_map_t& options);

	virtual void loop();
	virtual bool single_iteration();
};

#endif // _SOURCE_SOUNDFILE_H