#include	"marching_tetrahedron.h"
#include	"data.h"

#include	<stdlib.h>


#define		AXIS_X	(0)
#define		AXIS_Y	(1)
#define		AXIS_Z	(2)

#define		MAX_PAGE	(20)


//	Static variables

static DATA*	_data=NULL;
static float	_iso_value=0;
static int	_page_size=65535;
float*		_vertex_buffer[MAX_PAGE]={NULL};
int		_number_of_vertices=0;


//	Static function prototypes

static void	Marching_tetrahedron	(const int x,const int y,const int z);
static void	Isosurface_KDtree	(	const int node,
						const int axis,
						const int nx,	const int ny,	const int nz,
						const int mx,	const int my,	const int mz
					);




int	Isosurface_extraction(const DATA* const data,const float iso_value)
{
	if(!data||!data->rawdata)
		return ISOSURFACE_ERROR_INVALID_VOLUME_DATA;

	_data=(DATA*)data;
	_iso_value=iso_value;

	Isosurface_release();

	//	Allocate first page

	_vertex_buffer[0]=malloc(sizeof(float)*3*_page_size);
	Isosurface_KDtree(0,AXIS_X,0,0,0,_data->resol[0]-1,_data->resol[1]-1,_data->resol[2]-1);

	return ISOSURFACE_OK;
}

int	Isosurface_export_to_obj(const char* const filename)
{
	FILE*	fp;
	int	i,page,index_in_page;

	if(!_number_of_vertices) return 1;

	fp=fopen(filename,"w");

	if(!fp)
		return 1;

	for(i=0;i<_number_of_vertices;++i)
	{
		page		=i/_page_size;
		index_in_page	=i%_page_size;

		fprintf(fp,"v %f %f %f\n",_vertex_buffer[page][index_in_page*3+0],_vertex_buffer[page][index_in_page*3+1],_vertex_buffer[page][index_in_page*3+2]);
	}
	fputs("usemtl Material\ns off\n",fp);

	for(i=0;i<_number_of_vertices/3;++i)
	{
		fprintf(fp,"f %d %d %d\n",i*3+1,i*3+2,i*3+3);
	}

	fclose(fp);

	return ISOSURFACE_OK;

}


int	Isosurface_get_used_pagesize(void)
{
	return	(!_number_of_vertices%_page_size) ? _number_of_vertices/_page_size : _number_of_vertices/_page_size+1;
}

int	Isosurface_get_number_of_vertices(void)
{
	return	_number_of_vertices;
}


void	Isosurface_set_pagesize(int num_of_vertices)
{
	Isosurface_release();
	if(num_of_vertices>0) _page_size=num_of_vertices;
}

float*	Isosurface_get_buffer(int number)
{
	return _vertex_buffer[number];
}

void	Isosurface_release(void)
{
	int	i;

	if(_number_of_vertices)
	{
		for(i=0;i<MAX_PAGE;++i)
			if(_vertex_buffer[i])
			{
				free(_vertex_buffer[i]);
				_vertex_buffer[i]=NULL;
			}
		_number_of_vertices=0;
	}

}


static void	Isosurface_KDtree	(	const int node,
						const int axis,
						const int nx,	const int ny,	const int nz,
						const int mx,	const int my,	const int mz
					)
{
	int		x,y,z;
	ikdtree*	kdtree;

	kdtree=_data->iKDtree;


	//	Skip empty blocks

	if(_iso_value<kdtree->node[node].min||_iso_value>kdtree->node[node].max)
	{
		return;
	}else if(kdtree->node[node].child[0]==-1&&kdtree->node[node].child[1]==-1)
	{


		//	Proceed marching tetrahedron if leaf nodes reached.

		for(z=nz;z<mz;++z)
			for(y=ny;y<my;++y)
				for(x=nx;x<mx;++x)
				{
					Marching_tetrahedron(x,y,z);
				}
		return;
	}


	//	Switching axis between x,y,z

	switch(axis%3)
	{
		case AXIS_X:
			//if(kdtree->number_of_nodes==kdtree->buffer_size) return;
			if(kdtree->node[node].child[0]!=-1)Isosurface_KDtree(kdtree->node[node].child[0],axis+1,nx,ny,nz,(nx+mx)/2,my,mz);
			if(kdtree->node[node].child[1]!=-1)Isosurface_KDtree(kdtree->node[node].child[1],axis+1,(nx+mx)/2,ny,nz,mx,my,mz);
		break;
		case AXIS_Y:
			if(kdtree->node[node].child[0]!=-1)Isosurface_KDtree(kdtree->node[node].child[0],axis+1,nx,ny,nz,mx,(ny+my)/2,mz);
			if(kdtree->node[node].child[1]!=-1)Isosurface_KDtree(kdtree->node[node].child[1],axis+1,nx,(ny+my)/2,nz,mx,my,mz);
		break;
		case AXIS_Z:
			if(kdtree->node[node].child[0]!=-1)Isosurface_KDtree(kdtree->node[node].child[0],axis+1,nx,ny,nz,mx,my,(nz+mz)/2);
			if(kdtree->node[node].child[1]!=-1)Isosurface_KDtree(kdtree->node[node].child[1],axis+1,nx,ny,(nz+mz)/2,mx,my,mz);
		break;

	}

}

