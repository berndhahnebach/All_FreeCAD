/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/
 

#include "PreCompiled.h"

#ifndef _PreComp_
# include <qfiledialog.h>
# include <qmessagebox.h>
# include <qstatusbar.h>
# include <AIS_Trihedron.hxx>
# include <Geom_Axis2Placement.hxx>
# include <TNaming_NamedShape.hxx>
# include <TPrsStd_AISViewer.hxx>
#endif


#include "../App/Document.h"
#include "Application.h"
#include "Document.h"
#include "View3D.h"
#include "View3DInventor.h"
#include "View3DInventorEx.h"
#include "BitmapFactory.h"

using namespace Gui;

int Document::_iDocCount = 0;

Document::Document(App::Document* pcDocument,ApplicationWindow * app, const char * name)
  :_iWinCount(1), _pcAppWnd(app), _pcDocument(pcDocument)
{
  // new instance
  _iDocId = (++_iDocCount);

  // keeping an Instance of this document as long as at least one window lives
  _pcDocument->IncRef();

  Handle(TDocStd_Document) hcOcafDoc = pcDocument->GetOCCDoc();

  _pcDocument->Attach(this);

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
  if(App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View")->GetBool("UseInventorViewer",false) )
    createView("View3DIv");
  else
    createView("");
}

Document::~Document()
{
//	for(std::list<MDIView*>::iterator It = _LpcViews.begin();It != _LpcViews.end() ;It++) 
//		delete *It;

  _pcDocument->Detach(this);

  // remove the reverence from the object
  _pcDocument->DecRef();
}

void Document::OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason)
{
  Base::Console().Log("Document::OnChange()");

  onUpdate();
}

/// Save the document
bool Document::save(void)
{
  if(_pcDocument->IsSaved())
  {
    getDocument()->Save();
    return true;
  }
  else
  {
    return saveAs();
  }
}

/// Save the document under a new file name
bool Document::saveAs(void)
{
  getAppWnd()->statusBar()->message(tr("Saving file under new filename..."));
  QString fn = QFileDialog::getSaveFileName(0, "FreeCAD (*.FCStd *.FCPart)", getAppWnd());
  if (!fn.isEmpty())
  {
    getDocument()->SaveAs(fn.latin1());
    return true;
  }
  else
  {
    getAppWnd()->statusBar()->message(tr("Saving aborted"), 2000);
    return false;
  }
}

void Document::createView(const char* sType) 
{
  QPixmap FCIcon = Gui::BitmapFactory().pixmap("FCIcon");
  MDIView* pcView3D;
  if(strcmp(sType,"View3DIv") == 0){
    pcView3D = new Gui::View3DInventorEx(this,_pcAppWnd,"View3DIv");
  }else{
    pcView3D = new MDIView3D(this,_pcAppWnd,"View3DOCC");
  }


  QString aName = tr("%1%2:%3").arg(tr("Unnamed Document")).arg(_iDocId).arg(_iWinCount++);


  pcView3D->setCaption(aName);
  pcView3D->setIcon( FCIcon );
  pcView3D->resize( 400, 300 );
  _pcAppWnd->addWindow(pcView3D);
}

void Document::attachView(Gui::BaseView* pcView, bool bPassiv)
{
  if(!bPassiv)
    _LpcViews.push_back(pcView);
  else
    _LpcPassivViews.push_back(pcView);
}

void Document::detachView(Gui::BaseView* pcView, bool bPassiv)
{
  if(bPassiv)
  {
    _LpcPassivViews.remove(pcView);
  }else{
    _LpcViews.remove(pcView);

  // last view?
  if(_LpcViews.size() == 0)
  {
    // decouple a passiv views
    std::list<Gui::BaseView*>::iterator It = _LpcPassivViews.begin();
    while (It != _LpcPassivViews.end())
    {
      (*It)->setDocument(0);
      It = _LpcPassivViews.begin();
    }

    _pcAppWnd->onLastWindowClosed(this);
    }
  }
}

void Document::onUpdate(void)
{


  TDF_Label L = _pcDocument->GetActive();

  if(! L.IsNull()){

  if(!_ActivePresentation.IsNull())
    _ActivePresentation->Display(0);
    //_hContext->Display(_ActivePresentation->GetAIS(),0);
  // Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
    if(!L.FindAttribute(TPrsStd_AISPresentation::GetID(),_ActivePresentation))
      _ActivePresentation = TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
    // Display it
    _ActivePresentation->Display(1);
    //_hContext->Display(_ActivePresentation->GetAIS(),1);
  }

  std::list<Gui::BaseView*>::iterator It;

  for(It = _LpcViews.begin();It != _LpcViews.end();It++)
  {
    (*It)->onUpdate();
  }

  for(It = _LpcPassivViews.begin();It != _LpcPassivViews.end();It++)
  {
    (*It)->onUpdate();
  }
}

bool Document::isLastView(void)
{
  if(_LpcViews.size() <= 1)
    return true;

  return false;
}

