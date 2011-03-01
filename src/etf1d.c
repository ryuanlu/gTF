#include	<stdlib.h>
#include	<memory.h>

#include	"gTF.h"
#include	"glext.h"
#include	"tfaux.h"
#include	"aa2Dslicewin.h"
#include	"aa3Dtexwin.h"
#include	"va3Dtexwin.h"
#include	"mainwin.h"


#include	"etf1d.h"
#include	"util.h"

strETF1Dpanel	ETF1Dpanel;


static	void	_3DTexGetLocation(ETF1D_3Dtexloc* loc,unsigned int shader)
{
	loc->volumeloc		=glGetUniformLocation(shader,"volume");
	loc->tf1dloc		=glGetUniformLocation(shader,"tf1d");
	loc->stencilloc		=glGetUniformLocation(shader,"stencil");
	loc->coefloc		=glGetUniformLocation(shader,"coef");
	loc->shiftloc		=glGetUniformLocation(shader,"shift");
	loc->deltaloc		=glGetUniformLocation(shader,"delta");
	loc->normalloc		=glGetUniformLocation(shader,"normal");
	loc->levoy_swloc	=glGetUniformLocation(shader,"levoy_sw");
	loc->levoy_valueloc	=glGetUniformLocation(shader,"levoy_value");
	loc->levoy_thicknessloc	=glGetUniformLocation(shader,"levoy_thickness");

}


static void	DrawHistogram(int* his,int level,int max,int min)
{
	int i;
	glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0,level-1,0,200,0,100);	//	Horizon 0~(level-1),  Vertical 0~200

        //	Draw line strips in histogram window

        glBegin(GL_LINE_STRIP);
        for(i=0;i<level;++i)
        {
                //glVertex2f(i,(his[i]-min)*150/(max-min)+50.0f);
                glVertex2f(i,(his[i]-min)*150/(max-min+1)+50.0f);
        }
        glEnd();

	glMatrixMode(GL_PROJECTION);
        glPopMatrix();

}


void	tfCreateETF1Dpanel(int i)
{
	GtkBuilder*	builder;
	GtkCellRenderer*	cell;

	memset(&ETF1Dpanel,0,sizeof(strETF1Dpanel));

	builder=gtk_builder_new();

	gtk_builder_set_translation_domain(builder,"gtf");
	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/etf1d.xml",NULL);

	gtk_builder_connect_signals(builder,NULL);

	ETF1Dpanel.hcodisplay=GTK_WIDGET(gtk_builder_get_object(builder,"hcodisplay"));

	ETF1Dpanel.checkbox[0]=GTK_WIDGET(gtk_builder_get_object(builder,"applycolor"));
	ETF1Dpanel.checkbox[1]=GTK_WIDGET(gtk_builder_get_object(builder,"applyopacity"));

	ETF1Dpanel.colorbox[0]=GTK_WIDGET(gtk_builder_get_object(builder,"color_A"));
	ETF1Dpanel.colorbox[1]=GTK_WIDGET(gtk_builder_get_object(builder,"color_B"));

	ETF1Dpanel.select_value[0]=GTK_WIDGET(gtk_builder_get_object(builder,"value_A"));
	ETF1Dpanel.select_value[1]=GTK_WIDGET(gtk_builder_get_object(builder,"value_B"));


	ETF1Dpanel.cmapselect=GTK_WIDGET(gtk_builder_get_object(builder,"colormap"));
	ETF1Dpanel.assign=GTK_WIDGET(gtk_builder_get_object(builder,"fill"));
	ETF1Dpanel.status=GTK_WIDGET(gtk_builder_get_object(builder,"status"));

	ETF1Dpanel.vmax=GTK_WIDGET(gtk_builder_get_object(builder,"vmax"));
	ETF1Dpanel.vmin=GTK_WIDGET(gtk_builder_get_object(builder,"vmin"));
	ETF1Dpanel.hmax=GTK_WIDGET(gtk_builder_get_object(builder,"hmax"));

	ETF1Dpanel.vbox=GTK_WIDGET(gtk_builder_get_object(builder,"vbox1"));

	ETF1Dpanel.gmax=GTK_WIDGET(gtk_builder_get_object(builder,"gradient_magnitude"));
	ETF1Dpanel.levoy_sw=GTK_WIDGET(gtk_builder_get_object(builder,"levoy"));
	ETF1Dpanel.levoy_value=GTK_WIDGET(gtk_builder_get_object(builder,"levoy_value"));
	ETF1Dpanel.levoy_thickness=GTK_WIDGET(gtk_builder_get_object(builder,"levoy_thickness"));

	gtk_container_add(GTK_CONTAINER(tfGetPanel(i)),ETF1Dpanel.vbox);
	g_object_unref(G_OBJECT(builder));

	cell=gtk_cell_renderer_text_new();

	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ETF1Dpanel.cmapselect),cell,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ETF1Dpanel.cmapselect),cell,"text",0,NULL);


	ETF1Dpanel.id=i;

	//	Register functions

	gtftable.TF_init[i]=ETF1D_init;
	gtftable.TF_release[i]=ETF1D_release;

	gtftable.AA2Dslice_init[i]=ETF1D_InitAA2Dslice;
	gtftable.AA3Dtex_init[i]=ETF1D_InitAA3Dtex;
	gtftable.VA3Dtex_init[i]=ETF1D_InitVA3Dtex;

}