static void	AddVertex(const float* const vertex)
{
	int	page;
	page=_number_of_vertices/_page_size;
	if(_vertex_buffer[page]==NULL)
		_vertex_buffer[page]=malloc(sizeof(float)*_page_size*3);

	(_vertex_buffer[page])[(_number_of_vertices%_page_size)*3+0]=vertex[0];
	(_vertex_buffer[page])[(_number_of_vertices%_page_size)*3+1]=vertex[1];
	(_vertex_buffer[page])[(_number_of_vertices%_page_size)*3+2]=vertex[2];
	++_number_of_vertices;
}


static void	Marching_tetrahedron(const int x,const int y,const int z)
{

	const int tetrahedron[40]=	{	0,1,2,4,
						1,2,7,3,
						2,4,7,6,
						1,2,4,7,
						1,4,5,7,

						4,0,6,5,
						0,6,3,2,
						6,5,3,7,
						0,6,5,3,
						0,5,1,3,
					};


	const int index[12]=	{	0,1,	1,2,
					0,2,	2,3,
					0,3,	3,1
				};

	int	i,j,k=0;
	int	num_of_vertices;
	float	v[4];
	int	split_mode;
	float	tetrahedron_vertex[12];
	float	vertex[6];
	float	output_vertex[12];
	float	r;

	split_mode=((x+y+z)%2) ? 1:0;


	//	Split one cube into five tetrahedrons.

	for(i=0;i<5;++i)
	{
		//	Fill vertices into buffer

		for(j=0;j<4;++j)
		{

			//	Offset
			//	zyx	n(tetrahedron)
			//	000	0
			//	001	1
			//	010	2
			//	011	3
			//	100	4
			//	101	5
			//	110	6
			//	111	7
			//
			//	n & 1 ? 1:0 => x
			//	n & 2 ? 1:0 => y
			//	n & 4 ? 1:0 => z


			tetrahedron_vertex[j*3+0]=x+(tetrahedron[split_mode*20+i*4+j]&1);
			tetrahedron_vertex[j*3+1]=y+(tetrahedron[split_mode*20+i*4+j]&2 ? 1 : 0);
			tetrahedron_vertex[j*3+2]=z+(tetrahedron[split_mode*20+i*4+j]&4 ? 1 : 0);

		}


		//	Generate polygons

		for(j=0,num_of_vertices=0;j<6;++j)
		{

			vertex[0]=tetrahedron_vertex[index[j*2+0]*3+0];
			vertex[1]=tetrahedron_vertex[index[j*2+0]*3+1];
			vertex[2]=tetrahedron_vertex[index[j*2+0]*3+2];

			vertex[3]=tetrahedron_vertex[index[j*2+1]*3+0];
			vertex[4]=tetrahedron_vertex[index[j*2+1]*3+1];
			vertex[5]=tetrahedron_vertex[index[j*2+1]*3+2];

			v[0]=dataValue(_data,vertex[0],vertex[1],vertex[2]);
			v[1]=dataValue(_data,vertex[3],vertex[4],vertex[5]);


			if(v[0]==v[1]) continue;


			//	Intersection

			if((_iso_value-v[0]+(v[0]==_iso_value))*(_iso_value-v[1]+(v[1]==_iso_value)) < 0.0)
			{
				r=(float)(_iso_value-v[0])/(v[1]-v[0]);

				output_vertex[num_of_vertices*3+0]=vertex[0]+(vertex[3]-vertex[0])*r;
				output_vertex[num_of_vertices*3+1]=vertex[1]+(vertex[4]-vertex[1])*r;
				output_vertex[num_of_vertices*3+2]=vertex[2]+(vertex[5]-vertex[2])*r;
				++num_of_vertices;
			}

		}


		//	Add a triangle

		if(num_of_vertices>=3)
		for(k=0;k<3;++k)
		{
			AddVertex(&output_vertex[k*3]);
		}


		//	Add another triange to form a quad

		if(num_of_vertices==4)
		{
			AddVertex(&output_vertex[0]);
			AddVertex(&output_vertex[6]);
			AddVertex(&output_vertex[9]);
		}

	}
}


