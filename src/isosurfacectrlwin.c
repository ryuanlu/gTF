#include <stdlib.h>
#include <memory.h>
#include "isosurfacectrlwin.h"
#include "isosurfacewin.h"
#include "marching_tetrahedron.h"
#include "intl.h"
#include "glext.h"
#include "mainwin.h"



strIsosurfacectrlwin	Isosurfacectrlwin;

void	tfCreateIsosurfacectrlwin(void)
{
	GtkBuilder		*builder;

	memset(&Isosurfacectrlwin,0,sizeof(strIsosurfacectrlwin));

	Isosurfacectrlwin.settings=GLWin_settings_new(Isosurfacewin.glwin);

	builder=gtk_builder_new();

	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/isosurface_settings.xml",NULL);
	gtk_builder_connect_signals(builder,NULL);

	Isosurfacectrlwin.frame		=GTK_WIDGET(gtk_builder_get_object(builder,"settings"));
	Isosurfacectrlwin.value		=GTK_WIDGET(gtk_builder_get_object(builder,"value"));
	Isosurfacectrlwin.usage		=GTK_WIDGET(gtk_builder_get_object(builder,"usage"));

	gtk_table_attach(GTK_TABLE(Isosurfacectrlwin.settings->table),Isosurfacectrlwin.frame,0,1,0,1,GTK_FILL,GTK_EXPAND,10,10);

	g_object_unref(G_OBJECT(builder));
}


void	tfSetupIsosurfacectrlwin(void)
{
	char	str[64];

	//	Window properties

	sprintf(str,"%s %s",_("Iso-surface - Marching tetrahedron"),_("Settings"));
	gtk_window_set_title(GTK_WINDOW(Isosurfacectrlwin.settings->window),str);

	g_signal_connect(GTK_OBJECT(Isosurfacectrlwin.settings->apply_button),"clicked",G_CALLBACK(Isosurfacectrlwin_Apply_handler),NULL);

}

void	Isosurfacectrlwin_Apply_handler(GtkWidget *widget,gpointer user_data)
{
	float	i;
	int	x;
	int	nop,nov,result;
	char	str[10];

	GtkWidget*	dialog;

	i=gtk_spin_button_get_value(GTK_SPIN_BUTTON(Isosurfacectrlwin.value));

	if(!gtftable.data)	return;

	glcanvas_make_current(Isosurfacewin.glwin->glcanvas, NULL);

	if(!Isosurfacewin.glwin->render_flag||i!=Isosurfacectrlwin.iso_value)
	{

		//	Initialization

		Isosurfacewin_Release();
		Isosurfacewin_Init();

		Isosurfacectrlwin.iso_value=i;


		//	Lock all manipulatable windows

		gtk_widget_set_sensitive(Isosurfacectrlwin.settings->window,FALSE);
		gtk_widget_set_sensitive(Isosurfacewin.glwin->window,FALSE);
		gtk_widget_set_sensitive(Mainwin.window,FALSE);


		if(!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(Mainwin.ikdtree_menuitem)))
			Mainwin_IKDtreeToggled_handler(NULL,NULL);

		result=Isosurface_extraction(gtftable.data,Isosurfacectrlwin.iso_value);



		sprintf(str,"%d",Isosurface_get_number_of_vertices()*3/262144);
		gtk_label_set_text(GTK_LABEL(Isosurfacectrlwin.usage),str);


		//	Unlock windows

		gtk_widget_set_sensitive(Isosurfacectrlwin.settings->window,TRUE);
		gtk_widget_set_sensitive(Isosurfacewin.glwin->window,TRUE);
		gtk_widget_set_sensitive(Mainwin.window,TRUE);

		//	Prevent from using wrong RC switched while handling main loop

		glcanvas_make_current(Isosurfacewin.glwin->glcanvas, NULL);

		//	Handle buffer overflowing

		if(result!=ISOSURFACE_OK)
		{
			dialog=gtk_message_dialog_new(GTK_WINDOW(Isosurfacectrlwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_CLOSE,_("Vertex buffer overflow, please increase the buffer size."));
			textdomain("gtk20");
			gtk_window_set_title(GTK_WINDOW(dialog),_("Warning"));
			textdomain("gtf");
			gtk_dialog_run(GTK_DIALOG (dialog));
			gtk_widget_destroy(dialog);
		}

		//	Create display list for Isosurface


		nop=Isosurface_get_used_pagesize();
		nov=Isosurface_get_number_of_vertices();

		Isosurfacewin.displaylist=glGenLists(Isosurface_get_used_pagesize());


		//	Draw triangles in fully filled page

		for(x=0;x<nop-1;++x)
		{
			glNewList(Isosurfacewin.displaylist+x,GL_COMPILE);
			glVertexPointer(3,GL_FLOAT,0,Isosurface_get_buffer(x));
			glDrawArrays(GL_TRIANGLES,0,Isosurfacewin.page_size);
			glEndList();
		}


		//	Draw triangles in under filled page

		glNewList(Isosurfacewin.displaylist+x,GL_COMPILE);
		glVertexPointer(3,GL_FLOAT,0,Isosurface_get_buffer(x));
		glDrawArrays(GL_TRIANGLES,0,nov%Isosurfacewin.page_size ? nov%Isosurfacewin.page_size : Isosurfacewin.page_size);
		glEndList();

	}



	Isosurfacewin.glwin->render_flag=TRUE;

}
