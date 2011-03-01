#include	"isosurfacewin.h"
#include	"isosurfacectrlwin.h"
#include	"marching_tetrahedron.h"

#include	"glext.h"
#include	"gTF.h"
#include	"util.h"

#include	"mainwin.h"

#include	<stdlib.h>
#include	<memory.h>
#include	<math.h>


strIsosurfacewin	Isosurfacewin;



void	tfCreateIsosurfacewin(void)
{
	memset(&Isosurfacewin,0,sizeof(strIsosurfacewin));
	textdomain("gtf");
	Isosurfacewin.glwin=GLWin_new("Iso-surface - Marching tetrahedron",gtftable.sharedrc);
	Isosurfacewin.menuitem[0]=gtk_image_menu_item_new_with_label(_("Settings"));
	Isosurfacewin.menuitem[1]=gtk_check_menu_item_new_with_label(_("Draw bounding box"));
	Isosurfacewin.menuitem[2]=gtk_image_menu_item_new_with_label(_("Save as .obj"));

	Isosurfacewin.icon=gtk_image_new();
	gtk_image_set_from_stock(GTK_IMAGE(Isosurfacewin.icon),GTK_STOCK_PROPERTIES,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(Isosurfacewin.menuitem[0]),Isosurfacewin.icon);

	Isosurfacewin.icon=gtk_image_new();
	gtk_image_set_from_stock(GTK_IMAGE(Isosurfacewin.icon),GTK_STOCK_SAVE_AS,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(Isosurfacewin.menuitem[2]),Isosurfacewin.icon);

	gtk_menu_shell_prepend(GTK_MENU_SHELL(Isosurfacewin.glwin->menu),Isosurfacewin.menuitem[2]);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(Isosurfacewin.glwin->menu),Isosurfacewin.menuitem[1]);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(Isosurfacewin.glwin->menu),Isosurfacewin.menuitem[0]);

}

void	tfSetupIsosurfacewin(void)
{
	float		p[]={0.0f,0.0f,300.0f,1.0f};
	char*		text[2];
	int		i;


	//	Window properties

	gtk_window_set_default_size(GTK_WINDOW(Isosurfacewin.glwin->window),800,800);
	gtk_window_set_icon(GTK_WINDOW(Isosurfacewin.glwin->window),Mainwin.icon);
	gtk_widget_show_all(Isosurfacewin.glwin->menu);


	//	Signal connections

	g_signal_connect(GTK_OBJECT(Isosurfacewin.glwin->window),"delete-event",G_CALLBACK(Isosurfacewin_Close_handler),NULL);
	g_signal_connect(GTK_OBJECT(Isosurfacewin.glwin->glcanvas),"expose-event",G_CALLBACK(Isosurfacewin_glcanvas_handler),NULL);
	g_signal_connect(GTK_OBJECT(Isosurfacewin.glwin->glcanvas),"button-press-event",G_CALLBACK(Isosurfacewin_ButtonDown_handler),NULL);
	g_signal_connect(GTK_OBJECT(Isosurfacewin.glwin->glcanvas),"button-release-event",G_CALLBACK(Isosurfacewin_ButtonUp_handler),NULL);
	g_signal_connect(GTK_OBJECT(Isosurfacewin.glwin->glcanvas),"motion-notify-event",G_CALLBACK(Isosurfacewin_MouseMotion_handler),NULL);
	g_signal_connect(GTK_OBJECT(Isosurfacewin.glwin->glcanvas),"scroll-event",G_CALLBACK(Isosurfacewin_MouseWheel_handler),NULL);
	g_signal_connect(G_OBJECT(Isosurfacewin.menuitem[0]),"activate",G_CALLBACK(Isosurfacewin_Menu_handler),NULL);
	g_signal_connect(G_OBJECT(Isosurfacewin.menuitem[1]),"activate",G_CALLBACK(Isosurfacewin_Menu_handler),NULL);
	g_signal_connect(G_OBJECT(Isosurfacewin.menuitem[2]),"activate",G_CALLBACK(Isosurfacewin_Menu_handler),NULL);


	//	OpenGL initial state

	gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);


	//	Load Shader

	gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);

	Isosurfacewin.progobj=glCreateProgram();

	text[0]=ReadShaderText(DATADIR"/iso_surface.vs");
	text[1]=ReadShaderText(DATADIR"/iso_surface.fs");

	Isosurfacewin.shader[0]=glCreateShader(GL_VERTEX_SHADER);
	Isosurfacewin.shader[1]=glCreateShader(GL_FRAGMENT_SHADER);


	for(i=0;i<2;++i)
	{
		glAttachObjectARB(Isosurfacewin.progobj,Isosurfacewin.shader[i]);
		glShaderSource(Isosurfacewin.shader[i],1,(const char**)&text[i],NULL);
		glCompileShader(Isosurfacewin.shader[i]);
		//printShaderInfoLog(Isosurfacewin.shader[i]);
		free(text[i]);
	}



	glLinkProgram(Isosurfacewin.progobj);
	glUseProgram(Isosurfacewin.progobj);
	Isosurfacewin.normalloc=glGetUniformLocation(Isosurfacewin.progobj,"normal");
	Isosurfacewin.resolloc=glGetUniformLocation(Isosurfacewin.progobj,"resol");



	glEnable(GL_DEPTH_TEST);
	Isosurfacewin.glwin->fov=45.0f;
	Isosurfacewin.offset=0.0f;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	//glEnable(GL_NORMALIZE);
	glDisable(GL_BLEND);


	//	Setup light

	glLightfv(GL_LIGHT0,GL_POSITION,p);
	glEnable(GL_LIGHT0);


	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL,1.0);

	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,&Isosurfacewin.page_size);
	Isosurface_set_pagesize(Isosurfacewin.page_size-Isosurfacewin.page_size%3);
}


