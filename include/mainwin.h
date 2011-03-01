#ifndef MAINWIN_H_
#define MAINWIN_H_

#include	<gtk/gtk.h>
#include	"gTF.h"

typedef struct
{
	//	GTK widgets

	GtkWidget*		window;
	GtkWidget*		vbox;

	GdkPixbuf*		icon;
	int			n_frame;
	GtkWidget*		tf_selector;
	GtkWidget*		frame[NUMBER_OF_TF];

	GtkWidget*		view_menuitem[5];
	GtkWidget*		ikdtree_menuitem;

	GtkWidget*		opendialog;
	GtkWidget*		errordialog;
	GtkWidget*		about;


	GtkListStore*		tflist;


}strMainwin;

extern	strMainwin		Mainwin;

void	tfCreateMainwin(void);
void	tfSetupMainwin(void);
int	tfCreateTFpanel(const char*);
GtkWidget*	tfGetPanel(int n);

void	tfRelease(void);

G_MODULE_EXPORT	void	Mainwin_OpenClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_PropertiesClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_QuitClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_ViewClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_AboutClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_AboutClose_handler(GtkWidget *widget,gpointer user_data);

G_MODULE_EXPORT	void	Mainwin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_IKDtreeToggled_handler(GtkWidget *widget,gpointer user_data);

G_MODULE_EXPORT	void	Mainwin_GradientClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_IKDtreeClicked_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	Mainwin_HistogramEQClicked_handler(GtkWidget *widget,gpointer user_data);

G_MODULE_EXPORT	void	Mainwin_TFChanged_handler(GtkWidget *widget,gpointer user_data);


#endif /*MAINWIN_H_*/