void	tfSetupETF1Dpanel(void)
{
	GdkGLConfig	*glconfig;
	int		shader;

	//	Window properties

	gtk_widget_set_sensitive(ETF1Dpanel.colorbox[1],FALSE);
	gtk_widget_set_sensitive(ETF1Dpanel.select_value[1],FALSE);
	gtk_widget_set_sensitive(ETF1Dpanel.assign,FALSE);
	gtk_widget_set_sensitive(ETF1Dpanel.cmapselect,FALSE);


	//	Create OpenGL canvas

	glconfig=gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB|GDK_GL_MODE_DEPTH|GDK_GL_MODE_DOUBLE);
	gtk_widget_set_gl_capability(GTK_WIDGET(ETF1Dpanel.hcodisplay),glconfig,gtftable.sharedrc,TRUE,GDK_GL_RGBA_TYPE);

	gtk_widget_realize(ETF1Dpanel.hcodisplay);

	ETF1Dpanel.glrc=gtk_widget_get_gl_context(ETF1Dpanel.hcodisplay);
	ETF1Dpanel.gldrawable=gtk_widget_get_gl_drawable(ETF1Dpanel.hcodisplay);

	gdk_gl_drawable_make_current(ETF1Dpanel.gldrawable,ETF1Dpanel.glrc);


	//	Setup all shaders

	gtftable.shader[ETF1Dpanel.id].aa2Dslice=glLoadShader(DATADIR"/aa3Dtex.vs",DATADIR"/ETF1D_aa2Dslice.fs");
	gtftable.shader[ETF1Dpanel.id].aa3Dtex=glLoadShader(DATADIR"/aa3Dtex.vs",DATADIR"/ETF1D_3Dtex.fs");
	gtftable.shader[ETF1Dpanel.id].va3Dtex=glLoadShader(DATADIR"/va3Dtex.vs",DATADIR"/ETF1D_3Dtex.fs");

	shader=gtftable.shader[ETF1Dpanel.id].aa2Dslice;
	_3DTexGetLocation(&ETF1Dpanel.AA2Dsliceloc,shader);

	shader=gtftable.shader[ETF1Dpanel.id].aa3Dtex;
	_3DTexGetLocation(&ETF1Dpanel.AA3Dtexloc,shader);

	shader=gtftable.shader[ETF1Dpanel.id].va3Dtex;
	_3DTexGetLocation(&ETF1Dpanel.VA3Dtexloc,shader);
	VA3DTex_GetLocation(&ETF1Dpanel.VA3Dtexvsloc,shader);
	VA3DTexwin.vsloc[ETF1Dpanel.id]=&ETF1Dpanel.VA3Dtexvsloc;


}


