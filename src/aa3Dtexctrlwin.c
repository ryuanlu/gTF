#include <stdlib.h>
#include <memory.h>
#include "intl.h"
#include "gTF.h"
#include "glext.h"
#include "mainwin.h"
#include "aa3Dtexwin.h"
#include "aa3Dtexctrlwin.h"


strAA3DTexctrlwin	AA3DTexctrlwin;

void	tfCreateAA3DTexctrlwin(void)
{
	GtkBuilder		*builder;

	memset(&AA3DTexctrlwin,0,sizeof(strAA3DTexctrlwin));

	AA3DTexctrlwin.settings=GLWin_settings_new(AA3DTexwin.glwin);

	builder=gtk_builder_new();

	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/aa3dtex_settings.xml",NULL);
	gtk_builder_connect_signals(builder,NULL);

	AA3DTexctrlwin.frame		=GTK_WIDGET(gtk_builder_get_object(builder,"settings"));
	AA3DTexctrlwin.slice		=GTK_WIDGET(gtk_builder_get_object(builder,"slice"));

	gtk_table_attach(GTK_TABLE(AA3DTexctrlwin.settings->table),AA3DTexctrlwin.frame,0,1,0,1,GTK_FILL,GTK_EXPAND,10,10);
	g_object_unref(G_OBJECT(builder));
}


void	tfSetupAA3DTexctrlwin(void)
{
	char	str[64];

	//	Window properties

	sprintf(str,"%s %s",_("Volume Rendering - Axis aligned 3D texture"),_("Settings"));
	gtk_window_set_title(GTK_WINDOW(AA3DTexctrlwin.settings->window),str);

	//	Default settings
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(AA3DTexctrlwin.slice),2.0);


	g_signal_connect(GTK_OBJECT(AA3DTexctrlwin.settings->apply_button),"clicked",G_CALLBACK(AA3DTexctrlwin_Apply_handler),NULL);

}


void	AA3DTexctrlwin_Apply_handler(GtkWidget *widget,gpointer user_data)
{
	glcanvas_make_current(AA3DTexwin.glwin->glcanvas, NULL);
	AA3DTexwin.density=gtk_spin_button_get_value(GTK_SPIN_BUTTON(AA3DTexctrlwin.slice));

/*

	if(AA3DTexwin.render_flag)
	{
		//glDeleteLists(AA3DTexwin.displaylist,6);
		//AA3DTexwin_generate_displaylist();
	}

*/
	gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);
}
