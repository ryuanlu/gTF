#include	<stdlib.h>
#include	<memory.h>
#include	<math.h>

#include	"glext.h"

#include	"util.h"
#include	"mainwin.h"
#include	"glwin.h"
#include	"etf1d.h"
#include	"va3Dtexctrlwin.h"

#include	"va3Dtexwin.h"


strVA3DTexwin	VA3DTexwin;

const int	va3Dtex_nsequence[]=
{
	7,3,5,1,6,2,4,0,
	6,7,4,5,2,3,0,1,
	5,4,7,6,1,0,3,2,
	4,0,6,2,5,1,7,3,
	3,2,1,0,7,6,5,4,
	2,6,0,4,3,7,1,5,
	1,5,3,7,0,4,2,6,
	0,1,2,3,4,5,6,7
};

const int	va3Dtex_va[]=
{
	7,3,2,7,
	2,7,3,2,

	7,6,4,7,
	4,7,6,4,

	7,5,1,7,
	1,7,5,1
};

const int	va3Dtex_vb[]=
{
	3,2,0,3,
	6,3,2,0,

	6,4,0,6,
	5,6,4,0,

	5,1,0,5,
	3,5,1,0
};

float		va_vertices[24];


void	tfCreateVA3DTexwin(void)
{
	memset(&VA3DTexwin,0,sizeof(strVA3DTexwin));
	textdomain("gtf");
	VA3DTexwin.glwin=GLWin_new("Volume Rendering - View aligned 3D texture",gtftable.sharedrc);
	gtftable.sharedrc=VA3DTexwin.glwin->glrc;
	VA3DTexwin.menuitem[0]=gtk_image_menu_item_new_with_label(_("Settings"));
	VA3DTexwin.menuitem[1]=gtk_check_menu_item_new_with_label(_("Draw bounding box"));
	VA3DTexwin.icon=gtk_image_new();
	gtk_image_set_from_stock(GTK_IMAGE(VA3DTexwin.icon),GTK_STOCK_PROPERTIES,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(VA3DTexwin.menuitem[0]),VA3DTexwin.icon);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(VA3DTexwin.glwin->menu),VA3DTexwin.menuitem[1]);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(VA3DTexwin.glwin->menu),VA3DTexwin.menuitem[0]);

}

void	tfSetupVA3DTexwin(void)
{
	float		p[]={0.0f,0.0f,300.0f,1.0f};


	//	Window properties

	gtk_window_set_icon(GTK_WINDOW(VA3DTexwin.glwin->window),Mainwin.icon);
	gtk_widget_show_all(VA3DTexwin.glwin->menu);


	//	Signal connections

	g_signal_connect(G_OBJECT(VA3DTexwin.glwin->glcanvas),"expose-event",G_CALLBACK(VA3DTexwin_glcanvas_handler),VA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(VA3DTexwin.glwin->glcanvas),"button-press-event",G_CALLBACK(VA3DTexwin_ButtonDown_handler),VA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(VA3DTexwin.glwin->glcanvas),"button-release-event",G_CALLBACK(VA3DTexwin_ButtonUp_handler),VA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(VA3DTexwin.glwin->glcanvas),"motion-notify-event",G_CALLBACK(VA3DTexwin_MouseMotion_handler),VA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(VA3DTexwin.glwin->glcanvas),"scroll-event",G_CALLBACK(VA3DTexwin_MouseWheel_handler),VA3DTexwin.glwin);
	g_signal_connect(G_OBJECT(VA3DTexwin.glwin->window),"delete-event",G_CALLBACK(VA3DTexwin_Close_handler),NULL);
	g_signal_connect(G_OBJECT(VA3DTexwin.menuitem[0]),"activate",G_CALLBACK(VA3DTexwin_Menu_handler),NULL);
	g_signal_connect(G_OBJECT(VA3DTexwin.menuitem[1]),"activate",G_CALLBACK(VA3DTexwin_Menu_handler),NULL);


	gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);


	//	GL initial state

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glLightfv(GL_LIGHT0,GL_POSITION,p);
	glEnable(GL_LIGHT0);

	//	setup light parameters

	VA3DTexctrlwin_Apply_handler(NULL,NULL);
	GLWin_settings_apply_handler(NULL,VA3DTexctrlwin.settings);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	VA3DTexwin.offset=0.0f;
	VA3DTexwin.density=2.0f;

	VA3DTexwin.glwin->fov=45.0;
}

void	VA3DTex_GetLocation(VA3Dvsloc* loc,unsigned int shader)
{
	loc->sequenceloc=glGetUniformLocation(shader,"sequence");
	loc->valoc=glGetUniformLocation(shader,"v1");
	loc->vbloc=glGetUniformLocation(shader,"v2");
	loc->verticesloc=glGetUniformLocation(shader,"vertices");
	loc->frontindexloc=glGetUniformLocation(shader,"frontindex");
}