void	ETF1D_init(void)
{
	char	str[64];

	if(ETF1Dpanel.histogram_origin)
	{
		ETF1D_release();
	}


	ETF1Dpanel.histogram_origin=dataGenHistogram(gtftable.level,gtftable.data,&gtftable.hmax,&gtftable.hmin,FALSE);

	//	Update information in HCOdisplay

	sprintf(str,"%d",gtftable.data->vmax);
	gtk_label_set_text(GTK_LABEL(ETF1Dpanel.vmax),str);
	sprintf(str,"%d",gtftable.data->vmin);
	gtk_label_set_text(GTK_LABEL(ETF1Dpanel.vmin),str);
	sprintf(str,"%d",gtftable.hmax);
	gtk_label_set_text(GTK_LABEL(ETF1Dpanel.hmax),str);
	sprintf(str,"%.2f",gtftable.data->gmax);
	gtk_label_set_text(GTK_LABEL(ETF1Dpanel.gmax),str);

	ETF1Dpanel.histogram=dataGenHistogram(gtftable.level,gtftable.data,&gtftable.hmax,&gtftable.hmin,TRUE);


	//      Histogram View


	//	Create texture for colomap & opacity bar

	ETF1Dpanel.cmap=cmapNewHSV(gtftable.level);

	gdk_gl_drawable_make_current(ETF1Dpanel.gldrawable,ETF1Dpanel.glrc);


	glViewport(0,0,512,200);	//	Viewport must be resized, or canvas will be 1 point.


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(gtftable.data->vmin,gtftable.data->vmax,0,200,0,100);


	//	Create initial textures to substitude

	glEnable(GL_TEXTURE_1D);
	glGenTextures(1,&ETF1Dpanel.ctex);
	glGenTextures(1,&ETF1Dpanel.otex);
	glGenTextures(1,&ETF1Dpanel.etf1d_tex);

	ETF1Dpanel.cmaptex=calloc(1,TF1D_TEXTURE_WIDTH*3);
	ETF1Dpanel.opacitytex=calloc(1,TF1D_TEXTURE_WIDTH);

	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.ctex);
	glTexImage1D(GL_TEXTURE_1D,0,3,TF1D_TEXTURE_WIDTH,0,GL_RGB,GL_UNSIGNED_BYTE,ETF1Dpanel.cmaptex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.otex);
	glTexImage1D(GL_TEXTURE_1D,0,1,TF1D_TEXTURE_WIDTH,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,ETF1Dpanel.opacitytex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	ETF1Dpanel_UpdateTexture();

	ETF1Dpanel.selA=0;ETF1Dpanel.selB=0;
	ETF1Dpanel.enabled=TRUE;

	gtk_spin_button_set_range(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[0]),gtftable.data->vmin,gtftable.data->vmax);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[1]),gtftable.data->vmin,gtftable.data->vmax);

	ETF1D_Update();
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	gtk_spin_button_set_range(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_value),gtftable.data->vmin,gtftable.data->vmax);

	//gtk_widget_show(Mainwin.frame[ETF1Dpanel.id]);
}


void	ETF1D_release(void)
{
	if(!gtftable.data) return;
	gdk_gl_drawable_make_current(ETF1Dpanel.gldrawable,ETF1Dpanel.glrc);
	cmapRelease(ETF1Dpanel.cmap);
	glDeleteTextures(1,&ETF1Dpanel.ctex);
	glDeleteTextures(1,&ETF1Dpanel.otex);
	glDeleteTextures(1,&ETF1Dpanel.etf1d_tex);
	free(ETF1Dpanel.histogram_origin);
	free(ETF1Dpanel.histogram);
	free(ETF1Dpanel.cmaptex);
	free(ETF1Dpanel.opacitytex);
	ETF1Dpanel.histogram_origin=NULL;
	ETF1Dpanel.histogram=NULL;

}


void	ETF1D_InitAA2Dslice(void)
{
	gdk_gl_drawable_make_current(AA2DSlicewin.gldrawable,AA2DSlicewin.glrc);

	glUniform1f(ETF1Dpanel.AA2Dsliceloc.coefloc,65535.0/(gtftable.data->vmax-gtftable.data->vmin));
	glUniform1f(ETF1Dpanel.AA2Dsliceloc.shiftloc,(double)gtftable.data->vmin/(double)(gtftable.data->vmax-gtftable.data->vmin));

	//	Bind volume with texture 0

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gtftable.volume_tex);
	glUniform1i(ETF1Dpanel.AA2Dsliceloc.volumeloc,0);

	//	Bind colormap with texture 1

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.etf1d_tex);
	glUniform1i(ETF1Dpanel.AA2Dsliceloc.tf1dloc,1);


	AA2DSlicewin.render_flag=TRUE;
	AA2DSlicewin_AxisSelect(NULL,NULL);
}


