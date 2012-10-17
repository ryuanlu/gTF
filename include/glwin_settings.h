#ifndef GLWIN_SETTINGS_H_
#define GLWIN_SETTINGS_H_


#include "glwin.h"

typedef struct
{
	GtkWidget*	window;
	GLWin*		target;
	GtkWidget*	icon;

	GtkWidget*	exp_entry;
	GtkWidget*	fov_entry;

	GtkWidget*	apply_button;
	GtkWidget*	close_button;

	GtkWidget*	background;
	GtkWidget*	light_color[3];
	GtkWidget*	material_color[3];

	GtkWidget*	table;

	int		render_flag;

}GLWin_settings;


//	GLWin_settings_new
//	------------------------------------------------------------------------
//	Description	:	Create a new GLWin_settings window.
//	Parameters	:
//
//		glwin		Parent OpenGL window.
//
//	Returns	:	The pointer of new GLWin_settings object.
//

GLWin_settings*	GLWin_settings_new(GLWin* glwin);

G_MODULE_EXPORT	void	GLWin_settings_apply_handler(GtkWidget *widget,GLWin_settings *glwin_settings);


#endif /* GLWIN_SETTINGS_H_ */
