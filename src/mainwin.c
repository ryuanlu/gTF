#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "intl.h"
#include "glext.h"
#include "gTF.h"
#include "util.h"
#include "ikdtree.h"
#include "propertieswin.h"
#include "aa2Dslicewin.h"
#include "aa3Dtexwin.h"
#include "va3Dtexwin.h"
#include "isosurfacewin.h"
#include "isosurfacectrlwin.h"
#include "etf1d.h"
#include "mainwin.h"
#include "../cube.xpm"


strMainwin	Mainwin;

void	tfCreateMainwin(void)
{
	GtkBuilder*	builder;
	GtkCellRenderer*	cell;

	memset(&Mainwin,0,sizeof(strMainwin));

	//	Create widgets

	Mainwin.icon=gdk_pixbuf_new_from_xpm_data(cube_xpm);

	builder=gtk_builder_new();

	gtk_builder_set_translation_domain(builder,"gtf");
	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/gtf.xml",NULL);

	gtk_builder_connect_signals(builder,NULL);

	Mainwin.window		=GTK_WIDGET(gtk_builder_get_object(builder,"mainwin"));
	Mainwin.tf_selector	=GTK_WIDGET(gtk_builder_get_object(builder,"tf_selector"));
	Mainwin.vbox		=GTK_WIDGET(gtk_builder_get_object(builder,"vbox1"));
	Mainwin.about		=GTK_WIDGET(gtk_builder_get_object(builder,"aboutdialog"));

	Mainwin.view_menuitem[0]=GTK_WIDGET(gtk_builder_get_object(builder,"view_menuitem_1"));
	Mainwin.view_menuitem[1]=GTK_WIDGET(gtk_builder_get_object(builder,"view_menuitem_2"));
	Mainwin.view_menuitem[2]=GTK_WIDGET(gtk_builder_get_object(builder,"view_menuitem_3"));
	Mainwin.view_menuitem[3]=GTK_WIDGET(gtk_builder_get_object(builder,"view_menuitem_4"));
	Mainwin.view_menuitem[4]=GTK_WIDGET(gtk_builder_get_object(builder,"view_menuitem_5"));


	Mainwin.ikdtree_menuitem=GTK_WIDGET(gtk_builder_get_object(builder,"tools_menuitem_5"));

	cell			=gtk_cell_renderer_text_new();

	g_object_unref(G_OBJECT(builder));

	//gtk_widget_show_all(Mainwin.window);

	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(Mainwin.tf_selector),cell,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(Mainwin.tf_selector),cell,"text",0,NULL);

}


void	tfSetupMainwin(void)
{
	gtk_window_set_icon(GTK_WINDOW(Mainwin.window),Mainwin.icon);
	gtk_window_set_icon(GTK_WINDOW(Mainwin.about),Mainwin.icon);

	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(Mainwin.about),Mainwin.icon);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(Mainwin.about),_("Transfer Function"));
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(Mainwin.about),GTF_VERSION);

	gtk_widget_show_all(Mainwin.window);

}

int	tfCreateTFpanel(const char* name)
{
	int i;
	i=Mainwin.n_frame;
	Mainwin.frame[i]=gtk_frame_new(name);

	//gtk_widget_set_no_show_all(Mainwin.frame[i],TRUE);

	gtk_box_pack_start(GTK_BOX(Mainwin.vbox),Mainwin.frame[i],1,1,0);

	gtk_combo_box_append_text(GTK_COMBO_BOX(Mainwin.tf_selector),name);

	++Mainwin.n_frame;

//	if(i==0)
//	{
//		gtk_combo_box_set_active(GTK_COMBO_BOX(Mainwin.tf_selector),0);
//	}

	return i;
}

GtkWidget*	tfGetPanel(int n)
{
	return Mainwin.frame[n];
}

