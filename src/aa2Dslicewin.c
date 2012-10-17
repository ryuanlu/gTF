#include <stdlib.h>
#include <memory.h>
#include "intl.h"
#include "gTF.h"
#include "glext.h"
#include "mainwin.h"
#include "util.h"
#include "etf1d.h"
#include "aa2Dslicewin.h"

strAA2DSlicewin	AA2DSlicewin;

void	tfCreateAA2DSlicewin(void)
{
	memset(&AA2DSlicewin,0,sizeof(strAA2DSlicewin));
	AA2DSlicewin.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	AA2DSlicewin.vbox[0]=gtk_vbox_new(FALSE,0);
	AA2DSlicewin.hbox[0]=gtk_hbox_new(FALSE,0);
	AA2DSlicewin.canvas=gtk_drawing_area_new();

	AA2DSlicewin.axis=gtk_combo_box_new_text();

	textdomain("gtf");

	gtk_combo_box_append_text(GTK_COMBO_BOX(AA2DSlicewin.axis),_("Z axis - XY plane"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(AA2DSlicewin.axis),_("Y axis - ZX plane"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(AA2DSlicewin.axis),_("X axis - YZ plane"));

	AA2DSlicewin.slice=gtk_spin_button_new_with_range(0.0,0.0,1.0);


	gtk_box_pack_start(GTK_BOX(AA2DSlicewin.hbox[0]),AA2DSlicewin.axis,0,0,5);
	gtk_box_pack_start(GTK_BOX(AA2DSlicewin.hbox[0]),AA2DSlicewin.slice,0,0,5);

	gtk_box_pack_start(GTK_BOX(AA2DSlicewin.vbox[0]),AA2DSlicewin.canvas,1,1,0);
	gtk_box_pack_end(GTK_BOX(AA2DSlicewin.vbox[0]),AA2DSlicewin.hbox[0],0,0,2);


	gtk_container_add(GTK_CONTAINER(AA2DSlicewin.window),AA2DSlicewin.vbox[0]);

}

void	tfSetupAA2DSlicewin(void)
{
	//	Window properties

	gtk_window_set_title(GTK_WINDOW(AA2DSlicewin.window),_("Axis aligned 2D slice image"));
	gtk_widget_set_events(AA2DSlicewin.canvas,GDK_EXPOSURE_MASK);
	gtk_widget_set_size_request(AA2DSlicewin.canvas,512,512);

	gtk_combo_box_set_active(GTK_COMBO_BOX(AA2DSlicewin.axis),0);
	//gtk_widget_set_sensitive(AA2DSlicewin.axis,FALSE);
	gtk_window_set_icon(GTK_WINDOW(AA2DSlicewin.window),Mainwin.icon);


	g_signal_connect(GTK_OBJECT(AA2DSlicewin.canvas),"expose-event",G_CALLBACK(AA2DSlicewin_glcanvas_handler),NULL);
	g_signal_connect(GTK_OBJECT(AA2DSlicewin.window),"delete-event",G_CALLBACK(AA2DSlicewin_Close_handler),NULL);
	g_signal_connect(GTK_OBJECT(AA2DSlicewin.slice),"value-changed",G_CALLBACK(AA2DSlicewin_SliceSelect),NULL);
	g_signal_connect(GTK_OBJECT(AA2DSlicewin.axis),"changed",G_CALLBACK(AA2DSlicewin_AxisSelect),NULL);


	//	Initialize the OpenGL window

	AA2DSlicewin.glcanvas = gtk_glcanvas_new(AA2DSlicewin.canvas, gtftable.sharedrc);

	gtk_widget_realize(AA2DSlicewin.canvas);

	glcanvas_make_current(AA2DSlicewin.glcanvas, NULL);

#ifdef	__WIN32__

	//	These code should be put at the window setup procedure that runs first.

	glInitExtensions();
#endif


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


}

int	AA2DSlicewin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	float s;
	int a;

	glcanvas_make_current(AA2DSlicewin.glcanvas, NULL);

	if(!AA2DSlicewin.render_flag)
	{
		glcanvas_swap_buffers(AA2DSlicewin.glcanvas);

		return FALSE;
	}

	a=gtk_combo_box_get_active(GTK_COMBO_BOX(AA2DSlicewin.axis));
	s=gtk_spin_button_get_value(GTK_SPIN_BUTTON(AA2DSlicewin.slice))/gtftable.data->resol[2-a];



	glViewport(0,0,AA2DSlicewin.canvas->allocation.width,AA2DSlicewin.canvas->allocation.height);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,0,100);


	switch(a)
	{
		case 0:
			glBegin(GL_QUADS);
				glTexCoord3f(0,0,s);
				glVertex2f(-1,-1);
				glTexCoord3f(1,0,s);
				glVertex2f(1,-1);
				glTexCoord3f(1,1,s);
				glVertex2f(1,1);
				glTexCoord3f(0,1,s);
				glVertex2f(-1,1);
			glEnd();

		break;
		case 1:
			glBegin(GL_QUADS);
				glTexCoord3f(0,s,0);
				glVertex2f(-1,-1);
				glTexCoord3f(1,s,0);
				glVertex2f(1,-1);
				glTexCoord3f(1,s,1);
				glVertex2f(1,1);
				glTexCoord3f(0,s,1);
				glVertex2f(-1,1);
			glEnd();

		break;
		case 2:
			glBegin(GL_QUADS);
				glTexCoord3f(s,0,0);
				glVertex2f(-1,-1);
				glTexCoord3f(s,0,1);
				glVertex2f(1,-1);
				glTexCoord3f(s,1,1);
				glVertex2f(1,1);
				glTexCoord3f(s,1,0);
				glVertex2f(-1,1);
			glEnd();

		break;
	}

	glcanvas_swap_buffers(AA2DSlicewin.glcanvas);

	return FALSE;
}