int	Isosurfacewin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	int	x,y;
	int	i;
	//float	x,y,z;
	gtk_window_get_size(GTK_WINDOW(Isosurfacewin.glwin->window),&x,&y);

	gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,x,y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(Isosurfacewin.glwin->fov,(float)x/y,0.1f,5000.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	gluLookAt(0.0f,0.0f,ISOSURFACEWIN_DEFAULT_DISTANCE,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

	if(Isosurfacewin.glwin->render_flag)
	{
		glPushMatrix();

		glTranslatef((float)-(gtftable.data->m_resol[0]-1)/2,(float)-(gtftable.data->m_resol[1]-1)/2,(float)-(gtftable.data->m_resol[2]-1)/2);
		glScalef(gtftable.data->m_ratio[0],gtftable.data->m_ratio[1],gtftable.data->m_ratio[2]);


		//	Draw bounding box

		if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(Isosurfacewin.menuitem[1])))
		{
			glUseProgram(0);
			glColor4f(1.0,1.0,0.0,1.0);
			glDrawBoundingBox(gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2]);
			glUseProgram(Isosurfacewin.progobj);
		}


		//	Use display list to accelerate rendering speed.

		for(i=0;i<Isosurface_get_used_pagesize();++i)
			glCallList(Isosurfacewin.displaylist+i);


		glPopMatrix();
	}
	glPopMatrix();

	gdk_gl_drawable_swap_buffers(Isosurfacewin.glwin->gldrawable);

	return FALSE;
}

int	Isosurfacewin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(Mainwin.view_menuitem[3]),FALSE);
	gtk_widget_hide_all(Isosurfacectrlwin.settings->window);

	Isosurfacewin_Release();

	return FALSE;
}

int	Isosurfacewin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	if(event->button==1&&event->type==GDK_BUTTON_PRESS)
	{
		Isosurfacewin.prev_pos[0]=event->x;
		Isosurfacewin.prev_pos[1]=event->y;
		Isosurfacewin.move=TRUE;
	}

	return FALSE;
}