void	tfRelease(void)
{
	int	r;

	gtftable.TF_release[gtftable.current_tf]();

	dataClose(gtftable.data);
	glDeleteTextures(1,&gtftable.volume_tex);
	glDeleteTextures(1,&gtftable.gradient_tex);

	g_signal_emit_by_name(AA3DTexwin.glwin->window,"delete-event",NULL,&r);
	g_signal_emit_by_name(VA3DTexwin.glwin->window,"delete-event",NULL,&r);
	g_signal_emit_by_name(Isosurfacewin.glwin->window,"delete-event",NULL,&r);
	AA2DSlicewin_Close_handler(NULL,NULL,NULL);

}


void	Mainwin_OpenClicked_handler(GtkWidget *widget,gpointer user_data)
{
	char*	filename=NULL;
	char*	tmp=NULL;
	char	str[256];
	DATA*	data;

	//	Create Open File Dialog

	filename=FileDialog(OPEN_DIALOG,Mainwin.window);


	//
	//	Open volume file

	if(!filename)	return;

	data=dataOpen(filename);

	if(!data)
	{
		Mainwin.errordialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,_("Invalid volume information file"));
		textdomain("gtk20");
		gtk_window_set_title(GTK_WINDOW(Mainwin.errordialog),_("Warning"));
		textdomain("gtf");
		gtk_dialog_run(GTK_DIALOG (Mainwin.errordialog));
		gtk_widget_destroy(Mainwin.errordialog);
		free(filename);
		return;
	}

	if(gtftable.data)
		tfRelease();

	gtftable.data=data;


	//	Set window title

	tmp=filename;
	filename=g_locale_to_utf8(filename,-1,NULL,NULL,NULL);
	free(tmp);
	sprintf(str,"%s - %s",_("Transfer Function"),filename);
	gtk_label_set_label(GTK_LABEL(Propertieswin.filename),filename);
	free(filename);	// It should be deleted ?
	gtk_window_set_title(GTK_WINDOW(Mainwin.window),str);


	//	Load data from volume

	dataReadRAW(gtftable.data);

	Propertieswin_update();

	dataReadIKDtree(gtftable.data);
	dataReadGradient(gtftable.data);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(Mainwin.ikdtree_menuitem),gtftable.data->iKDtree ? TRUE:FALSE);
	gtk_widget_set_sensitive(Mainwin.ikdtree_menuitem,gtftable.data->iKDtree ? FALSE:TRUE);


	if(!gtftable.data->iKDtree) dataGetMaxMin(gtftable.data);

	dataGenGradient(gtftable.data);

	gtftable.level=gtftable.data->vmax-gtftable.data->vmin+1;

	//tree=IKDtree_new_from_volume(gtftable.data,21,4,50);
	//IKDtree_write_to_file(tree,"cthead.ikdtree");


	//	Load as textures

	glcanvas_make_current(AA3DTexwin.glwin->glcanvas, gtftable.sharedrc);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glGenTextures(1,&gtftable.volume_tex);
	glBindTexture(GL_TEXTURE_3D,gtftable.volume_tex);
	glTexImage3D(GL_TEXTURE_3D,0,GL_LUMINANCE16,gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2],0,GL_LUMINANCE,GL_UNSIGNED_SHORT,gtftable.data->rawdata);
	//glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY16F_ARB,gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2],0,GL_INTENSITY,GL_UNSIGNED_SHORT,gtftable.data->rawdata);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);


	//	GL_RGBA32F doesn't support GL_Linear filtering. Use GL_RGBA16F.

	glGenTextures(1,&gtftable.gradient_tex);
	glBindTexture(GL_TEXTURE_3D,gtftable.gradient_tex);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA16F,gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2],0,GL_RGBA,GL_FLOAT,gtftable.data->gradient);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);



	gtftable.TF_init[gtftable.current_tf]();



	gtk_widget_queue_draw(Mainwin.window);
}

void	Mainwin_PropertiesClicked_handler(GtkWidget *widget,gpointer user_data)
{
	gtk_widget_show_all(Propertieswin.window);
}

