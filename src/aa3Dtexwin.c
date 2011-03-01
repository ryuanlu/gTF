#include	<stdlib.h>
#include	<memory.h>
#include	<math.h>

#include	"gTF.h"
#include	"glext.h"
#include	"util.h"
#include	"mainwin.h"
#include	"aa3Dtexctrlwin.h"

#include	"etf1d.h"

#include	"aa3Dtexwin.h"

strAA3DTexwin	AA3DTexwin;
double diff;

void	tfCreateAA3DTexwin(void)
{
	memset(&AA3DTexwin,0,sizeof(strAA3DTexwin));
	textdomain("gtf");
	AA3DTexwin.glwin=GLWin_new("Volume Rendering - Axis aligned 3D texture",gtftable.sharedrc);
	gtftable.sharedrc=AA3DTexwin.glwin->glrc;
	AA3DTexwin.menuitem[0]=gtk_image_menu_item_new_with_label(_("Settings"));
	AA3DTexwin.menuitem[1]=gtk_check_menu_item_new_with_label(_("Draw bounding box"));
	AA3DTexwin.icon=gtk_image_new();
	gtk_image_set_from_stock(GTK_IMAGE(AA3DTexwin.icon),GTK_STOCK_PROPERTIES,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(AA3DTexwin.menuitem[0]),AA3DTexwin.icon);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(AA3DTexwin.glwin->menu),AA3DTexwin.menuitem[1]);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(AA3DTexwin.glwin->menu),AA3DTexwin.menuitem[0]);

}

void	tfSetupAA3DTexwin(void)
{
//	int		i;
//	char*		text[2];
	float		p[]={0.0f,0.0f,300.0f,1.0f};


	//	Window properties

	gtk_window_set_icon(GTK_WINDOW(AA3DTexwin.glwin->window),Mainwin.icon);
	gtk_widget_show_all(AA3DTexwin.glwin->menu);


	//	Signal connections

	g_signal_connect(G_OBJECT(AA3DTexwin.glwin->glcanvas),"expose-event",G_CALLBACK(AA3DTexwin_glcanvas_handler),AA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(AA3DTexwin.glwin->glcanvas),"button-press-event",G_CALLBACK(AA3DTexwin_ButtonDown_handler),AA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(AA3DTexwin.glwin->glcanvas),"button-release-event",G_CALLBACK(AA3DTexwin_ButtonUp_handler),AA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(AA3DTexwin.glwin->glcanvas),"motion-notify-event",G_CALLBACK(AA3DTexwin_MouseMotion_handler),AA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(AA3DTexwin.glwin->glcanvas),"scroll-event",G_CALLBACK(AA3DTexwin_MouseWheel_handler),AA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(AA3DTexwin.glwin->window),"delete-event",G_CALLBACK(AA3DTexwin_Close_handler),NULL);
	g_signal_connect(G_OBJECT(AA3DTexwin.menuitem[0]),"activate",G_CALLBACK(AA3DTexwin_Menu_handler),NULL);
	g_signal_connect(G_OBJECT(AA3DTexwin.menuitem[1]),"activate",G_CALLBACK(AA3DTexwin_Menu_handler),NULL);


	gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);


	//	GL initial state

	//glEnable(GL_TEXTURE_1D);
	//glEnable(GL_TEXTURE_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);


	glLightfv(GL_LIGHT0,GL_POSITION,p);
	glEnable(GL_LIGHT0);

	//	setup light parameters

	//AA3DTexctrlwin_Apply_handler(NULL,NULL);
	GLWin_settings_apply_handler(NULL,AA3DTexctrlwin.settings);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	AA3DTexwin.offset=0.0f;
	AA3DTexwin.density=2.0f;

}




