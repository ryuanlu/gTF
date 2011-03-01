#include	<stdlib.h>
#include	<GL/gl.h>

#include	"mainwin.h"
#include	"glwin_settings.h"


//	GLWin_settings_new
//	------------------------------------------------------------------------
//	Description	:	Create a new GLWin_settings window.
//	Parameters	:
//
//		glwin		Parent OpenGL window.
//
//	Returns	:	The pointer of new GLWin_settings object.
//

GLWin_settings*	GLWin_settings_new(GLWin* glwin)
{
	GLWin_settings	*glwin_settings;
	GtkBuilder		*builder;

	glwin_settings=calloc(sizeof(GLWin_settings),1);

	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/glwin_settings.xml",NULL);
	gtk_builder_connect_signals(builder,glwin_settings);

	glwin_settings->window		=GTK_WIDGET(gtk_builder_get_object(builder,"window"));

	glwin_settings->exp_entry		=GTK_WIDGET(gtk_builder_get_object(builder,"exp"));
	glwin_settings->fov_entry		=GTK_WIDGET(gtk_builder_get_object(builder,"fov"));

	glwin_settings->apply_button	=GTK_WIDGET(gtk_builder_get_object(builder,"apply"));
	glwin_settings->close_button	=GTK_WIDGET(gtk_builder_get_object(builder,"close"));

	glwin_settings->background		=GTK_WIDGET(gtk_builder_get_object(builder,"background"));

	glwin_settings->light_color[0]	=GTK_WIDGET(gtk_builder_get_object(builder,"light_ambient"));
	glwin_settings->light_color[1]	=GTK_WIDGET(gtk_builder_get_object(builder,"light_diffuse"));
	glwin_settings->light_color[2]	=GTK_WIDGET(gtk_builder_get_object(builder,"light_specular"));

	glwin_settings->material_color[0]	=GTK_WIDGET(gtk_builder_get_object(builder,"material_ambient"));
	glwin_settings->material_color[1]	=GTK_WIDGET(gtk_builder_get_object(builder,"material_diffuse"));
	glwin_settings->material_color[2]	=GTK_WIDGET(gtk_builder_get_object(builder,"material_specular"));

	glwin_settings->table		=GTK_WIDGET(gtk_builder_get_object(builder,"table1"));

	glwin_settings->target=glwin;

	gtk_window_set_icon(GTK_WINDOW(glwin_settings->window),Mainwin.icon);


	//	Default settings

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(glwin_settings->exp_entry),64);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(glwin_settings->fov_entry),45);

	g_object_unref(G_OBJECT(builder));

	return	glwin_settings;
}

void	GLWin_settings_apply_handler(GtkWidget *widget,GLWin_settings *glwin_settings)
{
	GdkColor	gc;
	float		color[4];

	gdk_gl_drawable_make_current(glwin_settings->target->gldrawable,glwin_settings->target->glrc);

	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->background),&gc);
	glClearColor((float)gc.red/65535,(float)gc.green/65535,(float)gc.blue/65535,0.0f);


	color[3]=1.0;
	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->light_color[0]),&gc);
	color[0]=(float)gc.red/65535;
	color[1]=(float)gc.green/65535;
	color[2]=(float)gc.blue/65535;
	glLightfv(GL_LIGHT0,GL_AMBIENT,color);

	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->light_color[1]),&gc);
	color[0]=(float)gc.red/65535;
	color[1]=(float)gc.green/65535;
	color[2]=(float)gc.blue/65535;
	glLightfv(GL_LIGHT0,GL_DIFFUSE,color);

	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->light_color[2]),&gc);
	color[0]=(float)gc.red/65535;
	color[1]=(float)gc.green/65535;
	color[2]=(float)gc.blue/65535;
	glLightfv(GL_LIGHT0,GL_SPECULAR,color);


	//	setup material parameters

	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->material_color[0]),&gc);
	color[0]=(float)gc.red/65535;
	color[1]=(float)gc.green/65535;
	color[2]=(float)gc.blue/65535;
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);

	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->material_color[1]),&gc);
	color[0]=(float)gc.red/65535;
	color[1]=(float)gc.green/65535;
	color[2]=(float)gc.blue/65535;
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);

	gtk_color_button_get_color(GTK_COLOR_BUTTON(glwin_settings->material_color[2]),&gc);
	color[0]=(float)gc.red/65535;
	color[1]=(float)gc.green/65535;
	color[2]=(float)gc.blue/65535;
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color);

	glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,gtk_spin_button_get_value(GTK_SPIN_BUTTON(glwin_settings->exp_entry)));

	glMatrixMode(GL_PROJECTION);
	glwin_settings->target->fov=gtk_spin_button_get_value(GTK_SPIN_BUTTON(glwin_settings->fov_entry));;

	gdk_window_invalidate_rect(glwin_settings->target->window->window,NULL,TRUE);

	if(widget) gtk_window_present(GTK_WINDOW(glwin_settings->target->window));

}


