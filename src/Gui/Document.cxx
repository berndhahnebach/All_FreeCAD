/** \file $RCSfile$
 *  \brief The Gui Document
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 

#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qstatusbar.h>
#	ifndef WNT
#		include <Graphic3d_GraphicDevice.hxx>
#	else
#		include <Graphic3d_WNTGraphicDevice.hxx>
#	endif
#endif


#include "../App/Document.h"
#include "Application.h"
#include "Document.h"
#include "View.h"



FCGuiDocument::FCGuiDocument(FCDocument* pcDocument,ApplicationWindow * app, const char * name)
	:_pcAppWnd(app),
	 _pcDocument(pcDocument)
{
	// keeping an Instance of this document as long as the window lives
	_pcDocument->_INCREF();

	TCollection_ExtendedString a3DName("Visu3D");
	_hViewer = Viewer(getenv("DISPLAY"),a3DName.ToExtString(),"",1000.0,V3d_XposYnegZpos,Standard_True,Standard_True);

    _hViewer->Init();
	_hViewer->SetDefaultLights();
	_hViewer->SetLightOn();

	_hContext =new AIS_InteractiveContext(_hViewer);

	// World coordinate system
	Handle(AIS_Trihedron) hTrihedron;
	hTrihedron = new AIS_Trihedron(new Geom_Axis2Placement(gp::XOY()));
	_hContext->Display(hTrihedron);
	_hContext->Deactivate(hTrihedron);

	// alwayes create at least one view
	CreateView();

}

FCGuiDocument::~FCGuiDocument()
{
	for(stlport::list<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end() ;It++) 
		delete *It;

	_pcDocument->_DECREF();
}

#include "Icons/FCIcon.xpm"

void FCGuiDocument::CreateView() 
{

    FCView* w = new FCView(this,0L,"View");
	
	// add to the view list of document
	_LpcViews.push_back(w);
	// add to the view list of Application window

	//connect( w, SIGNAL( message(const QString&, int) ), _pcAppWnd->statusBar(), SLOT( message(const QString&, int )) );
	//connect( w, SIGNAL(sendCloseView(FCView*)),this,SLOT(onCloseView(FCView*)));

	QString aName;
    w->setCaption(aName.sprintf("Document 1:1"));
	
    w->setIcon( FCIcon );
    // show the very first window in maximized mode

	w->resize( 400, 300 );
    if ( _LpcViews.size() == 1 )
		_pcAppWnd->addWindow(w,QextMdi::StandardAdd);
    else
		_pcAppWnd->addWindow(w);

}
/*
void FCGuiDocument::onCloseView(MDIWindow* theView)
{

  
  if(countOfWindow() == 1)
    emit sendCloseFCGuiDocument(this);
  else
    removeView(theView);
}

void FCGuiDocument::removeView(MDIWindow* theView)
{
	myViews.removeRef(theView);
	delete theView;
}

int FCGuiDocument::countOfWindow()
{
	return myViews.count();
}

Handle(AIS_InteractiveContext) FCGuiDocument::getContext()
{
	return myContext;
}

void FCGuiDocument::fitAll()
{
	for(MDIWindow* aView = (MDIWindow*) myViews.first(); aView; aView = (MDIWindow*) myViews.next())
		aView->fitAll();
}
*/
Handle(V3d_Viewer) FCGuiDocument::Viewer(const Standard_CString aDisplay,
										 const Standard_ExtString aName,
										 const Standard_CString aDomain,
										 const Standard_Real ViewSize,
										 const V3d_TypeOfOrientation ViewProj,
										 const Standard_Boolean ComputedMode,
										 const Standard_Boolean aDefaultComputedMode )
{
#	ifndef WNT
		static Handle(Graphic3d_GraphicDevice) defaultdevice;

		if(defaultdevice.IsNull()) defaultdevice = new Graphic3d_GraphicDevice(aDisplay);
		return new V3d_Viewer(defaultdevice,aName,aDomain,ViewSize,ViewProj,
								Quantity_NOC_GRAY30,V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT,
								ComputedMode,aDefaultComputedMode,V3d_TEX_NONE);
#	else
		static Handle(Graphic3d_WNTGraphicDevice) defaultdevice;

		if(defaultdevice.IsNull()) defaultdevice = new Graphic3d_WNTGraphicDevice();
		return new V3d_Viewer(defaultdevice,aName,aDomain,ViewSize,ViewProj,
								Quantity_NOC_GRAY30,V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT,
								ComputedMode,aDefaultComputedMode,V3d_TEX_NONE);
#	endif  // WNT
}


/*

void FCGuiDocument::onWireframe()
{
	myOperations->onWireframe();
}

void FCGuiDocument::onShading()
{
	myOperations->onShading();
}

void FCGuiDocument::onColor()
{
	myOperations->onColor();
}

void FCGuiDocument::onMaterial(Standard_Integer theMaterial)
{
	myOperations->onMaterial(theMaterial);
}

void FCGuiDocument::onMaterial()
{
	myOperations->onMaterial();
}

void FCGuiDocument::onTransparency(Standard_Real theTrans)
{
	myOperations->onTransparency(theTrans);
}

void FCGuiDocument::onTransparency()
{
	myOperations->onTransparency();
}

void FCGuiDocument::onDelete()
{
	myOperations->onDelete();
}
*/
