#include <stdbool.h>

#include <GL/glew.h>
#include <gtk/gtk.h>

#include "background.h"
#include "matrix.h"
#include "model.h"
#include "program.h"
#include "util.h"
#include "view.h"

static gboolean panning = FALSE;

void
on_resize (GtkGLArea *area, gint width, gint height)
{
	view_set_window(width, height);
	background_set_window(width, height);
}

gboolean
on_render (GtkGLArea *glarea, GdkGLContext *context)
{
	// Clear canvas:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw background:
	background_draw();

	// Draw model:
	model_draw();

	// Don't propagate signal:
	return TRUE;
}

void
on_realize (GtkGLArea *glarea)
{
	// Make current:
	gtk_gl_area_make_current(glarea);

	// Init GLEW:
	glewExperimental = GL_TRUE;
	glewInit();

	// Print version info:
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// Enable depth buffer:
	gtk_gl_area_set_has_depth_buffer(glarea, TRUE);

	// Init programs:
	programs_init();

	// Init background:
	background_init();

	// Init model:
	model_init();

	// Get frame clock:
	GdkGLContext *glcontext = gtk_gl_area_get_context(glarea);
	GdkWindow *glwindow = gdk_gl_context_get_window(glcontext);
	GdkFrameClock *frame_clock = gdk_window_get_frame_clock(glwindow);

	// Connect update signal:
	g_signal_connect_swapped
		( frame_clock
		, "update"
		, G_CALLBACK(gtk_gl_area_queue_render)
		, glarea
		) ;

	// Start updating:
	gdk_frame_clock_begin_updating(frame_clock);
}

gboolean
on_button_press (GtkWidget *widget, GdkEventButton *event)
{
	GtkAllocation allocation;
	gtk_widget_get_allocation(widget, &allocation);

	if (event->button == 1)
		if (panning == FALSE) {
			panning = TRUE;
			model_pan_start(event->x, allocation.height - event->y);
		}

	return FALSE;
}

gboolean
on_button_release (GtkWidget *widget, GdkEventButton *event)
{
	if (event->button == 1)
		panning = FALSE;

	return FALSE;
}

gboolean
on_motion_notify (GtkWidget *widget, GdkEventMotion *event)
{
	GtkAllocation allocation;
	gtk_widget_get_allocation(widget, &allocation);

	if (panning == TRUE)
		model_pan_move(event->x, allocation.height - event->y);

	return FALSE;
}

gboolean
on_scroll (GtkWidget* widget, GdkEventScroll *event)
{
	switch (event->direction)
	{
	case GDK_SCROLL_UP:
		view_z_decrease();
		break;

	case GDK_SCROLL_DOWN:
		view_z_increase();
		break;

	default:
		break;
	}

	return FALSE;
}

bool
gui_init (int *argc, char ***argv)
{
	// Initialize GTK:
	if (!gtk_init_check(argc, argv)) {
		fputs("Could not initialize GTK", stderr);
		return false;
	}

	return true;
}

bool
gui_run (void)
{
	// Create builder:
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "gui.glade", NULL);

	// Get reference to toplevel window:
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_builder_connect_signals(builder, NULL);
	g_object_unref(builder);

	// Show toplevel window:
	gtk_widget_show_all(window);

	// Enter GTK event loop:
	gtk_main();

	return true;
}
