#ifndef VA3DTEXWIN_H_
#define VA3DTEXWIN_H_

#include	"glwin.h"


#define	VA3DTEXWIN_DEFAULT_DISTANCE		(300)

typedef struct
{
	int	sequenceloc;
	int	verticesloc;
	int	valoc;
	int	vbloc;
	int	frontindexloc;
	int	distanceloc;

}VA3Dvsloc;


typedef struct
{
	GLWin*			glwin;

	GtkWidget*		icon;
	GtkWidget*		menuitem[2];

	int			progobj;
	int			shader[2];
	int			displaylist;

	int			prev_pos[2];
	int			move;
	float			offset;

	float			density;

	VA3Dvsloc*		vsloc[NUMBER_OF_TF];

}strVA3DTexwin;

extern	strVA3DTexwin	VA3DTexwin;
extern	float		va_vertices[24];
extern	const int	va3Dtex_nsequence[];
extern	const int	va3Dtex_va[];
extern	const int	va3Dtex_vb[];


void	VA3DTex_GetLocation(VA3Dvsloc* loc,unsigned int shader);

void	tfCreateVA3DTexwin(void);
void	tfSetupVA3DTexwin(void);

G_MODULE_EXPORT	int	VA3DTexwin_glcanvas_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	VA3DTexwin_Close_handler(GtkWidget *widget, GdkEvent *event,gpointer user_data);
G_MODULE_EXPORT	int	VA3DTexwin_MouseMotion_handler(GtkWidget *widget, GdkEventMotion *event,gpointer user_data);
G_MODULE_EXPORT	int	VA3DTexwin_ButtonDown_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	VA3DTexwin_ButtonUp_handler(GtkWidget *widget,GdkEventButton *event,gpointer user_data);
G_MODULE_EXPORT	int	VA3DTexwin_MouseWheel_handler(GtkWidget *widget, GdkEventScroll *event,gpointer user_data);
//void	VA3DTexwin_generate_displaylist(void);
void	VA3DTexwin_Menu_handler(GtkWidget *widget,gpointer user_data);
void	VA3DTexwin_Release(void);



#endif /* VA3DTEXWIN_H_ */
