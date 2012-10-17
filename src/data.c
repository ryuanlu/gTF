//	Transfer Function
//

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <math.h>
#include "inf.l.h"
#include "inf.y.h"
#include "ikdtree.h"
#include "data.h"



//	dataOpen
//	------------------------------------------------------------------------
//	Description	:	Open a .inf volume.
//	Parameters	:
//
//		filename	filename of the volume
//
//	Returns	:	The pointer of new volume object.
//

DATA*	dataOpen(const char* filename)
{
	FILE*	fp;
	DATA*	handle;
	//static	YY_BUFFER_STATE	bstate=NULL;

	fp=fopen(filename,"r");
	//yyin=fp;
	//reset();
	yyrestart(fp);
	yyreset();
	//yy_flush_buffer(((yy_buffer_stack)?(yy_buffer_stack)[(yy_buffer_stack_top)]:NULL));
	//if(bstate)	yy_delete_buffer(bstate);
	//bstate=yy_create_buffer(yyin,YY_BUF_SIZE);
	//yy_switch_to_buffer(bstate);
	handle=malloc(sizeof(DATA));
	memset(handle,0,sizeof(DATA));
	dataptr=&handle;
	//yyparse();

	if(!yyparse())
	{
		fclose(fp);
		return	handle;
	}

	free(handle);
	return NULL;
}


//	dataClose
//	------------------------------------------------------------------------
//	Description	:	Close an opened volume and free all allocated resource.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataClose(DATA* data)
{
	if(data&&data->rawfile_ptr)
	{
		if(data->rawdata) free(data->rawdata);
		//if(data->gradient_magnitude) free(data->gradient_magnitude);
		if(data->gradient) free(data->gradient);
		fclose(data->rawfile_ptr);
		if(data->ikdtreefile_ptr) fclose(data->ikdtreefile_ptr);
		if(data->iKDtree) IKDtree_delete(data->iKDtree);
		free(data);
	}
}


//	dataReadRAW
//	------------------------------------------------------------------------
//	Description	:	Read data in volume, you can access data by data->rawdata
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataReadRAW(DATA* data)
{
	int s,i;
	register unsigned char	t;
	register unsigned char*	p;
	register unsigned char*	q;

	if(!data||!data->rawfile_ptr||data->rawdata) return;

	s=data->totalsize;
	data->rawdata=malloc(s*data->sample_size);

	fread(data->rawdata,s*data->sample_size,1,data->rawfile_ptr);


	//	Convert unsigned char to unsigned short

	if(data->sample_size==1)
	{
		p=malloc(s*sizeof(unsigned short));
		memset(p,0,s*sizeof(unsigned short));

		q=(unsigned char*)data->rawdata;
		for(i=0;i<s;++i)
		{
			p[i*2]=q[i];
		}

		free(data->rawdata);
		data->rawdata=(unsigned short*)p;
	}


	//	Convert big-endian data to little-endian


	if(data->byte_order&&data->sample_size==2)
	{
		p=(unsigned char*)data->rawdata;

		for(i=0;i<s;++i)
		{
			//	swap 0,1
			t=p[i*2];
			p[i*2]=p[i*2+1];
			p[i*2+1]=t;


		}
	}

}


//	dataReadIKDtree
//	------------------------------------------------------------------------
//	Description	:	Read external IKDtree file if available.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataReadIKDtree(DATA* data)
{
	if(!data||!data->ikdtreefile_ptr||!data->rawdata) return;
	data->iKDtree=IKDtree_new_from_filep(data->ikdtreefile_ptr);
	data->vmax=data->iKDtree->node[0].max;
	data->vmin=data->iKDtree->node[0].min;
}


