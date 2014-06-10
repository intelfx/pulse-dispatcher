#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "log.h"

#include <unordered_map>
#include <string>
#include <stdexcept>

struct options_map_t: public std::unordered_map<std::string, std::string>
{
public:
	const std::string& get_string (const std::string& name) const
	{
		return at (name);
	}

	long get_int (const std::string& name) const
	{
		const std::string& value = get_string (name);
		char* endptr;
		long result = strtol (value.c_str(), &endptr, 0);
		if (*endptr != '\0') {
			err ("value of option '%s' is not valid integer: '%s'",
					name.c_str(), value.c_str());
			throw std::runtime_error ("value is not valid integer");
		} else {
			return result;
		}
	}

	double get_float (const std::string& name) const
	{
		const std::string& value = get_string (name);
		char* endptr;
		double result = strtod (value.c_str(), &endptr);
		if (*endptr != '\0') {
			err ("value of option '%s' is not valid floating-point value: '%s'",
					name.c_str(), value.c_str());
			throw std::runtime_error ("value is not valid floating-point value");
		} else {
			return result;
		}
	}

#define DEFAULT_WRAPPER(method, type)                                                         \
	type method (const std::string& name, type default_value) const                           \
		{ try { return method (name); } catch (std::exception& e) { return default_value; } }

	DEFAULT_WRAPPER (get_string, const std::string&)
	DEFAULT_WRAPPER (get_int, long)
	DEFAULT_WRAPPER (get_float, double)

#undef DEFAULT_WRAPPER

};

#endif // _OPTIONS_H