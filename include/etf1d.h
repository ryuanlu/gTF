#ifndef TF1D_H_
#define TF1D_H_

#include <gtk/gtk.h>
#include "gtk_glcanvas.h"
#include "va3Dtexwin.h"

#define	TF1D_TEXTURE_WIDTH		(4096)


typedef struct
{
	int	volumeloc;		//	Location of uniform variable(for volume) in shader
	int	tf1dloc;		//	Location of uniform variable(for colormap) in shader
	int	stencilloc;		//	Location of uniform variable(for gradient stencil) in shader
	int	coefloc;		//	Location of uniform variable(for correction coef) in shader
	int	shiftloc;		//	Location of uniform variable(for correction coef) in shader
	int	deltaloc;
	int	normalloc;
	int	levoy_swloc;
	int	levoy_valueloc;
	int	levoy_thicknessloc;

}ETF1D_3Dtexloc;


typedef struct
{
	//	GTK widgets

	int	id;

	int	enabled;


	GtkWidget*	hcodisplay;
	GtkGLCanvas	glcanvas;
	GtkWidget*	opendialog;

	GtkWidget*	cmapselect;
	GtkWidget*	assign;

	GtkWidget*	colorbox[2];
	GtkWidget*	checkbox[2];
	GtkWidget*	select_value[2];

	GtkWidget*	status;
	GtkWidget*	vbox;

	GtkWidget*	vmax;
	GtkWidget*	vmin;
	GtkWidget*	hmax;
	GtkWidget*	gmax;

	GtkWidget*	levoy_sw;
	GtkWidget*	levoy_value;
	GtkWidget*	levoy_thickness;


        int*	histogram;
        int*	histogram_origin;

        int*	histogram_gaussian;
	int*	histogram_quantization;
        int*	histogram_leftdiff;
        int*	histogram_rightdiff;

        int	SelectFrom;
	int	SelectTo;

	unsigned int	ctex,otex;	//	texture objects

	unsigned char*	opacitytex;	//	opacity texture for display
	unsigned char*	cmaptex;	//	colormap texture for display


	hCMAP*	cmap;		//	source colormap

	int	cursorpos;	//	Current cursor position
	int	selA;		//	Selected value (after single clicked)
	int	selB;
	int	selF;
	int	sel;		//	Selection flag
	int	spinlock;
	int	FS;


	unsigned int	etf1d_tex;			//	Texture object of colormap


	ETF1D_3Dtexloc	AA3Dtexloc;
	ETF1D_3Dtexloc	AA2Dsliceloc;
	ETF1D_3Dtexloc	VA3Dtexloc;
	VA3Dvsloc	VA3Dtexvsloc;

}strETF1Dpanel;

extern	strETF1Dpanel	ETF1Dpanel;


void	tfCreateETF1Dpanel(int i);
void	tfSetupETF1Dpanel(void);

void	ETF1D_init(void);
void	ETF1D_release(void);

void	ETF1D_InitAA2Dslice(void);
void	ETF1D_InitAA3Dtex(void);
void	ETF1D_InitVA3Dtex(void);


G_MODULE_EXPORT	int	ETF1Dpanel_hcodisplay_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	ETF1Dpanel_hcodisplayMouseMove_handler(GtkWidget *widget,GdkEventMotion *event,gpointer user_data);
G_MODULE_EXPORT	int	ETF1Dpanel_hcodisplayMouseDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	ETF1Dpanel_hcodisplayMouseUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_colorAChange_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_ToggleGradient(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_ValueSelect(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_Interpolate(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_AssignColormap(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_SaveColormap(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_ToggleLevoy(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF1Dpanel_LevoyValue(GtkWidget *widget,gpointer user_data);



void	ETF1D_Update(void);
void	ETF1Dpanel_UpdateTexture(void);


#endif /* TF1D_H_ */
