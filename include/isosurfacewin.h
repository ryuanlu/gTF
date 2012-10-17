#ifndef ISOSURFACEWIN_H_
#define ISOSURFACEWIN_H_


#include <gtk/gtk.h>
#include "glwin.h"


#define		ISOSURFACEWIN_DEFAULT_DISTANCE	(300)

typedef struct
{
	GLWin*		glwin;
	GtkWidget*	icon;

	GtkWidget*	menuitem[3];

	int		prev_pos[2];
	int		move;
	float		offset;

	int		progobj;
	int		shader[2];
	int		displaylist;

	int		page_size;

	int		normalloc;
	int		resolloc;

}strIsosurfacewin;

extern	strIsosurfacewin	Isosurfacewin;

void	tfCreateIsosurfacewin(void);
void	tfSetupIsosurfacewin(void);

G_MODULE_EXPORT	int	Isosurfacewin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	Isosurfacewin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	Isosurfacewin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	Isosurfacewin_ButtonUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	void	Isosurfacewin_Menu_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	int	Isosurfacewin_MouseMotion_handler(GtkWidget *widget, GdkEventMotion *event,gpointer user_data);
G_MODULE_EXPORT	int	Isosurfacewin_MouseWheel_handler(GtkWidget *widget, GdkEventScroll *event,gpointer user_data);

void	Isosurfacewin_Release(void);
void	Isosurfacewin_Init(void);


#endif /* ISOSURFACEWIN_H_ */
