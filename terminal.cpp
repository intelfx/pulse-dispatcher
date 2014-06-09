#include "terminal.h"
#include <termios.h>
#include <unistd.h>

namespace {

termios old_tio;

} // anonymous namespace

void terminal_setup()
{
	termios new_tio;

	/* get the terminal settings for stdin */
	tcgetattr (STDIN_FILENO, &old_tio);

	/* we want to keep the old setting to restore them a the end */
	new_tio = old_tio;

	/* disable canonical mode (buffered i/o) and local echo */
	new_tio.c_lflag &= (~ICANON & ~ECHO);

	/* set the new settings immediately */
	tcsetattr (STDIN_FILENO, TCSANOW, &new_tio);
}

void terminal_restore()
{
	tcsetattr (STDIN_FILENO, TCSANOW, &old_tio);
}