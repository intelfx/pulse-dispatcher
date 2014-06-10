#include "core.h"
#include "common.h"

#include "sinks/terminal.h"
#include "sinks/ftdi_bitbang.h"
#include "sources/terminal.h"

#include <string>
#include <sstream>
#include <stdexcept>

std::unique_ptr<AbstractSource> make_source (const std::string& name,
                                             const options_map_t& options)
{
	if (name == "terminal" || name == "term") {
		return std::unique_ptr<AbstractSource> (new TerminalSource);
	} else {
		throw std::runtime_error ("unrecognized source name");
	}
}

std::unique_ptr<AbstractSink> make_sink (const std::string& name,
                                         const options_map_t& options)
{
	if (name == "terminal" || name == "term" || name == "dbg") {
		return std::unique_ptr<AbstractSink> (new TerminalSink);
	} else if (name == "ftdi") {
		return std::unique_ptr<AbstractSink> (new FTDIBitbangSink);
	} else {
		throw std::runtime_error ("unrecognized sink name");
	}
}

int main (int argc, char** argv)
{
	for (int i = 1; i < argc; ++i) {
		try {
			std::string argument (argv[i]);
			std::stringstream argument_read_stream (argument);

			std::string type, name;
			options_map_t options;

			std::getline (argument_read_stream, type, ':');
			std::getline (argument_read_stream, name, ':');

			while (!argument_read_stream.eof()) {
				std::string option;
				std::getline (argument_read_stream, option, ':');
				std::stringstream option_read_stream (option);

				std::string name, value;
				std::getline (option_read_stream, name, '=');
				std::getline (option_read_stream, value);

				if (name.empty()) {
					throw std::runtime_error ("malformed option: name is empty");
				}

				options.emplace (std::move (name), std::move (value));
			}

			if (type == "source" || type == "src") {
				channels_mask_t source_channels = Core::string_to_mask (options.get_string ("channels").c_str());
				std::unique_ptr<AbstractSource> source = make_source (name, options);
				Core::instance.add_source (std::move (source), source_channels);
			} else if (type == "sink") {
				std::unique_ptr<AbstractSink> sink = make_sink (name, options);
				Core::instance.add_sink (std::move (sink));
			} else {
				throw std::runtime_error ("type is not 'source' or 'sink'");
			}
		} catch (std::exception& e) {
			crit ("Invalid argument '%s': %s", argv[i], e.what());
		}
	}

	Core::instance.run_sources();
}