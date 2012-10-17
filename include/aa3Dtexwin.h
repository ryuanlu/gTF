#ifndef AA3DTEXWIN_H_
#define AA3DTEXWIN_H_

#include "glwin.h"


#define	AA3DTEXWIN_DEFAULT_DISTANCE	(300)


typedef struct
{
	GLWin*		glwin;

	GtkWidget*	icon;
	GtkWidget*	menuitem[2];

	int		progobj;
	int		shader[2];
	int		displaylist;

	int		prev_pos[2];
	int		move;
	float		offset;

	float		density;

}strAA3DTexwin;

extern	strAA3DTexwin	AA3DTexwin;

void	tfCreateAA3DTexwin(void);
void	tfSetupAA3DTexwin(void);

G_MODULE_EXPORT	int	AA3DTexwin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	AA3DTexwin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	AA3DTexwin_MouseMotion_handler(GtkWidget *widget, GdkEventMotion *event,gpointer user_data);
G_MODULE_EXPORT	int	AA3DTexwin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	AA3DTexwin_ButtonUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	AA3DTexwin_MouseWheel_handler(GtkWidget *widget, GdkEventScroll *event,gpointer user_data);
//void	AA3DTexwin_generate_displaylist(void);
G_MODULE_EXPORT	void	AA3DTexwin_Menu_handler(GtkWidget *widget,gpointer user_data);
void	AA3DTexwin_Release(void);


#endif /* AA3DTEXWIN_H_ */