//	dataReadGradient
//	------------------------------------------------------------------------
//	Description	:	Read external gradient file if available.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataReadGradient(DATA* data)
{
	if(!data||!data->normalfile_ptr||!data->rawdata) return;
	data->gradient=malloc(sizeof(float)*4*data->totalsize);
	fread(&data->gmax,sizeof(float),1,data->normalfile_ptr);
	fread(&data->gmin,sizeof(float),1,data->normalfile_ptr);
	fread(data->gradient,sizeof(float)*4*data->totalsize,1,data->normalfile_ptr);
}


//	dataGetMaxMin
//	------------------------------------------------------------------------
//	Description	:	Calculate the maximum and minimum value in the volume.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataGetMaxMin(DATA* data)
{
	int max,min;
	int i,s;
	max=0;
	min=99999999;
	s=data->resol[0]*data->resol[1]*data->resol[2];

	for(i=0;i<s;++i)
	{
		if((float)(data->rawdata[i])>max) max=data->rawdata[i];
		if((float)(data->rawdata[i])<min) min=data->rawdata[i];
	}

	data->vmax=max;
	data->vmin=min;
}


//	dataGenHistogram
//	------------------------------------------------------------------------
//	Description	:	Generate a histogram and calculate the maximum and
//				minimum.
//	Parameters	:
//
//		level		resolution of the histogram
//		data		handle of the volume
//		max		pointer to save the maximum
//		min		pointer to save the minimum
//		filter		use log10 or not
//
//	Returns	:	pointer to the new histogram array.
//

int*	dataGenHistogram(int level,DATA* data,int* max,int* min,int filter)
{
	int	i,size,m,n;
	int*	histogram;

	if(!data->rawdata) return NULL;

	size=level*sizeof(int);
	histogram=malloc(size);
	memset(histogram,0,size);


	for(i=0;i<data->totalsize;++i)
	{
		//++histogram[(level-1)*(data->rawdata.ushort[i]-data->vmin)/(data->vmax-data->vmin)];
		++histogram[level*(data->rawdata[i]-data->vmin)/(data->vmax-data->vmin+1)];
	}


	if(filter)
	{
		for(i=0;i<level;++i)
		{
			if(histogram[i])histogram[i]=log(histogram[i])*10;
		}
	}
	m=0;n=1999999999;
	for(i=0;i<level;++i)
	{
		if(histogram[i]>m) m=histogram[i];
		if(histogram[i]<n) n=histogram[i];
	}
	*max=m;
	*min=n;
	return histogram;
}


//	dataGenHistogramEx
//	------------------------------------------------------------------------
//	Description	:	Generate a histogram and calculate the maximum and
//				minimum. (using floating point to get better data)
//	Parameters	:
//
//		level		resolution of the histogram
//		data		handle of the volume
//		max		pointer to save the maximum
//		min		pointer to save the minimum
//		filter		use log10 or not
//
//	Returns	:	pointer to the new histogram array.
//

int*	dataGenHistogramEx(int level,DATA* data,int* max,int* min,int filter)
{
	int	i,size,m,n;
	float j;
	int*	histogram;
	float*	histogramf;
	if(!data->rawdata||level>(data->vmax-data->vmin+1)||!level) return NULL;
	size=level*sizeof(int);
	histogram=malloc(size);
	histogramf=malloc(size);
	memset(histogram,0,size);
	memset(histogramf,0,size);


	//	Histogram(float)

	for(i=0;i<data->totalsize;++i)
	{
		j=(float)(level-1)*(data->rawdata[i]-data->vmin)/(data->vmax-data->vmin);
		histogramf[(int)j]+=(int)(j+1)-j;
		histogramf[(int)j+1]+=j-(int)j;
	}


	//	Convert to integer
	for(i=0;i<level;++i)
	{
		histogram[i]=histogramf[i];
	}

	if(filter)
	{
		for(i=0;i<level;++i)
		{
			if(histogram[i])histogram[i]=log(histogram[i])*10;
		}
	}
	m=0;n=1999999999;
	for(i=0;i<level;++i)
	{
		if(histogram[i]>m) m=histogram[i];
		if(histogram[i]<n) n=histogram[i];
	}

	if(max) *max=m;
	if(min) *min=n;
	return histogram;

}


