#include	<stdlib.h>
#include	"tfaux.h"



//	Generate a 3D texture with the specified colormap for volume rendering and slicing view

unsigned char*	texGen(DATA* data,hCMAP* hcmap)
{
	unsigned char *ptr=NULL;
	if(!data||!hcmap) return NULL;
	ptr=malloc(data->totalsize*4);
	texApplyColormap(ptr,data,hcmap);
	return ptr;
}

//      Get texture value

unsigned char	texValue(unsigned char* tex,DATA* data,int x,int y,int z,int c)
{
	return tex[data->resol[0]*data->resol[1]*z*4+data->resol[0]*y*4+x*4+c];
}

//	Apply colormap to an exist 3D texture

void	texApplyColormap(unsigned char* tex,DATA* data,hCMAP* hcmap)
{
	int	i,j;
	if(!tex||!data||!hcmap) return;

	for(i=0;i<data->totalsize;++i)
	{
		j=(data->rawdata[i]-data->vmin)*(hcmap->cmap_header.level-1)/(data->vmax-data->vmin);
		tex[i*4]=hcmap->colormap[j*4];
		tex[i*4+1]=hcmap->colormap[j*4+1];
		tex[i*4+2]=hcmap->colormap[j*4+2];
		tex[i*4+3]=hcmap->colormap[j*4+3];
	}

}


//	Apply opacity property to an exist 3D texture

void	texSetOpacity(unsigned char* tex,DATA* data,hCMAP* hcmap,int start,int length,int opacity)
{
	int	i,j;
	if(!tex||!data||start<0) return;


	for(i=0;i<data->totalsize;++i)
	{
		j=(data->rawdata[i]-data->vmin)*(hcmap->cmap_header.level-1)/(data->vmax-data->vmin);
		if(j>=start&&j<start+length)
			tex[i*4+3]=opacity;
	}

}

//	Apply gradient to the opacity of the 3D texture

//void	texApplyGradient(unsigned char* tex,DATA* data,int n,int m,int t,int p)
//{
//	int i,o,j;
//        if(t<data->gmin||n>m||n<0||m>255) return;
//        if(p<1||p>3) return;
//	for(i=0;i<data->totalsize;++i)
//	{
//		if(data->gradient_magnitude[i]>=t)o=(data->gradient_magnitude[i]-t)*(m-n)/(data->gmax-t)+n;
//                else o=0;
//                for(j=0;j<p;++j)
//		tex[i*4+3]=tex[i*4+3]*o/255;
//	}
//}

//	Color interpolation

void	colorInterpolate(int* data,const int c,const int start,const int end,const int va,const int vb)
{
	int i,l;
        l=end-start;
        if(!data||c>2) return;
        for(i=start;i<=end;++i)
        {
        	data[i*3+c]=va+(vb-va)*(i-start)/l;
        }
}

//	Extract RGB data from colormap

void	getRGBfromColormap(hCMAP* hcmap,int level,unsigned char* output)
{
	int	i,l;
	if(!output||!hcmap||level<1)	return;

	l=hcmap->cmap_header.level;
	for(i=0;i<level;++i)
	{
		output[i*3]=hcmap->colormap[(i*l/level)*4];
		output[i*3+1]=hcmap->colormap[(i*l/level)*4+1];
		output[i*3+2]=hcmap->colormap[(i*l/level)*4+2];
	}
}

//	Extract Alpha channel from colormap

void	getALPHAfromColormap(hCMAP* hcmap,int level,unsigned char* output)
{
	int	i,l;
	if(!output||!hcmap||level<1)	return;

	l=hcmap->cmap_header.level;
	for(i=0;i<level;++i)
	{
		output[i]=hcmap->colormap[(i*l/level)*4+3];
	}
}

//	Apply gradient to the opacity of the 3D texture

void	texGenGradientStencil(unsigned char* tex,DATA* data,int n,int m,int threshold,int power)
{
	int i,o,j;
        if(threshold<data->gmin||n>m||n<0||m>255) return;
        if(power<1||power>3) return;
	for(i=0;i<data->totalsize;++i)
	{
		tex[i]=255;
		//if(data->gradient_magnitude[i]>=threshold)o=(data->gradient_magnitude[i]-threshold)*(m-n)/(data->gmax-threshold)+n;
		if(data->gradient[i*4+3]>=threshold)o=(data->gradient[i*4+3]-threshold)*(m-n)/(data->gmax-threshold)+n;
                else o=0;
                for(j=0;j<power;++j)
		tex[i]=tex[i]*o/255;
	}
}


