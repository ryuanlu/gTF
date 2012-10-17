#include <stdlib.h>
#include <GL/glx.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include "gtk_glcanvas.h"



struct GtkGLCanvas
{
	GtkWidget*	canvas;
	Display*	display;
	Window		drawable;
	GLXContext	context;
};


GtkGLCanvas gtk_glcanvas_new(GtkWidget* widget, GLXContext sharelist)
{
	GtkGLCanvas glcanvas = NULL;
	int visual_attr[] = {	GLX_RGBA,
				GLX_DOUBLEBUFFER,
				GLX_RED_SIZE, 8,
				GLX_GREEN_SIZE, 8,
				GLX_BLUE_SIZE, 8,
				GLX_DEPTH_SIZE, 24,
				0 };
	XVisualInfo* vi = NULL;

	if(!widget) return NULL;

	glcanvas = calloc(1, sizeof(struct GtkGLCanvas));
	glcanvas->canvas = widget;

	gtk_widget_realize(widget);
	glcanvas->drawable = GDK_DRAWABLE_XID(widget->window);
	glcanvas->display = GDK_DRAWABLE_XDISPLAY(widget->window);

	vi = glXChooseVisual(glcanvas->display, DefaultScreen(glcanvas->display), visual_attr);
	glcanvas->context = glXCreateContext(glcanvas->display, vi, sharelist, GL_TRUE);

	gtk_widget_set_double_buffered(widget, FALSE);

	return glcanvas;
}

void gtk_glcanvas_delete(GtkGLCanvas* glcanvas)
{
	if(!*glcanvas) return;
	glXDestroyContext((*glcanvas)->display, (*glcanvas)->context);
	free(*glcanvas);
	*glcanvas = NULL;
}

void glcanvas_make_current(GtkGLCanvas glcanvas, GLXContext context)
{
	glXMakeCurrent(glcanvas->display, glcanvas->drawable, context == 0 ? glcanvas->context : context);
}

void glcanvas_swap_buffers(GtkGLCanvas glcanvas)
{
	glXSwapBuffers(glcanvas->display, glcanvas->drawable);
}

GLXContext glcanvas_get_render_context(GtkGLCanvas glcanvas)
{
	return glcanvas ? glcanvas->context : 0;
}