//	histogramGenDiff
//	------------------------------------------------------------------------
//	Description	:	Generate central difference of the histogram
//	Parameters	:
//
//		level		resolution of the histogram
//		histogram	pointer to histogram array
//
//	Returns	:	pointer to the new histogram array.
//

int*	histogramGenDiff(int level,int* histogram)
{
	int i;
	int*	diff;

	if(level<1||!histogram) return NULL;

	diff=malloc(sizeof(int)*level);

	for(i=0;i<level;++i)
	{
		if(i==0)
		{
			diff[i]=(histogram[i+1]-histogram[i]);
			continue;
		}
		if(i==level-1)
		{
			diff[i]=(histogram[i]-histogram[i-1]);
			continue;
		}
		diff[i]=(histogram[i+1]-histogram[i-1])/2;
	}

	return diff;
}


//	histogramGenLDiff
//	------------------------------------------------------------------------
//	Description	:	Generate left difference of the histogram.
//	Parameters	:
//
//		level		resolution of the histogram
//		histogram	pointer to histogram array
//
//	Returns	:	pointer to the new histogram array.
//

int*	histogramGenLDiff(int level,int* histogram)
{
	int i;
	int*	diff;

	if(level<1||!histogram) return NULL;

	diff=malloc(sizeof(int)*level);

	for(i=0;i<level;++i)
	{
		if(i==0)
		{
			diff[i]=0;
			continue;
		}

		diff[i]=(histogram[i]-histogram[i-1]);
	}

	return diff;

}


//	histogramGenRDiff
//	------------------------------------------------------------------------
//	Description	:	Generate right difference of the histogram.
//	Parameters	:
//
//		level		resolution of the histogram
//		histogram	pointer to histogram array
//
//	Returns	:	pointer to the new histogram array.
//

int*	histogramGenRDiff(int level,int* histogram)
{
	int i;
	int*	diff;

	if(level<1||!histogram) return NULL;

	diff=malloc(sizeof(int)*level);

	for(i=0;i<level;++i)
	{
		if(i==level-1)
		{
			diff[i]=0;
			continue;
		}
		diff[i]=(histogram[i+1]-histogram[i]);
	}

	return diff;

}


//	filterGenGaussian
//	------------------------------------------------------------------------
//	Description	:	Generate a gaussian map in double precision float.
//				Gaussian function g(x,s) = 1/Sum(wi)*e^(-x^2/(2*s^2))
//	Parameters	:
//
//		sigma		sigma of the gaussian function
//		size		radius
//
//	Returns	:	gaussian map pointer.
//

double*	filterGenGaussian(float sigma,int size)
{
	double*	filter,w;
	int		i;

	if(!sigma||size<0) return NULL;

	filter=(double*)malloc(sizeof(double)*(size*2+1));

	for(i=0,w=0.0;i<(size*2+1);++i)
	{
		filter[i]=exp(-(i-size)*(i-size)/(2*sigma*sigma));
		w+=filter[i];
	}
	for(i=0;i<(size*2+1);++i)
	{
		filter[i]/=w;
	}

	return	filter;
}

int*	histogramApplyGaussianFilter(int level,int* histogram,int* max,int* min,float sigma,int size)
{
	int	i,j,k,m,n;
	double	v;
	double*	gf;
	int*	out;

	if(!histogram||!level||!sigma||size<0) return NULL;

	gf=filterGenGaussian(sigma,size);
	out=(int*)malloc(sizeof(int)*level);

	m=0;
	n=999999999;
	for(i=0;i<level;++i)
	{
		v=0.0f;
		for(j=0;j<size*2+1;++j)
		{
			if(i+j-size<0) k=histogram[0];
			else if(i+j-size>=level) k=histogram[level-1];
			else k=histogram[i+j-size];
			v+=gf[j]*k;
		}
		out[i]=v;
		if(out[i]>m) m=out[i];
		if(out[i]<n) n=out[i];
	}
	if(max) *max=m;
	if(min) *min=n;
	return out;
}

