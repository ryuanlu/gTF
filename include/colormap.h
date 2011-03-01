//	colormap.h
//
//	Colormap processing functions
//

#ifndef COLORMAP_H_
#define COLORMAP_H_

#define	COLORMAP_SIDE_EXTEND	0x00000001


//	.cmap file header declarartion


typedef struct
{
	int	level;			//	colormap dimension
}strCMAPheader;

//	CMAP handle structure declaration

typedef struct
{
	strCMAPheader	cmap_header;	//	cmap header
	unsigned char	*colormap;	//	pointer to colormap
}hCMAP;


void	HSVtoRGB(unsigned char*,const int,const int,const int);
void	RGBtoHSV(int*,const unsigned char,const unsigned char,const unsigned char);

unsigned char*	cmapGetPtr(const hCMAP*);
hCMAP*	cmapNewFromFile(const char*);
hCMAP*	cmapNewRemap(hCMAP*,const int,const int,const int,const int);
void	cmapRemap(hCMAP*,const int,const int,const int,const int);
void	cmapPut(hCMAP*,hCMAP*,const int,const int);
void	cmapGet(hCMAP*,const int,unsigned char*);


//	Create new colormap with built-in pattern

hCMAP*	cmapNewBlank(const int);
hCMAP*	cmapNewGray(const int);
hCMAP*	cmapNewHSV(const int);
hCMAP*	cmapNewHot(const int);
hCMAP*	cmapNewCool(const int);
hCMAP*	cmapNewSpring(const int);
hCMAP*	cmapNewSummer(const int);
hCMAP*	cmapNewAutumn(const int);
hCMAP*	cmapNewWinter(const int);
hCMAP*	cmapNewCopper(const int);
hCMAP*	cmapNewLines(const int);

//

void	cmapWriteToFile(const char*,hCMAP*);
void	cmapRelease(hCMAP*);
void	cmapExport(const char*,hCMAP*);
void    cmapExportEx(const char* filename,hCMAP* hcmap);

#endif /*COLORMAP_H_*/
