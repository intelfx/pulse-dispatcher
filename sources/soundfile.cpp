#include "soundfile.h"
#include <common.h>

#include <limits>

namespace {

const double DEFAULT_THRESHOLD_LOW = 0.4,
             DEFAULT_THRESHOLD_HIGH = 1;
const long DEFAULT_PULSE_MSEC = 30;

} // anonymous namespace

SoundFileSource::SoundFileSource(const options_map_t& options)
	: filename_ (options.get_string ("file"))
	, handle_ (filename_)
	, threshold_low_ (std::numeric_limits<sample_t>::max() * options.get_float ("th_low", DEFAULT_THRESHOLD_LOW))
	, threshold_high_ (std::numeric_limits<sample_t>::max() * options.get_float ("th_high", DEFAULT_THRESHOLD_HIGH))
	, pulse_width_ (options.get_int ("pulse", DEFAULT_PULSE_MSEC))
	, play_file_ (options.get_bool ("play"))
{
	assert (!handle_.error(), "sndfile failed to open file '%s': %d (%s)",
	        filename_.c_str(), handle_.error(), handle_.strError());

	dbg ("reading%s file '%s' with threshold levels [%.2f; %.2f] and pulse width %d ms",
	     play_file_ ? " and playing" : "",
	     filename_.c_str(),
	     options.get_float ("th_low", DEFAULT_THRESHOLD_LOW),
	     options.get_float ("th_high", DEFAULT_THRESHOLD_HIGH),
	     options.get_int ("pulse", DEFAULT_PULSE_MSEC));

	set_frequency (handle_.samplerate());
}

bool SoundFileSource::check_sample (sample_t sample)
{
	return ((sample >= threshold_low_) && (sample <= threshold_high_));
}

void SoundFileSource::loop()
{
	if (play_file_) {
		char* call_string = nullptr;
		asprintf (&call_string, "aplay -q \"%s\" &", filename_.c_str());
		system (call_string);
		free (call_string);
	}

	FrequencySource::loop();
}

bool SoundFileSource::single_iteration()
{
	sample_t *sample = (sample_t*)alloca (sizeof (sample_t) * handle_.channels()),
	          max_value = 0;
	sf_count_t count = handle_.readf (sample, 1);

	for (size_t i = 0; i < handle_.channels(); ++i) {
		if (abs (sample[i]) > max_value) {
			max_value = abs (sample[i]);
		}
	}

	if (count < 1) {
		return false;
	} else {
		if (check_sample (max_value)) {
			pulse (pulse_width_);
		}
		return true;
	}
}