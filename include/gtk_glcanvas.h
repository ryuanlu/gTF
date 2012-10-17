#ifndef GTK_GLCANVAS_H_
#define GTK_GLCANVAS_H_

#ifndef	__WIN32__
#define	GL_GLEXT_PROTOTYPES
#endif

#include <GL/glx.h>

typedef struct GtkGLCanvas* GtkGLCanvas;


GtkGLCanvas gtk_glcanvas_new(GtkWidget* widget, GLXContext sharelist);
void gtk_glcanvas_delete(GtkGLCanvas* glcanvas);

void glcanvas_make_current(GtkGLCanvas glcanvas, GLXContext context);
void glcanvas_swap_buffers(GtkGLCanvas glcanvas);
GLXContext glcanvas_get_render_context(GtkGLCanvas glcanvas);



#endif /* GTK_GLCANVAS_H_ */