void	ETF1D_InitAA3Dtex(void)
{
	gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);

	glUniform1f(ETF1Dpanel.AA3Dtexloc.coefloc,65535.0/(gtftable.data->vmax-gtftable.data->vmin));
	glUniform1f(ETF1Dpanel.AA3Dtexloc.shiftloc,(double)gtftable.data->vmin/(double)(gtftable.data->vmax-gtftable.data->vmin));


	//	Create 3D texture directly from volume RAW data

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gtftable.volume_tex);
	glUniform1i(ETF1Dpanel.AA3Dtexloc.volumeloc,0);


	//	Create 1D texture from colormap RGBA data

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.etf1d_tex);
	glUniform1i(ETF1Dpanel.AA3Dtexloc.tf1dloc,1);


	//	Create 3D texture from normal

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D,gtftable.gradient_tex);
	glUniform1i(ETF1Dpanel.AA3Dtexloc.normalloc,2);



	//glUniform1i(ETF1Dpanel.AA3Dtexloc.swloc,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.gradient_sw)));
	glUniform3f(ETF1Dpanel.AA3Dtexloc.deltaloc,1.0/(gtftable.data->resol[0]-1),1.0/(gtftable.data->resol[1]-1),1.0/(gtftable.data->resol[2]-1));

	glUniform1f(ETF1Dpanel.AA3Dtexloc.levoy_valueloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_value)));
	glUniform1f(ETF1Dpanel.AA3Dtexloc.levoy_thicknessloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_thickness)));

	AA3DTexwin.glwin->render_flag=TRUE;

}


void	ETF1D_InitVA3Dtex(void)
{

	gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);

	glUniform1iv(ETF1Dpanel.VA3Dtexvsloc.sequenceloc,64,va3Dtex_nsequence);
	glUniform1iv(ETF1Dpanel.VA3Dtexvsloc.valoc,24,va3Dtex_va);
	glUniform1iv(ETF1Dpanel.VA3Dtexvsloc.vbloc,24,va3Dtex_vb);

	glUniform1f(ETF1Dpanel.VA3Dtexloc.coefloc,65535.0/(gtftable.data->vmax-gtftable.data->vmin));
	glUniform1f(ETF1Dpanel.VA3Dtexloc.shiftloc,(double)gtftable.data->vmin/(double)(gtftable.data->vmax-gtftable.data->vmin));


	//	Create 3D texture directly from volume RAW data

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gtftable.volume_tex);
	glUniform1i(ETF1Dpanel.VA3Dtexloc.volumeloc,0);


	//	Create 1D texture from colormap RGBA data

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.etf1d_tex);
	glUniform1i(ETF1Dpanel.VA3Dtexloc.tf1dloc,1);


	//	Create 3D texture from normal

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D,gtftable.gradient_tex);
	glUniform1i(ETF1Dpanel.VA3Dtexloc.normalloc,2);



	//glUniform1i(ETF1Dpanel.VA3Dtexloc.swloc,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.gradient_sw)));
	glUniform3f(ETF1Dpanel.VA3Dtexloc.deltaloc,1.0/(gtftable.data->resol[0]),1.0/(gtftable.data->resol[1]),1.0/(gtftable.data->resol[2]));
	VA3DTexwin.glwin->render_flag=TRUE;

	memset(va_vertices,0,sizeof(float)*24);

	va_vertices[21]=gtftable.data->resol[0];
	va_vertices[22]=gtftable.data->resol[1];
	va_vertices[23]=gtftable.data->resol[2];

	va_vertices[3]=va_vertices[21];
	va_vertices[9]=va_vertices[21];
	va_vertices[15]=va_vertices[21];

	va_vertices[7]=va_vertices[22];
	va_vertices[10]=va_vertices[22];
	va_vertices[19]=va_vertices[22];

	va_vertices[14]=va_vertices[23];
	va_vertices[17]=va_vertices[23];
	va_vertices[20]=va_vertices[23];

	glUniform3fv(ETF1Dpanel.VA3Dtexvsloc.verticesloc,8,va_vertices);

	glUniform1f(ETF1Dpanel.VA3Dtexloc.levoy_valueloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_value)));
	glUniform1f(ETF1Dpanel.VA3Dtexloc.levoy_thicknessloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_thickness)));


}



