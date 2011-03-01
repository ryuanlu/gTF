#ifndef IKDTREE_H_
#define IKDTREE_H_

#include	<stdio.h>
#include	<stdlib.h>

#include	"ikdtree_type.h"

#include	"data.h"





ikdtree*	IKDtree_new_from_volume(DATA* data,int max_depth,int min_dim,int min_diff);
ikdtree*	IKDtree_new_from_filep(FILE* fp);
void		IKDtree_delete(ikdtree* tree);

void		IKDtree_compute_min_max(unsigned short* max,unsigned short* min,DATA* data,int nx,int ny,int nz,int mx,int my,int mz);
void		IKDtree_split(ikdtree* kdtree,int node,int axis,DATA* data,int nx,int ny,int nz,int mx,int my,int mz);

void		IKDtree_write_to_file(ikdtree* tree,const char* filename);


#endif /* IKDTREE_H_ */
