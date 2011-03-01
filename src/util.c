#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<memory.h>
#include	"gTF.h"
#include	"glext.h"


#include	"util.h"


//	ReadShaderText
//	------------------------------------------------------------------------
//	Description	:	Read all text in a shader file.
//	Parameters	:
//
//		filename	Name of shader file.
//
//	Returns	:	New pointer to the string.
//


char*	ReadShaderText(const char* filename)
{
	char*	text;
	FILE*	fp;
	struct stat	statbuf;

	fp=fopen(filename,"r");

	if(!fp) return NULL;

	stat(filename,&statbuf);
	text=malloc(statbuf.st_size+1);
	fread(text,1,statbuf.st_size,fp);
	text[statbuf.st_size]=0;
	fclose(fp);

	return text;
}

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

char*	FileDialog(int type,GtkWidget* parent_window)
{
	GtkWidget*	dialog=NULL;
	char*	filename=NULL;
	char*	tmp=NULL;
	char*	path=NULL;

	switch(type)
	{
		case OPEN_DIALOG:
			dialog=gtk_file_chooser_dialog_new(_("Open File"),GTK_WINDOW(parent_window),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
		break;
		case SAVE_DIALOG:
			dialog=gtk_file_chooser_dialog_new(_("Save File"),GTK_WINDOW(parent_window),GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);
		break;
	}

	gtk_widget_show(dialog);

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
	{
		filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	}

	if(filename)
	{

		//	Handle with different encoding filename

		tmp=filename;
		filename=g_locale_from_utf8(filename,-1,NULL,NULL,NULL);
		free(tmp);
	}



	path=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));


	//	Handle with different encoding path

	tmp=path;
	path=g_locale_from_utf8(path,-1,NULL,NULL,NULL);
	free(tmp);


	gtk_widget_destroy(dialog);

	chdir(path);
	free(path);

	return	filename;
}

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

void	glTakeScreenshot(GtkWidget* window)
{
	char*			filename=NULL;
	char*			pixels;
	int			w,h,aw;
	BITMAPFILEHEADER	hd;
	BITMAPINFO		info;
	FILE			*fp;


	memset(&hd,0,sizeof(BITMAPFILEHEADER));
	memset(&info,0,sizeof(BITMAPINFO));
	gdk_drawable_get_size(window->window,&w,&h);
	filename=FileDialog(SAVE_DIALOG,window);

	fp=fopen(filename,"wb");

	if(!fp)
	{
		if(filename)	free(filename);
		return;
	}

	if(w%4)	aw=(w*3/4+1)*4;
	else		aw=w*3;


	pixels=malloc(aw*h);

	//glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE,pixels);


	//	BMP header

	info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth=w;
	info.bmiHeader.biHeight=h;
	info.bmiHeader.biPlanes=1;
	info.bmiHeader.biBitCount=24;
	info.bmiHeader.biCompression=BI_RGB;
	info.bmiHeader.biSizeImage=aw*h;
	info.bmiHeader.biXPelsPerMeter=2952;
	info.bmiHeader.biYPelsPerMeter=2952;
	hd.bfType=0x4d42;
	hd.bfSize=info.bmiHeader.biSizeImage+sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);
	hd.bfOffBits=sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);

	//	Write to BMP file

	fwrite(&hd,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&info,sizeof(BITMAPINFO),1,fp);
	fwrite(pixels,info.bmiHeader.biSizeImage,1,fp);
	fclose(fp);
	free(pixels);

}

//	matrix multiplation

void	matrix_x_vertex(float* v,float* M,float x,float y,float z)
{
	v[0]=M[0]*x+M[4]*y+M[8]*z+M[12];
	v[1]=M[1]*x+M[5]*y+M[9]*z+M[13];
	v[2]=M[2]*x+M[6]*y+M[10]*z+M[14];
	v[3]=M[3]*x+M[7]*y+M[11]*z+M[15];

	v[0]/=v[3];v[1]/=v[3];v[2]/=v[3];v[3]/=v[3];
}

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

int	glLoadShader(const char* vs_file,const char* fs_file)
{
	unsigned int		progobj;
	unsigned int		shader[2];
	char*			text[2];
	int			i;

	progobj=glCreateProgram();

	text[0]=ReadShaderText(vs_file);
	text[1]=ReadShaderText(fs_file);

	if(text[0]) shader[0]=glCreateShader(GL_VERTEX_SHADER);
	if(text[1]) shader[1]=glCreateShader(GL_FRAGMENT_SHADER);

	for(i=0;i<2;++i)
	{
		if(!shader[i]) continue;

		glAttachObjectARB(progobj,shader[i]);
		glShaderSource(shader[i],1,(const char**)&text[i],NULL);
		glCompileShader(shader[i]);
		free(text[i]);
	}

	glLinkProgram(progobj);

	return progobj;
}

//	Draw a block on area (x1,y1) to (x2,y2)

void	glDrawBlock(float x1,float y1,float x2,float y2)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
		glVertex2f(x1,y1);
		glTexCoord2f(1.0,0.0);
		glVertex2f(x2,y1);
		glTexCoord2f(1.0,1.0);
		glVertex2f(x2,y2);
		glTexCoord2f(0.0,1.0);
		glVertex2f(x1,y2);
	glEnd();
}


void	glDrawBoundingBox(float x,float y,float z)
{

	glBegin(GL_LINE_STRIP);
		glVertex3f(0,0,0);
		glVertex3f(x,0,0);
		glVertex3f(x,y,0);
		glVertex3f(0,y,0);
		glVertex3f(0,0,0);

		glVertex3f(0,0,z);
		glVertex3f(x,0,z);
		glVertex3f(x,y,z);
		glVertex3f(0,y,z);
		glVertex3f(0,0,z);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(x,0,0);
		glVertex3f(x,0,z);

		glVertex3f(x,y,0);
		glVertex3f(x,y,z);

		glVertex3f(0,y,0);
		glVertex3f(0,y,z);

	glEnd();
}



#ifndef	__WIN32__

void printShaderInfoLog(int obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(int obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

#endif
