#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "intl.h"
#include "glext.h"
#include "tfaux.h"
#include "aa3Dtexwin.h"
#include "mainwin.h"
#include "etf2d.h"
#include "util.h"

strETF2Dpanel	ETF2Dpanel;


static	void	_3DTexGetLocation(ETF2D_3Dtexloc* loc,unsigned int shader)
{
	loc->volumeloc	=glGetUniformLocation(shader,"volume");
	loc->colormaploc=glGetUniformLocation(shader,"colormap");
	loc->opacityloc	=glGetUniformLocation(shader,"opacity");
	loc->stencilloc	=glGetUniformLocation(shader,"stencil");
	loc->coefloc	=glGetUniformLocation(shader,"coef");
	loc->shiftloc	=glGetUniformLocation(shader,"shift");
	loc->deltaloc	=glGetUniformLocation(shader,"delta");
	loc->normalloc	=glGetUniformLocation(shader,"normal");
	loc->gmaxloc	=glGetUniformLocation(shader,"gmax");
}


static	float	calculate_length(struct _position* p,int x,int y)
{
	return sqrt((x-p->x)*(x-p->x)+(y-p->y)*(y-p->y));
}


static	void	write_position(struct _position* p,int x,int y)
{
	if(x<0)	x=0;
	if(x>511)	x=511;
	if(y<0)	y=0;
	if(y>255)	y=255;

	p->x		=x;
	p->y		=y;
	p->value	=x*gtftable.level/512+gtftable.data->vmin;
	p->gradient	=(255-y)*(gtftable.data->gmax-gtftable.data->gmin+1)/256;
}

static void	genHistogram2D(DATA* data,unsigned int** histogram,unsigned char** texture)
{
	int	i,w,h,d;
	int	x,y;

	double	m,normalized,a,b;
	int	l;

	if(!data) return;

	w=data->vmax-data->vmin+1;
	h=data->gmax+1;
	*histogram=calloc(sizeof(unsigned int),w*h);


	m=0;

	for(i=0;i<data->totalsize;++i)
	{
		if(data->gradient[i*4+3]*data->gmax<2.0)
			l=0.0;
		else	l=data->gradient[i*4+3];

		d=w*l+data->rawdata[i];

		++((*histogram)[d]);

		if(m<(*histogram)[d]) m=(*histogram)[d];
	}

	m=10.0*log(m);

	*texture=calloc(sizeof(unsigned char),TF2D_TEXTURE_WIDTH*TF2D_TEXTURE_HEIGHT);

	for(y=0;y<TF2D_TEXTURE_HEIGHT;++y)
	{
		for(x=0;x<TF2D_TEXTURE_WIDTH;++x)
		{
			a=x*w/TF2D_TEXTURE_WIDTH;
			b=y*h/TF2D_TEXTURE_HEIGHT;

			d=w*b+a;

			if((*histogram)[d]>1)
				normalized=255.0*10.0*log((*histogram)[d])/m;
			else	normalized=0.0;

			(*texture)[TF2D_TEXTURE_WIDTH*y+x]=normalized;
		}
	}

}


static	void	opacitymap_writepixel(int value,int gradient,int opacity)
{
	if(value<0) value=0;
	if(value>=gtftable.level) value=gtftable.level-1;

	if(gradient<0) gradient=0;
	if(gradient>gtftable.data->gmax) gradient=gtftable.data->gmax;

	ETF2Dpanel.opacitymap[gtftable.level*gradient+value]=opacity;
}


static	void	fill(int x,int y,float nx,float ny)
{
	int		mode;
	double	h;
	double	v;
	double	sigma;


	mode=gtk_combo_box_get_active(GTK_COMBO_BOX(ETF2Dpanel.mode_selector));
	h=gtk_range_get_value(GTK_RANGE(ETF2Dpanel.opacity_h));
	v=gtk_range_get_value(GTK_RANGE(ETF2Dpanel.opacity_v));
	sigma=gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF2Dpanel.sigma));

	switch(mode)
	{
		case 0:
			h/=255.0;
		break;
		case 1:
			h=h/255.0*(1.0-2.0*fabs(nx-0.5));
		break;
		case 2:
			h=h/255.0*exp(-(nx-0.5)*(nx-0.5)/(2*sigma*sigma));
		break;
	}

	v=v/255.0*pow(ny,gtk_range_get_value(GTK_RANGE(ETF2Dpanel.exp)));


	opacitymap_writepixel(x,y,255.0*h*v);
}


