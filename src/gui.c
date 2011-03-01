#include	"mainwin.h"
#include	"propertieswin.h"
#include	"aa2Dslicewin.h"
#include	"aa3Dtexwin.h"
#include	"aa3Dtexctrlwin.h"
#include	"va3Dtexwin.h"
#include	"va3Dtexctrlwin.h"
#include	"isosurfacewin.h"
#include	"isosurfacectrlwin.h"

#include	"etf1d.h"
#include	"etf2d.h"

#include	"gui.h"
#include	"glext.h"

void	tfSetupGUI(void)
{
	//	Create GTK windows

	tfCreateMainwin();
	tfCreateAA2DSlicewin();
	tfCreateAA3DTexwin();
	tfCreateAA3DTexctrlwin();
	tfCreateVA3DTexwin();
	tfCreateVA3DTexctrlwin();
	tfCreateIsosurfacewin();
	tfCreateIsosurfacectrlwin();
	tfCreatePropertieswin();



	//	Set windows' properties and bindings among procedures and signals

	tfSetupMainwin();
	tfSetupPropertieswin();
	tfSetupAA3DTexctrlwin();
	tfSetupVA3DTexctrlwin();
	tfSetupAA2DSlicewin();
	tfSetupAA3DTexwin();
	tfSetupVA3DTexwin();
	tfSetupIsosurfacewin();
	tfSetupIsosurfacectrlwin();



	//	Create transfer functions' panels

	tfCreateETF1Dpanel(tfCreateTFpanel(_("Enhanced 1D Transfer Function")));
	tfCreateETF2Dpanel(tfCreateTFpanel(_("Enhanced 2D Transfer Function")));
	tfSetupETF1Dpanel();
	tfSetupETF2Dpanel();


	gtk_combo_box_set_active(GTK_COMBO_BOX(Mainwin.tf_selector),0);


}

