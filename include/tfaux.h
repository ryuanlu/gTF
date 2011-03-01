#ifndef TFAUX_H_
#define TFAUX_H_

#include "data.h"
#include "colormap.h"


unsigned char*	texGen(DATA*,hCMAP*);
void	texApplyColormap(unsigned char*,DATA*,hCMAP*);
void	texSetOpacity(unsigned char*,DATA*,hCMAP*,int,int,int);
unsigned char	texValue(unsigned char*,DATA*,int,int,int,int);
//void	texApplyGradient(unsigned char*,DATA*,int,int,int,int);
void	colorInterpolate(int*,const int,const int,const int,const int,const int);
void	getRGBfromColormap(hCMAP* hcmap,int level,unsigned char* output);
void	getALPHAfromColormap(hCMAP* hcmap,int level,unsigned char* output);
void	texGenGradientStencil(unsigned char* tex,DATA* data,int n,int m,int threshold,int power);




#endif /*TFAUX_H_*/