//	Histogram & Colormap & Opacity display handler

int	ETF1Dpanel_hcodisplay_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	int m,n;
	char str[64];

	gdk_gl_drawable_make_current(ETF1Dpanel.gldrawable,ETF1Dpanel.glrc);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	No volume loaded, return
	if(!gtftable.data)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		gdk_gl_drawable_swap_buffers(ETF1Dpanel.gldrawable);
		return TRUE;
	}

	m=gtftable.data->vmax;
	n=gtftable.data->vmin;

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//	Draw opacity bar
	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.otex);
	glDrawBlock(n,0.0,m,25.0);

	//	Draw colormap bar
	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.ctex);
	glDrawBlock(n,25.0,m,50.0);

	//	Draw red selection area

	glDisable(GL_TEXTURE_1D);

	if(ETF1Dpanel.selA!=ETF1Dpanel.selB)
	{
		glColor3f(1.0,0.0,0);
		glDrawBlock(ETF1Dpanel.selA,50.0,ETF1Dpanel.selB,200.0);
	}

	//	Draw histogram

	glColor3f(0,1.0,0);
	DrawHistogram(ETF1Dpanel.histogram,m-n+1,gtftable.hmax,gtftable.hmin);

	//	Draw cursor

	glColor3f(1.0,1.0,0);
	glBegin(GL_LINES);
		glVertex2f(ETF1Dpanel.cursorpos,200);
		glVertex2f(ETF1Dpanel.cursorpos,0);
	glEnd();


	glEnable(GL_TEXTURE_1D);
	glColor3f(1.0,1.0,1.0);
	gdk_gl_drawable_swap_buffers(ETF1Dpanel.gldrawable);


	//	Update information that mouse pointed
	sprintf(str,"%s: %d , %s: %d",_("Value"),ETF1Dpanel.cursorpos,_("Freq."),ETF1Dpanel.histogram_origin[ETF1Dpanel.cursorpos-gtftable.data->vmin]);
	gtk_label_set_text(GTK_LABEL(ETF1Dpanel.status),str);

	return FALSE;
}

int	ETF1Dpanel_hcodisplayMouseMove_handler(GtkWidget *widget,GdkEventMotion *event,gpointer user_data)
{
	if(!gtftable.data) return FALSE;
	if(event->x<0) event->x=0;

	//	Convert position on window to the function value
	ETF1Dpanel.cursorpos=event->x*gtftable.level/512+gtftable.data->vmin;

	//	Boundary check
	if(ETF1Dpanel.cursorpos<gtftable.data->vmin)	ETF1Dpanel.cursorpos=gtftable.data->vmin;
	if(ETF1Dpanel.cursorpos>gtftable.data->vmax)	ETF1Dpanel.cursorpos=gtftable.data->vmax;

	//	If in selection now, assign value to selB, the end of selected block
	if(ETF1Dpanel.sel) ETF1Dpanel.selB=ETF1Dpanel.cursorpos;
	ETF1Dpanel_hcodisplay_handler(0,0,0);
	return FALSE;
}

int	ETF1Dpanel_hcodisplayMouseDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	GdkColor	color;


	//	return if no volume loaded

	if(event->button!=1||!gtftable.data) return FALSE;


	ETF1Dpanel.selA=event->x*gtftable.level/512+gtftable.data->vmin;
	ETF1Dpanel.sel=1;

	color.red=ETF1Dpanel.cmap->colormap[(ETF1Dpanel.selA-gtftable.data->vmin)*4+0]*65535/255;
	color.green=ETF1Dpanel.cmap->colormap[(ETF1Dpanel.selA-gtftable.data->vmin)*4+1]*65535/255;
	color.blue=ETF1Dpanel.cmap->colormap[(ETF1Dpanel.selA-gtftable.data->vmin)*4+2]*65535/255;

	gtk_color_button_set_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]),&color);
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]),ETF1Dpanel.cmap->colormap[(ETF1Dpanel.selA-gtftable.data->vmin)*4+3]*65535/255);

	//TF1Dpanel.spinlock=0;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[0]),ETF1Dpanel.selA);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[1]),ETF1Dpanel.selA);


	return FALSE;
}

