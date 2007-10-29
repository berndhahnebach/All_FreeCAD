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
# include <qfileinfo.h>
# include <qmessagebox.h>
# include <qstatusbar.h>
# include <boost/signals.hpp>
# include <boost/bind.hpp>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Matrix.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include <App/Document.h>
#include <App/DocumentObject.h>

#include "Application.h"
#include "MainWindow.h"
#include "Tree.h"
#include "Document.h"
#include "DocumentPy.h"
#include "Command.h"
#include "FileDialog.h"
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
  :_iWinCount(1), _isClosing(false), _isModified(false), _pcAppWnd(app), _pcDocument(pcDocument)
{
  // new instance
  _iDocId = (++_iDocCount);


  //_pcDocument->Attach(this);
  _pcDocument->installDocumentHook(this);
  // connect signal

  //pcDocument->m_sig.connect(boost::bind(&Gui::Document::refresh, this, _1));
  //boost::bind(&Gui::Document::slotNewObject, this, _1)

  //App::Document::connection_t  m_connection;
  //m_connection = pcDocument->connect(boost::bind(&Document::slotNewObject, this, _1));
  pcDocument->signalNewObject.connect(boost::bind(&Gui::Document::slotNewObject, this, _1));
  pcDocument->signalDeletedObject.connect(boost::bind(&Gui::Document::slotDeletedObject, this, _1));
  pcDocument->signalChangedObject.connect(boost::bind(&Gui::Document::slotChangedObject, this, _1, _2));
  pcDocument->signalRenamedObject.connect(boost::bind(&Gui::Document::slotRenamedObject, this, _1));
  pcDocument->signalActivatedObject.connect(boost::bind(&Gui::Document::slotActivatedObject, this, _1));
  //pcDocument->signalNewObject.connect(&(this->slotNewObject));
  
  // pointer to the python class
  // NOTE: As this Python object doesn't get returned to the interpreter we mustn't increment it (Werner Jan-12-2006)
  _pcDocPy = new Gui::DocumentPy(this);

  if(App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Document")->GetBool("UsingUndo",false))
    _pcDocument->setUndoMode(1);


/*  // set the ViewProvider root
  pcSelection        = new SoSelection();
  pcSelection->ref();
  pcSelection->addFinishCallback(Document::sFinishSelectionCallback,this);
  pcSelection->addSelectionCallback( Document::sMadeSelection, this );
  pcSelection->addDeselectionCallback( Document::sUnmadeSelection, this );
*/
  //pcTreeItem = new DocItem(Gui::getMainWindow()->getTreeView()->getMainItem(),this);
  //pcTreeItem = Gui::getMainWindow()->getTreeView()->NewDoc(this);
  // open at least one viewer
  //createView("View3DIv");
}


Document::~Document()
{
  // e.g. if document gets closed from within a Python command
  _isClosing = true;
  //FIXME: Needs a redesign here because we shouldn't delete the views directly that inhert from QWidget
  //because this might cause serious problems in Qt internals. It's better to call their close() method.
  //Maybe MDIView shouldn't inherit BaseView but BaseView should have an MDIView...
#if 0
  while ( _LpcViews.size() > 0 ) {
    QWidget* w = dynamic_cast<QWidget*>(_LpcViews.front());
    if (w) {
      w->close(); _LpcViews.pop_front();
    } else {
      delete _LpcViews.front();
    }
  }

  qApp->processEvents();
#else
  // Calls Document::detachView()
  while ( _LpcViews.size() > 0 )
    delete _LpcViews.front();
#endif

  std::map<App::DocumentObject*,ViewProviderDocumentObject*>::iterator it;
  for(it = _ViewProviderMap.begin();it != _ViewProviderMap.end(); ++it)
    delete it->second;
  std::map<std::string,ViewProvider*>::iterator it2;
  for(it2 = _ViewProviderMapAnotation.begin();it2 != _ViewProviderMapAnotation.end(); ++it2)
    delete it2->second;

//  pcSelection->unref();
    //Gui::getMainWindow()->getTreeView()->DeleteDoc(this);
    //pcTreeItem = 0;

  // remove from the tree

  //delete (pcTreeItem);
  //_pcDocument->Detach(this);
  _pcDocument->removeDocumentHook();

  // remove the reference from the object
  _pcDocPy->setInvalid();
  _pcDocPy->DecRef();
  //_pcDocument->DecRef();
}

//*****************************************************************************************************
// 3D viewer handling
//*****************************************************************************************************

void Document::update(void)
{
    //std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator It1;
    //for (It1=_ViewProviderMap.begin();It1!=_ViewProviderMap.end();++It1)
    //     It1->second->update();
    //std::map<std::string,ViewProvider*>::const_iterator It2;
    //for (It2=_ViewProviderMapAnotation.begin();It2!=_ViewProviderMapAnotation.end();++It2)
    //     It2->second->update();

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

ViewProvider * Document::getAnotationViewProvider(const char* name) const
{
  std::map<std::string,ViewProvider*>::const_iterator it = _ViewProviderMapAnotation.find( name );
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


ViewProvider* Document::getViewProvider(App::DocumentObject* Feat) const
{
    std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator
    it = _ViewProviderMap.find( Feat );
    return ( (it != _ViewProviderMap.end()) ? it->second : 0 );
}


ViewProvider *Document::getViewProviderByName(const char* name) const
{
    // first check on feature name
    App::DocumentObject *pcFeat = getDocument()->getObject(name);

    if (pcFeat)
    {
        std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator
        it = _ViewProviderMap.find( pcFeat );

        if (it != _ViewProviderMap.end())
            return it->second;
    } else {
        // then try annotation name
        std::map<std::string,ViewProvider*>::const_iterator it2 = _ViewProviderMapAnotation.find( name );

        if (it2 != _ViewProviderMapAnotation.end())
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

  if(pcProv && pcProv->getTypeId().isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) {
    ((ViewProviderDocumentObject*)pcProv)->Visibility.setValue(true);
  }
}

/// set the feature in Noshow
void Document::setHide(const char* name)
{
  ViewProvider* pcProv = getViewProviderByName(name);

  if(pcProv && pcProv->getTypeId().isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) {
    ((ViewProviderDocumentObject*)pcProv)->Visibility.setValue(false);
  }
}

/// set the feature in Noshow
void Document::setPos(const char* name, const Base::Matrix4D& rclMtrx)
{
  ViewProvider* pcProv = getViewProviderByName(name);

  if(pcProv)
    pcProv->setTransformation(rclMtrx);

}

//*****************************************************************************************************
// Document
//*****************************************************************************************************
void Document::slotNewObject(App::DocumentObject& Obj)
{
  Base::Console().Log("Document::slotNewObject() called\n");
  std::string cName = Obj.getViewProviderName();
  if (cName.empty()) {
    // handle document object with no view provider specified
    Base::Console().Log("%s has no view provider specified\n", Obj.getTypeId().getName());
    return;
  }
  setModified(true);
  ViewProviderDocumentObject *pcProvider = (ViewProviderDocumentObject*) Base::Type::createInstanceByName(cName.c_str(),true);
  if ( pcProvider )
  {
    // type not derived from ViewProviderDocumentObject!!!
    assert(pcProvider->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId()));
    _ViewProviderMap[&Obj] = pcProvider;

    try{
      // if succesfully created set the right name and calculate the view
      pcProvider->attach(&Obj);
    }catch(const Base::MemoryException& e){
      Base::Console().Error("Memory exception in '%s' thrown: %s\n",Obj.getNameInDocument(),e.what());
    }catch(Base::Exception &e){
      e.ReportException();
    }
#ifndef FC_DEBUG
    catch(...){
      Base::Console().Error("App::Document::_RecomputeFeature(): Unknown exception in Feature \"%s\" thrown\n",Obj.getNameInDocument());
    }
#endif
    std::list<Gui::BaseView*>::iterator VIt;
    // cycling to all views of the document
    for(VIt = _LpcViews.begin();VIt != _LpcViews.end();VIt++)
    {
      View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
      if(pcIvView)
        pcIvView->getViewer()->addViewProvider(pcProvider);
    }

    // adding to the tree
    signalNewObject(*pcProvider);
    //pcTreeItem->addViewProviderDocumentObject(pcProvider);

  }else{
    Base::Console().Warning("Gui::Document::slotNewObject() no view provider for the object %s found\n",cName.c_str());
  }

}

void Document::slotDeletedObject(App::DocumentObject& Obj)
{
    std::list<Gui::BaseView*>::iterator VIt;
    setModified(true);
    Base::Console().Log("Document::slotDeleteObject() called\n");
  
    // cycling to all views of the document
    ViewProvider* viewProvider = getViewProvider(&Obj);
    for (VIt = _LpcViews.begin();VIt != _LpcViews.end();VIt++) {
        View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
        if (pcIvView && viewProvider)
            pcIvView->getViewer()->removeViewProvider(viewProvider);
    }

    if (viewProvider && viewProvider->getTypeId().isDerivedFrom(
        ViewProviderDocumentObject::getClassTypeId())) {
        // removing from tree
        signalDeletedObject(*(static_cast<ViewProviderDocumentObject*>(viewProvider)));

        delete viewProvider;
        _ViewProviderMap.erase(&Obj);
    }
}

void Document::slotChangedObject(App::DocumentObject& Obj, App::Property& Prop)
{
    Base::Console().Log("Document::slotChangedObject() called\n");
    ViewProvider* viewProvider = getViewProvider(&Obj);
    if (viewProvider) {
        try {
            viewProvider->update(&Prop);
        } catch(const Base::MemoryException& e) {
            Base::Console().Error("Memory exception in '%s' thrown: %s\n",Obj.getNameInDocument(),e.what());
        } catch(Base::Exception &e){
            e.ReportException();
        } catch (...) {
            Base::Console().Error("Cannot update representation for '%s'.\n", Obj.getNameInDocument());
        }

        // The call of setActiveMode() must be delayed to wait until the associated
        // document object is fully built, otherwise we run into strange effects
        if (Obj.StatusBits.test(2)) {
            Obj.StatusBits.reset(2);
            if (viewProvider->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId()))
                static_cast<ViewProviderDocumentObject*>(viewProvider)->setActiveMode();
        }

        if (viewProvider->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId()))
            signalChangedObject(*(static_cast<ViewProviderDocumentObject*>(viewProvider)));
    }
}

void Document::slotRenamedObject(App::DocumentObject& Obj)
{
    ViewProvider* viewProvider = getViewProvider(&Obj);
    if (viewProvider && viewProvider->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) {
        signalRenamedObject(*(static_cast<ViewProviderDocumentObject*>(viewProvider)));
    }
}

void Document::slotActivatedObject(App::DocumentObject& Obj)
{
    ViewProvider* viewProvider = getViewProvider(&Obj);
    if (viewProvider && viewProvider->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) {
        signalActivatedObject(*(static_cast<ViewProviderDocumentObject*>(viewProvider)));
    }
}

void Document::setModified(bool b)
{
  _isModified = b;
  std::list<MDIView*> mdis = getMDIViews();
  for ( std::list<MDIView*>::iterator it = mdis.begin(); it != mdis.end(); ++it )
  {
    QString cap = (*it)->windowTitle();
    if ( b && !cap.endsWith(" *") )
    {
      cap += " *";
      (*it)->setWindowTitle(cap);
    }
    else if ( !b && cap.endsWith(" *") )
    {
      cap = cap.left(cap.length()-2);
      (*it)->setWindowTitle(cap);
    }
  }
}

/// Save the document
bool Document::save(void)
{
  if(_pcDocument->isSaved())
  {
    Gui::WaitCursor wc;
    Command::doCommand(Command::Doc,"App.getDocument(\"%s\").save()", _pcDocument->getName());
    setModified(false);
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
  getMainWindow()->statusBar()->showMessage(QObject::tr("Save document under new filename..."));

  QString fn = QFileDialog::getSaveFileName(getMainWindow(), QObject::tr("Save FreeCAD Document"), 
               QDir::currentPath(), QObject::tr("FreeCAD document (*.FCStd)") );
  if (!fn.isEmpty())
  {
    if ( !fn.endsWith(".FCStd"))
      fn += ".FCStd";

    QFileInfo fi;
    fi.setFile(fn);

    const char * DocName = App::GetApplication().getDocumentName(getDocument());

    // save as new file name
    Gui::WaitCursor wc;
    Command::doCommand(Command::Doc,"App.getDocument(\"%s\").FileName = \"%s\"", DocName, (const char*)fn.toUtf8());
    Command::doCommand(Command::Doc,"App.getDocument(\"%s\").save()", DocName );
    setModified(false);

    getMainWindow()->appendRecentFile(fi.filePath());
    return true;
  }
  else
  {
    getMainWindow()->statusBar()->showMessage(QObject::tr("Saving aborted"), 2000);
    return false;
  }
}

/** 
 * Adds a separate XML file to the projects file that contains information about the view providers.
 */
void Document::Save (Base::Writer &writer) const
{
  // It's only possible to add extra information if force of XML is disabled
  if ( writer.isForceXML() == false )
    writer.addFile("GuiDocument.xml", this);
}

/** 
 * Loads a separate XML file from the projects file with information about the view providers.
 */
void Document::Restore(Base::XMLReader &reader)
{
  reader.addFile("GuiDocument.xml",this);
}

/**
 * Restores the properties of the view providers.
 */
void Document::RestoreDocFile(Base::Reader &reader)
{
  // We must create an XML parser to read from the input stream
  Base::XMLReader xmlReader("GuiDocument.xml", reader);

  int i,Cnt;

  xmlReader.readElement("Document");
  long scheme = xmlReader.getAttributeAsInteger("SchemaVersion");

  // At this stage all the document objects and their associated view providers exist.
  // Now we must restore the properties of the view providers only.
  //
  // SchemeVersion "1"
  if ( scheme == 1 ) {
    // read the viewproviders itself
    xmlReader.readElement("ViewProviderData");
    Cnt = xmlReader.getAttributeAsInteger("Count");
    for(i=0 ;i<Cnt ;i++)
    {
      xmlReader.readElement("ViewProvider");
      std::string name = xmlReader.getAttribute("name");
      ViewProvider* pObj = getViewProviderByName(name.c_str());
      if(pObj) // check if this feature has been registered
      {
        pObj->Restore(xmlReader);

        // As the view providers don't get notified when their proprties change while reading we must force this here
        std::map<std::string,App::Property*> Map;
        pObj->getPropertyMap(Map);
        for ( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
          it->second->touch();
      }
      xmlReader.readEndElement("ViewProvider");
    }
    xmlReader.readEndElement("ViewProviderData");

    // read camera settings
    xmlReader.readElement("Camera");
    const char* ppReturn = xmlReader.getAttribute("settings");
    std::string sMsg = "SetCamera ";
    sMsg += ppReturn;
    if ( strcmp(ppReturn, "") != 0 ) { // non-empty attribute
      if ( _pcAppWnd->sendHasMsgToActiveView("SetCamera") )
        _pcAppWnd->sendMsgToActiveView(sMsg.c_str());
    }
  }

  xmlReader.readEndElement("Document");
}

/**
 * Saves the properties of the view providers.
 */
void Document::SaveDocFile (Base::Writer &writer) const
{
  writer.Stream() << "<?xml version='1.0' encoding='utf-8'?>" << std::endl
         << "<!--" << std::endl
         << " FreeCAD Document, see http://free-cad.sourceforge.net for more informations..." << std::endl
         << "-->" << std::endl;

  writer.Stream() << "<Document SchemaVersion=\"1\">" << std::endl;

  std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator it;
  
  // writing the view provider names itself
  writer.incInd(); // indention for 'ViewProviderData Count'
  writer.Stream() << writer.ind() << "<ViewProviderData Count=\"" << _ViewProviderMap.size() <<"\">" << std::endl;

  bool xml = writer.isForceXML();
  writer.setForceXML(true);
  writer.incInd(); // indention for 'ViewProvider name'
  for(it = _ViewProviderMap.begin(); it != _ViewProviderMap.end(); ++it)
  {
    App::DocumentObject* doc = it->first;
    ViewProvider* obj = it->second;
    writer.Stream() << writer.ind() << "<ViewProvider name=\"" << doc->getNameInDocument() << "\">" << std::endl;   
    obj->Save(writer);
    writer.Stream() << writer.ind() << "</ViewProvider>" << std::endl;
  }
  writer.setForceXML(xml);

  writer.decInd(); // indention for 'ViewProvider name'
  writer.Stream() << writer.ind() << "</ViewProviderData>" << std::endl;
  writer.decInd();  // indention for 'ViewProviderData Count'

  // set camera settings
  QString viewPos="";
  if ( _pcAppWnd->sendHasMsgToActiveView("GetCamera") ) {
    const char* ppReturn=0;
    _pcAppWnd->sendMsgToActiveView("GetCamera",&ppReturn);
  
    // remove the first line because it's a comment like '#Inventor V2.1 ascii'
    QStringList lines = QString(ppReturn).split("\n");
    if ( lines.size() > 1 ) {
      lines.pop_front();
      viewPos = lines.join(" ");
    }
  }

  writer.incInd(); // indention for camera settings
  writer.Stream() << writer.ind() << "<Camera settings=\"" <<  (const char*)viewPos.toAscii() <<"\"/>" << std::endl;
  writer.decInd(); // indention for camera settings
  writer.Stream() << "</Document>" << std::endl;
}

void Document::createView(const char* sType) 
{
    QPixmap FCIcon = Gui::BitmapFactory().pixmap(App::Application::Config()["AppIcon"].c_str());
    MDIView* pcView3D=0;
    if (strcmp(sType,"View3DIv") == 0){
        pcView3D = new Gui::View3DInventor(this,getMainWindow());

        // add the selction node of the document
        //((View3DInventor*)pcView3D)->getViewer()->addSelectionNode(pcSelection);
    
        // attach the viewprovider
        std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator It1;
        for (It1=_ViewProviderMap.begin();It1!=_ViewProviderMap.end();++It1)
            ((View3DInventor*)pcView3D)->getViewer()->addViewProvider(It1->second);
        std::map<std::string,ViewProvider*>::const_iterator It2;
        for (It2=_ViewProviderMapAnotation.begin();It2!=_ViewProviderMapAnotation.end();++It2)
            ((View3DInventor*)pcView3D)->getViewer()->addViewProvider(It2->second);

    } else /* if(strcmp(sType,"View3DOCC") == 0){
        pcView3D = new MDIView3D(this,_pcAppWnd,"View3DOCC");
    }else*/
    {
        Base::Console().Error("Document::createView(): Unknown view type: %s\n",sType);
        return;
    }

    const char* name = getDocument()->getName();

    QString aName = QString("%1 : %3").arg(name).arg(_iWinCount++);


    pcView3D->setWindowTitle(aName);
    pcView3D->setWindowIcon( FCIcon );
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
  if(/*! _pcDocument->isSaved() ||*/ isModified() // at the moment we cannot determine if a saved document has been modified
    //&& _pcDocument->GetOCCDoc()->StorageVersion() < _pcDocument->GetOCCDoc()->Modifications() 
    //&& _pcDocument->GetOCCDoc()->CanClose() == CDM_CCS_OK
      )
  {
#   ifndef FC_DEBUG
    switch(QMessageBox::question( getActiveView(), QObject::tr("Unsaved document"),QObject::tr("Save document before close?"),
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

/// Geter for the Active View
MDIView* Document::getActiveView(void) const
{
  // get the main window's active view 
  MDIView* active = getMainWindow()->activeWindow();

  // get all MDI views of the document
  std::list<MDIView*> mdis = getMDIViews();

  // check whether the active view is part of this document
  bool ok=false;
  for ( std::list<MDIView*>::const_iterator it = mdis.begin(); it != mdis.end(); ++it ) {
    if ( (*it) == active ) {
      ok = true;
      break;
    }
  }

  // the active view is not part of this document, just use the first view
  if (!ok && !mdis.empty())
    active = mdis.front();

  return active;
}


//--------------------------------------------------------------------------
// UNDO REDO transaction handling  
//--------------------------------------------------------------------------
/** Open a new Undo transaction on the active document
 *  This method open a new UNDO transaction on the active document. This transaction
 *  will later apear in the UNDO REDO dialog with the name of the command. If the user 
 *  recall the transaction everything changed on the document between OpenCommand() and 
 *  CommitCommand will be undone (or redone). You can use an alternetive name for the 
 *  operation default is the Command name.
 *  @see CommitCommand(),AbortCommand()
 */
void Document::openCommand(const char* sName)
{
  getDocument()->openTransaction(sName);
}

void Document::commitCommand(void)
{
  getDocument()->commitTransaction();	
}

void Document::abortCommand(void)
{

  getDocument()->abortTransaction();	
}

/// Get an Undo string vector with the Undo names
std::vector<std::string> Document::getUndoVector(void) const
{
  return getDocument()->getAvailableUndoNames();
}

/// Get an Redo string vector with the Redo names
std::vector<std::string> Document::getRedoVector(void) const
{
  return getDocument()->getAvailableRedoNames();
}

/// Will UNDO  one or more steps
void Document::undo(int iSteps)
{
  for (int i=0;i<iSteps;i++)
  {
    getDocument()->undo();
  }
}

/// Will REDO  one or more steps
void Document::redo(int iSteps)
{
  for (int i=0;i<iSteps;i++)
  {
    getDocument()->redo();
  }
}




PyObject* Document::getPyObject(void)
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