//	Static slicing rate version
/*
void	AA3DTexwin_generate_displaylist(void)
{
	int	d,list;
	double	dir,diff;
	double	s,e,i;
	double	x,y,z;

	if(!AA3DTexwin.density) return;
	diff=1.0/AA3DTexwin.density;

	gdk_gl_drawable_make_current(AA3DTexwin.gldrawable,AA3DTexwin.glrc);

	//list=glGenLists(6);
	//AA3DTexwin.displaylist=list;

	x=gtftable.data->m_resol[0];
	y=gtftable.data->m_resol[1];
	z=gtftable.data->m_resol[2];


	//	+- X axis

	for(d=0;d<2;++d)
	{
		//glNewList(list+d,GL_COMPILE);

		dir=1-2*d;
		if(dir==1)
		{
			s=1;e=gtftable.data->m_resol[0];
		}else
		{
			s=gtftable.data->m_resol[0];e=1;
		}

		glBegin(GL_QUADS);
		for(i=s;((dir>0)&&i<e)||((dir<0)&&i>e);i+=dir*diff)
		{
			glTexCoord3d(i/x,1/y,1/z);
			glVertex3f(i,0,0);
			glTexCoord3d(i/x,1,1/z);
			glVertex3f(i,y,0);
			glTexCoord3d(i/x,1,1);
			glVertex3f(i,y,z);
			glTexCoord3d(i/x,1/y,1);
			glVertex3f(i,0,z);
		}
		glEnd();
		//glEndList();
	}


	//	+- Y axis

	for(d=0;d<2;++d)
	{
		//glNewList(list+2+d,GL_COMPILE);

		dir=1-2*d;
		if(dir==1)
		{
			s=1;e=gtftable.data->m_resol[1];
		}else
		{
			s=gtftable.data->m_resol[1];e=1;
		}

		glBegin(GL_QUADS);
		for(i=s;((dir>0)&&i<e)||((dir<0)&&i>e);i+=dir*diff)
		{
			glTexCoord3d(1/x,i/y,1/z);
			glVertex3f(0,i,0);
			glTexCoord3d(1,i/y,1/z);
			glVertex3f(x,i,0);
			glTexCoord3d(1,i/y,1);
			glVertex3f(x,i,z);
			glTexCoord3d(1/x,i/y,1);
			glVertex3f(0,i,z);
		}
		glEnd();
		//glEndList();
	}


	//	+- Z axis

	for(d=0;d<2;++d)
	{
		//glNewList(list+4+d,GL_COMPILE);

		dir=1-2*d;
		if(dir==1)
		{
			s=1;e=gtftable.data->m_resol[2];
		}else
		{
			s=gtftable.data->m_resol[2];e=1;
		}

		glBegin(GL_QUADS);
		for(i=s;((dir>0)&&i<e)||((dir<0)&&i>e);i+=dir*diff)
		{
			glTexCoord3d(1/x,1/y,i/z);
			glVertex3f(0,0,i);
			glTexCoord3d(1,1/y,i/z);
			glVertex3f(x,0,i);
			glTexCoord3d(1,1,i/z);
			glVertex3f(x,y,i);
			glTexCoord3d(1/x,1,i/z);
			glVertex3f(0,y,i);
		}
		glEnd();
		//glEndList();
	}


}
*/

//	Dynamic slicing rate version

