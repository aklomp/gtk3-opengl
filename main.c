#include "gui.h"

// Application entry point
int
main (int argc, char **argv)
{
	return gui_init(&argc, &argv) && gui_run() ? 0 : 1;
}
