#ifndef AA2DSLICEWIN_H_
#define AA2DSLICEWIN_H_

#include	<gtk/gtk.h>
#include	<gtk/gtkgl.h>

typedef struct
{
	GtkWidget*		window;
	GtkWidget*		vbox[4];
	GtkWidget*		hbox[4];
	GtkWidget*		icon;
	GtkWidget*		glcanvas;

	GtkWidget*		frame;
	GtkWidget*		axis;
	GtkWidget*		slice;
	GtkWidget*		separator;

	GdkGLContext*		glrc;
	GdkGLDrawable*	gldrawable;


	int			render_flag;

	int			progobj;
	int			shader[2];



}strAA2DSlicewin;

extern	strAA2DSlicewin	AA2DSlicewin;

void	tfCreateAA2DSlicewin(void);
void	tfSetupAA2DSlicewin(void);

G_MODULE_EXPORT	int	AA2DSlicewin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	void	AA2DSlicewin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	void	AA2DSlicewin_SliceSelect(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	AA2DSlicewin_AxisSelect(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	AA2DSlicewin_Release(void);




#endif /* AA2DSLICEWIN_H_ */
