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

#include "PreCompiled.h"

#ifndef _PreComp_
#	include <iostream>
#	include <stdio.h>
#	include <stack>
#	include <Standard_Failure.hxx>
#	include <qapplication.h>
#	include <qaction.h>
#	include <qcursor.h>
#	include <qfiledialog.h>
#	include <qmessagebox.h>
#	include <qpopupmenu.h>
#	include <qvbox.h>
#	include <qsplitter.h>
#	include <qtabbar.h>
#	include <qthread.h>
#endif

#include "Application.h"
#include "Command.h"
#include "View3D.h"
#include "MouseModel.h"
#include "Document.h"
//#include "Tree.h"

using namespace Gui;

// global graphic device (set in DocWindow)
#ifdef FC_OS_WIN32
	extern Handle_Graphic3d_WNTGraphicDevice	hGraphicDevice;
#else
	extern Handle_Graphic3d_GraphicDevice		hGraphicDevice;
#endif



FCView3D::FCView3D( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags )
    :MDIView( pcDocument,parent, name, wflags)
{
	_pcView3D = new View3D(_pcDocument,this);

  setFocusProxy( _pcView3D );
  setCentralWidget( _pcView3D );
}

FCView3D::~FCView3D()
{
  delete _pcView3D;
}

/*

void FCView3D::closeEvent(QCloseEvent* e)
{

//  int iButton = QMessageBox::warning(this, "FreeCAD", "Save changes to file?", "Yes", "No", "Cancel", 0);
//  if (iButton != 1) // yes/cancel
//    return;


	emit sendCloseView(this);
//	QextMdiChildView::closeEvent(e);
}
*/
const char *FCView3D::GetName(void)
{
	return "View3D";
}

bool FCView3D::OnMsg(const char* pMsg)
{
	//printf("Msg: %s View: %p\n",pMsg,this);

	if (_pcView3D->OnMsg(pMsg)) return true;
//	if (_pcTree->OnMsg(pMsg))   return true;
//assert(0);
	return false;
}


void FCView3D::Update(void)
{
  _pcView3D->GetView()->Redraw();
}

/*

void MDIView::fitAll()
{
	myView->fitAll();
}


*/
void FCView3D::onWindowActivated ()
{
  //myOperations->onSelectionChanged();
}

void FCView3D::setCursor(const QCursor& aCursor)
{
  _pcView3D->setCursor(aCursor);
}

void FCView3D::dump()
{
	static QString filter = "Images Files (*.bmp *.gif *.xwd)";
	QFileDialog fd ( 0, 0, true );     
	fd.setFilters( filter );
	fd.setCaption ( "INF_APP_IMPORT" );
	int ret = fd.exec(); 
    
	/* update the desktop after the dialog is closed */
	qApp->processEvents();

	QString file (ret == QDialog::Accepted ? fd.selectedFile() : QString::null);

	if ( !file.isNull() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  if(!_pcView3D->ScreenDump( (const Standard_CString) file.latin1())) {
	    QApplication::restoreOverrideCursor();
	    QMessageBox::information ( qApp->mainWidget(),"TIT_ERROR", "INF_ERROR", "BTN_OK",
		  		       QString::null, QString::null, 0, 0);
	    qApp->processEvents();  /* update desktop */	
	  } else 
	    QApplication::restoreOverrideCursor();
	}
}







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
  setFocusPolicy (ClickFocus);
}

bool View3D::InitCasCadeView(void)
{
    _hView = new V3d_OrthographicView(_hViewer);
	// Now some window specific tasks		
	try{
		// create a new OCC Window from the OCC Graphic device and the win ID of the QT OpenGL Render Window
#		ifdef FC_OS_WIN32
			Handle(WNT_Window) hWindow = new WNT_Window(Handle(Graphic3d_WNTGraphicDevice)::DownCast(_hContext->CurrentViewer()->Device()),winId());//WNT_GraphicDevice
#		else
			Handle(Xw_Window) hWindow  = new Xw_Window (Handle(Graphic3d_GraphicDevice)::DownCast(_hContext->CurrentViewer()->Device()),winId());
#		endif
		// map the window ( can crash when OGL setings and Viewer setings not the same)
		_hView->SetWindow(hWindow);
		if (!hWindow->IsMapped()) hWindow->Map();
	}catch(Standard_Failure){
		cerr << "Failure caught:"<<Standard_Failure::Caught()<<endl;
		return false;
	}

	_hView->SetDegenerateModeOn();
	_hView->MustBeResized();
	_hView->Update();
	_hView->SetAntialiasingOn();
//	_hView->SetBackgroundColor(Quantity_NOC_DARKKHAKI);
//	_hView->SetBackgroundColor(Quantity_NOC_DARKOLIVEGREEN);
//	_hView->SetBackgroundColor(Quantity_NOC_DARKORCHID);
//	_hView->SetBackgroundColor(Quantity_NOC_DARKORCHID);
//	_hView->SetBackgroundColor(Quantity_NOC_DARKSEAGREEN);
	_hView->SetBackgroundColor(Quantity_NOC_BLACK);


	// pushing the standard mouse model
	PushMouseModel(new FCMouseModelStd);

   	bIsInit = true;
	
    return true;
}


