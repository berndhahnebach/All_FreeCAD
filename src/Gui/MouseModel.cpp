
// Handling for precompiled headers
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qevent.h>
#	include <assert.h>
#endif

#include "MouseModel.h"
#include "View3D.h"
 

View3D            &FCMouseModel::GetView3D(void) 
{ 
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return *_pcView3D;
}

Handle_V3d_View   &FCMouseModel::GetView  (void) 
{ 
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return _pcView3D->GetView();
}

Handle_V3d_Viewer &FCMouseModel::GetViewer(void) 
{ 
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return _pcView3D->GetViewer();
}

Handle_AIS_InteractiveContext &FCMouseModel::GetContext(void)
{
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return _pcView3D->GetContext();
}


// **** MouseModelStd ********************************************************

// Buttons to use
#define OCC_ROTATION 		5 		// MidButton & RightButton
#define OCC_PANNING  		4		// MidButton
#define OCC_ZOOM     		6		// MidButton & LeftButton
#define OCC_SELECTION 		1		// LeftButton
//#define OCC_ADDSELECTION 	9		// LeftButton & ShiftButton
#define OCC_ADDSELECTION 	17		// LeftButton & CtrlButton
#define OCC_SHOWPOPUP       2		// LeftButton

void FCMouseModelStd::mousePressEvent( QMouseEvent *cEvent)
{
	iX = cEvent->x();
	iY = cEvent->y();
	//cout << "View3D: MousePress:"<< cEvent->state() +  cEvent->button()<<endl;
	switch(cEvent->state() + cEvent->button())
	{
		case OCC_ROTATION:
			GetView()->StartRotation(iX,iY);
			mode = rotation;
			break;
		case OCC_PANNING:
			mode = panning;
			break;
		case OCC_ZOOM:
			mode = zooming;
			break;
		case OCC_SELECTION:
			mode = selection;
			break;
		case OCC_ADDSELECTION:
			mode = addselection;
			break;
		case OCC_SHOWPOPUP:
			GetView3D().ShowPopup(iX,iY);
			break;
		default:
			mode = nothing;
	}
	//cout << "View3D: Mode:"<< (int) mode <<endl;
}

void FCMouseModelStd::mouseReleaseEvent( QMouseEvent *cEvent)
{
	//cout << "View3D: MouseRelease" <<endl;
	switch(mode)
	{
		case selection:
			GetContext()->MoveTo(cEvent->x(),cEvent->x(),GetView());
			GetContext()->Select();
			break;
		case addselection:
			GetContext()->MoveTo(cEvent->x(),cEvent->x(),GetView());
			GetContext()->ShiftSelect();
			break;
		default:
			;	
	}
	mode = nothing;
	
}

void FCMouseModelStd::mouseMoveEvent( QMouseEvent *cEvent)
{
#	ifndef WNT
		QApplication::flushX();
#	endif

	//cout << "View3D: MouseMove" <<endl;
	switch(mode)
	{
		case nothing:
			break;
		case rotation:
			GetView()->Rotation(cEvent->x(),cEvent->y());
			break;
		case panning:
		  //myView->Panning(cEvent->x()-iX,iY-cEvent->y()); // Dont work proper ?!?!
		  // get width size in Window system
		  V3d_Coordinate xc,yc;
		  GetView()->Center(xc,yc);	
		  Standard_Integer Xc,Yc;
		  GetView()->Convert(xc,yc,Xc,Yc);
		  // set the new center
		  GetView()->SetCenter(Xc - (cEvent->x()-iX),Yc - (cEvent->y()-iY));

			break;
		case zooming:
			GetView()->Zoom(iY,iX,cEvent->y(),cEvent->x());
			break;
		default:
			;
	}
	GetContext()->MoveTo(cEvent->x(),cEvent->y(),GetView());
	
	iX = cEvent->x();
	iY = cEvent->y();

}

void FCMouseModelStd::wheelEvent ( QWheelEvent * cEvent)
{
	int zDelta = cEvent->delta();
	Quantity_Length fWidth, fHeight;
	GetView()->Size(fWidth, fHeight);
	float fLog = float(log10(fWidth));
	int   nExp = int(fLog);

	// Zoom begrenzen
	if ((nExp > -6 && zDelta > 0) || (nExp < 8 && zDelta < 0))
	{
		GetView()->Zoom(0,0,zDelta,0);
	}
	else if (zDelta > 0)
	{
		//GetConsole().Message("Cannot zoom in any more\n");
	}
	else
	{
		//GetConsole().Message("Cannot zoom out any more\n");
	}
}


void FCMouseModelStd::mouseDoubleClickEvent ( QMouseEvent * )
{


}

void FCMouseModelStd::keyPressEvent ( QKeyEvent * )
{


}

void FCMouseModelStd::keyReleaseEvent ( QKeyEvent * )
{


}

