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
# include <qdir.h>
# include <qfiledialog.h>
# include <qfileinfo.h>
# include <qmessagebox.h>
# include <qstatusbar.h>
# include <Inventor/nodes/SoSelection.h>
#endif


#include <Base/Exception.h>

#include <App/Document.h>
#include <App/Feature.h>

#include "Application.h"
#include "MainWindow.h"
#include "Tree.h"
#include "Document.h"
#include "DocumentPy.h"
#include "Command.h"
#include "View3DInventor.h"
#include "View3DInventorViewer.h"
#include "BitmapFactory.h"
#include "ViewProviderFeature.h"
#include "Selection.h"
#include "SoFCSelection.h"
#include "WaitCursor.h"

using namespace Gui;

/* TRANSLATOR Gui::Document */

int Document::_iDocCount = 0;

Document::Document(App::Document* pcDocument,Application * app, const char * name)
  :_iWinCount(1), _isClosing(false), _pcAppWnd(app), _pcDocument(pcDocument)
{
  // new instance
  _iDocId = (++_iDocCount);

  // keeping an Instance of this document as long as at least one window lives
//  _pcDocument->IncRef();

  //Handle(TDocStd_Document) hcOcafDoc = pcDocument->GetOCCDoc();

  _pcDocument->Attach(this);

  // pointer to the python class
  // NOTE: As this Python object doesn't get returned to the interpreter we mustn't increment it (Werner Jan-12-2006) 
  _pcDocPy = new Gui::DocumentPy(this);

/*  // set the ViewProvider root
  pcSelection        = new SoSelection();
  pcSelection->ref();
  pcSelection->addFinishCallback(Document::sFinishSelectionCallback,this);
  pcSelection->addSelectionCallback( Document::sMadeSelection, this );
  pcSelection->addDeselectionCallback( Document::sUnmadeSelection, this );
*/
  //pcTreeItem = new DocItem(Gui::getMainWindow()->getTreeView()->getMainItem(),this);
  pcTreeItem = Gui::getMainWindow()->getTreeView()->NewDoc(this);
  // open at least one viewer
  createView("View3DIv");
}

Document::~Document()
{
  // e.g. if document gets closed from within a Python command
  _isClosing = true;
  while ( _LpcViews.size() > 0 )
  {
    delete _LpcViews.front();
  }

  std::map<App::Feature*,ViewProvider*>::iterator it;
  for(it = _ViewProviderMap.begin();it != _ViewProviderMap.end(); ++it)
    delete it->second;

//  pcSelection->unref();
    Gui::getMainWindow()->getTreeView()->DeleteDoc(this);
    pcTreeItem = 0;

  // remove from the tree

  //delete (pcTreeItem);
  _pcDocument->Detach(this);

  // remove the reference from the object
  _pcDocPy->DecRef();
  //_pcDocument->DecRef();
}

//*****************************************************************************************************
// 3D viewer handling
//*****************************************************************************************************

void Document::update(void)
{
  for(std::map<App::Feature*,ViewProvider*>::const_iterator It1=_ViewProviderMap.begin();It1!=_ViewProviderMap.end();++It1)
    It1->second->update();
  for(std::map<std::string,ViewProvider*>::const_iterator It2=_ViewProviderMapAnotation.begin();It2!=_ViewProviderMapAnotation.end();++It2)
    It2->second->update();

  onUpdate();
}

void Document::setAnotationViewProvider(const char* name, ViewProvider *pcProvider)
{
  std::list<Gui::BaseView*>::iterator VIt;

  // already in ?
  std::map<std::string,ViewProvider*>::iterator it = _ViewProviderMapAnotation.find( name );
  if(it != _ViewProviderMapAnotation.end())
    rmvAnotationViewProvider(name);

  // add 
  _ViewProviderMapAnotation[name] = pcProvider;

  // cycling to all views of the document
  for(VIt = _LpcViews.begin();VIt != _LpcViews.end();VIt++)
  {
    View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
    if(pcIvView)
      pcIvView->getViewer()->addViewProvider(pcProvider);
  }

}

ViewProvider * Document::getAnotationViewProvider(const char* name)
{
  std::map<std::string,ViewProvider*>::iterator it = _ViewProviderMapAnotation.find( name );
  return ( (it != _ViewProviderMapAnotation.end()) ? it->second : 0 );

}

void Document::rmvAnotationViewProvider(const char* name)
{
  std::map<std::string,ViewProvider*>::iterator it = _ViewProviderMapAnotation.find( name );
  std::list<Gui::BaseView*>::iterator VIt;

  // cycling to all views of the document
  for(VIt = _LpcViews.begin();VIt != _LpcViews.end();VIt++)
  {
    View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
    if(pcIvView)
      pcIvView->getViewer()->removeViewProvider(it->second);
  }

  delete it->second;
  _ViewProviderMapAnotation.erase(it); 
}


