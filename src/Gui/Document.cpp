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
 

#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qstatusbar.h>
#	ifndef FC_OS_WIN32
#		include <Graphic3d_GraphicDevice.hxx>
#	else
#		include <Graphic3d_WNTGraphicDevice.hxx>
#	endif
#endif


#include "../App/Document.h"
#include "Application.h"
#include "Document.h"
#include "View3D.h"



FCGuiDocument::FCGuiDocument(FCDocument* pcDocument,ApplicationWindow * app, const char * name)
	:_pcAppWnd(app),
	 _pcDocument(pcDocument),
	 _iWinCount(0)
{
	// keeping an Instance of this document as long as at least one window lives
	_pcDocument->_INCREF();

	Handle(TDocStd_Document) hcOcafDoc = pcDocument->GetOCCDoc();

	// seting up a new Viewer +++++++++++++++++++++++++++++++++++++++++++++++
	TCollection_ExtendedString a3DName("Visu3D");
	_hViewer = Viewer(getenv("DISPLAY"),
		              a3DName.ToExtString(),
					  "",
					  1000.0,
					  V3d_XposYnegZpos,
					  Standard_True,
					  Standard_True);
	TPrsStd_AISViewer::New(hcOcafDoc->Main(),_hViewer);

    _hViewer->Init();
	_hViewer->SetDefaultLights();
	_hViewer->SetLightOn();

	// seting up a new interactive context +++++++++++++++++++++++++++++++++++++++++++++++
	//_hContext =new AIS_InteractiveContext(_hViewer);
	TPrsStd_AISViewer::Find(hcOcafDoc->Main(), _hContext);
	_hContext->SetDisplayMode(AIS_Shaded);

	// World coordinate system
	Handle(AIS_Trihedron) hTrihedron;
	hTrihedron = new AIS_Trihedron(new Geom_Axis2Placement(gp::XOY()));
	_hContext->Display(hTrihedron);
	_hContext->Deactivate(hTrihedron);

	// alwayes create at least one view
	CreateView("View3D");

}

FCGuiDocument::~FCGuiDocument()
{
	for(std::list<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end() ;It++) 
		delete *It;

	// remove the reverence from the object
	_pcDocument->_DECREF();
}


/// Save the document
void FCGuiDocument::Save(void)
{
	if(_pcDocument->IsSaved())
		GetDocument()->Save();
	else
		SaveAs();

}

/// Save the document under a new file name
void FCGuiDocument::SaveAs(void)
{
	GetAppWnd()->statusBar()->message("Saving file under new filename...");
	QString fn = QFileDialog::getSaveFileName(0, "FreeCAD (*.FCStd *.FCPart)", GetAppWnd());
	if (!fn.isEmpty())
	{

		GetDocument()->SaveAs(fn.latin1());
	}
	else
	{
		GetAppWnd()->statusBar()->message("Saving aborted", 2000);
	}
}




#include "Icons/FCIcon.xpm"

void FCGuiDocument::CreateView(const char* sType) 
{

    FCView3D* w = new FCView3D(this,0L,"View3D");
	
	// add to the view list of document
	_LpcViews.push_back(w);
	// add to the view list of Application window

	//connect( w, SIGNAL( message(const QString&, int) ), _pcAppWnd->statusBar(), SLOT( message(const QString&, int )) );
	//connect( w, SIGNAL(sendCloseView(FCView*)),this,SLOT(onCloseView(FCView*)));
	connect( w, SIGNAL(sendCloseView(FCView*)),this,SLOT(slotCloseView(FCView*)));

	QString aName;
//	aName.sprintf("%s:%d",_pcDocument->GetName(),_iWinCount++);
	aName.sprintf("%s:%d","Document",_iWinCount++);

	FCSingelView* pcSingelView = new FCSingelView(w,_pcAppWnd,"3DView");

    pcSingelView->setCaption(aName);
	pcSingelView->setTabCaption(aName);
    pcSingelView->setIcon( FCIcon );
	pcSingelView->resize( 400, 300 );
    if ( _LpcViews.size() == 1 )
		_pcAppWnd->addWindow(pcSingelView,QextMdi::StandardAdd);
    else
		_pcAppWnd->addWindow(pcSingelView);

}

void FCGuiDocument::slotCloseView(FCView* theView)
{

	_LpcViews.remove(theView);
	// last view?
	if(_LpcViews.size() == 0)
	{
		OnLastViewClosed();
	}
}

