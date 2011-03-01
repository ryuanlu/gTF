//	colormap.c
//
//	Colormap processing functions
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "colormap.h"


//	HSVtoRGB
//	------------------------------------------------------------------------
//	Description	:	Convert color from HSV space to RGB.
//	Parameters	:
//
//		rgb		output RGB value in R8G8B8
//		H,S,V		input HSV value
//
//	Returns	:	nothing
//

void	HSVtoRGB(unsigned char* rgb,const int H,const int S,const int V)
{
	int i;
	float f, p, q, t,h,s,v;

	if(S==0) {
		rgb[0]=V*255/100;
		rgb[1]=V*255/100;
		rgb[2]=V*255/100;
		return;
	}

	h=(float)H/60;			// sector 0 to 5
	s=(float)S/100;
	v=(float)V/100;
	i=floor(h);
	f=h-i;			// factorial part of h
	p=v*(1-s);
	q=v*(1-s*f);
	t=v*(1-s*(1-f));

	switch(i) {
			case 0:
				rgb[0]=v*255;
				rgb[1]=t*255;
				rgb[2]=p*255;
				break;
			case 1:
				rgb[0]=q*255;
				rgb[1]=v*255;
				rgb[2]=p*255;
				break;
			case 2:
				rgb[0]=p*255;
				rgb[1]=v*255;
				rgb[2]=t*255;
				break;
			case 3:
				rgb[0]=p*255;
				rgb[1]= q*255;
				rgb[2]= v*255;
				break;
			case 4:
				rgb[0]=t*255;
				rgb[1]=p*255;
				rgb[2]=v*255;
				break;
			default:		// case 5:
				rgb[0]=v*255;
				rgb[1]=p*255;
				rgb[2]=q*255;
				break;
		}
}


//	RGBtoHSV
//	------------------------------------------------------------------------
//	Description	:	Convert color from RGB space to HSV.
//	Parameters	:
//
//		hsv		output HSV value
//		H,S,V		input RGB value
//
//	Returns	:	nothing
//

void	RGBtoHSV(int* hsv,const unsigned char R,const unsigned char G,const unsigned char B)
{
	float min, max, delta,h;
        float rgb[3];
        int i;
        rgb[0]=(float)R/255;
        rgb[1]=(float)G/255;
        rgb[2]=(float)B/255;

	min = 10.0f;
	max = 0.0f;

	for(i=0;i<3;++i)
        {
        	if(rgb[i]>max) max=rgb[i];
                if(rgb[i]<min) min=rgb[i];
        }


	hsv[2]=max*100;				// v

	delta = max - min;

	if( max != 0 )
		hsv[1]=delta*100/max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		hsv[1] = 0;
		hsv[0] = 0;
		return;
	}

	if( rgb[0] == max )
		h = ( rgb[1] - rgb[2] ) / delta;		// between yellow & magenta
	else if( rgb[1] == max )
		h = 2 + ( rgb[2] - rgb[0] ) / delta;	// between cyan & yellow
	else
		h = 4 + ( rgb[0] - rgb[1] ) / delta;	// between magenta & cyan

	h *= 60;				// degrees
	if( h < 0 )
		h += 360;
        hsv[0]=h;

}


//	cmapGetPtr
//	------------------------------------------------------------------------
//	Description	:	Get the pointer to colormap data.
//	Parameters	:
//
//		hcmap		colormap handle
//
//	Returns	:	the pointer to colormap data
//

unsigned char*	cmapGetPtr(const hCMAP* hcmap)
{
	if(hcmap)
		return hcmap->colormap;
	else return NULL;
}


//	cmapNewFromFile
//	------------------------------------------------------------------------
//	Description	:	Create a new colormap handle and load colormap from
//				a file.
//	Parameters	:
//
//		filename	colormap file
//
//	Returns	:	pointer to new colormap handle
//