int*	histogramApplyGaussianFilterEx(int level,int* histogram,int* max,int* min,float sigma,int size)
{
	int	i,j,k,m,n;
	double	v;
	double*	gf;
	int*	out;

	if(!histogram||!level||!sigma||size<0) return NULL;

	gf=filterGenGaussian(sigma,size);
	out=(int*)malloc(sizeof(int)*level);

	m=0;
	n=999999999;
	memcpy(out,histogram,sizeof(int)*level);

	for(i=0;i<level;++i)
	{
		v=0.0f;
		for(j=0;j<size*2+1;++j)
		{
			if(i+j-size<0) k=out[0];
			else if(i+j-size>=level) k=out[level-1];
			else k=out[i+j-size];
			v+=gf[j]*k;
		}
		out[i]=v;
		if(out[i]>m) m=out[i];
		if(out[i]<n) n=out[i];
	}
	if(max) *max=m;
	if(min) *min=n;
	return out;
}


//	dataValue
//	------------------------------------------------------------------------
//	Description	:	Return value in volume at position (x,y,z).
//	Parameters	:
//
//		data		handle of the volume
//		x,y,z		position
//
//	Returns	:	gaussian map pointer.
//

int	dataValue(DATA* data,int x,int y,int z)
{
	register int i;
/*
	if(!data)
	{
		invalid++;
		return 0;
	}
	if(x>=data->resol[0]||x<0||y>=data->resol[1]||y<0||z>=data->resol[2]||z<0)
	{
		invalid++;
		return 0;
	}
*/
	i=data->resol[0]*data->resol[1]*z+data->resol[0]*y+x;

	return  data->rawdata[i];

}


//	Calculate the gradient length of the RAW data

