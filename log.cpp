#include "log.h"
#include <string.h>

bool do_not_abort = false;

const char* __file_name_pretty (const char* file_path)
{
	if (const char* file_name = strrchr (file_path, '/')) {
		return file_name + 1;
	} else if (const char* file_name = strrchr (file_path, '\\')) {
		return file_name + 1;
	} else {
		return file_path;
	}
}