void	tfCreateETF2Dpanel(int i)
{
	GtkBuilder*	builder;
	GtkCellRenderer*	cell;

	memset(&ETF2Dpanel,0,sizeof(strETF2Dpanel));

	builder=gtk_builder_new();

	gtk_builder_set_translation_domain(builder,"gtf");
	gtk_builder_add_from_file(builder,DATADIR"/gtkbuilder/etf2d.xml",NULL);

	gtk_builder_connect_signals(builder,NULL);

	ETF2Dpanel.hdisplay=GTK_WIDGET(gtk_builder_get_object(builder,"hdisplay"));

	ETF2Dpanel.vbox=GTK_WIDGET(gtk_builder_get_object(builder,"vbox1"));

	ETF2Dpanel.vmax=GTK_WIDGET(gtk_builder_get_object(builder,"vmax"));
	ETF2Dpanel.vmin=GTK_WIDGET(gtk_builder_get_object(builder,"vmin"));
	ETF2Dpanel.gmax=GTK_WIDGET(gtk_builder_get_object(builder,"gmax"));

	ETF2Dpanel.status=GTK_WIDGET(gtk_builder_get_object(builder,"status"));
	ETF2Dpanel.histogram_selector=GTK_WIDGET(gtk_builder_get_object(builder,"histogram_selector"));

	ETF2Dpanel.histogram_switch=GTK_WIDGET(gtk_builder_get_object(builder,"histogram"));
	ETF2Dpanel.opacity_switch=GTK_WIDGET(gtk_builder_get_object(builder,"opacity"));

	ETF2Dpanel.colormap_selector=GTK_WIDGET(gtk_builder_get_object(builder,"colormap_selector"));
	ETF2Dpanel.mode_selector=GTK_WIDGET(gtk_builder_get_object(builder,"mode_selector"));

	ETF2Dpanel.opacity_h=GTK_WIDGET(gtk_builder_get_object(builder,"opacity_h"));
	ETF2Dpanel.opacity_v=GTK_WIDGET(gtk_builder_get_object(builder,"opacity_v"));
	ETF2Dpanel.sigma=GTK_WIDGET(gtk_builder_get_object(builder,"sigma"));
	ETF2Dpanel.exp=GTK_WIDGET(gtk_builder_get_object(builder,"exp"));

	ETF2Dpanel.select_value[0]=GTK_WIDGET(gtk_builder_get_object(builder,"value_A"));
	ETF2Dpanel.select_value[1]=GTK_WIDGET(gtk_builder_get_object(builder,"value_B"));

	ETF2Dpanel.color[0]=GTK_WIDGET(gtk_builder_get_object(builder,"color_A"));
	ETF2Dpanel.color[1]=GTK_WIDGET(gtk_builder_get_object(builder,"color_B"));


	cell=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ETF2Dpanel.histogram_selector),cell,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ETF2Dpanel.histogram_selector),cell,"text",0,NULL);

	cell=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ETF2Dpanel.colormap_selector),cell,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ETF2Dpanel.colormap_selector),cell,"text",0,NULL);

	cell=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ETF2Dpanel.mode_selector),cell,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ETF2Dpanel.mode_selector),cell,"text",0,NULL);



	gtk_container_add(GTK_CONTAINER(tfGetPanel(i)),ETF2Dpanel.vbox);
	g_object_unref(G_OBJECT(builder));




	ETF2Dpanel.id=i;

	gtftable.TF_init[i]=ETF2D_init;
	gtftable.TF_release[i]=ETF2D_release;

	gtftable.AA2Dslice_init[i]=ETF2D_InitAA2Dslice;
	gtftable.AA3Dtex_init[i]=ETF2D_InitAA3Dtex;
	gtftable.VA3Dtex_init[i]=ETF2D_InitVA3Dtex;

}

void	tfSetupETF2Dpanel(void)
{
	int		shader;

	gtk_range_set_value(GTK_RANGE(ETF2Dpanel.opacity_h),255.0);
	gtk_range_set_value(GTK_RANGE(ETF2Dpanel.opacity_v),255.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF2Dpanel.sigma),0.1);


	//	Create OpenGL canvas

	ETF2Dpanel.glcanvas = gtk_glcanvas_new(ETF2Dpanel.hdisplay, gtftable.sharedrc);
	//gtk_widget_realize(ETF2Dpanel.hdisplay);
	glcanvas_make_current(ETF2Dpanel.glcanvas, NULL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//	Setup all shaders

	gtftable.shader[ETF2Dpanel.id].aa2Dslice=glLoadShader(DATADIR"/aa3Dtex.vs",DATADIR"/ETF2D_aa2Dslice.fs");
	gtftable.shader[ETF2Dpanel.id].aa3Dtex=glLoadShader(DATADIR"/aa3Dtex.vs",DATADIR"/ETF2D_3Dtex.fs");
	gtftable.shader[ETF2Dpanel.id].va3Dtex=glLoadShader(DATADIR"/va3Dtex.vs",DATADIR"/ETF2D_3Dtex.fs");

//	shader=gtftable.shader[ETF2Dpanel.id].aa2Dslice;
//
//	ETF2Dpanel.AA2Dslice_tex3dloc=glGetUniformLocation(shader,"tex");
//	ETF2Dpanel.AA2Dslice_cmaploc=glGetUniformLocation(shader,"map");
//	ETF2Dpanel.AA2Dslice_stencilloc=glGetUniformLocation(shader,"stencil");
//	ETF2Dpanel.AA2Dslice_coefloc=glGetUniformLocation(shader,"coef");
//	ETF2Dpanel.AA2Dslice_shiftloc=glGetUniformLocation(shader,"shift");
//	ETF2Dpanel.AA2Dslice_swloc=glGetUniformLocation(shader,"sw");

	shader=gtftable.shader[ETF2Dpanel.id].aa3Dtex;
	_3DTexGetLocation(&ETF2Dpanel.AA3Dtexloc,shader);

	shader=gtftable.shader[ETF2Dpanel.id].va3Dtex;
	_3DTexGetLocation(&ETF2Dpanel.VA3Dtexloc,shader);
	VA3DTex_GetLocation(&ETF2Dpanel.VA3Dtexvsloc,shader);
	VA3DTexwin.vsloc[ETF2Dpanel.id]=&ETF2Dpanel.VA3Dtexvsloc;
}


