#ifndef ETF2D_H_
#define ETF2D_H_

#include	<gtk/gtk.h>
#include	<gtk/gtkgl.h>
#include	"va3Dtexwin.h"

#define	TF2D_TEXTURE_WIDTH		(2048)
#define	TF2D_TEXTURE_HEIGHT		(2048)

#define	TF2D_DISTANCE_TO_GRAB_POINT	(10.0)

#define	TF2D_MODE_BOX_DRAWING	(1)
#define	TF2D_MODE_BOX_SELECTED	(2)
#define	TF2D_MODE_POINT_MOVING	(3)

#define	TF2D_MODE_COLORMAP_SELECTING	(4)
#define	TF2D_MODE_COLORMAP_SELECTED	(5)

typedef struct
{
	int	volumeloc;		//	Location of uniform variable(for volume) in shader
	int	colormaploc;		//	Location of uniform variable(for colormap) in shader
	int	opacityloc;
	int	stencilloc;		//	Location of uniform variable(for gradient stencil) in shader
	int	coefloc;		//	Location of uniform variable(for correction coef) in shader
	int	shiftloc;		//	Location of uniform variable(for correction coef) in shader
	int	deltaloc;
	int	normalloc;
	int	gmaxloc;

}ETF2D_3Dtexloc;



typedef struct
{
	int	id;

	GtkWidget*	hdisplay;
	GtkWidget*	vbox;

	GtkWidget*	vmax;
	GtkWidget*	vmin;
	GtkWidget*	gmax;
	GtkWidget*	gmin;
	GtkWidget*	status;

	GtkWidget*	histogram_selector;
	GtkWidget*	histogram_switch;
	GtkWidget*	opacity_switch;

	GtkWidget*	colormap_selector;

	GtkWidget*	mode_selector;
	GtkWidget*	opacity_h;
	GtkWidget*	opacity_v;
	GtkWidget*	sigma;
	GtkWidget*	exp;

	GtkWidget*	select_value[2];

	int		select_for_color[4];
	GtkWidget*	color[2];

	struct _position
	{
		int	x;
		int	y;
		int	value;
		float	gradient;

	}cursorpos[2],selection[4];

	int	selection_status;
	int	selected;


	unsigned int*	histogram2D;
	unsigned char*	histogram2Dtex;
	unsigned int	histogram_texobj;

	unsigned char*	opacitymap;
	unsigned char*	opacitytex;
	unsigned int	opacity_texobj;

	hCMAP*		colormap;
	unsigned char*	colormaptex;
	unsigned int	colormap_texobj;

	//	OpenGL involved

	GdkGLContext*	glrc;
	GdkGLDrawable*	gldrawable;


	ETF2D_3Dtexloc	AA3Dtexloc;
	ETF2D_3Dtexloc	VA3Dtexloc;
	VA3Dvsloc	VA3Dtexvsloc;

}strETF2Dpanel;


extern	strETF2Dpanel	ETF2Dpanel;


void	tfCreateETF2Dpanel(int i);
void	tfSetupETF2Dpanel(void);

void	ETF2D_init(void);
void	ETF2D_release(void);

void	ETF2D_InitAA2Dslice(void);
void	ETF2D_InitAA3Dtex(void);
void	ETF2D_InitVA3Dtex(void);

G_MODULE_EXPORT	int	ETF2Dpanel_hdisplay_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	ETF2Dpanel_hdisplayMouseMove_handler(GtkWidget *widget,GdkEventMotion *event,gpointer user_data);
G_MODULE_EXPORT	int	ETF2Dpanel_hdisplayMouseDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	ETF2Dpanel_hdisplayMouseUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_ResetClicked_handler(GtkWidget* widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_toggle_switch(GtkWidget* widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_FillClicked_handler(GtkWidget* widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_AssignColormap(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_Interpolate(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_colorAChange_handler(GtkWidget *widget,gpointer user_data);
G_MODULE_EXPORT	void	ETF2Dpanel_ValueChange_handler(GtkWidget *widget,gpointer user_data);


void	ETF2Dpanel_UpdateTexture(void);
void	ETF2D_fill_opacitymap(void (*fill_func)(int,int,float,float));


#endif /* ETF2D_H_ */