void	Mainwin_QuitClicked_handler(GtkWidget *widget,gpointer user_data)
{
	Mainwin_Close_handler(NULL,NULL,NULL);
}


void	Mainwin_ViewClicked_handler(GtkWidget *widget,gpointer user_data)
{
	int r;
	if(widget==Mainwin.view_menuitem[0])
	{
		//	Show or close AA2Dslice window

		if(GTK_WIDGET_VISIBLE(AA2DSlicewin.window))
		{
			AA2DSlicewin_Close_handler(NULL,NULL,NULL);

		}
		else
		{
			gtk_widget_show_all(AA2DSlicewin.window);
			if(!gtftable.data) return;

			gtftable.AA2Dslice_init[gtftable.current_tf]();

		}

	}
	if(widget==Mainwin.view_menuitem[1])
	{
		//	Show or close AA3Dtex window

		if(GTK_WIDGET_VISIBLE(AA3DTexwin.glwin->window))
		{
			g_signal_emit_by_name(AA3DTexwin.glwin->window,"delete-event",NULL,&r);
		}
		else
		{
			gtk_widget_show_all(AA3DTexwin.glwin->window);
			if(!gtftable.data) return;

			gtftable.AA3Dtex_init[gtftable.current_tf]();

		}
	}

	if(widget==Mainwin.view_menuitem[2])
	{
		//	Show or close VA3Dtex window

		if(GTK_WIDGET_VISIBLE(VA3DTexwin.glwin->window))
		{
			g_signal_emit_by_name(VA3DTexwin.glwin->window,"delete-event",NULL,&r);
		}
		else
		{
			gtk_widget_show_all(VA3DTexwin.glwin->window);
			if(!gtftable.data) return;

			gtftable.VA3Dtex_init[gtftable.current_tf]();

		}
	}


	if(widget==Mainwin.view_menuitem[3])
	{

		//	Show or close Isosurfacewin window

		if(GTK_WIDGET_VISIBLE(Isosurfacewin.glwin->window))
			g_signal_emit_by_name(Isosurfacewin.glwin->window,"delete-event",NULL,&r);
		else
		{
			gtk_widget_show_all(Isosurfacewin.glwin->window);
			Isosurfacewin_Init();
			gtk_widget_show_all(Isosurfacectrlwin.settings->window);
		}
	}


}

void	Mainwin_AboutClicked_handler(GtkWidget *widget,gpointer user_data)
{
	gtk_widget_show_all(Mainwin.about);
}

void	Mainwin_AboutClose_handler(GtkWidget *widget,gpointer user_data)
{
	gtk_widget_hide_all(Mainwin.about);
}

void	Mainwin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	tfRelease();
	gtk_main_quit();
}

void	Mainwin_IKDtreeToggled_handler(GtkWidget *widget,gpointer user_data)
{
	int	i,n;

	if(!gtftable.data->iKDtree)
	{
		//	Get the minimal dimension

		n=9999;
		for(i=0;i<3;++i)
			if(n>gtftable.data->resol[i]) n=gtftable.data->resol[i];

		gtftable.data->iKDtree=IKDtree_new_from_volume(gtftable.data,log(n)/log(2)*3,4,(gtftable.data->vmax-gtftable.data->vmin)/20);

	}
}

