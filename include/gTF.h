#ifndef GTF_H_
#define GTF_H_

#include	<libintl.h>
#include	<gtk/gtkgl.h>
#include	"data.h"
#include	"colormap.h"


//	Version string

#include	"../version"


//	Macro for i18n(gettext)

#define	_(String)	gettext(String)


//	Number of transfer functions

#define	NUMBER_OF_TF	(3)


typedef struct
{
	DATA    	*data;		//	Volume data handle
	unsigned short	*rawdata;	//	Pointer to RAW data, same as data->rawdata

	int	hmax,hmin;
	//int	gmax,gmin;
	int	level;		//	Function value MAX-MIN+1

	GdkGLContext*	sharedrc;

	unsigned int	volume_tex;
	unsigned int	gradient_tex;

	struct
	{
		unsigned int		aa2Dslice;
		unsigned int		aa3Dtex;
		unsigned int		va3Dtex;

	}shader[NUMBER_OF_TF];


	int	is_tf_ready;
	int	current_tf;
	int	previous_tf;


	void	(*TF_init[NUMBER_OF_TF])(void);
	void	(*TF_release[NUMBER_OF_TF])(void);

	void	(*AA2Dslice_init[NUMBER_OF_TF])(void);
	void	(*AA3Dtex_init[NUMBER_OF_TF])(void);
	void	(*VA3Dtex_init[NUMBER_OF_TF])(void);

	void	(*AA2Dslice_release[NUMBER_OF_TF])(void);
	void	(*AA3Dtex_release[NUMBER_OF_TF])(void);
	void	(*VA3Dtex_release[NUMBER_OF_TF])(void);


}gTFTable;

extern gTFTable		gtftable;
extern const char	gTF_version[];

#endif /*GTF_H_*/