void	ETF2D_init(void)
{
	char	str[64];

	sprintf(str,"%d",gtftable.data->vmax);
	gtk_label_set_text(GTK_LABEL(ETF2Dpanel.vmax),str);
	sprintf(str,"%d",gtftable.data->vmin);
	gtk_label_set_text(GTK_LABEL(ETF2Dpanel.vmin),str);
	sprintf(str,"%.2f",gtftable.data->gmax);
	gtk_label_set_text(GTK_LABEL(ETF2Dpanel.gmax),str);


	gtk_spin_button_set_range(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[0]),gtftable.data->vmin,gtftable.data->vmax);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[1]),gtftable.data->vmin,gtftable.data->vmax);


	glcanvas_make_current(ETF2Dpanel.glcanvas, NULL);

	glViewport(0,0,512,281);

	genHistogram2D(gtftable.data,&ETF2Dpanel.histogram2D,&ETF2Dpanel.histogram2Dtex);


	ETF2Dpanel.colormap=cmapNewHSV(gtftable.level);
	ETF2Dpanel.colormaptex=calloc(1,TF2D_TEXTURE_WIDTH*3);
	ETF2Dpanel.opacitymap=calloc(1,gtftable.level*(int)(gtftable.data->gmax+1));
	ETF2Dpanel.opacitytex=calloc(1,TF2D_TEXTURE_WIDTH*TF2D_TEXTURE_HEIGHT);

	//memset(ETF2Dpanel.opacitymap,255,gtftable.level*gtftable.data->gmax+1);

	glGenTextures(1,&ETF2Dpanel.histogram_texobj);
	glGenTextures(1,&ETF2Dpanel.colormap_texobj);
	glGenTextures(1,&ETF2Dpanel.opacity_texobj);


	glDisable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.histogram_texobj);
	glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE8,TF2D_TEXTURE_WIDTH,TF2D_TEXTURE_HEIGHT,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,ETF2Dpanel.histogram2Dtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.opacity_texobj);
	//glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE8,TF2D_TEXTURE_WIDTH,TF2D_TEXTURE_HEIGHT,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,ETF2Dpanel.opacitytex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA8,TF2D_TEXTURE_WIDTH,TF2D_TEXTURE_HEIGHT,0,GL_ALPHA,GL_UNSIGNED_BYTE,ETF2Dpanel.opacitytex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_1D);

	glBindTexture(GL_TEXTURE_1D,ETF2Dpanel.colormap_texobj);
	glTexImage1D(GL_TEXTURE_1D,0,3,TF2D_TEXTURE_WIDTH,0,GL_RGB,GL_UNSIGNED_BYTE,ETF2Dpanel.colormaptex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	ETF2Dpanel_UpdateTexture();

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

void	ETF2D_release(void)
{
	if(!gtftable.data) return;
	glcanvas_make_current(ETF2Dpanel.glcanvas, NULL);
	glDeleteTextures(1,&ETF2Dpanel.histogram_texobj);
	glDeleteTextures(1,&ETF2Dpanel.colormap_texobj);
	glDeleteTextures(1,&ETF2Dpanel.opacity_texobj);
	cmapRelease(ETF2Dpanel.colormap);
	free(ETF2Dpanel.histogram2D);
	free(ETF2Dpanel.histogram2Dtex);
	free(ETF2Dpanel.colormaptex);
	free(ETF2Dpanel.opacitytex);

	ETF2Dpanel.histogram2D=NULL;
	ETF2Dpanel.histogram2Dtex=NULL;
	ETF2Dpanel.colormap=NULL;
	ETF2Dpanel.colormaptex=NULL;
	ETF2Dpanel.opacitymap=NULL;
	ETF2Dpanel.opacitytex=NULL;

}

void	ETF2D_InitAA2Dslice(void)
{

}

void	ETF2D_InitAA3Dtex(void)
{
	glcanvas_make_current(AA3DTexwin.glwin->glcanvas, NULL);

	glUniform1f(ETF2Dpanel.AA3Dtexloc.coefloc,65535.0/(gtftable.data->vmax-gtftable.data->vmin));
	glUniform1f(ETF2Dpanel.AA3Dtexloc.shiftloc,(double)gtftable.data->vmin/(double)(gtftable.data->vmax-gtftable.data->vmin));

	//	Create 3D texture directly from volume RAW data

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gtftable.volume_tex);
	glUniform1i(ETF2Dpanel.AA3Dtexloc.volumeloc,0);


	//	Create 1D texture from colormap RGBA data

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D,ETF2Dpanel.colormap_texobj);
	glUniform1i(ETF2Dpanel.AA3Dtexloc.colormaploc,1);

	//	Create 1D texture from colormap RGBA data

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.opacity_texobj);
	glUniform1i(ETF2Dpanel.AA3Dtexloc.opacityloc,2);


	//	Create 3D texture from normal

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D,gtftable.gradient_tex);
	glUniform1i(ETF2Dpanel.AA3Dtexloc.normalloc,3);
	glUniform1f(ETF2Dpanel.AA3Dtexloc.gmaxloc,gtftable.data->gmax);
	glUniform3f(ETF2Dpanel.AA3Dtexloc.deltaloc,1.0/(gtftable.data->resol[0]-1),1.0/(gtftable.data->resol[1]-1),1.0/(gtftable.data->resol[2]-1));
	AA3DTexwin.glwin->render_flag=TRUE;


}

