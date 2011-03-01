#include	<stdlib.h>
#include	"util.h"
#include	"gTF.h"

#include	"glwin.h"
#include	"aa2Dslicewin.h"



//	GLWin_new
//	------------------------------------------------------------------------
//	Description	:	Create a new OpenGL window.
//	Parameters	:
//
//		title		The title of the OpenGL window.
//		shared_glrc	Shared OpenGL render context.
//
//	Returns	:	The pointer of new GLWin object.
//

GLWin*		GLWin_new(const char* title,GdkGLContext* shared_glrc)
{
	GLWin		*glwin;
	GtkBuilder	*builder;
	GdkGLConfig	*glconfig;

	glwin=calloc(sizeof(GLWin),1);

	//	Create widgets

	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/glwin.xml",NULL);
	gtk_builder_connect_signals(builder,glwin);


	glwin->window		=GTK_WIDGET(gtk_builder_get_object(builder,"glwin"));
	glwin->glcanvas	=GTK_WIDGET(gtk_builder_get_object(builder,"glcanvas"));

	glwin->menu		=GTK_WIDGET(gtk_builder_get_object(builder,"popup_menu"));
	g_object_ref(G_OBJECT(glwin->menu));

	glwin->menuitem	=GTK_WIDGET(gtk_builder_get_object(builder,"snapshot_menuitem"));
	glwin->icon		=GTK_WIDGET(gtk_builder_get_object(builder,"camera"));

	g_object_unref(G_OBJECT(builder));


	//	OpenGL setup

	glconfig=gdk_gl_config_new_by_mode(GDK_GL_MODE_RGB|GDK_GL_MODE_DEPTH|GDK_GL_MODE_DOUBLE);
	gtk_widget_set_gl_capability(glwin->glcanvas,glconfig,shared_glrc,TRUE,GDK_GL_RGBA_TYPE);

	gtk_widget_realize(glwin->glcanvas);
	gtk_widget_show_all(glwin->menu);

	glwin->glrc=gtk_widget_get_gl_context(glwin->glcanvas);
	glwin->gldrawable=gtk_widget_get_gl_drawable(glwin->glcanvas);


	//	Window properties

	gtk_window_set_title(GTK_WINDOW(glwin->window),_(title));
	gdk_window_set_cursor(glwin->glcanvas->window,gdk_cursor_new(GDK_HAND1));
	gtk_window_set_default_size(GTK_WINDOW(glwin->window),800,800);
	glwin->fov=45.0f;
	return	glwin;
}



int	GLWin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,GLWin* glwin)
{
	if(event->button==3&&event->type==GDK_BUTTON_PRESS)
		gtk_menu_popup(GTK_MENU(glwin->menu),NULL,NULL,NULL,NULL,event->button,event->time);

	return FALSE;
}

int	GLWin_PopupMenu_handler(GtkWidget *widget,GLWin* glwin)
{
	gtk_menu_popup(GTK_MENU(glwin->menu),NULL,NULL,NULL,NULL,0,0);

	return FALSE;
}


int	GLWin_Menu_handler(GtkWidget *widget,GLWin* glwin)
{
	if(widget==glwin->menuitem)
	{
		gdk_gl_drawable_make_current(glwin->gldrawable,glwin->glrc);
		glTakeScreenshot(glwin->window);
	}
	return FALSE;
}