ViewProvider* Document::getViewProvider(App::Feature* Feat)
{
  std::map<App::Feature*,ViewProvider*>::iterator it = _ViewProviderMap.find( Feat );
  return ( (it != _ViewProviderMap.end()) ? it->second : 0 );
}


ViewProvider *Document::getViewProviderByName(const char* name)
{
  // first check on feature name
  App::Feature *pcFeat = getDocument()->getFeature(name);

  if(pcFeat)
  {
    std::map<App::Feature*,ViewProvider*>::iterator it = _ViewProviderMap.find( pcFeat );

    if(it != _ViewProviderMap.end())
      return it->second;
  }else {
    // then try annotation name
    std::map<std::string,ViewProvider*>::iterator it2 = _ViewProviderMapAnotation.find( name );

    if(it2 != _ViewProviderMapAnotation.end())
      return it2->second;
  }

  return 0;
}


bool Document::isShow(const char* name)
{
  ViewProvider* pcProv = getViewProviderByName(name);

  if(pcProv)
    return pcProv->isShow();
  else 
    return false;
}

/// put the feature in show
void Document::setShow(const char* name)
{
  ViewProvider* pcProv = getViewProviderByName(name);

  if(pcProv)
    pcProv->show();
}

/// set the feature in Noshow
void Document::setHide(const char* name)
{
  ViewProvider* pcProv = getViewProviderByName(name);

  if(pcProv)
    pcProv->hide();
}

/// set the feature in Noshow
void Document::setPos(const char* name, const Matrix4D& rclMtrx)
{
  ViewProvider* pcProv = getViewProviderByName(name);

  if(pcProv)
    pcProv->setTransformation(rclMtrx);

}

//*****************************************************************************************************
// Document
//*****************************************************************************************************

void Document::OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::Document::OnChange()");
#endif

  if(Reason.Why == App::DocChanges::Rename)
  {
    onRename();
    return;
  }

  std::list<Gui::BaseView*>::iterator VIt;

  // remove the representation of Features no longer exist
  std::set<App::Feature*>::const_iterator It;
  for(It=Reason.DeletedFeatures.begin();It!=Reason.DeletedFeatures.end();It++)
  {
    // cycling to all views of the document
    ViewProvider* vpInv = getViewProvider( *It );
    for(VIt = _LpcViews.begin();VIt != _LpcViews.end();VIt++)
    {
      View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
      if(pcIvView && vpInv)
      {
        pcIvView->getViewer()->removeViewProvider( vpInv );
      }
    }

    // removing from tree
    pcTreeItem->removeViewProviderFeature(dynamic_cast<ViewProviderFeature*>( vpInv ));



    if ( vpInv )
    {
      delete vpInv;
      _ViewProviderMap.erase(*It);
    }
  }

  // set up new providers
  for(It=Reason.NewFeatures.begin();It!=Reason.NewFeatures.end();It++)
  {
    std::string cName = (*It)->type();
    // check if still the same ViewProvider
    ViewProviderFeature *pcProvider = ViewProviderFeatureFactory().Produce((*It)->type());
    _ViewProviderMap[*It] = pcProvider;
    if(pcProvider)
    {
      try{
        // if succesfully created set the right name an calculate the view
        pcProvider->attach(*It);
      }catch(const Base::MemoryException& e){
        Base::Console().Error("Memory exception in feature '%s' thrown: %s\n",(*It)->getName(),e.what());
        (*It)->setError(e.what());
      }catch(Base::Exception &e){
        e.ReportException();
      }
#ifndef FC_DEBUG
      catch(...){
        Base::Console().Error("App::Document::_RecomputeFeature(): Unknown exception in Feature \"%s\" thrown\n",(*It)->getName());
      }
#endif

      // cycling to all views of the document
      for(VIt = _LpcViews.begin();VIt != _LpcViews.end();VIt++)
      {
        View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
        if(pcIvView)
          pcIvView->getViewer()->addViewProvider(pcProvider);
      }

      // adding to the tree
      pcTreeItem->addViewProviderFeature(pcProvider);

    }else{
      Base::Console().Warning("Gui::View3DInventorEx::onUpdate() no view provider for the Feature %s found\n",(*It)->type());
    }
  }

  // update recalculated features
  for(It=Reason.UpdatedFeatures.begin();It!=Reason.UpdatedFeatures.end();It++)
  {
    ViewProvider* vpInv = getViewProvider( *It );
    if ( vpInv )
      vpInv->update();
  }

  //getViewer()->addViewProvider()
  
  
  onUpdate();
}