void	Mainwin_GradientClicked_handler(GtkWidget *widget,gpointer user_data)
{
	char*	filename=NULL;
	FILE*	fp=NULL;

	if(!gtftable.data)	return;

	filename=FileDialog(SAVE_DIALOG,Mainwin.window);
	if(!filename)	return;

	fp=fopen(filename,"wb");
	free(filename);

	if(!fp)
	{
		Mainwin.errordialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,_("Failed to open file"));
		textdomain("gtk20");
		gtk_window_set_title(GTK_WINDOW(Mainwin.errordialog),_("Warning"));
		textdomain("gtf");
		gtk_dialog_run(GTK_DIALOG (Mainwin.errordialog));
		gtk_widget_destroy(Mainwin.errordialog);
		fclose(fp);
		return;
	}

	fwrite(&gtftable.data->gmax,sizeof(float),1,fp);
	fwrite(&gtftable.data->gmin,sizeof(float),1,fp);
	fwrite(gtftable.data->gradient,sizeof(float)*4*gtftable.data->totalsize,1,fp);
	fclose(fp);

	Mainwin.errordialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,_("File saved successfully"));
	textdomain("gtk20");
	//gtk_window_set_title(GTK_WINDOW(Mainwin.errordialog),_("Warning"));
	textdomain("gtf");
	gtk_dialog_run(GTK_DIALOG (Mainwin.errordialog));
	gtk_widget_destroy(Mainwin.errordialog);

}

void	Mainwin_IKDtreeClicked_handler(GtkWidget *widget,gpointer user_data)
{
	char*	filename=NULL;

	if(!gtftable.data)	return;

	filename=FileDialog(SAVE_DIALOG,Mainwin.window);
	if(!filename)	return;

//	if(!fp)
//	{
//		Mainwin.errordialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,_("Failed to open file"));
//		textdomain("gtk20");
//		gtk_window_set_title(GTK_WINDOW(Mainwin.errordialog),_("Warning"));
//		textdomain("gtf");
//		gtk_dialog_run(GTK_DIALOG (Mainwin.errordialog));
//		gtk_widget_destroy(Mainwin.errordialog);
//		fclose(fp);
//		return;
//	}

	if(!gtftable.data->iKDtree)
		Mainwin_IKDtreeToggled_handler(NULL,NULL);

	IKDtree_write_to_file(gtftable.data->iKDtree,filename);
	free(filename);

	Mainwin.errordialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,_("File saved successfully"));
	textdomain("gtk20");
	//gtk_window_set_title(GTK_WINDOW(Mainwin.errordialog),_("Warning"));
	textdomain("gtf");
	gtk_dialog_run(GTK_DIALOG(Mainwin.errordialog));
	gtk_widget_destroy(Mainwin.errordialog);

}

void	Mainwin_HistogramEQClicked_handler(GtkWidget *widget,gpointer user_data)
{
	if(!gtftable.data) return;

	dataHistogramEqualization(gtftable.data);
	gtftable.TF_release[gtftable.previous_tf]();
	gtftable.TF_init[gtftable.current_tf]();


}

void	Mainwin_TFChanged_handler(GtkWidget *widget,gpointer user_data)
{
	gtftable.previous_tf=gtftable.current_tf;
	gtftable.current_tf=gtk_combo_box_get_active(GTK_COMBO_BOX(Mainwin.tf_selector));

	gtk_widget_hide(Mainwin.frame[gtftable.previous_tf]);

	if(gtftable.data)
	{
		gtftable.TF_release[gtftable.previous_tf]();
		gtftable.TF_init[gtftable.current_tf]();
	}

	glcanvas_make_current(AA2DSlicewin.glcanvas, NULL);
	glUseProgram(gtftable.shader[gtftable.current_tf].aa2Dslice);
	if(GTK_WIDGET_VISIBLE(AA2DSlicewin.window))gtftable.AA2Dslice_init[gtftable.current_tf]();

	glcanvas_make_current(AA3DTexwin.glwin->glcanvas, NULL);
	glUseProgram(gtftable.shader[gtftable.current_tf].aa3Dtex);
	if(GTK_WIDGET_VISIBLE(AA3DTexwin.glwin->window))gtftable.AA3Dtex_init[gtftable.current_tf]();

	glcanvas_make_current(VA3DTexwin.glwin->glcanvas, NULL);
	glUseProgram(gtftable.shader[gtftable.current_tf].va3Dtex);
	if(GTK_WIDGET_VISIBLE(VA3DTexwin.glwin->window))gtftable.VA3Dtex_init[gtftable.current_tf]();


	gtk_widget_show(Mainwin.frame[gtftable.current_tf]);


}