int	VA3DTexwin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	int	x,y,f=0,i;
	float	d,zf,zn;
	float	v[4];
	float	M[16];


	gtk_window_get_size(GTK_WINDOW(VA3DTexwin.glwin->window),&x,&y);


	gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,x,y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(VA3DTexwin.glwin->fov,(float)x/y,0.1f,5000.0f);
	glMatrixMode(GL_MODELVIEW);

	if(!VA3DTexwin.glwin->render_flag)
	{
		gdk_gl_drawable_swap_buffers(VA3DTexwin.glwin->gldrawable);

		return FALSE;
	}

	glPushMatrix();
	gluLookAt(0.0f,0.0f,VA3DTEXWIN_DEFAULT_DISTANCE,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

	glTranslatef((float)-(gtftable.data->m_resol[0]-1)/2,(float)-(gtftable.data->m_resol[1]-1)/2,(float)-(gtftable.data->m_resol[2]-1)/2);
	glScalef(gtftable.data->m_ratio[0],gtftable.data->m_ratio[1],gtftable.data->m_ratio[2]);



	if(VA3DTexwin.glwin->render_flag)
	{


		//	Draw bounding box

		if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(VA3DTexwin.menuitem[1])))
		{
			glUseProgram(0);
			glColor4f(1.0,1.0,0.0,1.0);
			glDrawBoundingBox(gtftable.data->resol[0],gtftable.data->resol[1],gtftable.data->resol[2]);
			glUseProgram(gtftable.shader[gtftable.current_tf].va3Dtex);
		}



		glGetFloatv(GL_MODELVIEW_MATRIX,M);
		zn=-99999.0f;
		zf=9999.0f;

		for(i=0;i<8;++i)
		{

			//	Transform vertex into eye coordinates.

			matrix_x_vertex(v,M,va_vertices[i*3+0],va_vertices[i*3+1],va_vertices[i*3+2]);


			//	Determine the index of the nearest vertex & store the depth

			if(v[2]>=zn)
			{
				f=i;
				zn=v[2];
			}


			//	Store the farest depth

			if(v[2]<=zf)
			{
				zf=v[2];
			}

		}


		//	Send front index to current tf's vertex shader

		glUniform1i(VA3DTexwin.vsloc[gtftable.current_tf]->frontindexloc,f);


		//	Generate polygons

		for(d=zf;d<zn;d+=(1.0/VA3DTexwin.density))
		{
			glBegin(GL_POLYGON);
				for(i=0;i<6;++i)
					glVertex2f(i,d);
			glEnd();
		}

	}
	glPopMatrix();

	gdk_gl_drawable_swap_buffers(VA3DTexwin.glwin->gldrawable);
	return FALSE;
}



int	VA3DTexwin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(Mainwin.view_menuitem[2]),FALSE);
	VA3DTexwin_Release();

	gtk_widget_hide_all(VA3DTexctrlwin.settings->window);

	return FALSE;
}

int	VA3DTexwin_MouseMotion_handler(GtkWidget *widget, GdkEventMotion *event,gpointer user_data)
{
	float	M[16];

	float	a[4];

	if(!VA3DTexwin.glwin->render_flag||!VA3DTexwin.move) return FALSE;

	a[1]=event->x-VA3DTexwin.prev_pos[0];
	a[0]=event->y-VA3DTexwin.prev_pos[1];
	a[2]=0;
	a[3]=sqrt(a[0]*a[0]+a[1]*a[1]);
	a[0]/=a[3];
	a[1]/=a[3];

	VA3DTexwin.prev_pos[0]=event->x;
	VA3DTexwin.prev_pos[1]=event->y;

	gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);

        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        glLoadIdentity();
        glTranslatef(0,0,-VA3DTexwin.offset-VA3DTEXWIN_DEFAULT_DISTANCE);

        glRotatef(a[3]/3,a[0],a[1],a[2]);
        glTranslatef(0,0,VA3DTexwin.offset+VA3DTEXWIN_DEFAULT_DISTANCE);

        glMultMatrixf(M);


	gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window,NULL,TRUE);

	return FALSE;
}

int	VA3DTexwin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	if(event->button==1&&event->type==GDK_BUTTON_PRESS)
	{
		VA3DTexwin.prev_pos[0]=event->x;
		VA3DTexwin.prev_pos[1]=event->y;
		VA3DTexwin.move=TRUE;
	}

	return FALSE;
}


int	VA3DTexwin_ButtonUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	if(VA3DTexwin.move)
	{
		VA3DTexwin.move=FALSE;
		//printf("%lf\n",diff);
	}
	return FALSE;
}

int	VA3DTexwin_MouseWheel_handler(GtkWidget *widget, GdkEventScroll *event,gpointer user_data)
{
	int dir;
	float M[16];

	if(!VA3DTexwin.glwin->render_flag) return FALSE;
	gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);

	dir=event->direction ? 10:-10;

	VA3DTexwin.offset+=dir;

        glGetFloatv(GL_MODELVIEW_MATRIX,M);
        glLoadIdentity();
        glTranslatef(0,0,-dir);
        glMultMatrixf(M);

	gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window,NULL,TRUE);
	return FALSE;
}


void	VA3DTexwin_Release(void)
{
	if(VA3DTexwin.glwin->render_flag)
	{
		gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);

		if(gtftable.VA3Dtex_release[gtftable.current_tf])
			gtftable.VA3Dtex_release[gtftable.current_tf]();

		VA3DTexwin.glwin->render_flag=FALSE;
	}
}

void	VA3DTexwin_Menu_handler(GtkWidget *widget,gpointer user_data)
{
	if(widget==VA3DTexwin.menuitem[0])
	{
		gtk_widget_show_all(VA3DTexctrlwin.settings->window);
		gtk_window_present(GTK_WINDOW(VA3DTexctrlwin.settings->window));
	}
	if(widget==VA3DTexwin.menuitem[1])
	{
		gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window,NULL,TRUE);
	}

}

