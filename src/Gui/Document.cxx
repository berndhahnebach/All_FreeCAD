
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qstatusbar.h>
#endif



#include "Application.h"
#include "Document.h"
#include "View.h"

#include <qstatusbar.h>

#ifndef WNT
#include <Graphic3d_GraphicDevice.hxx>
#else
#include <Graphic3d_WNTGraphicDevice.hxx>
#endif


FCGuiDocument::FCGuiDocument(FCDocument* pcDocument,ApplicationWindow * app, const char * name)
	:_pcAppWnd(app),
	 _pcDocument(pcDocument)
{

	TCollection_ExtendedString a3DName("Visu3D");
	_hViewer = Viewer(getenv("DISPLAY"),a3DName.ToExtString(),"",1000.0,V3d_XposYnegZpos,Standard_True,Standard_True);

    _hViewer->Init();
	_hViewer->SetDefaultLights();
	_hViewer->SetLightOn();

	_hContext =new AIS_InteractiveContext(_hViewer);

	// alwayes create at least one view
	CreateView();

}

FCGuiDocument::~FCGuiDocument()
{
	for(stlport::list<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end() ;It++) 
		delete *It;
}


void FCGuiDocument::CreateView() 
{
	QWorkspace* ws = _pcAppWnd->getWorkspace();

    FCView* w = new FCView(this,ws,"View");
	_LpcViews.push_back(w);

	//connect( w, SIGNAL( message(const QString&, int) ), _pcAppWnd->statusBar(), SLOT( message(const QString&, int )) );
	//connect( w, SIGNAL(sendCloseView(FCView*)),this,SLOT(onCloseView(FCView*)));

	QString aName;
    w->setCaption(aName.sprintf("Document 1:1"));

    //QString dir = ApplicationWindow::getResourceDir();
	
    //w->setIcon( QPixmap(dir+tr("ICON_DOC")) );
    // show the very first window in maximized mode

	w->resize( 400, 300 );
    if ( ws->windowList().isEmpty() )
		w->showMaximized();
    else
		w->show();
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
