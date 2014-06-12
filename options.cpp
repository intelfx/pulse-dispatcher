#include "options.h"
#include <stdexcept>
#include <string>

const std::string& options_map_t::get_string (const std::string& name) const
{
	return at (name);
}

long int options_map_t::get_int (const std::string& name) const
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

double options_map_t::get_float (const std::string& name) const
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

bool options_map_t::get_bool (const std::string& name) const
{
	auto it = find (name);

	if (it == end()) {
		return false;
	}

	const std::string& value = it->second;

	if (value == "0"
	        || value == "false"
	        || value == "off") {
		return false;
	}

	if (value.empty()
	        || value == "1"
	        || value == "true"
	        || value == "on") {
		return true;
	}

	throw std::runtime_error ("value is not valid boolean value");
}