void	ETF2D_InitVA3Dtex(void)
{
	glcanvas_make_current(VA3DTexwin.glwin->glcanvas, NULL);

	glUniform1iv(ETF2Dpanel.VA3Dtexvsloc.sequenceloc,64,va3Dtex_nsequence);
	glUniform1iv(ETF2Dpanel.VA3Dtexvsloc.valoc,24,va3Dtex_va);
	glUniform1iv(ETF2Dpanel.VA3Dtexvsloc.vbloc,24,va3Dtex_vb);

	glUniform1f(ETF2Dpanel.VA3Dtexloc.coefloc,65535.0/(gtftable.data->vmax-gtftable.data->vmin));
	glUniform1f(ETF2Dpanel.VA3Dtexloc.shiftloc,(double)gtftable.data->vmin/(double)(gtftable.data->vmax-gtftable.data->vmin));

	//	Create 3D texture directly from volume RAW data

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D,gtftable.volume_tex);
	glUniform1i(ETF2Dpanel.VA3Dtexloc.volumeloc,0);


	//	Create 1D texture from colormap RGBA data

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D,ETF2Dpanel.colormap_texobj);
	glUniform1i(ETF2Dpanel.VA3Dtexloc.colormaploc,1);

	//	Create 1D texture from colormap RGBA data

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.opacity_texobj);
	glUniform1i(ETF2Dpanel.VA3Dtexloc.opacityloc,2);


	//	Create 3D texture from normal

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D,gtftable.gradient_tex);
	glUniform1i(ETF2Dpanel.VA3Dtexloc.normalloc,3);
	glUniform1f(ETF2Dpanel.VA3Dtexloc.gmaxloc,gtftable.data->gmax);
	glUniform3f(ETF2Dpanel.VA3Dtexloc.deltaloc,1.0/(gtftable.data->resol[0]-1),1.0/(gtftable.data->resol[1]-1),1.0/(gtftable.data->resol[2]-1));

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

	glUniform3fv(ETF2Dpanel.VA3Dtexvsloc.verticesloc,8,va_vertices);

}




