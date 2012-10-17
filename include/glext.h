#ifndef GLEXT_H_
#define GLEXT_H_

#ifndef	__WIN32__
#define	GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef	__WIN32__

extern	PFNGLCREATEPROGRAMPROC		glCreateProgram;
extern	PFNGLCREATESHADERPROC		glCreateShader;
extern	PFNGLATTACHOBJECTARBPROC		glAttachObjectARB;
extern	PFNGLSHADERSOURCEPROC		glShaderSource;
extern	PFNGLCOMPILESHADERPROC		glCompileShader;
extern	PFNGLLINKPROGRAMPROC			glLinkProgram;
extern	PFNGLUSEPROGRAMPROC			glUseProgram;
extern	PFNGLGETUNIFORMLOCATIONPROC		glGetUniformLocation;
extern	PFNGLUNIFORM1IPROC			glUniform1i;
extern	PFNGLUNIFORM1IVPROC			glUniform1iv;
extern	PFNGLUNIFORM1FPROC			glUniform1f;
extern	PFNGLUNIFORM3FPROC			glUniform3f;
extern	PFNGLUNIFORM3FVPROC			glUniform3fv;
extern	PFNGLGETOBJECTPARAMETERIVARBPROC	glGetObjectParameterivARB;
extern	PFNGLACTIVETEXTUREPROC		glActiveTexture;
extern	PFNGLTEXIMAGE3DPROC			glTexImage3D;


void	glInitExtensions(void);

#endif

#endif /* GLEXT_H_ */
