/***************************************************************************
                          3DView.cpp  -  description
                             -------------------
    begin                : Mon Jan 1 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <iostream>
#	include <stdio.h>
#	include <Standard_Failure.hxx>
#	include <qapplication.h>
#	include <qpopupmenu.h>
#endif

#include "View3D.h"
#include "MouseModel.h"
#include "Document.h"


// global graphic device (set in DocWindow)
#ifdef WNT
	extern Handle_Graphic3d_WNTGraphicDevice	hGraphicDevice;
#else
	extern Handle_Graphic3d_GraphicDevice		hGraphicDevice;
#endif




View3D::View3D(FCGuiDocument*  pcDocument,
			   QWidget *parent, 
			   const char *name, 
			   WFlags f)
		    :QGLWidget(QGLFormat(
								 QGL::DoubleBuffer|
								 QGL::DepthBuffer|
                                 QGL::DirectRendering
								 ),
                       parent,name,0,f),
			 _pcDocument(pcDocument)

{
	setMouseTracking(true);
	bIsInit         = false;

	// OCC init
    _hContext       = pcDocument->GetContext();
	_hViewer        = _hContext->CurrentViewer();
    // Creating a new Ortographic View with the Viewer from the document

	// rest of the init for OCC is done after the Window is 
	// created (show()) in InitCasCadeView()
}

bool View3D::InitCasCadeView(void)
{
    _hView = new V3d_OrthographicView(_hViewer);
	// Now some window specific tasks		
	try{
		// create a new OCC Window from the OCC Graphic device and the win ID of the QT OpenGL Render Window
#		ifdef WNT
			Handle(WNT_Window) hWindow = new WNT_Window(Handle(Graphic3d_WNTGraphicDevice)::DownCast(_hContext->CurrentViewer()->Device()),winId());//WNT_GraphicDevice
#		else
			Handle(Xw_Window) hWindow  = new Xw_Window (Handle(Graphic3d_GraphicDevice)::DownCast(_hContext->CurrentViewer()->Device()),winId());
#		endif
		// map the window ( can crash when OGL setings and Viewer setings not the same)
		_hView->SetWindow(hWindow);
		if (!hWindow->IsMapped()) hWindow->Map();
	}catch(Standard_Failure){
		cerr << "Failure catched:"<<Standard_Failure::Caught()<<endl;
		return false;
	}

	_hView->SetDegenerateModeOn();
	_hView->MustBeResized();
	_hView->Update();

	// pushing the standard mouse model
	PushMouseModel(new FCMouseModelStd);

   	bIsInit = true;
	
    return true;
}


void View3D::mousePressEvent		( QMouseEvent *cEvent){_cMouseStack.top()->mousePressEvent( cEvent);}
void View3D::mouseReleaseEvent		( QMouseEvent *cEvent){_cMouseStack.top()->mouseReleaseEvent( cEvent);}
void View3D::mouseDoubleClickEvent	( QMouseEvent *cEvent){_cMouseStack.top()->mouseDoubleClickEvent( cEvent);}
void View3D::keyPressEvent			( QKeyEvent	  *cEvent){_cMouseStack.top()->keyPressEvent( cEvent);}
void View3D::keyReleaseEvent		( QKeyEvent   *cEvent){_cMouseStack.top()->keyReleaseEvent( cEvent);}
void View3D::wheelEvent             ( QWheelEvent *cEvent){_cMouseStack.top()->wheelEvent( cEvent);}
void View3D::mouseMoveEvent			( QMouseEvent *cEvent)
{ 
	QApplication::flushX(); 
	_cMouseStack.top()->mouseMoveEvent( cEvent);
}
 

void View3D::ShowPopup(int x, int y)
{
	//cout << "View3D: ShowPopup:"<< x << "  " << y <<endl;
	QPopupMenu *pcMenu = new QPopupMenu;
	QPopupMenu *pcSubMenu = new QPopupMenu;
	pcSubMenu->insertItem("Front",    this,SLOT(SetViewFront()) );
	pcSubMenu->insertItem("Top",      this,SLOT(SetViewTop())   );
	pcSubMenu->insertItem("Right",    this,SLOT(SetViewRight()) );
	pcSubMenu->insertItem("Rear",     this,SLOT(SetViewRear())  );
	pcSubMenu->insertItem("Bottom",   this,SLOT(SetViewBottom()));
	pcSubMenu->insertItem("Left",     this,SLOT(SetViewLeft())  );
	pcSubMenu->insertItem("Axometric",this,SLOT(SetViewAxo())   );
	pcMenu->insertItem("Fit all",     this,SLOT(SetViewFitAll()));
	pcMenu->insertItem("Std Views",pcSubMenu);
	
	pcMenu->exec(QCursor::pos());
}


void View3D::paintEvent (QPaintEvent * )
{

	if(!bIsInit)
		InitCasCadeView();

	if (!_hView.IsNull())
		_hView->Redraw();
}

/** Update the view when resize event occur.  */
void View3D::resizeEvent (QResizeEvent * e)
{
	if(bIsInit){
		if (!_hView.IsNull()){
			QApplication::flushX();
			_hView->MustBeResized();
		}
	}
}

// Managing MouseModels
void View3D::PushMouseModel	(FCMouseModel *pcModel)
{
	pcModel->initMouseModel(this);
	_cMouseStack.push(pcModel);

}

void View3D::PopMouseModel	(void)
{
	// preserve std. mousemodel from poping
	if(_cMouseStack.size() > 1)
	{
		_cMouseStack.top()->releaseMouseModel();
		_cMouseStack.pop();
	}
}

FCMouseModel* View3D::GetMouseModel	(void)
{
	return _cMouseStack.top();
}

bool View3D::ScreenDump(Standard_CString theFile)
{
	_hView->Redraw();
	return _hView->Dump(theFile);
}

	