int	ETF2Dpanel_hdisplay_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data)
{
	int	m,n,i;

	glcanvas_make_current(ETF2Dpanel.glcanvas, NULL);

	//	No volume loaded, return
	if(!gtftable.data)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glcanvas_swap_buffers(ETF2Dpanel.glcanvas);
		return TRUE;
	}

	m=gtftable.data->vmax;
	n=gtftable.data->vmin;

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,511,0,280);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	//	Draw 2D histogram

	glDisable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.histogram_texobj);

	if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(ETF2Dpanel.histogram_switch)))
	{
		glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.histogram_texobj);
		glDrawBlock(0,25,511,280);
	}

	if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(ETF2Dpanel.opacity_switch)))
	{
		glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.opacity_texobj);
		glDrawBlock(0,25,511,280);
	}

	//	Draw 1D colormap

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_1D);

	glBindTexture(GL_TEXTURE_1D,ETF2Dpanel.colormap_texobj);
	glDrawBlock(0,0,511,24);


	//	Draw cursor

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_1D);

	glColor3f(1.0,1.0,0.0);
	glBegin(GL_LINES);
		glVertex2f(ETF2Dpanel.cursorpos[0].x,0);
		glVertex2f(ETF2Dpanel.cursorpos[0].x,280);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(0,280-ETF2Dpanel.cursorpos[0].y);
		glVertex2f(511,280-ETF2Dpanel.cursorpos[0].y);
	glEnd();


	//	Draw selection area

	if(ETF2Dpanel.selection_status==TF2D_MODE_BOX_DRAWING)
	{
		glColor4f(0.0,1.0,0.0,0.2);
		glDrawBlock(ETF2Dpanel.cursorpos[1].x,280-ETF2Dpanel.cursorpos[1].y,ETF2Dpanel.cursorpos[0].x,280-ETF2Dpanel.cursorpos[0].y);
	}


	//	Draw selection frame if in TF2D_MODE_BOX_SELECTED or TF2D_MODE_POINT_MOVING

	if(ETF2Dpanel.selection_status>=TF2D_MODE_BOX_SELECTED/*||ETF2Dpanel.selection_status==TF2D_MODE_POINT_MOVING*/)
	{
		glColor3f(1.0,0.0,0.0);
		glBegin(GL_LINE_STRIP);
		for(i=0;i<4;++i)
		{
			glVertex2f(ETF2Dpanel.selection[i].x,280-ETF2Dpanel.selection[i].y);
		}
		glVertex2f(ETF2Dpanel.selection[0].x,280-ETF2Dpanel.selection[0].y);
		glEnd();

		glPointSize(5.0);
		glBegin(GL_POINTS);
		for(i=0;i<4;++i)
		{
			glVertex2f(ETF2Dpanel.selection[i].x,280-ETF2Dpanel.selection[i].y);
		}
		glEnd();

		if(ETF2Dpanel.selection_status==TF2D_MODE_POINT_MOVING)
		{
			glPointSize(10.0);
			glColor3f(1.0,1.0,0.0);
			glBegin(GL_POINTS);
				glVertex2f(ETF2Dpanel.selection[ETF2Dpanel.selected].x,280-ETF2Dpanel.selection[ETF2Dpanel.selected].y);
			glEnd();
		}

		glPointSize(1.0);
	}

	if(ETF2Dpanel.selection_status==TF2D_MODE_COLORMAP_SELECTING||ETF2Dpanel.selection_status==TF2D_MODE_COLORMAP_SELECTED)
	{
		glColor3f(1.0,1.0,1.0);

		//glDrawBlock(ETF2Dpanel.select_for_color[1],12,ETF2Dpanel.select_for_color[3],25);
		glBegin(GL_LINE_STRIP);
			glVertex2f(ETF2Dpanel.select_for_color[1],0);
			glVertex2f(ETF2Dpanel.select_for_color[3],0);
			glVertex2f(ETF2Dpanel.select_for_color[3],25);
			glVertex2f(ETF2Dpanel.select_for_color[1],25);
			glVertex2f(ETF2Dpanel.select_for_color[1],0);
		glEnd();


	}

	glColor3f(1.0,1.0,1.0);
	glcanvas_swap_buffers(ETF2Dpanel.glcanvas);

	return FALSE;
}

void	ETF2Dpanel_UpdateTexture(void)
{
	int	x,y,i;
	int	w,h;


	//	Update textures for display

	getRGBfromColormap(ETF2Dpanel.colormap,TF2D_TEXTURE_WIDTH,ETF2Dpanel.colormaptex);
	glcanvas_make_current(ETF2Dpanel.glcanvas, NULL);

	glBindTexture(GL_TEXTURE_1D,ETF2Dpanel.colormap_texobj);
	glTexSubImage1D(GL_TEXTURE_1D,0,0,TF2D_TEXTURE_WIDTH,GL_RGB,GL_UNSIGNED_BYTE,ETF2Dpanel.colormaptex);

	//	Remapping

	for(y=0;y<TF2D_TEXTURE_HEIGHT;++y)
		for(x=0;x<TF2D_TEXTURE_WIDTH;++x)
		{
			w=gtftable.level;
			h=gtftable.data->gmax+1;

			i=(h*y/TF2D_TEXTURE_HEIGHT)*w+(w*x/TF2D_TEXTURE_WIDTH);

			ETF2Dpanel.opacitytex[TF2D_TEXTURE_WIDTH*y+x]=ETF2Dpanel.opacitymap[i];
		}

	glBindTexture(GL_TEXTURE_2D,ETF2Dpanel.opacity_texobj);
	//glTexSubImage2D(GL_TEXTURE_2D,0,0,0,TF2D_TEXTURE_WIDTH,TF2D_TEXTURE_HEIGHT,GL_LUMINANCE,GL_UNSIGNED_BYTE,ETF2Dpanel.opacitytex);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,TF2D_TEXTURE_WIDTH,TF2D_TEXTURE_HEIGHT,GL_ALPHA,GL_UNSIGNED_BYTE,ETF2Dpanel.opacitytex);

	gdk_window_invalidate_rect(AA3DTexwin.glwin->window->window,NULL,TRUE);
	gdk_window_invalidate_rect(VA3DTexwin.glwin->window->window,NULL,TRUE);
	ETF2Dpanel_hdisplay_handler(0,0,0);

}


