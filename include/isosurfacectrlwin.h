#ifndef ISOSURFACECTRLWIN_H_
#define ISOSURFACECTRLWIN_H_


#include <gtk/gtk.h>
#include "glwin_settings.h"


typedef struct
{
	GtkWidget*	window;
	GtkWidget*	frame;

	float		iso_value;
	GtkWidget*	value;
	GtkWidget*	usage;

	GLWin_settings*	settings;


}strIsosurfacectrlwin;

extern	strIsosurfacectrlwin	Isosurfacectrlwin;

void	tfCreateIsosurfacectrlwin(void);
void	tfSetupIsosurfacectrlwin(void);

G_MODULE_EXPORT	void	Isosurfacectrlwin_Apply_handler(GtkWidget *widget,gpointer user_data);


#endif /* ISOSURFACECTRLWIN_H_ */
