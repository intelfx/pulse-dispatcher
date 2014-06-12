#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "log.h"

#include <exception>
#include <unordered_map>

struct options_map_t: public std::unordered_map<std::string, std::string>
{
public:
	const std::string& get_string (const std::string& name) const;
	long get_int (const std::string& name) const;
	double get_float (const std::string& name) const;
	bool get_bool (const std::string& name) const;

#define DEFAULT_WRAPPER(method, type)                                                         \
	type method (const std::string& name, type default_value) const                           \
		{ try { return method (name); } catch (std::exception& e) { return default_value; } }

	DEFAULT_WRAPPER (get_string, const std::string&)
	DEFAULT_WRAPPER (get_int, long)
	DEFAULT_WRAPPER (get_float, double)

#undef DEFAULT_WRAPPER

};

#endif // _OPTIONS_H