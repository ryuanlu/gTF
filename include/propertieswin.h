#ifndef PROPERTIESWIN_H_
#define PROPERTIESWIN_H_

#include <gtk/gtk.h>

typedef struct
{
	GtkWidget*	window;

	GtkWidget*	content;
	GtkWidget*	filename;
	GtkListStore*	properties;

}strPropertieswin;

extern	strPropertieswin	Propertieswin;


void	tfCreatePropertieswin(void);
void	tfSetupPropertieswin(void);

G_MODULE_EXPORT	void	Propertieswin_Close_handler(GtkWidget *widget,gpointer user_data);

void	Propertieswin_update(void);



#endif /* PROPERTIESWIN_H_ */
