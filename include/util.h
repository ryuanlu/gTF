#ifndef UTIL_H_
#define UTIL_H_

#include	<gtk/gtk.h>

#define	OPEN_DIALOG		0x00
#define	SAVE_DIALOG		0x01


/* structures for defining DIBs */

#define	BI_RGB		0L



#pragma pack(1)

//	#pragma pack(1)	=>	Fix the 32 bits alignment problem
//				sizeof(BITMAPFILEHEADER) should be 14, not 16.


typedef struct tagBITMAPFILEHEADER {

	short	bfType;
	int	bfSize;
        short	bfReserved1;
        short	bfReserved2;
        int	bfOffBits;

}BITMAPFILEHEADER,*PBITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER{

	int	biSize;
	int	biWidth;
	int	biHeight;
	short	biPlanes;
	short	biBitCount;
        int	biCompression;
        int	biSizeImage;
        int	biXPelsPerMeter;
        int	biYPelsPerMeter;
        int	biClrUsed;
        int	biClrImportant;

} BITMAPINFOHEADER,*PBITMAPINFOHEADER;


#pragma pack()	//	Restore alignment settings



typedef struct tagRGBQUAD {

        char	rgbBlue;
        char	rgbGreen;
        char	rgbRed;
        char	rgbReserved;

} RGBQUAD;

typedef struct tagBITMAPINFO {

	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD		bmiColors[1];

}BITMAPINFO,*PBITMAPINFO;


//	ReadShaderText
//	------------------------------------------------------------------------
//	Description	:	Read all text in a shader file.
//	Parameters	:
//
//		filename	Name of shader file.
//
//	Returns	:	New pointer to the string.
//

char*	ReadShaderText(const char* filename);


//	FileDialog
//	------------------------------------------------------------------------
//	Description	:	Create & run a file dialog window.
//	Parameters	:
//
//		type			Type of dialog, you  may use OPEN_DIALOG or SAVE_DIALOG.
//		parent_window		Pointer to parent GTK window .
//
//	Returns	:	New pointer to the string of selected file name,
//				you have to free that by yourself.
//

char*	FileDialog(int type,GtkWidget* parent_window);


//	glTakeScreenshot
//	------------------------------------------------------------------------
//	Description	:	Take a screenshot in a OpenGL window, you need to
//				change current render context to assign OpenGL canvas.
//	Parameters	:
//
//		window		The window which you want to take a screenshot in.
//
//	Returns	:	nothing.
//

void	glTakeScreenshot(GtkWidget* window);

void	matrix_x_vertex(float* v,float* M,float x,float y,float z);


//	glLoadShader
//	------------------------------------------------------------------------
//	Description	:	Take a screenshot in a OpenGL window, you need to
//				change current render context to assign OpenGL canvas.
//	Parameters	:
//
//		window		The window which you want to take a screenshot in.
//
//	Returns	:	OpenGL Object number of the program object.
//

int	glLoadShader(const char* vs_file,const char* fs_file);


//	glDrawBlock
//	------------------------------------------------------------------------
//	Description	:	Draw a block
//	Parameters	:
//
//		x1,y1		first vertex
//		x1,y2		second vertex
//
//	Returns	:	none
//

void	glDrawBlock(float x1,float y1,float x2,float y2);


//	glDrawBoundingBox
//	------------------------------------------------------------------------
//	Description	:	Draw a cube in wireframes
//	Parameters	:
//
//		x,y,z		width, height, depth
//
//	Returns	:	none
//

void	glDrawBoundingBox(float x,float y,float z);



void	printShaderInfoLog(int obj);
void	printProgramInfoLog(int obj);


#endif /* UTIL_H_ */