int	ETF1Dpanel_hcodisplayMouseUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	GdkColor	color;
	int tmp;
	if(event->button!=1||!gtftable.data) return FALSE;
	ETF1Dpanel.selB=event->x*gtftable.level/512+gtftable.data->vmin;

	//	To ensure A is always at the position before B
	//	Swap selected position A,B if necessary

	if(ETF1Dpanel.selA>ETF1Dpanel.selB)
	{
		tmp=ETF1Dpanel.selB;
		ETF1Dpanel.selB=ETF1Dpanel.selA;
		ETF1Dpanel.selA=tmp;
	}

	//	Boundary check
	if(ETF1Dpanel.selB>gtftable.data->vmax)
		ETF1Dpanel.selB=gtftable.data->vmax;
	if(ETF1Dpanel.selA<gtftable.data->vmin)
		ETF1Dpanel.selA=gtftable.data->vmin;


	ETF1Dpanel.spinlock=1;

	if(ETF1Dpanel.selA!=ETF1Dpanel.selB)
	{
		gtk_widget_set_sensitive(ETF1Dpanel.colorbox[1],TRUE);
		gtk_widget_set_sensitive(ETF1Dpanel.cmapselect,TRUE);
		gtk_widget_set_sensitive(ETF1Dpanel.select_value[1],TRUE);
		gtk_widget_set_sensitive(ETF1Dpanel.assign,TRUE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[0]),ETF1Dpanel.selA);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[1]),ETF1Dpanel.selB);
	}else
	{
		gtk_widget_set_sensitive(ETF1Dpanel.colorbox[1],FALSE);
		gtk_widget_set_sensitive(ETF1Dpanel.cmapselect,FALSE);
		gtk_widget_set_sensitive(ETF1Dpanel.select_value[1],FALSE);
		gtk_widget_set_sensitive(ETF1Dpanel.assign,FALSE);

	}

	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]),&color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[1]),&color);
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[1]),gtk_color_button_get_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0])));
	gtk_combo_box_set_active(GTK_COMBO_BOX(ETF1Dpanel.cmapselect),-1);

	//	Reset selection flag
	ETF1Dpanel.sel=0;

	return FALSE;
}


void	ETF1Dpanel_colorAChange_handler(GtkWidget *widget,gpointer user_data)
{
	GdkColor	color;

	if(!gtftable.data) return;

	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]),&color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[1]),&color);
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[1]),gtk_color_button_get_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0])));

	if(ETF1Dpanel.selA!=ETF1Dpanel.selB) return;

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.checkbox[0])))
	{
		ETF1Dpanel.cmap->colormap[ETF1Dpanel.selA*4]=color.red*255/65535;
		ETF1Dpanel.cmap->colormap[ETF1Dpanel.selA*4+1]=color.green*255/65535;
		ETF1Dpanel.cmap->colormap[ETF1Dpanel.selA*4+2]=color.blue*255/65535;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.checkbox[1])))
		ETF1Dpanel.cmap->colormap[ETF1Dpanel.selA*4+3]=gtk_color_button_get_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]))*255/65535;


	ETF1Dpanel_UpdateTexture();
	ETF1D_Update();

}

