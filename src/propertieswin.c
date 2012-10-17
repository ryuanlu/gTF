#include <memory.h>
#include "intl.h"
#include "gTF.h"
#include "mainwin.h"
#include "propertieswin.h"

strPropertieswin	Propertieswin;

void	tfCreatePropertieswin(void)
{
	GtkBuilder		*builder;
	GtkCellRenderer*	cell;
	GtkTreeViewColumn*	col[2];


	memset(&Propertieswin,0,sizeof(strPropertieswin));

	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/properties.xml",NULL);
	gtk_builder_connect_signals(builder,NULL);

	Propertieswin.window=GTK_WIDGET(gtk_builder_get_object(builder,"window"));
	Propertieswin.content=GTK_WIDGET(gtk_builder_get_object(builder,"properties_view"));
	Propertieswin.properties=GTK_LIST_STORE(gtk_builder_get_object(builder,"properties"));
	Propertieswin.filename=GTK_WIDGET(gtk_builder_get_object(builder,"filename"));
	g_object_unref(G_OBJECT(builder));

	cell=gtk_cell_renderer_text_new();

	col[0]=gtk_tree_view_column_new_with_attributes("Item",cell,"markup",0,"xalign",2,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(Propertieswin.content), col[0]);

	col[1]=gtk_tree_view_column_new_with_attributes("Content",cell,"markup",1,"xalign",3,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(Propertieswin.content), col[1]);

}


void	tfSetupPropertieswin(void)
{
	gtk_window_set_title(GTK_WINDOW(Propertieswin.window),_("Properties"));
	gtk_window_set_icon(GTK_WINDOW(Propertieswin.window),Mainwin.icon);
}


void	Propertieswin_Close_handler(GtkWidget *widget,gpointer user_data)
{
	gtk_widget_hide_all(Propertieswin.window);

}

void	Propertieswin_update(void)
{
	char	str[256];
	GtkTreeIter	iter;

	if(!gtftable.data)	return;

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	gtk_list_store_set(Propertieswin.properties,&iter,1,gtftable.data->rawfilename,-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	sprintf(str,"%d x %d x %d",gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2]);
	gtk_list_store_set(Propertieswin.properties,&iter,1,str,-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	sprintf(str,"%.3f : %.3f : %.3f",gtftable.data->ratio[0],gtftable.data->ratio[1],gtftable.data->ratio[2]);
	gtk_list_store_set(Propertieswin.properties,&iter,1,str,-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	if(gtftable.data->sample_size==1)
		gtk_list_store_set(Propertieswin.properties,&iter,1,"unsigned char",-1);
	if(gtftable.data->sample_size==2)
		gtk_list_store_set(Propertieswin.properties,&iter,1,"unsigned short",-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	if(gtftable.data->byte_order==BYTE_ORDER_BIG_ENDIAN)
		gtk_list_store_set(Propertieswin.properties,&iter,1,"Big-endian",-1);
	else	gtk_list_store_set(Propertieswin.properties,&iter,1,"Little-endian",-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);



	sprintf(str,"%ld bytes",(long int)(gtftable.data->totalsize*gtftable.data->sample_size));
	gtk_list_store_set(Propertieswin.properties,&iter,1,str,-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	dataGetMaxMin(gtftable.data);
	sprintf(str,"%d / %d",gtftable.data->vmax,gtftable.data->vmin);
	gtk_list_store_set(Propertieswin.properties,&iter,1,str,-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	if(gtftable.data->ikdtreefilename)
		gtk_list_store_set(Propertieswin.properties,&iter,1,gtftable.data->ikdtreefilename,-1);
	else
		gtk_list_store_set(Propertieswin.properties,&iter,1,_("<b>Unassigned</b>"),-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


	if(gtftable.data->normalfilename)
		gtk_list_store_set(Propertieswin.properties,&iter,1,gtftable.data->normalfilename,-1);
	else
		gtk_list_store_set(Propertieswin.properties,&iter,1,_("<b>Unassigned</b>"),-1);
	gtk_tree_model_iter_next(GTK_TREE_MODEL(Propertieswin.properties),&iter);


}