int	ETF2Dpanel_hdisplayMouseMove_handler(GtkWidget *widget,GdkEventMotion *event,gpointer user_data)
{
	char		str[64];
	const int	table[]={1,0,3,2};

	if(!gtftable.data) return FALSE;

	write_position(&ETF2Dpanel.cursorpos[0],event->x,event->y);

	if(ETF2Dpanel.selection_status==TF2D_MODE_POINT_MOVING)
	{
		memcpy(&ETF2Dpanel.selection[ETF2Dpanel.selected],&ETF2Dpanel.cursorpos[0],sizeof(struct _position));

		ETF2Dpanel.selection[table[ETF2Dpanel.selected]].y=ETF2Dpanel.cursorpos[0].y;
		ETF2Dpanel.selection[table[ETF2Dpanel.selected]].gradient=ETF2Dpanel.cursorpos[0].gradient;

	}

	if(ETF2Dpanel.selection_status==TF2D_MODE_COLORMAP_SELECTING)
	{
		ETF2Dpanel.select_for_color[2]=ETF2Dpanel.cursorpos[0].value;
		ETF2Dpanel.select_for_color[3]=ETF2Dpanel.cursorpos[0].x;

		gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[1]),ETF2Dpanel.select_for_color[2]);

	}

	sprintf(str,"%s: %d, %s: %d",_("Value"),ETF2Dpanel.cursorpos[0].value,_("Gradient magnitude"),(int)ETF2Dpanel.cursorpos[0].gradient);
	gtk_label_set_text(GTK_LABEL(ETF2Dpanel.status),str);

	ETF2Dpanel_hdisplay_handler(0,0,0);

	return FALSE;
}


int	ETF2Dpanel_hdisplayMouseDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	int	i;
	if(event->button!=1||!gtftable.data) return FALSE;

	write_position(&ETF2Dpanel.cursorpos[0],event->x,event->y);
	write_position(&ETF2Dpanel.cursorpos[1],event->x,event->y);


	//	Set selected point to none (-1)

	ETF2Dpanel.selected=-1;

	for(i=0;i<4;++i)
	{
		if(calculate_length(&ETF2Dpanel.selection[i],event->x,event->y)<TF2D_DISTANCE_TO_GRAB_POINT)
			ETF2Dpanel.selected=i;
	}

	ETF2Dpanel.selection_status=(ETF2Dpanel.selected==(-1)) ? TF2D_MODE_BOX_DRAWING : TF2D_MODE_POINT_MOVING;

	if(event->y>=256)
	{
		ETF2Dpanel.selection_status=TF2D_MODE_COLORMAP_SELECTING;
		ETF2Dpanel.select_for_color[0]=ETF2Dpanel.cursorpos[0].value;
		ETF2Dpanel.select_for_color[1]=ETF2Dpanel.cursorpos[0].x;

		gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[0]),ETF2Dpanel.select_for_color[0]);
	}



	ETF2Dpanel_hdisplay_handler(0,0,0);

	return FALSE;
}


