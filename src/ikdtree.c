#include	<math.h>
#include	"ikdtree.h"

ikdtree*	IKDtree_new_from_volume(DATA* data,int max_depth,int min_dim,int min_diff)
{
	ikdtree*	kdtree;

	kdtree=calloc(1,sizeof(ikdtree));
	kdtree->node=calloc(pow(2,max_depth+1)-1,sizeof(ikdtree_node));

	kdtree->buffer_size=pow(2,max_depth+1)-1;

	kdtree->min_diff=min_diff;
	kdtree->min_dim=min_dim;
	kdtree->max_depth=max_depth;

	IKDtree_split(kdtree,0,0,data,0,0,0,data->resol[0]-1,data->resol[1]-1,data->resol[2]-1);

	return	kdtree;
}

ikdtree*	IKDtree_new_from_filep(FILE* fp)
{
	ikdtree*	kdtree=NULL;

	if(!fp) return NULL;

	kdtree=calloc(1,sizeof(ikdtree));

	fread(&kdtree->number_of_nodes,sizeof(int),1,fp);
	kdtree->node=calloc(kdtree->number_of_nodes,sizeof(ikdtree_node));
	fread(kdtree->node,sizeof(ikdtree_node),kdtree->number_of_nodes,fp);
	return	kdtree;
}


void	IKDtree_delete(ikdtree* tree)
{
	if(!tree) return;
	free(tree->node);
	free(tree);
}


void	IKDtree_compute_min_max(unsigned short* max,unsigned short* min,DATA* data,int nx,int ny,int nz,int mx,int my,int mz)
{
	int	x,y,z,i;

	*max=0;
	*min=65535;

	for(z=nz;z<=mz;++z)
		for(y=ny;y<=my;++y)
			for(x=nx;x<=mx;++x)
			{
				i=dataValue(data,x,y,z);
				if(i>*max)	*max=i;
				if(i<*min)	*min=i;
			}
}


void	IKDtree_split(ikdtree* kdtree,int node,int axis,DATA* data,int nx,int ny,int nz,int mx,int my,int mz)
{
	++kdtree->number_of_nodes;
	IKDtree_compute_min_max(&kdtree->node[node].max,&kdtree->node[node].min,data,nx,ny,nz,mx,my,mz);

	kdtree->node[node].child[0]=-1;
	kdtree->node[node].child[1]=-1;

	if(mx-nx<kdtree->min_dim||my-ny<kdtree->min_dim||mz-nz<kdtree->min_dim||
	kdtree->node[node].max-kdtree->node[node].min<kdtree->min_diff||axis>=kdtree->max_depth)
	{
		return;
	}


	switch(axis%3)
	{
		case 0:
			kdtree->node[node].child[0]=kdtree->number_of_nodes;
			IKDtree_split(kdtree,kdtree->node[node].child[0],axis+1,data,nx,ny,nz,(nx+mx)/2,my,mz);
			kdtree->node[node].child[1]=kdtree->number_of_nodes;
			IKDtree_split(kdtree,kdtree->node[node].child[1],axis+1,data,(nx+mx)/2,ny,nz,mx,my,mz);
		break;
		case 1:
			kdtree->node[node].child[0]=kdtree->number_of_nodes;
			IKDtree_split(kdtree,kdtree->node[node].child[0],axis+1,data,nx,ny,nz,mx,(ny+my)/2,mz);
			kdtree->node[node].child[1]=kdtree->number_of_nodes;
			IKDtree_split(kdtree,kdtree->node[node].child[1],axis+1,data,nx,(ny+my)/2,nz,mx,my,mz);
		break;
		case 2:
			kdtree->node[node].child[0]=kdtree->number_of_nodes;
			IKDtree_split(kdtree,kdtree->node[node].child[0],axis+1,data,nx,ny,nz,mx,my,(nz+mz)/2);
			kdtree->node[node].child[1]=kdtree->number_of_nodes;
			IKDtree_split(kdtree,kdtree->node[node].child[1],axis+1,data,nx,ny,(nz+mz)/2,mx,my,mz);
		break;

	}
}

void	IKDtree_write_to_file(ikdtree* tree,const char* filename)
{
	FILE*	fp;

	if(!tree||!filename) return;

	fp=fopen(filename,"wb");

	if(!fp) return;

	fwrite(&tree->number_of_nodes,sizeof(int),1,fp);
	fwrite(tree->node,sizeof(ikdtree_node),tree->number_of_nodes,fp);
	fclose(fp);

}



