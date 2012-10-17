#ifndef MARCHING_TETRAHEDRON_H_
#define MARCHING_TETRAHEDRON_H_


#include "data.h"

#define	ISOSURFACE_OK				(0x00)
#define	ISOSURFACE_ERROR_INVALID_VOLUME_DATA	(0x01)


//	Isosurface_extraction
//	------------------------------------------------------------------------
//	Description	:	Perform marching tetrahedron to extract
//				iso-surfaces.
//	Parameters	:
//
//		data		handle of the opened volume
//		iso_value	iso-value you want to use
//
//	Returns	:	The result of extraction.
//

int	Isosurface_extraction(const DATA* const data,const float iso_value);


//	Isosurface_release
//	------------------------------------------------------------------------
//	Description	:	Release memory allocated by Isosurface functions
//
//	Parameters	:
//
//	Returns	:
//

void	Isosurface_release(void);


//	Isosurface_set_pagesize
//	------------------------------------------------------------------------
//	Description	:	Set page size you want to split vertex array
//				into.
//	Parameters	:
//
//		num_of_vertices		number of vertices in one page
//
//	Returns	:
//

void	Isosurface_set_pagesize(int num_of_vertices);


//	Isosurface_get_used_pagesize
//	------------------------------------------------------------------------
//	Description	:	Get page usage.
//
//	Parameters	:
//
//	Returns	:	page usage
//

int	Isosurface_get_used_pagesize(void);


//	Isosurface_get_buffer
//	------------------------------------------------------------------------
//	Description	:	Get a vertex buffer pointer.
//
//	Parameters	:
//
//		number		page number
//
//	Returns	:	a vertex buffer pointer
//

float*	Isosurface_get_buffer(int number);


//	Isosurface_get_number_of_vertices
//	------------------------------------------------------------------------
//	Description	:	Get total number of vertices in all pages.
//
//	Parameters	:
//
//	Returns	:	number of vertices
//

int	Isosurface_get_number_of_vertices(void);


//	Isosurface_export_to_obj
//	------------------------------------------------------------------------
//	Description	:	Save triangles to a .obj file
//
//	Parameters	:
//
//		filename	filename you want to save as
//
//	Returns	:	ISOSURFACE_OK if successfully saved
//

int	Isosurface_export_to_obj(const char* const filename);



#endif /* MARCHING_TETRAHEDRON_H_ */
