#ifndef DATA_H_
#define DATA_H_

#include <stdio.h>
#include "ikdtree_type.h"

//	Flag

#define	DATA_FIELD_RAW_FILE		0x00000001
#define	DATA_FIELD_RESOLUTION		0x00000002
#define	DATA_FIELD_RATIO		0x00000004
#define	DATA_FIELD_SAMPLE_TYPE		0x00000008
#define	DATA_FIELD_BYTE_ORDER		0x00000010
#define	DATA_FIELD_VALUE_MAX		0x00000020
#define	DATA_FIELD_VALUE_MIN		0x00000040



//	Sample type

#define	SAMPLE_TYPE_UNKNOWN		0x00000000
#define	SAMPLE_TYPE_UCHAR		0x00000001
#define	SAMPLE_TYPE_USHORT		0x00000002


//	Byte order

#define	BYTE_ORDER_LITTLE_ENDIAN	0x00000000
#define	BYTE_ORDER_BIG_ENDIAN		0x00000001


//	Handle	declaration

typedef struct
{
	int	flag;

	FILE*	rawfile_ptr;
	FILE*	ikdtreefile_ptr;
	FILE*	normalfile_ptr;

	char*	rawfilename;
	char*	ikdtreefilename;
	char*	normalfilename;

	int	resol[3];
	float	ratio[3];

	float	m_ratio[3];		//	Normalized ratio, ensure all value greater than 1.0
	int	m_resol[3];		//	resol * m_ratio

	float	unit_dim[3];

	int	totalsize;		//	Dimension of the volume, equal to resol[0] * resol[1] * resol[2]
	int	sample_type;
	int	sample_size;		//	in bytes
	int	byte_order;

	int	vmax,vmin;

	unsigned short*	rawdata;
	ikdtree*	iKDtree;

	//float*		gradient_magnitude;		//	Pointer to gradient volume
	//unsigned short*		gradient;		//	Pointer to gradient volume
	float*		gradient;		//	Pointer to gradient volume
	//unsigned char*		gradient;		//	Pointer to gradient volume

	float		gmax,gmin;

}DATA;


//	Variables from lex & yacc

extern FILE*	yyin;
extern DATA**	dataptr;


//	Functions


//	dataOpen
//	------------------------------------------------------------------------
//	Description	:	Open a .inf volume.
//	Parameters	:
//
//		filename	filename of the volume
//
//	Returns	:	The pointer of new volume object.
//

DATA*	dataOpen(const char* filename);


//	dataClose
//	------------------------------------------------------------------------
//	Description	:	Close an opened volume and free all allocated resource.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataClose(DATA* data);


//	dataReadRAW
//	------------------------------------------------------------------------
//	Description	:	Read data in volume, you can access data by data->rawdata
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataReadRAW(DATA* data);


//	dataReadIKDtree
//	------------------------------------------------------------------------
//	Description	:	Read external IKDtree file if available.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataReadIKDtree(DATA* data);


//	dataGetMaxMin
//	------------------------------------------------------------------------
//	Description	:	Calculate the maximum and minimum value in the volume.
//	Parameters	:
//
//		data		handle of the volume
//
//	Returns	:	nothing.
//

void	dataGetMaxMin(DATA* data);


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

int*	dataGenHistogram(int level,DATA* data,int* max,int* min,int filter);


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

int*	dataGenHistogramEx(int level,DATA* data,int* max,int* min,int filter);


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

int*	histogramGenDiff(int level,int* histogram);


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

int*	histogramGenLDiff(int level,int* histogram);


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

int*	histogramGenRDiff(int level,int* histogram);


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

double*	filterGenGaussian(float sigma,int level);
int*	histogramApplyGaussianFilter(int level,int* histogram,int* max,int* min,float sigma,int size);
int*	histogramApplyGaussianFilterEx(int level,int* histogram,int* max,int* min,float sigma,int size);


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

int	dataValue(DATA*,int x,int y,int z);
void	dataGenGradient(DATA* data);
void	dataReadGradient(DATA* data);
void	dataHistogramEqualization(DATA* data);


#endif /*DATA_H_*/