hCMAP*	cmapNewFromFile(const char* filename)
{
	hCMAP	*hcmap=NULL;
	FILE	*fp=NULL;
	int	size=0;
	fp=fopen(filename,"rb");

	if(!fp)	return NULL;
	hcmap=malloc(sizeof(hCMAP));
	memset(hcmap,0,sizeof(hCMAP));
	fread(&hcmap->cmap_header,sizeof(strCMAPheader),1,fp);
	size=4*hcmap->cmap_header.level;
	hcmap->colormap=malloc(size);
	fread(hcmap->colormap,size,1,fp);
	fclose(fp);
	return hcmap;
}


//	cmapRelease
//	------------------------------------------------------------------------
//	Description	:	Release resource of colormap handle.
//	Parameters	:
//
//		hcmap		colormap handle
//
//	Returns	:	nothing
//

void	cmapRelease(hCMAP* hcmap)
{
	if(!hcmap)	return;
	if(hcmap->colormap)	free(hcmap->colormap);
	free(hcmap);
}


//	cmapWriteToFile
//	------------------------------------------------------------------------
//	Description	:	Write colormap to a file.
//	Parameters	:
//
//		filename	output colormap file
//		hcmap		colormap handle
//
//	Returns	:	nothing
//

void	cmapWriteToFile(const char* filename,hCMAP* hcmap)
{
	FILE	*fp=NULL;
	if(!filename||!hcmap||!hcmap->colormap) return;
	fp=fopen(filename,"wb");
	if(!fp) return;
	fwrite(&hcmap->cmap_header,sizeof(strCMAPheader),1,fp);
	fwrite(hcmap->colormap,4*hcmap->cmap_header.level,1,fp);
	fclose(fp);
}


//	cmapNewRemap
//	------------------------------------------------------------------------
//	Description	:	Remap an existing colormap to another dimension as
//				a new colormap.
//	Parameters	:
//
//		i_hcmap	input colormap handle
//		level		dimension of new colormap
//		begin		start location to write the new colormap
//		len		dimension to remap
//		ext		boolean constant that controls whether filling space
//				with side color
//
//	Returns	:	pointer to new colormap
//

hCMAP*	cmapNewRemap(hCMAP* i_hcmap,const int level,const int begin,const int len,const int ext)
{
	hCMAP*	o_hcmap;
	int	i,j,length;

	if(!len) length=level-begin; else length=len;
	if(!i_hcmap||!i_hcmap->colormap) return NULL;

	o_hcmap=malloc(sizeof(hCMAP));
	memset(o_hcmap,0,sizeof(hCMAP));

	o_hcmap->cmap_header.level=level;

	o_hcmap->colormap=malloc(level*4);
	memset(o_hcmap->colormap,0,level*4);

	for(i=begin;i<begin+length;++i)
	{
		for(j=0;j<4;++j)
		{
			o_hcmap->colormap[i*4+j]=i_hcmap->colormap[(int)((i-begin)*(i_hcmap->cmap_header.level)/(length))*4+j];
		}
	}

	switch(ext)
	{
		case COLORMAP_SIDE_EXTEND:
			for(i=0;i<begin;++i)
			{
				for(j=0;j<4;++j)
				{
					o_hcmap->colormap[i*4+j]=o_hcmap->colormap[begin*4+j];
				}
			}
			for(i=begin+length;i<level;++i)
			{
				for(j=0;j<4;++j)
				{
					o_hcmap->colormap[i*4+j]=o_hcmap->colormap[(begin+length-1)*4+j];
				}
			}
		break;
	}
	return o_hcmap;
}


//	Remap function

void	cmapRemap(hCMAP* hcmap,const int level,const int begin,const int length,const int ext)
{
	hCMAP*	tmp;
	tmp=cmapNewRemap(hcmap,level,begin,length,ext);
	if(!tmp) return;
	hcmap->cmap_header.level=level;
	hcmap->colormap=tmp->colormap;
	free(tmp);
}

//	Generate an N level blank colormap

