#ifndef GLWIN_H_
#define GLWIN_H_

#include	<gtk/gtk.h>
#include	<gtk/gtkgl.h>


typedef struct
{
	GtkWidget*		window;
	GtkWidget*		icon;
	GtkWidget*		glcanvas;

	GtkWidget*		menu;
	GtkWidget*		menuitem;

	GdkGLContext*		glrc;
	GdkGLDrawable*	gldrawable;

	int			render_flag;

	float			fov;

}GLWin;


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

GLWin*		GLWin_new(const char* title,GdkGLContext* shared_glrc);

G_MODULE_EXPORT	int	GLWin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,GLWin* glwin);
G_MODULE_EXPORT	int	GLWin_PopupMenu_handler(GtkWidget *widget,GLWin* glwin);
G_MODULE_EXPORT	int	GLWin_Menu_handler(GtkWidget *widget,GLWin* glwin);


#endif /* GLWIN_H_ */
