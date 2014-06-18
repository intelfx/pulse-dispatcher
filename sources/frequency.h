#ifndef _SOURCE_FREQUENCY_H
#define _SOURCE_FREQUENCY_H

#include <source.h>

class FrequencySource: public AbstractSource
{
	unsigned frequency_;

protected:
	FrequencySource (const options_map_t& options);

	void set_frequency (unsigned freq);

	virtual void loop();
	virtual bool single_iteration() = 0;
};

#endif // _SOURCE_FREQUENCY_H