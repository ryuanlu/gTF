#ifndef AA3DTEXCTRLWIN_H_
#define AA3DTEXCTRLWIN_H_

#include <gtk/gtk.h>
#include "glwin_settings.h"


typedef struct
{
	GtkWidget*	frame;
	GtkWidget*	slice;

	GLWin_settings*	settings;

}strAA3DTexctrlwin;

extern	strAA3DTexctrlwin	AA3DTexctrlwin;

void	tfCreateAA3DTexctrlwin(void);
void	tfSetupAA3DTexctrlwin(void);

G_MODULE_EXPORT	void	AA3DTexctrlwin_Apply_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	AA3DTexctrlwin_Close_handler(GtkWidget *widget,gpointer user_data);


#endif /* AA3DTEXCTRLWIN_H_ */