int	ETF2Dpanel_hdisplayMouseUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
{
	int	a,b,c,d;
	//int	x,y;


	if(event->button!=1||!gtftable.data) return FALSE;

	write_position(&ETF2Dpanel.cursorpos[0],event->x,event->y);

	if(ETF2Dpanel.selection_status==TF2D_MODE_BOX_DRAWING)
	{
		if(ETF2Dpanel.cursorpos[0].x==ETF2Dpanel.cursorpos[1].x&&ETF2Dpanel.cursorpos[0].y==ETF2Dpanel.cursorpos[1].y)
			ETF2Dpanel.selection_status=0;
		else	ETF2Dpanel.selection_status=2;
	}

	if(ETF2Dpanel.selection_status==TF2D_MODE_BOX_SELECTED)
	{
		if(ETF2Dpanel.cursorpos[0].gradient<ETF2Dpanel.cursorpos[1].gradient)
		{
			if(ETF2Dpanel.cursorpos[0].value<ETF2Dpanel.cursorpos[1].value)
			{
				memcpy(&ETF2Dpanel.selection[0],&ETF2Dpanel.cursorpos[0],sizeof(struct _position));
				memcpy(&ETF2Dpanel.selection[2],&ETF2Dpanel.cursorpos[1],sizeof(struct _position));
				a=1;b=3;c=2;d=0;

			}else
			{
				memcpy(&ETF2Dpanel.selection[1],&ETF2Dpanel.cursorpos[0],sizeof(struct _position));
				memcpy(&ETF2Dpanel.selection[3],&ETF2Dpanel.cursorpos[1],sizeof(struct _position));
				a=0;b=2;c=3;d=1;
			}
		}else
		{
			if(ETF2Dpanel.cursorpos[0].value<ETF2Dpanel.cursorpos[1].value)
			{
				memcpy(&ETF2Dpanel.selection[3],&ETF2Dpanel.cursorpos[0],sizeof(struct _position));
				memcpy(&ETF2Dpanel.selection[1],&ETF2Dpanel.cursorpos[1],sizeof(struct _position));
				a=0;b=2;c=3;d=1;
			}else
			{
				memcpy(&ETF2Dpanel.selection[2],&ETF2Dpanel.cursorpos[0],sizeof(struct _position));
				memcpy(&ETF2Dpanel.selection[0],&ETF2Dpanel.cursorpos[1],sizeof(struct _position));
				a=1;b=3;c=2;d=0;
			}

		}

		ETF2Dpanel.selection[a].x=ETF2Dpanel.selection[c].x;
		ETF2Dpanel.selection[a].value=ETF2Dpanel.selection[c].value;
		ETF2Dpanel.selection[a].y=ETF2Dpanel.selection[d].y;
		ETF2Dpanel.selection[a].gradient=ETF2Dpanel.selection[d].gradient;

		ETF2Dpanel.selection[b].x=ETF2Dpanel.selection[d].x;
		ETF2Dpanel.selection[b].value=ETF2Dpanel.selection[d].value;
		ETF2Dpanel.selection[b].y=ETF2Dpanel.selection[c].y;
		ETF2Dpanel.selection[b].gradient=ETF2Dpanel.selection[c].gradient;

	}

	if(ETF2Dpanel.selection_status==TF2D_MODE_POINT_MOVING)
	{
		memcpy(&ETF2Dpanel.selection[ETF2Dpanel.selected],&ETF2Dpanel.cursorpos[0],sizeof(struct _position));
		ETF2Dpanel.selection_status=TF2D_MODE_BOX_SELECTED;
	}

	if(ETF2Dpanel.selection_status==TF2D_MODE_COLORMAP_SELECTING)
	{
		ETF2Dpanel.select_for_color[2]=ETF2Dpanel.cursorpos[0].value;
		ETF2Dpanel.select_for_color[3]=ETF2Dpanel.cursorpos[0].x;

		gtk_spin_button_set_value(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[1]),ETF2Dpanel.select_for_color[2]);

		ETF2Dpanel.selection_status=TF2D_MODE_COLORMAP_SELECTED;
	}

	ETF2Dpanel_hdisplay_handler(0,0,0);

	return FALSE;
}


void	ETF2D_fill_opacitymap(void (*fill_func)(int,int,float,float))
{
	float	m_left,m_right;
	float	nx,ny;
	int	sx,sy,ex,ey;
	int	x,y;

	m_left		=(ETF2Dpanel.selection[3].value-ETF2Dpanel.selection[0].value)/(ETF2Dpanel.selection[3].gradient-ETF2Dpanel.selection[0].gradient);
	m_right	=(ETF2Dpanel.selection[2].value-ETF2Dpanel.selection[1].value)/(ETF2Dpanel.selection[2].gradient-ETF2Dpanel.selection[1].gradient);

	sy=ETF2Dpanel.selection[0].gradient;
	ey=ETF2Dpanel.selection[3].gradient;

	for(y=sy;y<=ey;++y)
	{
		sx=m_left*(y-ETF2Dpanel.selection[3].gradient)+ETF2Dpanel.selection[3].value;
		ex=m_right*(y-ETF2Dpanel.selection[2].gradient)+ETF2Dpanel.selection[2].value;

		ny=(float)(y-sy)/(ey-sy);

		for(x=sx;x<=ex;++x)
		{
			nx=(float)(x-sx)/(ex-sx);
			fill_func(x,y,nx,ny);
		}
	}
}

void	ETF2Dpanel_toggle_switch(GtkWidget* widget,gpointer user_data)
{
	ETF2Dpanel_hdisplay_handler(0,0,0);
}

void	ETF2Dpanel_ResetClicked_handler(GtkWidget* widget,gpointer user_data)
{
	if(!gtftable.data) return;

	ETF2Dpanel.selection_status=0;
	memset(ETF2Dpanel.opacitymap,0,gtftable.level*(int)(gtftable.data->gmax+1));
	ETF2Dpanel_UpdateTexture();

}

