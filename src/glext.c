#ifdef	__WIN32__

#include "glext.h"
#include <windows.h>


PFNGLCREATEPROGRAMPROC		glCreateProgram;
PFNGLCREATESHADERPROC		glCreateShader;
PFNGLATTACHOBJECTARBPROC		glAttachObjectARB;
PFNGLSHADERSOURCEPROC		glShaderSource;
PFNGLCOMPILESHADERPROC		glCompileShader;
PFNGLLINKPROGRAMPROC			glLinkProgram;
PFNGLUSEPROGRAMPROC			glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC		glGetUniformLocation;
PFNGLUNIFORM1IPROC			glUniform1i;
PFNGLUNIFORM1IVPROC			glUniform1iv;
PFNGLUNIFORM1FPROC			glUniform1f;
PFNGLUNIFORM3FPROC			glUniform3f;
PFNGLUNIFORM3FVPROC			glUniform3fv;
PFNGLGETOBJECTPARAMETERIVARBPROC	glGetObjectParameterivARB;
PFNGLACTIVETEXTUREPROC		glActiveTexture;
PFNGLTEXIMAGE3DPROC			glTexImage3D;

void	glInitExtensions(void)
{
	glCreateProgram		=(PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glCreateShader		=(PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glAttachObjectARB		=(PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
	glShaderSource		=(PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader		=(PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glLinkProgram			=(PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glUseProgram			=(PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glGetUniformLocation		=(PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i			=(PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv			=(PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform1f			=(PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform3f			=(PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
	glUniform3fv			=(PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glGetObjectParameterivARB	=(PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
	glActiveTexture		=(PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glTexImage3D			=(PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
}

#endif