void	dataGenGradient(DATA* data)
{
	int x,y,z;
	float s,v[3]={0.0f};
	int i[2];

	if(!data||data->gradient) return;

	i[0]=data->resol[0];
	i[1]=data->resol[0]*data->resol[1];

	//data->gradient_magnitude=calloc(sizeof(int),data->totalsize);
	//data->gradient=calloc(sizeof(unsigned short),data->totalsize*3);
	//data->gradient=calloc(sizeof(unsigned char),data->totalsize*3);
	data->gradient=calloc(sizeof(float),data->totalsize*4);

	data->gmax=0;
	data->gmin=99999999;

	for(z=0;z<data->resol[2];++z)
	{
		for(y=0;y<data->resol[1];++y)
		{
			for(x=0;x<data->resol[0];++x)
			{
				if(x==0)
				{
					v[0]=(dataValue(data,x+1,y,z)-dataValue(data,x,y,z))/data->m_ratio[0];
				}else if(x==data->resol[0]-1)
				{
					v[0]=(dataValue(data,x,y,z)-dataValue(data,x-1,y,z))/data->m_ratio[0];
				}else
				{
					v[0]=(dataValue(data,x+1,y,z)-dataValue(data,x-1,y,z))/2.0/data->m_ratio[0];
				}

				if(y==0)
				{
					v[1]=(dataValue(data,x,y+1,z)-dataValue(data,x,y,z))/data->m_ratio[1];
				}else if(y==data->resol[1]-1)
				{
					v[1]=(dataValue(data,x,y,z)-dataValue(data,x,y-1,z))/data->m_ratio[1];
				}else
				{
					v[1]=(dataValue(data,x,y+1,z)-dataValue(data,x,y-1,z))/2.0/data->m_ratio[1];
				}

				if(z==0)
				{
					v[2]=(dataValue(data,x,y,z+1)-dataValue(data,x,y,z))/data->m_ratio[2];
				}else if(z==data->resol[2]-1)
				{
					v[2]=(dataValue(data,x,y,z)-dataValue(data,x,y,z-1))/data->m_ratio[2];
				}else
				{
					v[2]=(dataValue(data,x,y,z+1)-dataValue(data,x,y,z-1))/2.0/data->m_ratio[2];
				}

				/*if(v[0]||v[1]||v[2])*///data->gradient_magnitude[i[1]*z+i[0]*y+x]=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
				//s=data->gradient_magnitude[i[1]*z+i[0]*y+x];
				s=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

/*
				data->gradient[i[1]*z*3+i[0]*y*3+x*3+0]=(1.0+v[0]/s)*0.5*255;
				data->gradient[i[1]*z*3+i[0]*y*3+x*3+1]=(1.0+v[1]/s)*0.5*255;
				data->gradient[i[1]*z*3+i[0]*y*3+x*3+2]=(1.0+v[2]/s)*0.5*255;
*/



/*
				data->gradient[i[1]*z*3+i[0]*y*3+x*3+0]=(1.0+v[0]/s)*0.5*65535;
				data->gradient[i[1]*z*3+i[0]*y*3+x*3+1]=(1.0+v[1]/s)*0.5*65535;
				data->gradient[i[1]*z*3+i[0]*y*3+x*3+2]=(1.0+v[2]/s)*0.5*65535;
*/


//				data->gradient[i[1]*z*4+i[0]*y*4+x*4+0]=(1.0+v[0]/s)*0.5;
//				data->gradient[i[1]*z*4+i[0]*y*4+x*4+1]=(1.0+v[1]/s)*0.5;
//				data->gradient[i[1]*z*4+i[0]*y*4+x*4+2]=(1.0+v[2]/s)*0.5;
//				data->gradient[i[1]*z*4+i[0]*y*4+x*4+3]=s;


				if(s!=0)
				{
					data->gradient[i[1]*z*4+i[0]*y*4+x*4+0]=v[0]/s;
					data->gradient[i[1]*z*4+i[0]*y*4+x*4+1]=v[1]/s;
					data->gradient[i[1]*z*4+i[0]*y*4+x*4+2]=v[2]/s;
				}else
				{
					data->gradient[i[1]*z*4+i[0]*y*4+x*4+0]=0.0;
					data->gradient[i[1]*z*4+i[0]*y*4+x*4+1]=0.0;
					data->gradient[i[1]*z*4+i[0]*y*4+x*4+2]=0.0;

				}
				data->gradient[i[1]*z*4+i[0]*y*4+x*4+3]=s;



				if(s>data->gmax)data->gmax=s;
				if(s<data->gmin)data->gmin=s;
			}
		}
	}

//	for(x=0;x<data->totalsize;++x)
//		data->gradient[x*4+3]/=data->gmax;



}


void	dataHistogramEqualization(DATA* data)
{
	int*	table;
	int*	histogram;
	int	i;
	unsigned short*	raw;
	long long	t;
	FILE*	fp;

	if(!data)	return;

	histogram	=calloc(sizeof(int)*2048,1);
	table		=calloc(sizeof(int)*2048,1);
	raw		=malloc(sizeof(unsigned short)*data->totalsize);


	//memset(histogram,0,sizeof(int)*(data->vmax+1));
	for(i=0;i<data->totalsize;++i)
	{
		++histogram[data->rawdata[i]];
	}

	memcpy(table,histogram,sizeof(int)*(data->vmax+1));

	for(i=1;i<=2047;++i)
	{
		table[i]=table[i-1]+table[i];
	}

	for(i=0;i<data->totalsize;++i)
	{
		t=(long long)table[data->rawdata[i]]*2047/table[2047];
		raw[i]=t;
	}

	free(data->rawdata);
	free(histogram);
	free(table);

	data->rawdata=raw;

	fp=fopen("out.raw","wb");
	fwrite(raw,2*data->totalsize,1,fp);
	fclose(fp);
}

