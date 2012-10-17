#ifndef VA3DTEXCTRLWIN_H_
#define VA3DTEXCTRLWIN_H_

#include <gtk/gtk.h>
#include "glwin_settings.h"


typedef struct
{
	GtkWidget*	frame;
	GtkWidget*	slice;

	GLWin_settings*	settings;

}strVA3DTexctrlwin;

extern	strVA3DTexctrlwin	VA3DTexctrlwin;

void	tfCreateVA3DTexctrlwin(void);
void	tfSetupVA3DTexctrlwin(void);

G_MODULE_EXPORT	void	VA3DTexctrlwin_Apply_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	VA3DTexctrlwin_Close_handler(GtkWidget *widget,gpointer user_data);


#endif /* VA3DTEXCTRLWIN_H_ */
