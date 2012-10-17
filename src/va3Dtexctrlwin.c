#include <memory.h>
#include "intl.h"
#include "mainwin.h"
#include "va3Dtexwin.h"
#include "va3Dtexctrlwin.h"


strVA3DTexctrlwin	VA3DTexctrlwin;

void	tfCreateVA3DTexctrlwin(void)
{
	GtkBuilder		*builder;

	memset(&VA3DTexctrlwin,0,sizeof(strVA3DTexctrlwin));

	VA3DTexctrlwin.settings=GLWin_settings_new(VA3DTexwin.glwin);

	builder=gtk_builder_new();

	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/va3dtex_settings.xml",NULL);
	gtk_builder_connect_signals(builder,NULL);

	VA3DTexctrlwin.frame		=GTK_WIDGET(gtk_builder_get_object(builder,"settings"));
	VA3DTexctrlwin.slice		=GTK_WIDGET(gtk_builder_get_object(builder,"slice"));

	gtk_table_attach(GTK_TABLE(VA3DTexctrlwin.settings->table),VA3DTexctrlwin.frame,0,1,0,1,GTK_FILL,GTK_EXPAND,10,10);
	g_object_unref(G_OBJECT(builder));
}


void	tfSetupVA3DTexctrlwin(void)
{
	char	str[64];

	//	Window properties

	sprintf(str,"%s %s",_("Volume Rendering - View aligned 3D texture"),_("Settings"));
	gtk_window_set_title(GTK_WINDOW(VA3DTexctrlwin.settings->window),str);

	//	Default settings
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(VA3DTexctrlwin.slice),2.0);


	g_signal_connect(GTK_OBJECT(VA3DTexctrlwin.settings->apply_button),"clicked",G_CALLBACK(VA3DTexctrlwin_Apply_handler),NULL);

}


void	VA3DTexctrlwin_Apply_handler(GtkWidget *widget,gpointer user_data)
{
	glcanvas_make_current(VA3DTexwin.glwin->glcanvas, NULL);

	VA3DTexwin.density=gtk_spin_button_get_value(GTK_SPIN_BUTTON(VA3DTexctrlwin.slice));

/*

	if(VA3DTexwin.render_flag)
	{
		//glDeleteLists(VA3DTexwin.displaylist,6);
		//VA3DTexwin_generate_displaylist();
	}

*/
	gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window, NULL, TRUE);
}