void FCGuiDocument::OnLastViewClosed(void)
{

	_pcAppWnd->OnLastWindowClosed(this);
}

/** 
 *  This method check if the Document can close. It checks on 
 *  the save state of the document and is abel to abort the close!
 */
void FCGuiDocument::closeEvent ( QCloseEvent * e )
{
	if(! _pcDocument->IsSaved()
		&& _pcDocument->GetOCCDoc()->StorageVersion() < _pcDocument->GetOCCDoc()->Modifications() 
		&& _pcDocument->GetOCCDoc()->CanClose() == CDM_CCS_OK)
	{
		switch(QMessageBox::warning( _pcAppWnd, "Unsaved document","Save file bevore close?","Yes","No","Cancel",0,2))
		{
		case 0:
			//GetApplication().
			Save();
			e->accept();
			break;
		case 1:
			e->accept();
			break;
		case 2:
			break;
		}
	}else
		e->accept();

//	for(std::list<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end() ;It++) 
//		(*It)->childWindowCloseRequest(e);
//		(*It)->close(e);

}




/// send Messages to the active view
bool FCGuiDocument::SendMsgToViews(const char* pMsg)
{

	bool bResult = false;

	for(std::list<FCView*>::iterator It = _LpcViews.begin();It != _LpcViews.end();It++)
	{
		if( (*It)->OnMsg(pMsg))
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

/// send Messages to all views
bool FCGuiDocument::SendMsgToActiveView(const char* pMsg)
{

	if(_pcActiveView)
		return _pcActiveView->OnMsg(pMsg);
	else
		return false;
}



/// Geter for the Active View
FCView* FCGuiDocument::GetActiveView(void)
{
	return _pcAppWnd->GetActiveView();
}


//--------------------------------------------------------------------------
// UNDO REDO transaction handling  
//--------------------------------------------------------------------------
/** Open a new Undo transaction on the active document
 *  This methode open a new UNDO transaction on the active document. This transaction
 *  will later apear in the UNDO REDO dialog with the name of the command. If the user 
 *  recall the transaction everything changed on the document between OpenCommand() and 
 *  CommitCommand will be undone (or redone). You can use an alternetive name for the 
 *  operation default is the Command name.
 *  @see CommitCommand(),AbortCommand()
 */
void FCGuiDocument::OpenCommand(const char* sName)
{
	// check on double open Commands
	assert(!GetDocument()->HasOpenCommand());

	listUndoNames.push_back(sName);

	GetDocument()->NewCommand();
	
}

void FCGuiDocument::CommitCommand(void)
{
	GetDocument()->CommitCommand();	
}

void FCGuiDocument::AbortCommand(void)
{
	listUndoNames.pop_back();

	GetDocument()->AbortCommand();	
}

/// Get an Undo string vector with the Undo names
std::vector<std::string> FCGuiDocument::GetUndoVector(void)
{
	std::vector<std::string> vecTemp;

	//std::copy(listUndoNames.begin(),listUndoNames.end(),vecTemp.begin());

	for(std::list<std::string>::iterator It=listUndoNames.begin();It!=listUndoNames.end();It++)
		vecTemp.push_back(*It);

	return vecTemp;
}

/// Get an Redo string vector with the Redo names
std::vector<std::string> FCGuiDocument::GetRedoVector(void)
{
	std::vector<std::string> vecTemp;

	std::copy(listRedoNames.begin(),listRedoNames.end(),vecTemp.begin());

	return vecTemp;
}

/// Will UNDO  one or more steps
void FCGuiDocument::Undo(int iSteps)
{

	for (int i=0;i<iSteps;i++)
	{
		GetDocument()->Undo();
	}

}

/// Will REDO  one or more steps
void FCGuiDocument::Redo(int iSteps)
{
	for (int i=0;i<iSteps;i++)
	{
		GetDocument()->Undo();
	}

}




Handle(V3d_Viewer) FCGuiDocument::Viewer(const Standard_CString aDisplay,
										 const Standard_ExtString aName,
										 const Standard_CString aDomain,
										 const Standard_Real ViewSize,
										 const V3d_TypeOfOrientation ViewProj,
										 const Standard_Boolean ComputedMode,
										 const Standard_Boolean aDefaultComputedMode )
{
#	ifndef FC_OS_WIN32
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
#	endif  // FC_OS_WIN32
}



#include "moc_Document.cpp"