void	AA2DSlicewin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(Mainwin.view_menuitem[0]),FALSE);
	AA2DSlicewin_Release();
	gtk_widget_hide_all(AA2DSlicewin.window);
}

void	AA2DSlicewin_SliceSelect(GtkWidget *widget,gpointer user_data)
{
	gdk_window_invalidate_rect(AA2DSlicewin.window->window, NULL, TRUE);

}

void	AA2DSlicewin_AxisSelect(GtkWidget *widget,gpointer user_data)
{
	int a;
	a=gtk_combo_box_get_active(GTK_COMBO_BOX(AA2DSlicewin.axis));
	if(!gtftable.data)	return;

	switch(a)
	{
		case 0:
			gtk_widget_set_size_request(AA2DSlicewin.canvas,gtftable.data->m_resol[0],gtftable.data->m_resol[1]);
			gtk_spin_button_set_range(GTK_SPIN_BUTTON(AA2DSlicewin.slice),1,gtftable.data->resol[2]);
		break;
		case 1:
			gtk_widget_set_size_request(AA2DSlicewin.canvas,gtftable.data->m_resol[2],gtftable.data->m_resol[0]);
			gtk_spin_button_set_range(GTK_SPIN_BUTTON(AA2DSlicewin.slice),1,gtftable.data->resol[1]);
		break;
		case 2:
			gtk_widget_set_size_request(AA2DSlicewin.canvas,gtftable.data->m_resol[1],gtftable.data->m_resol[2]);
			gtk_spin_button_set_range(GTK_SPIN_BUTTON(AA2DSlicewin.slice),1,gtftable.data->resol[0]);
		break;
	}
	gdk_window_invalidate_rect(AA2DSlicewin.window->window, NULL, TRUE);
}

void	AA2DSlicewin_Release(void)
{
	if(AA2DSlicewin.render_flag)
	{
		glcanvas_make_current(AA2DSlicewin.glcanvas, NULL);

		if(gtftable.AA2Dslice_release[gtftable.current_tf])
			gtftable.AA2Dslice_release[gtftable.current_tf]();

		AA2DSlicewin.render_flag=FALSE;
	}

}


