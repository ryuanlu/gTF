#include	<gtk/gtk.h>
#include	<gtk/gtkgl.h>
#include	<locale.h>
#include	<memory.h>

#include	"gTF.h"
#include	"gTFconf.h"
#include	"gui.h"



int main(int argc,char** argv)
{
	//	arguments parsing
	//	initializing

	gtk_init(&argc,&argv);
	gtk_gl_init(&argc, &argv);

	memset(&gtftable,0,sizeof(gTFTable));

	//	Locale initialization

	//setenv("LANGUAGE","en",TRUE);

#ifdef	__WIN32__
	bindtextdomain("gtk20","../share/locale");
	bindtextdomain("gtf","../share/locale");
#else
	bindtextdomain("gtk20",DATADIR"/../locale");
	bindtextdomain("gtf",DATADIR"/../locale");
#endif
	//


	//	GTK GUI construction

	tfSetupGUI();

	//

	gtk_main();	//	GTK main loop

	return 0;
}

