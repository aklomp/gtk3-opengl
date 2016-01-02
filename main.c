#include <stdbool.h>

#include "gui.h"

// Application entry point
int
main (int argc, char **argv)
{
	if (!gui_init(&argc, &argv))
		return 1;

	if (!gui_run())
		return 1;

	return 0;
}
