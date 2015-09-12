#include <GL/glew.h>
#include <gtk/gtk.h>

#include "matrix.h"
#include "model.h"
#include "program.h"
#include "view.h"

static void
on_resize (GtkGLArea *area, gint width, gint height)
{
	view_recalc(width, height);
}

static gboolean
on_render (GtkGLArea *glarea, GdkGLContext *context)
{
	// Clear canvas:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw model:
	model_draw();

	// Don't propagate signal:
	return TRUE;
}

static void
on_update (GdkFrameClock *clock, GtkWidget *glarea)
{
	gtk_widget_queue_draw(glarea);
}

static void
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

	// Init program:
	program_init();

	// Init model:
	model_init();

	// Get frame clock:
	GdkGLContext *glcontext = gtk_gl_area_get_context(glarea);
	GdkWindow *glwindow = gdk_gl_context_get_window(glcontext);
	GdkFrameClock *frame_clock = gdk_window_get_frame_clock(glwindow);

	// Start updating:
	g_signal_connect(frame_clock, "update", G_CALLBACK(on_update), glarea);
	gdk_frame_clock_begin_updating(frame_clock);
}

int
gui_run (int argc, char **argv)
{
	gtk_init(&argc, &argv);

	// Create toplevel window, add GtkGLArea:
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *glarea = gtk_gl_area_new();
	gtk_container_add(GTK_CONTAINER(window), glarea);

	// Connect window signals:
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Connect glarea signals:
	g_signal_connect(glarea, "realize", G_CALLBACK(on_realize), NULL);
	g_signal_connect(glarea, "render",  G_CALLBACK(on_render),  NULL);
	g_signal_connect(glarea, "resize",  G_CALLBACK(on_resize),  NULL);

	gtk_widget_show_all(window);

	// Enter GTK event loop:
	gtk_main();

	return 0;
}
