#ifndef _LOG_H
#define _LOG_H

const char* __file_name_pretty (const char* file_path);

static const char* __file_name_translation_unit()
{
	static const char* const file_name = __file_name_pretty (__BASE_FILE__);
	return file_name;
}

extern bool do_not_abort;

#define _log(tag, fmt, ...) fprintf (stderr, "\r" tag " [%s] " fmt "\n", __file_name_translation_unit(), ## __VA_ARGS__)

#ifndef NDEBUG
# define dbg(...) _log ("d", __VA_ARGS__)
#endif // NDEBUG

#define log(...)  _log (" ", __VA_ARGS__)
#define note(...) _log ("I", __VA_ARGS__)
#define warn(...) _log ("W", __VA_ARGS__)
#define err(...)  _log ("E", __VA_ARGS__)
#define crit(...) do { _log ("C", __VA_ARGS__); if (!do_not_abort) abort(); } while (false)

#define assert(exp, fmt, ...) do { if (!(exp)) crit ("Assertion failed: " #exp ": " fmt, ## __VA_ARGS__); } while (false)

#endif // _LOG_H