int	AA3DTexwin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	int	wx,wy;
	float	M[16];
	float	axis[4];
	float	o[4];
	float	dota[3];
	//float	dir;
	int	a=0;

	double	s,e,i;
	double	x,y,z;
	double	dir;


	gtk_window_get_size(GTK_WINDOW(AA3DTexwin.glwin->window),&wx,&wy);
	gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,wx,wy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(AA3DTexwin.glwin->fov,(float)wx/wy,0.1f,5000.0f);
	glMatrixMode(GL_MODELVIEW);


	if(!AA3DTexwin.glwin->render_flag)
	{
		gdk_gl_drawable_swap_buffers(AA3DTexwin.glwin->gldrawable);

		return FALSE;
	}

	glPushMatrix();

	gluLookAt(0.0f,0.0f,AA3DTEXWIN_DEFAULT_DISTANCE,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);


	glPushMatrix();
	glTranslatef(-gtftable.data->m_resol[0]/2,-gtftable.data->m_resol[1]/2,-gtftable.data->m_resol[2]/2);


	glGetFloatv(GL_MODELVIEW_MATRIX,M);
	matrix_x_vertex(o,M,0,0,0);


	//	[(axis-o)] * [ 0,0,-1]	X axis . -Ze

	matrix_x_vertex(axis,M,1,0,0);
	dota[0]=-(axis[2]-o[2]);
	//dota[0]=-M[8];


	//	[(axis-o)] * [ 0,0,-1]	Y axis . -Ze

	matrix_x_vertex(axis,M,0,1,0);
	dota[1]=-(axis[2]-o[2]);


	//	[(axis-o)] * [ 0,0,-1]	Z axis . -Ze

	matrix_x_vertex(axis,M,0,0,1);
	dota[2]=-(axis[2]-o[2]);


	//	Detect which axis to align

	if(fabs(dota[0])>=fabs(dota[1])&&fabs(dota[0])>=fabs(dota[2]))
	{
		a=0;
	}
	if(fabs(dota[1])>=fabs(dota[0])&&fabs(dota[1])>=fabs(dota[2]))
	{
		a=1;
	}
	if(fabs(dota[2])>=fabs(dota[0])&&fabs(dota[2])>=fabs(dota[1]))
	{
		a=2;
	}


	//	Determine which direction, increase or decrease

	if(dota[a]<0)	dir=1;
	else		dir=-1;

	x=gtftable.data->m_resol[0];
	y=gtftable.data->m_resol[1];
	z=gtftable.data->m_resol[2];


	//	Draw bounding box

	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(AA3DTexwin.menuitem[1])))
	{
		glUseProgram(0);
		glColor4f(1.0,1.0,0.0,1.0);
		glDrawBoundingBox(x,y,z);
		glUseProgram(gtftable.shader[gtftable.current_tf].aa3Dtex);
	}


	//diff=1.0/(AA3DTexwin.density*(1.0/fabs(dota[a])));
	diff=fabs(dota[a])/(AA3DTexwin.density);
	//diff=10;

	//	X axis aligned
	if(a==0)
	{
		if(dir==1)
		{
			s=1;e=gtftable.data->m_resol[0];
		}else
		{
			s=gtftable.data->m_resol[0];e=1;
		}

		glBegin(GL_QUADS);
		for(i=s;((dir>0)&&i<e)||((dir<0)&&i>e);i+=dir*diff)
		{
			glNormal3f(1,0,0);
			glTexCoord3d(i/x,1/y,1/z);
			glVertex3f(i,0,0);
			glTexCoord3d(i/x,1,1/z);
			glVertex3f(i,y,0);
			glTexCoord3d(i/x,1,1);
			glVertex3f(i,y,z);
			glTexCoord3d(i/x,1/y,1);
			glVertex3f(i,0,z);
		}
		glEnd();

	}

	//	Y axis aligned
	if(a==1)
	{
		if(dir==1)
		{
			s=1;e=gtftable.data->m_resol[1];
		}else
		{
			s=gtftable.data->m_resol[1];e=1;
		}

		glBegin(GL_QUADS);
		for(i=s;((dir>0)&&i<e)||((dir<0)&&i>e);i+=dir*diff)
		{
			glNormal3f(0,1,0);
			glTexCoord3d(1/x,i/y,1/z);
			glVertex3f(0,i,0);
			glTexCoord3d(1,i/y,1/z);
			glVertex3f(x,i,0);
			glTexCoord3d(1,i/y,1);
			glVertex3f(x,i,z);
			glTexCoord3d(1/x,i/y,1);
			glVertex3f(0,i,z);
		}
		glEnd();

	}

	//	Z axis aligned
	if(a==2)
	{
		if(dir==1)
		{
			s=1;e=gtftable.data->m_resol[2];
		}else
		{
			s=gtftable.data->m_resol[2];e=1;
		}

		glBegin(GL_QUADS);
		for(i=s;((dir>0)&&i<e)||((dir<0)&&i>e);i+=dir*diff)
		{
			glNormal3f(0,0,1);
			glTexCoord3d(1/x,1/y,i/z);
			glVertex3f(0,0,i);
			glTexCoord3d(1,1/y,i/z);
			glVertex3f(x,0,i);
			glTexCoord3d(1,1,i/z);
			glVertex3f(x,y,i);
			glTexCoord3d(1/x,1,i/z);
			glVertex3f(0,y,i);
		}
		glEnd();

	}

	glPopMatrix();
	glPopMatrix();

	gdk_gl_drawable_swap_buffers(AA3DTexwin.glwin->gldrawable);

	return FALSE;
}