void	Isosurfacewin_Menu_handler(GtkWidget *widget,gpointer user_data)
{
	char*	filename;

	if(widget==Isosurfacewin.menuitem[0])
	{
		gtk_widget_show_all(Isosurfacectrlwin.settings->window);
		gtk_window_present(GTK_WINDOW(Isosurfacectrlwin.settings->window));
	}
	if(widget==Isosurfacewin.menuitem[1])
	{
		gdk_window_invalidate_rect(Isosurfacewin.glwin->window->window,NULL,TRUE);
	}
	if(widget==Isosurfacewin.menuitem[2])
	{
		if(!gtftable.data||!Isosurface_get_number_of_vertices()) return;

		filename=FileDialog(OPEN_DIALOG,Mainwin.window);

		if(filename)
		{
			if(Isosurface_export_to_obj(filename)==ISOSURFACE_OK)
			{
				Mainwin.errordialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,_("File saved successfully"));
				gtk_dialog_run(GTK_DIALOG(Mainwin.errordialog));
				gtk_widget_destroy(Mainwin.errordialog);
			}
			free(filename);
		}
	}

}

int	Isosurfacewin_MouseMotion_handler(GtkWidget *widget, GdkEventMotion *event,gpointer user_data)
{
	float	M[16];

	float	a[4];

	if(!Isosurfacewin.glwin->render_flag||!Isosurfacewin.move) return FALSE;


	//	Model rotation with mouse motion.

	a[1]=event->x-Isosurfacewin.prev_pos[0];
	a[0]=event->y-Isosurfacewin.prev_pos[1];
	a[2]=0;
	a[3]=sqrt(a[0]*a[0]+a[1]*a[1]);
	a[0]/=a[3];
	a[1]/=a[3];

	Isosurfacewin.prev_pos[0]=event->x;
	Isosurfacewin.prev_pos[1]=event->y;

	gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);

        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        glLoadIdentity();
        glTranslatef(0,0,-Isosurfacewin.offset-ISOSURFACEWIN_DEFAULT_DISTANCE);

        glRotatef(a[3]/3,a[0],a[1],a[2]);
        glTranslatef(0,0,Isosurfacewin.offset+ISOSURFACEWIN_DEFAULT_DISTANCE);

        glMultMatrixf(M);


	gdk_window_invalidate_rect(Isosurfacewin.glwin->window->window,NULL,TRUE);

	return FALSE;
}

int	Isosurfacewin_ButtonUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	if(Isosurfacewin.move)
	{
		Isosurfacewin.move=FALSE;
	}
	return FALSE;
}

int	Isosurfacewin_MouseWheel_handler(GtkWidget *widget, GdkEventScroll *event,gpointer user_data)
{
	int dir;
	float M[16];

	if(!Isosurfacewin.glwin->render_flag) return FALSE;


	//	Do zoom in / zoom out.

	gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);

	dir=event->direction ? 10:-10;

	Isosurfacewin.offset+=dir;

        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        glLoadIdentity();
        glTranslatef(0,0,-dir);
        glMultMatrixf(M);


	gdk_window_invalidate_rect(Isosurfacewin.glwin->window->window,NULL,TRUE);

	return FALSE;
}

void	Isosurfacewin_Release(void)
{
	if(Isosurface_get_number_of_vertices())
	{
		//	Release vertex buffer & display list.

		gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);
		Isosurface_release();
		glDeleteLists(Isosurfacewin.displaylist,Isosurface_get_used_pagesize());
		Isosurfacewin.glwin->render_flag=FALSE;

	}
}

void	Isosurfacewin_Init(void)
{
	if(!gtftable.data) return;
	gdk_gl_drawable_make_current(Isosurfacewin.glwin->gldrawable,Isosurfacewin.glwin->glrc);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gtftable.gradient_tex);
	glUniform1i(Isosurfacewin.normalloc,0);
	glUniform3f(Isosurfacewin.resolloc,gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2]);

}

