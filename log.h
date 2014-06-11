#ifndef _LOG_H
#define _LOG_H

const char* __file_name_pretty (const char* file_path);

static const char* __file_name_translation_unit()
{
	static const char* const file_name = __file_name_pretty (__BASE_FILE__);
	return file_name;
}

#define __write_msg(tag, fmt, ...) fprintf (stderr, "\r" tag " [%s] " fmt "\n", __file_name_translation_unit(), ## __VA_ARGS__)

#ifndef NDEBUG
# define dbg(...) __write_msg ("d", __VA_ARGS__)
#endif // NDEBUG

#define log(...)  __write_msg (" ", __VA_ARGS__)
#define note(...) __write_msg ("I", __VA_ARGS__)
#define warn(...) __write_msg ("W", __VA_ARGS__)
#define err(...)  __write_msg ("E", __VA_ARGS__)
#define crit(...) do { __write_msg ("C", __VA_ARGS__); abort(); } while (false)

#define assert(exp, fmt, ...) do { if (!(exp)) crit ("Assertion failed: " #exp ": " fmt, ## __VA_ARGS__); } while (false)

#endif // _LOG_H