void	ETF1Dpanel_Interpolate(GtkWidget *widget,gpointer user_data)
{
	int		tmp;
	int		x,l,n;
	GdkColor	color[2];
	int		ca[4],cb[4];

	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]),&color[0]);
	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[1]),&color[1]);

	ca[0]=(int)color[0].red;
	ca[1]=(int)color[0].green;
	ca[2]=(int)color[0].blue;
	ca[3]=(int)gtk_color_button_get_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[0]));

	cb[0]=(int)color[1].red;
	cb[1]=(int)color[1].green;
	cb[2]=(int)color[1].blue;
	cb[3]=(int)gtk_color_button_get_alpha(GTK_COLOR_BUTTON(ETF1Dpanel.colorbox[1]));

	if(ETF1Dpanel.selA>ETF1Dpanel.selB)
	{
		tmp=ETF1Dpanel.selB;
		ETF1Dpanel.selB=ETF1Dpanel.selA;
		ETF1Dpanel.selA=tmp;
	}


	l=ETF1Dpanel.selB-ETF1Dpanel.selA+1;
	n=ETF1Dpanel.selA-gtftable.data->vmin;

	for(x=ETF1Dpanel.selA-gtftable.data->vmin;x<=ETF1Dpanel.selB-gtftable.data->vmin;++x)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.checkbox[0])))
		{
			ETF1Dpanel.cmap->colormap[x*4]=(ca[0]+(x-n)*(cb[0]-ca[0])/l)*255/65535;
			ETF1Dpanel.cmap->colormap[x*4+1]=(ca[1]+(x-n)*(cb[1]-ca[1])/l)*255/65535;
			ETF1Dpanel.cmap->colormap[x*4+2]=(ca[2]+(x-n)*(cb[2]-ca[2])/l)*255/65535;
		}
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.checkbox[1])))
			ETF1Dpanel.cmap->colormap[x*4+3]=(ca[3]+(x-n)*(cb[3]-ca[3])/l)*255/65535;
	}

	ETF1Dpanel_UpdateTexture();
	ETF1D_Update();



}

void	ETF1Dpanel_ValueSelect(GtkWidget *widget,gpointer user_data)
{
	if(ETF1Dpanel.spinlock)
	{
		ETF1Dpanel.spinlock=0;
		return;
	}

	ETF1Dpanel.selA=gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[0]));
	ETF1Dpanel.selB=gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.select_value[1]));

	//gdk_window_invalidate_rect(ETF1Dpanel.hcodisplay->window,NULL,TRUE);
}

void	ETF1Dpanel_AssignColormap(GtkWidget *widget,gpointer user_data)
{
	int	i,l,tmp;
	hCMAP*	cmap=NULL;
	hCMAP*	c=NULL;
	char*	filename;

	i=gtk_combo_box_get_active(GTK_COMBO_BOX(ETF1Dpanel.cmapselect));

	if(ETF1Dpanel.selA>ETF1Dpanel.selB)
	{
		tmp=ETF1Dpanel.selB;
		ETF1Dpanel.selB=ETF1Dpanel.selA;
		ETF1Dpanel.selA=tmp;
	}


	l=ETF1Dpanel.selB-ETF1Dpanel.selA+1;

	if(i==-1) return;

	switch(i)
	{
		case 0:
			cmap=cmapNewHSV(l);
		break;
		case 1:
			cmap=cmapNewGray(l);
		break;
		case 2:
			cmap=cmapNewHot(l);
		break;
		case 3:
			cmap=cmapNewCool(l);
		break;
		case 4:
			cmap=cmapNewSpring(l);
		break;
		case 5:
			cmap=cmapNewSummer(l);
		break;
		case 6:
			cmap=cmapNewAutumn(l);
		break;
		case 7:
			cmap=cmapNewWinter(l);
		break;
		case 8:
			cmap=cmapNewCopper(l);
		break;
		case 9:
			filename=FileDialog(OPEN_DIALOG,Mainwin.window);
			c=cmapNewFromFile(filename);
			free(filename);
			if(!c) return;
			cmap=cmapNewRemap(c,l,0,l,FALSE);
			cmapRelease(c);
		break;
	}

	cmapPut(ETF1Dpanel.cmap,cmap,l,ETF1Dpanel.selA);
	cmapRelease(cmap);

	ETF1Dpanel_UpdateTexture();
	ETF1D_Update();

}