void View3D::mousePressEvent		( QMouseEvent *cEvent){_cMouseStack.top()->mousePressEvent( cEvent);      }
void View3D::mouseReleaseEvent		( QMouseEvent *cEvent){_cMouseStack.top()->mouseReleaseEvent( cEvent);    }
void View3D::mouseDoubleClickEvent	( QMouseEvent *cEvent){_cMouseStack.top()->mouseDoubleClickEvent( cEvent);}
void View3D::keyPressEvent			( QKeyEvent	  *cEvent){_cMouseStack.top()->keyPressEvent( cEvent);        }
void View3D::keyReleaseEvent		( QKeyEvent   *cEvent){_cMouseStack.top()->keyReleaseEvent( cEvent);      }
void View3D::focusInEvent			( QFocusEvent *cEvent){ShowDimension();                                   }
void View3D::wheelEvent				( QWheelEvent *cEvent){_cMouseStack.top()->wheelMouseEvent( cEvent);}
void View3D::mouseMoveEvent			( QMouseEvent *cEvent)
{ 
	QApplication::flushX();
	_cMouseStack.top()->moveMouseEvent( cEvent);
}

void View3D::hideEvent ( QHideEvent * cEvent )
{
	ApplicationWindow::Instance->setPaneText(2, QString(" Dimension"));
}
 
bool View3D::OnMsg(const char* pMsg)
{
	if(strcmp("ViewBottom",pMsg) == 0 ){
		_hView->SetProj(V3d_Zneg);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewFront",pMsg) == 0 ){
		_hView->SetProj(V3d_Yneg);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewLeft",pMsg) == 0 ){
		_hView->SetProj(V3d_Xpos);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewRear",pMsg) == 0 ){
		_hView->SetProj(V3d_Ypos);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewRight",pMsg) == 0 ){
		_hView->SetProj(V3d_Xneg);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewTop",pMsg) == 0 ){
		_hView->SetProj(V3d_Zpos);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewAxo",pMsg) == 0 ){
		_hView->SetProj(V3d_XposYnegZpos);
		//_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}else if(strcmp("ViewFit",pMsg) == 0 ){
		_hView->FitAll();
		_hView->ZFitAll();
		return true;
	}
	return false;
}

void View3D::ShowPopup(int x, int y)
{
	CommandManager &rcCmdMgr = ApplicationWindow::Instance->GetCommandManager();

	QPopupMenu *pcMenu = new QPopupMenu;
	QPopupMenu *pcSubMenu = new QPopupMenu;

	// view menu -----------------------------------------------------------------------
	rcCmdMgr.addTo("Std_ViewFitAll"  ,pcSubMenu);
	rcCmdMgr.addTo("Std_ViewAxo"     ,pcSubMenu);
	pcSubMenu->insertSeparator();
	rcCmdMgr.addTo("Std_ViewFront"   ,pcSubMenu);
	rcCmdMgr.addTo("Std_ViewRight"   ,pcSubMenu);
	rcCmdMgr.addTo("Std_ViewTop" ,pcSubMenu);
	pcSubMenu->insertSeparator();
	rcCmdMgr.addTo("Std_ViewRear"  ,pcSubMenu);
	rcCmdMgr.addTo("Std_ViewLeft"  ,pcSubMenu);
	rcCmdMgr.addTo("Std_ViewBottom",pcSubMenu);

	
	// Popup menu -----------------------------------------------------------------------

	rcCmdMgr.addTo("Std_ViewFitAll",pcMenu);

	pcMenu->insertItem("Std Views",pcSubMenu);
	
	// show the menu	
	pcMenu->exec(QCursor::pos());
}

void View3D::ShowDimension (void) const
{
	if ( _hView.IsNull() )
    return; // no valid view

  Quantity_Length fWidth, fHeight;
	_hView->Size(fWidth, fHeight);

	float fLog = float(log10(fWidth)), fFactor;
	int   nExp = int(fLog);
	char  szDim[20];

	if (nExp >= 6)
	{
		fFactor = 1.0e+6f;
		strcpy(szDim, "km");
	}
	else if (nExp >= 3)
	{
		fFactor = 1.0e+3f;
		strcpy(szDim, "m");
	}
	else if ((nExp >= 0) && (fLog > 0.0f))
	{
		fFactor = 1.0e+0f;
		strcpy(szDim, "mm");
	}
	else if (nExp >= -3)
	{
		fFactor = 1.0e-3f;
		strcpy(szDim, "um");
	}
	else 
	{
		fFactor = 1.0e-6f;
		strcpy(szDim, "nm");
	}

	char szSize[100];
	sprintf(szSize, " %.2f x %.2f %s", fWidth / fFactor, fHeight / fFactor, szDim);
  
	ApplicationWindow::Instance->setPaneText(2, QString(szSize));
}


void View3D::paintEvent (QPaintEvent * cEvent)
{
	if(!bIsInit)
		InitCasCadeView();

	if (!_hView.IsNull())
		_hView->Redraw();

	// do this repaint last 
	if (_cMouseStack.size() > 0)
		_cMouseStack.top()->paintEvent( cEvent);
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

	// do this repaint last 
	if (_cMouseStack.size() > 0)
		_cMouseStack.top()->resizeEvent( e );
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

	
#include "moc_View3D.cpp"