/// Save the document
bool Document::save(void)
{
  if(_pcDocument->isSaved())
  {
    Gui::WaitCursor wc;
    Command::doCommand(Command::Doc,"App.save(\"%s\")", _pcDocument->getName());
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
  getMainWindow()->statusBar()->message(tr("Saving file under new filename..."));

  // use current path as default
  std::string path = QDir::currentDirPath().latin1();
  FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");
  path = hPath->GetASCII("FileOpenSavePath", path.c_str());
	QString dir = path.c_str();
  QString fn = QFileDialog::getSaveFileName(dir, "FreeCAD (*.FCStd)", getMainWindow());
  if (!fn.isEmpty())
  {
    if ( !fn.endsWith(".FCStd"))
      fn += ".FCStd";

    // save as new file name
    Gui::WaitCursor wc;
    Command::doCommand(Command::Doc,"App.saveAs(\"%s\",\"%s\")", _pcDocument->getName(), fn.latin1());

    QFileInfo fi;
		fi.setFile(fn);
    hPath->SetASCII("FileOpenSavePath", fi.dirPath(true).latin1());
    getMainWindow()->appendRecentFile( fi.filePath().latin1() );
    return true;
  }
  else
  {
    getMainWindow()->statusBar()->message(QObject::tr("Saving aborted"), 2000);
    return false;
  }
}

void Document::createView(const char* sType) 
{
  QPixmap FCIcon = Gui::BitmapFactory().pixmap(App::Application::Config()["AppIcon"].c_str());
  MDIView* pcView3D=0;
  if(strcmp(sType,"View3DIv") == 0){
//    pcView3D = new Gui::View3DInventorEx(this,_pcAppWnd,"View3DIv");
    pcView3D = new Gui::View3DInventor(this,getMainWindow(),"View3DIv");

    // add the selction node of the document
    //((View3DInventor*)pcView3D)->getViewer()->addSelectionNode(pcSelection);
    
    // attach the viewprovider
    for(std::map<App::Feature*,ViewProvider*>::const_iterator It1=_ViewProviderMap.begin();It1!=_ViewProviderMap.end();++It1)
      ((View3DInventor*)pcView3D)->getViewer()->addViewProvider(It1->second);
    for(std::map<std::string,ViewProvider*>::const_iterator It2=_ViewProviderMapAnotation.begin();It2!=_ViewProviderMapAnotation.end();++It2)
      ((View3DInventor*)pcView3D)->getViewer()->addViewProvider(It2->second);

  }else /* if(strcmp(sType,"View3DOCC") == 0){
    pcView3D = new MDIView3D(this,_pcAppWnd,"View3DOCC");
  }else*/
  {
    Base::Console().Error("Document::createView(): Unknown view type: %s\n",sType);
    return;
  }

  const char* name = getDocument()->getName();

  QString aName = tr("%1 : %3").arg(name).arg(_iWinCount++);


  pcView3D->setCaption(aName);
  pcView3D->setIcon( FCIcon );
  pcView3D->resize( 400, 300 );
  getMainWindow()->addWindow(pcView3D);
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
    if(find(_LpcPassivViews.begin(),_LpcPassivViews.end(),pcView) != _LpcPassivViews.end())
      _LpcPassivViews.remove(pcView);
  }else{
    if(find(_LpcViews.begin(),_LpcViews.end(),pcView) != _LpcViews.end())
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

    // is already  closing the document
    if ( _isClosing == false )
      _pcAppWnd->onLastWindowClosed(this);
    }
  }
}

void Document::onUpdate(void)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::Document::onUpdate()");
#endif

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