/** 
 *  This method check if the Document can close. It checks on 
 *  the save state of the document and is abel to abort the close!
 */
void Document::canClose ( QCloseEvent * e )
{
  if(! _pcDocument->IsSaved()
    && _pcDocument->GetOCCDoc()->StorageVersion() < _pcDocument->GetOCCDoc()->Modifications() 
    && _pcDocument->GetOCCDoc()->CanClose() == CDM_CCS_OK)
  {
#   ifndef FC_DEBUG
      switch(QMessageBox::warning( getActiveView(), tr("Unsaved document"),tr("Save document before close?"),
        tr("Yes"),tr("No"),tr("Cancel"),0,2))
      {
      case 0:
        //GetApplication().
        if (save())
          e->accept();
        else
          e->ignore();
        break;
      case 1:
        e->accept();
        break;
      case 2:
        break;
      }
#   else
      e->accept();
#   endif
  }else
    e->accept();
}

void Document::closeEvent ( QCloseEvent * e )
{
  canClose(e);
}

void Document::closeAllViews(void)
{
  std::list<Gui::BaseView*>::iterator It;

  while ( (It = _LpcViews.begin()) != _LpcViews.end())
  {
    (*It)->onClose();
  }

  assert(_LpcViews.size() == 0);
}


/// send Messages to the active view
bool Document::sendMsgToViews(const char* pMsg)
{
  std::list<Gui::BaseView*>::iterator It;

  for(It = _LpcViews.begin();It != _LpcViews.end();It++)
  {
    if( (*It)->onMsg(pMsg))
    {
      return true;
    }
  }

  for(It = _LpcPassivViews.begin();It != _LpcPassivViews.end();It++)
  {
    if( (*It)->onMsg(pMsg))
    {
      return true;
    }
  }

  return false;
}

/// send Messages to all views
bool Document::sendMsgToActiveView(const char* pMsg)
{
  if(_pcActiveView)
    return _pcActiveView->onMsg(pMsg);
  else
    return false;
}



/// Geter for the Active View
MDIView* Document::getActiveView(void)
{
  return _pcAppWnd->activeView();
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
void Document::openCommand(const char* sName)
{
  // check on double open Commands
  assert(!getDocument()->HasOpenCommand());

  listUndoNames.push_back(sName);

  getDocument()->NewCommand();
}

void Document::commitCommand(void)
{
  getDocument()->CommitCommand();	
}

void Document::abortCommand(void)
{
  listUndoNames.pop_back();

  getDocument()->AbortCommand();	
}

/// Get an Undo string vector with the Undo names
std::vector<std::string> Document::getUndoVector(void)
{
  std::vector<std::string> vecTemp;

  //std::copy(listUndoNames.begin(),listUndoNames.end(),vecTemp.begin());

  for(std::list<std::string>::iterator It=listUndoNames.begin();It!=listUndoNames.end();It++)
    vecTemp.push_back(*It);

  return vecTemp;
}

/// Get an Redo string vector with the Redo names
std::vector<std::string> Document::getRedoVector(void)
{
  std::vector<std::string> vecTemp;

  std::copy(listRedoNames.begin(),listRedoNames.end(),vecTemp.begin());

  return vecTemp;
}

/// Will UNDO  one or more steps
void Document::undo(int iSteps)
{
  for (int i=0;i<iSteps;i++)
  {
    getDocument()->Undo();
  }
}

/// Will REDO  one or more steps
void Document::redo(int iSteps)
{
  for (int i=0;i<iSteps;i++)
  {
    getDocument()->Undo();
  }
}




Handle(V3d_Viewer) Document::Viewer(const Standard_CString aDisplay,
                     const Standard_ExtString aName,
                     const Standard_CString aDomain,
                     const Standard_Real ViewSize,
                     const V3d_TypeOfOrientation ViewProj,
                     const Standard_Boolean ComputedMode,
                     const Standard_Boolean aDefaultComputedMode )
{
# ifndef FC_OS_WIN32
    static Handle(Graphic3d_GraphicDevice) defaultdevice;

    if(defaultdevice.IsNull()) defaultdevice = new Graphic3d_GraphicDevice(aDisplay);
    return new V3d_Viewer(defaultdevice,aName,aDomain,ViewSize,ViewProj,
                Quantity_NOC_GRAY30,V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT,
                ComputedMode,aDefaultComputedMode,V3d_TEX_NONE);
# else
    static Handle(Graphic3d_WNTGraphicDevice) defaultdevice;

    if(defaultdevice.IsNull()) defaultdevice = new Graphic3d_WNTGraphicDevice();
    return new V3d_Viewer(defaultdevice,aName,aDomain,ViewSize,ViewProj,
                Quantity_NOC_GRAY30,V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT,
                ComputedMode,aDefaultComputedMode,V3d_TEX_NONE);
# endif  // FC_OS_WIN32
}



#include "moc_Document.cpp"