int	AA3DTexwin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(Mainwin.view_menuitem[1]),FALSE);
	AA3DTexwin_Release();

	gtk_widget_hide_all(AA3DTexctrlwin.settings->window);

	return FALSE;
}

int	AA3DTexwin_MouseMotion_handler(GtkWidget *widget, GdkEventMotion *event,gpointer user_data)
{
	float	M[16];

	float	a[4];

	if(!AA3DTexwin.glwin->render_flag||!AA3DTexwin.move) return FALSE;

	a[1]=event->x-AA3DTexwin.prev_pos[0];
	a[0]=event->y-AA3DTexwin.prev_pos[1];
	a[2]=0;
	a[3]=sqrt(a[0]*a[0]+a[1]*a[1]);
	a[0]/=a[3];
	a[1]/=a[3];

	AA3DTexwin.prev_pos[0]=event->x;
	AA3DTexwin.prev_pos[1]=event->y;

	gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);

        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        glLoadIdentity();
        glTranslatef(0,0,-AA3DTexwin.offset-AA3DTEXWIN_DEFAULT_DISTANCE);

        glRotatef(a[3]/3,a[0],a[1],a[2]);
        glTranslatef(0,0,AA3DTexwin.offset+AA3DTEXWIN_DEFAULT_DISTANCE);

        glMultMatrixf(M);


	gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);

	return FALSE;
}

int	AA3DTexwin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	if(event->button==1&&event->type==GDK_BUTTON_PRESS)
	{
		AA3DTexwin.prev_pos[0]=event->x;
		AA3DTexwin.prev_pos[1]=event->y;
		AA3DTexwin.move=TRUE;
	}

	return FALSE;
}


int	AA3DTexwin_ButtonUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	if(AA3DTexwin.move)
	{
		AA3DTexwin.move=FALSE;
		//printf("%lf\n",diff);
	}
	return FALSE;
}

int	AA3DTexwin_MouseWheel_handler(GtkWidget *widget, GdkEventScroll *event,gpointer user_data)
{
	int dir;
	float M[16];

	if(!AA3DTexwin.glwin->render_flag) return FALSE;
	gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);

	dir=event->direction ? 10:-10;

	AA3DTexwin.offset+=dir;

        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        glLoadIdentity();
        glTranslatef(0,0,-dir);
        glMultMatrixf(M);

	gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);
	return FALSE;
}


void	AA3DTexwin_Release(void)
{
	if(AA3DTexwin.glwin->render_flag)
	{
		gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);

		if(gtftable.AA3Dtex_release[gtftable.current_tf])
			gtftable.AA3Dtex_release[gtftable.current_tf]();

		AA3DTexwin.glwin->render_flag=FALSE;
	}
}

void	AA3DTexwin_Menu_handler(GtkWidget *widget,gpointer user_data)
{
	if(widget==AA3DTexwin.menuitem[0])
	{
		gtk_widget_show_all(AA3DTexctrlwin.settings->window);
		gtk_window_present(GTK_WINDOW(AA3DTexctrlwin.settings->window));
	}
	if(widget==AA3DTexwin.menuitem[1])
	{
		gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);
	}

}