hCMAP*	cmapNewBlank(const int level)
{
	hCMAP*	o_hcmap;

	if(level<=0) return NULL;

	o_hcmap=malloc(sizeof(hCMAP));
	memset(o_hcmap,0,sizeof(hCMAP));

	o_hcmap->cmap_header.level=level;

	o_hcmap->colormap=malloc(level*4);
	memset(o_hcmap->colormap,0,level*4);
	return o_hcmap;
}

//	Generate an N level Gray Colormap

hCMAP*	cmapNewGray(const int level)
{
	hCMAP*	o_hcmap;
	int	i,j;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);

	for(i=0;i<level;++i)
	{
		for(j=0;j<3;++j)
		{
			o_hcmap->colormap[i*4+j]=(unsigned char)((float)255/(level-1)*i);
		}
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;
}

//	Generate an N level HSV Colormap

hCMAP*	cmapNewHSV(const int level)
{
	hCMAP*	o_hcmap;
	int	i;
	//float	s,p;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	/*
	for(i=0,p=0,s=0;i<level;i++,p+=s)
	{
		if(p>255) p=255;
		if(p<0) p=0;
		o_hcmap->colormap[((int)(i+(float)level/2)%level)*4]=p;
		o_hcmap->colormap[((int)(i+(float)level*5/6)%level)*4+1]=p;
		o_hcmap->colormap[((int)(i+(float)level/6)%level)*4+2]=p;
		o_hcmap->colormap[i*4+3]=255;
		if(i>level/6&&i<=level/3)
			s=(float)255*6/(level-1);
		else if(i>level*2/3&&i<=level*5/6)
			s=(float)-255*6/(level-1);
		else s=0;
	}
	*/

	for(i=0;i<level;++i)
	{
		//HSVtoRGB(&o_hcmap->colormap[i*4],(int)(352*i/(level-1)),100,100);
		HSVtoRGB(&o_hcmap->colormap[i*4],(int)(352*i/(level)),100,100);
		o_hcmap->colormap[i*4+3]=255;
	}

	return o_hcmap;
}

//	Generate an N level Hot Colormap

hCMAP*	cmapNewHot(const int level)
{
	hCMAP*	o_hcmap;
	int	i;
	float	s,p;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);

	for(i=0,p=0,s=0;i<level;++i,p+=s)
	{
		if(p>255) p=255;
		if(p<0) p=0;
		o_hcmap->colormap[i*4]=p;
		if((i+level/3)<level)
			o_hcmap->colormap[(i+level/3)*4+1]=p;
		if((i+level*2/3)<level)
			o_hcmap->colormap[(i+level*2/3)*4+2]=p;
		o_hcmap->colormap[i*4+3]=255;

		if(i<(int)level/3)
			s=(float)255*3/(level-1);
		else
			s=0;
	}
	return o_hcmap;
}

//	Generate an N level Cool Colormap

