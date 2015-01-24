/* reference program for the colors of the log tags */
#define DISP_LEVEL	DEBUG_LEVEL
#include <logger.h>

/*
 * Print all of the logging levels with their color names.
 */
static void print_colors()
{
	printlg(DEBUG_LEVEL, "Brown\n");
	printlg(ASSERT_LEVEL, "Cyan\n");
	printlg(INFO_LEVEL, "Green\n");
	printlg(WARNING_LEVEL, "Yellow\n");
	printlg(ERROR_LEVEL, "Light Red\n");
	printlg(FATAL_LEVEL, "Light Magenta\n");
}

int main(void)
{
	print_colors();

	return 0;
}