void	ETF1D_Update(void)
{
	hCMAP*	cmap;

	//	Remap colormap exceeding video card's MAX texture dimension to appropriate size.

	cmap=cmapNewRemap(ETF1Dpanel.cmap,TF1D_TEXTURE_WIDTH,0,TF1D_TEXTURE_WIDTH,0);
	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.etf1d_tex);
	//glTexImage1D(GL_TEXTURE_1D,0,4,gtftable.level,0,GL_RGBA,GL_UNSIGNED_BYTE,ETF1Dpanel.cmap->colormap);
	glTexImage1D(GL_TEXTURE_1D,0,4,TF1D_TEXTURE_WIDTH,0,GL_RGBA,GL_UNSIGNED_BYTE,cmap->colormap);
	cmapRelease(cmap);

	gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);
	gdk_window_invalidate_rect(AA2DSlicewin.window->window,NULL,TRUE);
	gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window,NULL,TRUE);

}


void	ETF1Dpanel_UpdateTexture(void)
{
	//	Update 1D texture for display

	getRGBfromColormap(ETF1Dpanel.cmap,TF1D_TEXTURE_WIDTH,ETF1Dpanel.cmaptex);
	getALPHAfromColormap(ETF1Dpanel.cmap,TF1D_TEXTURE_WIDTH,ETF1Dpanel.opacitytex);

	gdk_gl_drawable_make_current(ETF1Dpanel.gldrawable,ETF1Dpanel.glrc);

	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.ctex);
	glTexSubImage1D(GL_TEXTURE_1D,0,0,TF1D_TEXTURE_WIDTH,GL_RGB,GL_UNSIGNED_BYTE,ETF1Dpanel.cmaptex);

	glBindTexture(GL_TEXTURE_1D,ETF1Dpanel.otex);
	glTexSubImage1D(GL_TEXTURE_1D,0,0,TF1D_TEXTURE_WIDTH,GL_LUMINANCE,GL_UNSIGNED_BYTE,ETF1Dpanel.opacitytex);

	gdk_window_invalidate_rect(ETF1Dpanel.hcodisplay->window,NULL,TRUE);
}

void	ETF1Dpanel_SaveColormap(GtkWidget *widget,gpointer user_data)
{
	GtkWidget*	dialog;
	char*		filename;

	if(!gtftable.data) return;

	filename=FileDialog(SAVE_DIALOG,Mainwin.window);
	cmapWriteToFile(filename,ETF1Dpanel.cmap);
	free(filename);

	dialog=gtk_message_dialog_new(GTK_WINDOW(Mainwin.window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,_("File saved successfully"));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void	ETF1Dpanel_ToggleLevoy(GtkWidget *widget,gpointer user_data)
{
	if(!gtftable.data) return;

	//glUniform1i(ETF1Dpanel.AA3Dtexloc.levoy_swloc,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.levoy_sw)));

	if(GTK_WIDGET_VISIBLE(AA3DTexwin.glwin->window))
	{
		gdk_gl_drawable_make_current(AA3DTexwin.glwin->gldrawable,AA3DTexwin.glwin->glrc);
		glUniform1i(ETF1Dpanel.AA3Dtexloc.levoy_swloc,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.levoy_sw)));
		glUniform1f(ETF1Dpanel.AA3Dtexloc.levoy_valueloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_value)));
		glUniform1f(ETF1Dpanel.AA3Dtexloc.levoy_thicknessloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_thickness)));

		gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);
	}
//	if(GTK_WIDGET_VISIBLE(AA2DSlicewin.window))
//	{
//		gdk_gl_drawable_make_current(AA2DSlicewin.gldrawable,AA2DSlicewin.glrc);
//		glUniform1i(ETF1Dpanel.AA3Dtexloc.levoy_swloc,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.levoy_sw)));
//		gdk_window_invalidate_rect(AA2DSlicewin.window->window,NULL,TRUE);
//	}
	if(GTK_WIDGET_VISIBLE(VA3DTexwin.glwin->window))
	{
		gdk_gl_drawable_make_current(VA3DTexwin.glwin->gldrawable,VA3DTexwin.glwin->glrc);
		glUniform1i(ETF1Dpanel.VA3Dtexloc.levoy_swloc,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ETF1Dpanel.levoy_sw)));
		glUniform1f(ETF1Dpanel.VA3Dtexloc.levoy_valueloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_value)));
		glUniform1f(ETF1Dpanel.VA3Dtexloc.levoy_thicknessloc,gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF1Dpanel.levoy_thickness)));

		gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window,NULL,TRUE);
	}

}