void	ETF2Dpanel_FillClicked_handler(GtkWidget* widget,gpointer user_data)
{
	if(!gtftable.data) return;

	ETF2D_fill_opacitymap(fill);
	ETF2Dpanel_UpdateTexture();
}

void	ETF2Dpanel_AssignColormap(GtkWidget *widget,gpointer user_data)
{
	int	i,l,tmp;
	hCMAP*	cmap=NULL;
	hCMAP*	c=NULL;
	char*	filename;


	if(ETF2Dpanel.selection_status!=TF2D_MODE_COLORMAP_SELECTED) return;


	i=gtk_combo_box_get_active(GTK_COMBO_BOX(ETF2Dpanel.colormap_selector));

	if(ETF2Dpanel.select_for_color[0]>ETF2Dpanel.select_for_color[2])
	{
		tmp=ETF2Dpanel.select_for_color[0];
		ETF2Dpanel.select_for_color[0]=ETF2Dpanel.select_for_color[2];
		ETF2Dpanel.select_for_color[2]=tmp;

		tmp=ETF2Dpanel.select_for_color[3];
		ETF2Dpanel.select_for_color[3]=ETF2Dpanel.select_for_color[1];
		ETF2Dpanel.select_for_color[1]=tmp;

	}

	l=ETF2Dpanel.select_for_color[2]-ETF2Dpanel.select_for_color[0]+1;

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

	cmapPut(ETF2Dpanel.colormap,cmap,l,ETF2Dpanel.select_for_color[0]);
	cmapRelease(cmap);

	ETF2Dpanel_UpdateTexture();
	gtk_combo_box_set_active(GTK_COMBO_BOX(ETF2Dpanel.colormap_selector),-1);
}

void	ETF2Dpanel_Interpolate(GtkWidget *widget,gpointer user_data)
{
	int		tmp;
	int		x,l,n;
	GdkColor	color[2];
	int		ca[4],cb[4];

	if(ETF2Dpanel.selection_status!=TF2D_MODE_COLORMAP_SELECTED) return;

	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF2Dpanel.color[0]),&color[0]);
	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF2Dpanel.color[1]),&color[1]);

	ca[0]=(int)color[0].red;
	ca[1]=(int)color[0].green;
	ca[2]=(int)color[0].blue;

	cb[0]=(int)color[1].red;
	cb[1]=(int)color[1].green;
	cb[2]=(int)color[1].blue;

	if(ETF2Dpanel.select_for_color[0]>ETF2Dpanel.select_for_color[2])
	{
		tmp=ETF2Dpanel.select_for_color[0];
		ETF2Dpanel.select_for_color[0]=ETF2Dpanel.select_for_color[2];
		ETF2Dpanel.select_for_color[2]=tmp;

		tmp=ETF2Dpanel.select_for_color[3];
		ETF2Dpanel.select_for_color[3]=ETF2Dpanel.select_for_color[1];
		ETF2Dpanel.select_for_color[1]=tmp;

	}


	l=ETF2Dpanel.select_for_color[2]-ETF2Dpanel.select_for_color[0]+1;
	n=ETF2Dpanel.select_for_color[0]-gtftable.data->vmin;


	for(x=ETF2Dpanel.select_for_color[0]-gtftable.data->vmin;x<=ETF2Dpanel.select_for_color[2]-gtftable.data->vmin;++x)
	{
		ETF2Dpanel.colormap->colormap[x*4]=(ca[0]+(x-n)*(cb[0]-ca[0])/l)*255/65535;
		ETF2Dpanel.colormap->colormap[x*4+1]=(ca[1]+(x-n)*(cb[1]-ca[1])/l)*255/65535;
		ETF2Dpanel.colormap->colormap[x*4+2]=(ca[2]+(x-n)*(cb[2]-ca[2])/l)*255/65535;
	}

	ETF2Dpanel_UpdateTexture();

}

void	ETF2Dpanel_colorAChange_handler(GtkWidget *widget,gpointer user_data)
{
	GdkColor	color;

	gtk_color_button_get_color(GTK_COLOR_BUTTON(ETF2Dpanel.color[0]),&color);
	gtk_color_button_set_color(GTK_COLOR_BUTTON(ETF2Dpanel.color[1]),&color);
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON(ETF2Dpanel.color[1]),gtk_color_button_get_alpha(GTK_COLOR_BUTTON(ETF2Dpanel.color[0])));

}

void	ETF2Dpanel_ValueChange_handler(GtkWidget *widget,gpointer user_data)
{
	ETF2Dpanel.select_for_color[0]=gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[0]));
	ETF2Dpanel.select_for_color[2]=gtk_spin_button_get_value(GTK_SPIN_BUTTON(ETF2Dpanel.select_value[1]));
}