void Document::onRename(void)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::Document::onRename()");
#endif
  Gui::Selection().clearSelection();

  std::list<Gui::BaseView*>::iterator It;

  for(It = _LpcViews.begin();It != _LpcViews.end();It++)
  {
    (*It)->onRename(this);
  }

  for(It = _LpcPassivViews.begin();It != _LpcPassivViews.end();It++)
  {
    (*It)->onRename(this);
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
  if(! _pcDocument->isSaved() || true // at the moment we cannot determine if a saved document has been modified
    //&& _pcDocument->GetOCCDoc()->StorageVersion() < _pcDocument->GetOCCDoc()->Modifications() 
    //&& _pcDocument->GetOCCDoc()->CanClose() == CDM_CCS_OK
      )
  {
#   ifndef FC_DEBUG
      switch(QMessageBox::question( getActiveView(), tr("Unsaved document"),tr("Save document before close?"),
        QMessageBox::Yes|QMessageBox::Default,QMessageBox::No,QMessageBox::Cancel|QMessageBox::Escape ))
      {
      case QMessageBox::Yes: // "Yes" was pressed
        //GetApplication().
        if (save())
          e->accept(); // -> can be closed. document was saved
        else
          e->ignore(); // -> abort, because saving of document was aborted
        break;
      case QMessageBox::No: // "No" was pressed
        e->accept();   // -> can be closed without saving
        break;
      case QMessageBox::Cancel: // "Cancel" was pressed
        e->ignore();   // -> abort
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

std::list<MDIView*> Document::getMDIViews() const
{ 
  std::list<MDIView*> views;
  for ( std::list<BaseView*>::const_iterator it = _LpcViews.begin(); it != _LpcViews.end(); ++it )
  {
    MDIView* view = dynamic_cast<MDIView*>(*it);
    if ( view ) views.push_back( view );
  }

  return views;
}

/// send Messages to the active view
bool Document::sendMsgToViews(const char* pMsg)
{
  std::list<Gui::BaseView*>::iterator It;
  const char** pReturnIgnore=0;

  for(It = _LpcViews.begin();It != _LpcViews.end();It++)
  {
    if( (*It)->onMsg(pMsg,pReturnIgnore))
    {
      return true;
    }
  }

  for(It = _LpcPassivViews.begin();It != _LpcPassivViews.end();It++)
  {
    if( (*It)->onMsg(pMsg,pReturnIgnore))
    {
      return true;
    }
  }

  return false;
}

/// send Messages to all views
bool Document::sendMsgToActiveView(const char* pMsg,const char** pReturn)
{
  if(_pcActiveView)
    return _pcActiveView->onMsg(pMsg,pReturn);
  else
    return false;
}



/// Geter for the Active View
MDIView* Document::getActiveView(void)
{
  return getMainWindow()->activeWindow();
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


/*

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

  */

Base::PyObjectBase * Document::getPyObject(void)
{
  _pcDocPy->IncRef();
  return _pcDocPy;
}

#if 0
void Document::sFinishSelectionCallback(void *pcDocument,SoSelection *path)
{
  static_cast<Document*>(pcDocument)->finishSelectionCallback(path);
}

void Document::finishSelectionCallback(SoSelection *)
{
  Base::Console().Log("SelectionCallback\n");
}


void Document::sMadeSelection(void *pcDocument,SoPath *path)
{
  static_cast<Document*>(pcDocument)->madeSelection(path);
}

// Callback function triggered for selection / deselection.
void Document::madeSelection(  SoPath * path )
{
  for(int i = 0; i<path->getLength();i++)
  {
    SoNode *node = path->getNodeFromTail(i);
    if (node->getTypeId() == SoFCSelection::getClassTypeId()) 
    {
      SoFCSelection * SelNode = (SoFCSelection *)node;  // safe downward cast, knows the type
      Base::Console().Log("Select:%s.%s.%s \n",SelNode->documentName.getValue().getString(),
                                               SelNode->featureName.getValue().getString(),
                                               SelNode->subElementName.getValue().getString());

      SelNode->selected = SoFCSelection::SELECTED;

      Gui::Selection().addSelection(SelNode->documentName.getValue().getString(),
                                    SelNode->featureName.getValue().getString(),
                                    SelNode->subElementName.getValue().getString());
    
    }

  }



/*
  for(std::set<ViewProvider*>::iterator It = _ViewProviderSet.begin();It!=_ViewProviderSet.end();It++)
    for(int i = 0; i<path->getLength();i++)
      if((*It)->getRoot() == path->getNodeFromTail(i))
      {
        (*It)->selected(this,path);
        return;
      }
*/

}

void Document::sUnmadeSelection(void *pcDocument,SoPath *path)
{
  static_cast<Document*>(pcDocument)->unmadeSelection(path);
}

// Callback function triggered for deselection.
void Document::unmadeSelection(  SoPath * path )
{
  for(int i = 0; i<path->getLength();i++)
  {
    SoNode *node = path->getNodeFromTail(i);
    if (node->getTypeId() == SoFCSelection::getClassTypeId()) 
    {
      SoFCSelection * SelNode = (SoFCSelection *)node;  // safe downward cast, knows the type
      Base::Console().Log("Unselect:%s.%s.%s \n",SelNode->documentName.getValue().getString(),
                                               SelNode->featureName.getValue().getString(),
                                               SelNode->subElementName.getValue().getString());
      SelNode->selected = SoFCSelection::NOTSELECTED;

      Gui::Selection().rmvSelection(SelNode->documentName.getValue().getString(),
                                    SelNode->featureName.getValue().getString(),
                                    SelNode->subElementName.getValue().getString());
    


    }

  }
}


#endif

#include "moc_Document.cpp"