hCMAP*	cmapNewCool(const int level)
{
	hCMAP*	o_hcmap;
	int	i;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	for(i=0;i<level;++i)
	{
		o_hcmap->colormap[i*4+0]=(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+1]=255-(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+2]=255;
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;
}

//	Generate an N level Spring Colormap

hCMAP*	cmapNewSpring(const int level)
{
	hCMAP*	o_hcmap;
	int	i;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	for(i=0;i<level;++i)
	{
		o_hcmap->colormap[i*4+0]=255;
		o_hcmap->colormap[i*4+1]=(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+2]=255-(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;

}

//	Generate an N level Summer Colormap

hCMAP* cmapNewSummer(const int level)
{
	hCMAP*	o_hcmap;
	int	i;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	for(i=0;i<level;++i)
	{
		o_hcmap->colormap[i*4+0]=(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+1]=128+(unsigned char)((float)127/(level-1)*i);
		o_hcmap->colormap[i*4+2]=102;
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;

}

//	Generate an N level Autumn Colormap

hCMAP*	cmapNewAutumn(const int level)
{
	hCMAP*	o_hcmap;
	int	i;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	for(i=0;i<level;++i)
	{
		o_hcmap->colormap[i*4+0]=255;
		o_hcmap->colormap[i*4+1]=(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+2]=0;
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;

}

//	Generate an N level Winter Colormap

hCMAP*	cmapNewWinter(const int level)
{
	hCMAP*	o_hcmap;
	int	i;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	for(i=0;i<level;++i)
	{
		o_hcmap->colormap[i*4+0]=0;
		o_hcmap->colormap[i*4+1]=(unsigned char)((float)255/(level-1)*i);
		o_hcmap->colormap[i*4+2]=255-(unsigned char)((float)127/(level-1)*i);
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;

}

//	Generate an N level Copper Colormap

hCMAP*	cmapNewCopper(const int level)
{
	hCMAP*	o_hcmap;
	int	i;
	float	s,p;

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);
	for(i=0,s=0,p=0;i<level;++i,p+=s)
	{
		if(p>255) p=255;
		o_hcmap->colormap[i*4+0]=p;
		o_hcmap->colormap[i*4+1]=(unsigned char)((float)199/(level-1)*i);
		o_hcmap->colormap[i*4+2]=(unsigned char)((float)127/(level-1)*i);
		o_hcmap->colormap[i*4+3]=255;
		if(i<=level*5/6)
			s=(float)255*6/(level-1)/5;
		else
			s=0;
	}
	return o_hcmap;

}

//	Generate an N level Lines Colormap

hCMAP*	cmapNewLines(const int level)
{
	hCMAP*	o_hcmap;
	int	i,j;
	unsigned char	map[21]={0,0,255,0,128,0,255,0,0,0,191,191,191,0,191,191,191,0,64,64,64};

	if(level<=0) return NULL;

	o_hcmap=cmapNewBlank(level);

	for(i=0;i<level;++i)
	{
		j=i%7;
		o_hcmap->colormap[i*4]=map[j*3];
		o_hcmap->colormap[i*4+1]=map[j*3+1];
		o_hcmap->colormap[i*4+2]=map[j*3+2];
		o_hcmap->colormap[i*4+3]=255;
	}
	return o_hcmap;

}

//	Draw a colormap to another one

void	cmapPut(hCMAP* o_hcmap,hCMAP* i_hcmap,const int level,const int begin)
{
	hCMAP*  tmp;
	tmp=cmapNewRemap(i_hcmap,level,0,0,0);
	memcpy(o_hcmap->colormap+begin*4,tmp->colormap,4*level);
	cmapRelease(tmp);
}

//	Export colormap

void    cmapExport(const char* filename,hCMAP* hcmap)
{
	FILE* fp;
	float* color;
	int i,s;
	if(!hcmap) return;
	fp=fopen(filename,"wb");
	if(!fp) return;

	color=malloc(4096);
	memset(color,0,4096);
	for(i=0;i<256;++i)
	{
		s=(hcmap->cmap_header.level-1)*i/255;
		color[i*4+0]=(float)hcmap->colormap[s*4+0]/255.0f;
		color[i*4+1]=(float)hcmap->colormap[s*4+1]/255.0f;
		color[i*4+2]=(float)hcmap->colormap[s*4+2]/255.0f;
		color[i*4+3]=(float)hcmap->colormap[s*4+3]/255.0f;
	}
	fwrite(color,4096,1,fp);
	fclose(fp);
	free(color);
}

void    cmapExportEx(const char* filename,hCMAP* hcmap)
{
	FILE* fp;
	int i;
	if(!hcmap) return;
	fp=fopen(filename,"w");
	if(!fp) return;

	fprintf(fp,"%d\n",hcmap->cmap_header.level);
	for(i=0;i<hcmap->cmap_header.level;++i)
	{
		fprintf(fp,"%f %f %f %f\n",hcmap->colormap[i*4+0]/255.0f,hcmap->colormap[i*4+1]/255.0f,hcmap->colormap[i*4+2]/255.0f,hcmap->colormap[i*4+3]/255.0f);
	}
	fclose(fp);

}
