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
#include "ViewProviderDocumentObject.h"
#include "Selection.h"
#include "SoFCSelection.h"
#include "WaitCursor.h"
#include "Thumbnail.h"

using namespace Gui;

namespace Gui {

// Pimpl class
struct DocumentP
{
    Thumbnail thumb;
    int        _iWinCount;
    int        _iDocId;
    bool       _isClosing;
    bool       _isModified;
    ViewProvider*   _pcInEdit;
    Application*    _pcAppWnd;
    // the doc/Document
    App::Document*  _pcDocument;
    /// List of all registered views
    std::list<Gui::BaseView*> _LpcViews;
    /// List of all registered views
    std::list<Gui::BaseView*> _LpcPassivViews;
    std::map<App::DocumentObject*,ViewProviderDocumentObject*> _ViewProviderMap;
    std::map<std::string,ViewProvider*> _ViewProviderMapAnnotation;
};

} // namespace Gui

/* TRANSLATOR Gui::Document */

int Document::_iDocCount = 0;

Document::Document(App::Document* pcDocument,Application * app)
{
    d = new DocumentP;
    d->_iWinCount = 1;
    // new instance
    d->_iDocId = (++_iDocCount);
    d->_isClosing = false;
    d->_isModified = false;
    d->_pcAppWnd = app;
    d->_pcDocument = pcDocument;
    d->_pcInEdit = 0;

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
    pcDocument->signalSaveDocument.connect(boost::bind(&Gui::Document::Save, this, _1));
    pcDocument->signalRestoreDocument.connect(boost::bind(&Gui::Document::Restore, this, _1));

    // pointer to the python class
    // NOTE: As this Python object doesn't get returned to the interpreter we
    // mustn't increment it (Werner Jan-12-2006)
    _pcDocPy = new Gui::DocumentPy(this);

    if (App::GetApplication().GetParameterGroupByPath
        ("User parameter:BaseApp/Preferences/Document")->GetBool("UsingUndo",false))
        d->_pcDocument->setUndoMode(1);
}

Document::~Document()
{
    // e.g. if document gets closed from within a Python command
    d->_isClosing = true;
    // Calls Document::detachView()
    while (d->_LpcViews.size() > 0)
        delete d->_LpcViews.front();

    std::map<App::DocumentObject*,ViewProviderDocumentObject*>::iterator it;
    for (it = d->_ViewProviderMap.begin();it != d->_ViewProviderMap.end(); ++it)
        delete it->second;
    std::map<std::string,ViewProvider*>::iterator it2;
    for (it2 = d->_ViewProviderMapAnnotation.begin();it2 != d->_ViewProviderMapAnnotation.end(); ++it2)
        delete it2->second;

    // remove the reference from the object
    _pcDocPy->setInvalid();
    _pcDocPy->DecRef();
    delete d;
}

//*****************************************************************************************************
// 3D viewer handling
//*****************************************************************************************************

bool Document::setEdit(Gui::ViewProvider* p, int ModNum)
{
    if (d->_pcInEdit)
        resetEdit();
    View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(getActiveView());
    if (pcIvView && pcIvView->getViewer()->setEdit(p,ModNum)) {
        d->_pcInEdit = p;
        if (d->_pcInEdit->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) 
            signalInEdit(*(static_cast<ViewProviderDocumentObject*>(d->_pcInEdit)));
    }
    else
        return false;
    return true;
}

void Document::resetEdit(void)
{
    std::list<Gui::BaseView*>::iterator VIt;
    if (d->_pcInEdit){
        for (VIt = d->_LpcViews.begin();VIt != d->_LpcViews.end();VIt++) {
            View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
            if (pcIvView)
                pcIvView->getViewer()->resetEdit();
        }

        if (d->_pcInEdit->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) 
            signalResetEdit(*(static_cast<ViewProviderDocumentObject*>(d->_pcInEdit)));
        d->_pcInEdit = 0;
    }
}

ViewProvider *Document::getInEdit(void) const
{
    return d->_pcInEdit;
}

void Document::setAnnotationViewProvider(const char* name, ViewProvider *pcProvider)
{
    std::list<Gui::BaseView*>::iterator VIt;

    // already in ?
    std::map<std::string,ViewProvider*>::iterator it = d->_ViewProviderMapAnnotation.find(name);
    if (it != d->_ViewProviderMapAnnotation.end())
        removeAnnotationViewProvider(name);

    // add 
    d->_ViewProviderMapAnnotation[name] = pcProvider;

    // cycling to all views of the document
    for(VIt = d->_LpcViews.begin();VIt != d->_LpcViews.end();VIt++) {
        View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
        if (pcIvView)
            pcIvView->getViewer()->addViewProvider(pcProvider);
    }
}

ViewProvider * Document::getAnnotationViewProvider(const char* name) const
{
    std::map<std::string,ViewProvider*>::const_iterator it = d->_ViewProviderMapAnnotation.find(name);
    return ( (it != d->_ViewProviderMapAnnotation.end()) ? it->second : 0 );
}

void Document::removeAnnotationViewProvider(const char* name)
{
    std::map<std::string,ViewProvider*>::iterator it = d->_ViewProviderMapAnnotation.find(name);
    std::list<Gui::BaseView*>::iterator VIt;

    // cycling to all views of the document
    for (VIt = d->_LpcViews.begin();VIt != d->_LpcViews.end();VIt++) {
        View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
        if (pcIvView)
            pcIvView->getViewer()->removeViewProvider(it->second);
    }

    delete it->second;
    d->_ViewProviderMapAnnotation.erase(it); 
}


ViewProvider* Document::getViewProvider(App::DocumentObject* Feat) const
{
    std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator
    it = d->_ViewProviderMap.find( Feat );
    return ( (it != d->_ViewProviderMap.end()) ? it->second : 0 );
}

std::vector<ViewProvider*> Document::getViewProvidersOfType(const Base::Type& typeId) const
{
    std::vector<ViewProvider*> Objects;
    for (std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator it = 
         d->_ViewProviderMap.begin(); it != d->_ViewProviderMap.end(); ++it ) {
        if (it->second->getTypeId().isDerivedFrom(typeId))
            Objects.push_back(it->second);
    }
    return Objects;
}

ViewProvider *Document::getViewProviderByName(const char* name) const
{
    // first check on feature name
    App::DocumentObject *pcFeat = getDocument()->getObject(name);

    if (pcFeat)
    {
        std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator
        it = d->_ViewProviderMap.find( pcFeat );

        if (it != d->_ViewProviderMap.end())
            return it->second;
    } else {
        // then try annotation name
        std::map<std::string,ViewProvider*>::const_iterator it2 = d->_ViewProviderMapAnnotation.find( name );

        if (it2 != d->_ViewProviderMapAnnotation.end())
            return it2->second;
    }

    return 0;
}

bool Document::isShow(const char* name)
{
    ViewProvider* pcProv = getViewProviderByName(name);
    return pcProv ? pcProv->isShow() : false;
}

/// put the feature in show
void Document::setShow(const char* name)
{
    ViewProvider* pcProv = getViewProviderByName(name);

    if (pcProv && pcProv->getTypeId().isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) {
        ((ViewProviderDocumentObject*)pcProv)->Visibility.setValue(true);
    }
}

/// set the feature in Noshow
void Document::setHide(const char* name)
{
    ViewProvider* pcProv = getViewProviderByName(name);

    if (pcProv && pcProv->getTypeId().isDerivedFrom(ViewProviderDocumentObject::getClassTypeId())) {
        ((ViewProviderDocumentObject*)pcProv)->Visibility.setValue(false);
    }
}

/// set the feature in Noshow
void Document::setPos(const char* name, const Base::Matrix4D& rclMtrx)
{
    ViewProvider* pcProv = getViewProviderByName(name);
    if (pcProv)
        pcProv->setTransformation(rclMtrx);

}

//*****************************************************************************************************
// Document
//*****************************************************************************************************
void Document::slotNewObject(App::DocumentObject& Obj)
{
    //Base::Console().Log("Document::slotNewObject() called\n");
    std::string cName = Obj.getViewProviderName();
    if (cName.empty()) {
        // handle document object with no view provider specified
        Base::Console().Log("%s has no view provider specified\n", Obj.getTypeId().getName());
        return;
    }
  
    setModified(true);
    Base::BaseClass* base = static_cast<Base::BaseClass*>(Base::Type::createInstanceByName(cName.c_str(),true));
    if (base) {
        // type not derived from ViewProviderDocumentObject!!!
        assert(base->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId()));
        ViewProviderDocumentObject *pcProvider = static_cast<ViewProviderDocumentObject*>(base);
        d->_ViewProviderMap[&Obj] = pcProvider;

        try {
            // if succesfully created set the right name and calculate the view
            pcProvider->attach(&Obj);
            pcProvider->updateView();
            pcProvider->setActiveMode();
        }
        catch(const Base::MemoryException& e){
            Base::Console().Error("Memory exception in '%s' thrown: %s\n",Obj.getNameInDocument(),e.what());
        }
        catch(Base::Exception &e){
            e.ReportException();
        }
#ifndef FC_DEBUG
        catch(...){
            Base::Console().Error("App::Document::_RecomputeFeature(): Unknown exception in Feature \"%s\" thrown\n",Obj.getNameInDocument());
        }
#endif
        std::list<Gui::BaseView*>::iterator VIt;
        // cycling to all views of the document
        for (VIt = d->_LpcViews.begin();VIt != d->_LpcViews.end();VIt++) {
            View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
            if(pcIvView)
                pcIvView->getViewer()->addViewProvider(pcProvider);
        }
    
        // adding to the tree
        signalNewObject(*pcProvider);
    }
    else {
        Base::Console().Warning("Gui::Document::slotNewObject() no view provider for the object %s found\n",cName.c_str());
    }
}

void Document::slotDeletedObject(App::DocumentObject& Obj)
{
    std::list<Gui::BaseView*>::iterator VIt;
    setModified(true);
    //Base::Console().Log("Document::slotDeleteObject() called\n");
  
    // cycling to all views of the document
    ViewProvider* viewProvider = getViewProvider(&Obj);
    for (VIt = d->_LpcViews.begin();VIt != d->_LpcViews.end();VIt++) {
        View3DInventor *pcIvView = dynamic_cast<View3DInventor *>(*VIt);
        if (pcIvView && viewProvider) {
            if (d->_pcInEdit == viewProvider)
                resetEdit();
            pcIvView->getViewer()->removeViewProvider(viewProvider);
        }
    }

    if (viewProvider && viewProvider->getTypeId().isDerivedFrom(
        ViewProviderDocumentObject::getClassTypeId())) {
        // removing from tree
        signalDeletedObject(*(static_cast<ViewProviderDocumentObject*>(viewProvider)));

        delete viewProvider;
        d->_ViewProviderMap.erase(&Obj);
    }
}

void Document::slotChangedObject(App::DocumentObject& Obj, App::Property& Prop)
{
    //Base::Console().Log("Document::slotChangedObject() called\n");
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

        if (viewProvider->isDerivedFrom(ViewProviderDocumentObject::getClassTypeId()))
            signalChangedObject(*(static_cast<ViewProviderDocumentObject*>(viewProvider)));
    }

    // a property of an object has changed
    setModified(true);
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
    d->_isModified = b;
    
    std::list<MDIView*> mdis = getMDIViews();
    for (std::list<MDIView*>::iterator it = mdis.begin(); it != mdis.end(); ++it) {
        (*it)->setWindowModified(b);
    }
}

bool Document::isModified() const
{
    return d->_isModified;
}

App::Document* Document::getDocument(void) const
{
    return d->_pcDocument;
}

/// Save the document
bool Document::save(void)
{
    if (d->_pcDocument->isSaved()) {
        Gui::WaitCursor wc;
        Command::doCommand(Command::Doc,"App.getDocument(\"%s\").save()"
                                       ,d->_pcDocument->getName());
        setModified(false);
        return true;
    }
    else {
        return saveAs();
    }
}

/// Save the document under a new file name
bool Document::saveAs(void)
{
    getMainWindow()->statusBar()->showMessage(QObject::tr("Save document under new filename..."));

    QString exe = QString::fromUtf8(App::Application::Config()["ExeName"].c_str());
    QString fn = QFileDialog::getSaveFileName(getMainWindow(), QObject::tr("Save %1 Document").arg(exe), 
                 QDir::currentPath(), QObject::tr("%1 document (*.FCStd)").arg(exe));
    if (!fn.isEmpty()) {
        QString file = fn.toLower();
        if (!file.endsWith(QLatin1String(".fcstd"))) {
            fn += QLatin1String(".fcstd");
            QFileInfo fi;
            fi.setFile(fn);
            if (fi.exists()) {
                // if we auto-append the extension make sure that we don't override an existing file
                int ret = QMessageBox::question(getMainWindow(), QObject::tr("Save As"), 
                    QObject::tr("%1 already exists.\n"
                                "Do you want to replace it?").arg(fn),
                                QMessageBox::Yes|QMessageBox::Default,
                                QMessageBox::No|QMessageBox::Escape); 
                if (ret != QMessageBox::Yes)
                    fn = QString();
            }
        }
    }

    if (!fn.isEmpty()) {
        QFileInfo fi;
        fi.setFile(fn);
        QString bn = fi.baseName();

        const char * DocName = App::GetApplication().getDocumentName(getDocument());

        // save as new file name
        Gui::WaitCursor wc;
        Command::doCommand(Command::Doc,"App.getDocument(\"%s\").FileName = \"%s\""
                                       , DocName, (const char*)fn.toUtf8());
        Command::doCommand(Command::Doc,"App.getDocument(\"%s\").Label = \"%s\""
                                       , DocName, (const char*)bn.toUtf8());
        Command::doCommand(Command::Doc,"App.getDocument(\"%s\").save()"
                                       , DocName);
        setModified(false);

        getMainWindow()->appendRecentFile(fi.filePath());
        return true;
    }
    else {
        getMainWindow()->statusBar()->showMessage(QObject::tr("Saving aborted"), 2000);
        return false;
    }
}

unsigned int Document::getMemSize (void) const
{
    unsigned int size = 0;

    // size of the view providers in the document
    std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator it;
    for (it = d->_ViewProviderMap.begin(); it != d->_ViewProviderMap.end(); ++it)
        size += it->second->getMemSize();
    return size;
}

/** 
 * Adds a separate XML file to the projects file that contains information about the view providers.
 */
void Document::Save (Base::Writer &writer) const
{
    // It's only possible to add extra information if force of XML is disabled
    if (writer.isForceXML() == false) {
        writer.addFile("GuiDocument.xml", this);

        if (App::GetApplication().GetParameterGroupByPath
            ("User parameter:BaseApp/Preferences/Document")->GetBool("SaveThumbnail",false)) {
            std::list<MDIView*> mdi = getMDIViews();
            for (std::list<MDIView*>::iterator it = mdi.begin(); it != mdi.end(); ++it) {
                if ((*it)->getTypeId().isDerivedFrom(View3DInventor::getClassTypeId())) {
                    View3DInventorViewer* view = static_cast<View3DInventor*>(*it)->getViewer();
                    d->thumb.setFileName(d->_pcDocument->FileName.getValue());
                    d->thumb.setSize(128);
                    d->thumb.setViewer(view);
                    d->thumb.Save(writer);
                    break;
                }
            }
        }
    }
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
    if (scheme == 1) {
        // read the viewproviders itself
        xmlReader.readElement("ViewProviderData");
        Cnt = xmlReader.getAttributeAsInteger("Count");
        for (i=0 ;i<Cnt ;i++) {
            xmlReader.readElement("ViewProvider");
            std::string name = xmlReader.getAttribute("name");
            ViewProvider* pObj = getViewProviderByName(name.c_str());
            if (pObj) // check if this feature has been registered
            {
                pObj->Restore(xmlReader);

                // As the view providers don't get notified when their proprties
                // change while reading we must force this here
                std::map<std::string,App::Property*> Map;
                pObj->getPropertyMap(Map);
                for (std::map<std::string,App::Property*>::iterator it = Map.begin();
                     it != Map.end(); ++it)
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
        if (strcmp(ppReturn, "") != 0) { // non-empty attribute
            if (d->_pcAppWnd->sendHasMsgToActiveView("SetCamera"))
                d->_pcAppWnd->sendMsgToActiveView(sMsg.c_str());
        }
    }

    xmlReader.readEndElement("Document");

    // reset modifeid flag
    setModified(false);
}

/**
 * Saves the properties of the view providers.
 */
void Document::SaveDocFile (Base::Writer &writer) const
{
    writer.Stream() << "<?xml version='1.0' encoding='utf-8'?>" << std::endl
                    << "<!--" << std::endl
                    << " FreeCAD Document, see http://free-cad.sourceforge.net for more informations..."
                    << std::endl << "-->" << std::endl;

    writer.Stream() << "<Document SchemaVersion=\"1\">" << std::endl;

    std::map<App::DocumentObject*,ViewProviderDocumentObject*>::const_iterator it;

    // writing the view provider names itself
    writer.incInd(); // indention for 'ViewProviderData Count'
    writer.Stream() << writer.ind() << "<ViewProviderData Count=\"" 
                    << d->_ViewProviderMap.size() <<"\">" << std::endl;

    bool xml = writer.isForceXML();
    writer.setForceXML(true);
    writer.incInd(); // indention for 'ViewProvider name'
    for(it = d->_ViewProviderMap.begin(); it != d->_ViewProviderMap.end(); ++it) {
        App::DocumentObject* doc = it->first;
        ViewProvider* obj = it->second;
        writer.Stream() << writer.ind() << "<ViewProvider name=\""
                        << doc->getNameInDocument() << "\">" << std::endl;
        obj->Save(writer);
        writer.Stream() << writer.ind() << "</ViewProvider>" << std::endl;
    }
    writer.setForceXML(xml);

    writer.decInd(); // indention for 'ViewProvider name'
    writer.Stream() << writer.ind() << "</ViewProviderData>" << std::endl;
    writer.decInd();  // indention for 'ViewProviderData Count'

    // set camera settings
    QString viewPos;
    if (d->_pcAppWnd->sendHasMsgToActiveView("GetCamera")) {
        const char* ppReturn=0;
        d->_pcAppWnd->sendMsgToActiveView("GetCamera",&ppReturn);
  
        // remove the first line because it's a comment like '#Inventor V2.1 ascii'
        QStringList lines = QString(QString::fromAscii(ppReturn)).split(QLatin1String("\n"));
        if (lines.size() > 1) {
            lines.pop_front();
            viewPos = lines.join(QLatin1String(" "));
        }
    }

    writer.incInd(); // indention for camera settings
    writer.Stream() << writer.ind() << "<Camera settings=\"" 
                    << (const char*)viewPos.toAscii() <<"\"/>" << std::endl;
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
        for (It1=d->_ViewProviderMap.begin();It1!=d->_ViewProviderMap.end();++It1)
            ((View3DInventor*)pcView3D)->getViewer()->addViewProvider(It1->second);
        std::map<std::string,ViewProvider*>::const_iterator It2;
        for (It2=d->_ViewProviderMapAnnotation.begin();It2!=d->_ViewProviderMapAnnotation.end();++It2)
            ((View3DInventor*)pcView3D)->getViewer()->addViewProvider(It2->second);

    } else /* if(strcmp(sType,"View3DOCC") == 0){
        pcView3D = new MDIView3D(this,_pcAppWnd,"View3DOCC");
    }else*/
    {
        Base::Console().Error("Document::createView(): Unknown view type: %s\n",sType);
        return;
    }

    const char* name = getDocument()->Label.getValue();

    QString aName = QString::fromAscii("%1 : %2[*]")
        .arg(QString::fromUtf8(name)).arg(d->_iWinCount++);

    pcView3D->setWindowTitle(aName);
    pcView3D->setWindowIcon(FCIcon);
    pcView3D->resize( 400, 300 );
    getMainWindow()->addWindow(pcView3D);
}

void Document::attachView(Gui::BaseView* pcView, bool bPassiv)
{
  if(!bPassiv)
    d->_LpcViews.push_back(pcView);
  else
    d->_LpcPassivViews.push_back(pcView);
}

void Document::detachView(Gui::BaseView* pcView, bool bPassiv)
{
    if (bPassiv) {
        if (find(d->_LpcPassivViews.begin(),d->_LpcPassivViews.end(),pcView)
            != d->_LpcPassivViews.end())
        d->_LpcPassivViews.remove(pcView);
    }
    else {
        if (find(d->_LpcViews.begin(),d->_LpcViews.end(),pcView)
            != d->_LpcViews.end())
        d->_LpcViews.remove(pcView);

        // last view?
        if (d->_LpcViews.size() == 0) {
            // decouple a passive view
            std::list<Gui::BaseView*>::iterator It = d->_LpcPassivViews.begin();
            while (It != d->_LpcPassivViews.end()) {
                (*It)->setDocument(0);
                It = d->_LpcPassivViews.begin();
            }

            // is already  closing the document
            if (d->_isClosing == false)
                d->_pcAppWnd->onLastWindowClosed(this);
        }
    }
}

void Document::onUpdate(void)
{
#ifdef FC_LOGUPDATECHAIN
    Base::Console().Log("Acti: Gui::Document::onUpdate()");
#endif

    std::list<Gui::BaseView*>::iterator It;

    for(It = d->_LpcViews.begin();It != d->_LpcViews.end();It++) {
        (*It)->onUpdate();
    }

    for(It = d->_LpcPassivViews.begin();It != d->_LpcPassivViews.end();It++) {
        (*It)->onUpdate();
    }
}

void Document::onRelabel(void)
{
#ifdef FC_LOGUPDATECHAIN
    Base::Console().Log("Acti: Gui::Document::onRelabel()");
#endif

    std::list<Gui::BaseView*>::iterator It;

    for (It = d->_LpcViews.begin();It != d->_LpcViews.end();It++) {
        (*It)->onRelabel(this);
    }

    for (It = d->_LpcPassivViews.begin();It != d->_LpcPassivViews.end();It++) {
        (*It)->onRelabel(this);
    }
}

bool Document::isLastView(void)
{
    if (d->_LpcViews.size() <= 1)
        return true;
    return false;
}

/** 
 *  This method checks if the document can be closed. It checks on
 *  the save state of the document and is able to abort the closing.
 */
bool Document::canClose ()
{
    if (!getDocument()->isClosable()) {
        QMessageBox::warning(getActiveView(),
            QObject::tr("Document not closable"),
            QObject::tr("The document is not closable for the moment."));
        return false;
    }

    if (!isModified())
        return true;
    bool ok = true;
    switch(QMessageBox::question(getActiveView(),
        QObject::tr("Unsaved document"),
        QObject::tr("Save document before close?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No,
        QMessageBox::Cancel | QMessageBox::Escape))
    {
    case QMessageBox::Yes:
        ok = save();
        break;
    case QMessageBox::No:
        ok = true;
        break;
    case QMessageBox::Cancel:
        ok = false;
        break;
    }

    return ok;
}

std::list<MDIView*> Document::getMDIViews() const
{
    std::list<MDIView*> views;
    for (std::list<BaseView*>::const_iterator it = d->_LpcViews.begin();
         it != d->_LpcViews.end(); ++it) {
        MDIView* view = dynamic_cast<MDIView*>(*it);
        if (view)
            views.push_back(view);
    }

    return views;
}

/// send messages to the active view
bool Document::sendMsgToViews(const char* pMsg)
{
    std::list<Gui::BaseView*>::iterator It;
    const char** pReturnIgnore=0;

    for (It = d->_LpcViews.begin();It != d->_LpcViews.end();It++) {
        if ((*It)->onMsg(pMsg,pReturnIgnore)) {
            return true;
        }
    }

    for (It = d->_LpcPassivViews.begin();It != d->_LpcPassivViews.end();It++) {
        if ((*It)->onMsg(pMsg,pReturnIgnore)) {
            return true;
        }
    }

    return false;
}

/// Getter for the active view
MDIView* Document::getActiveView(void) const
{
    // get the main window's active view 
    MDIView* active = getMainWindow()->activeWindow();

    // get all MDI views of the document
    std::list<MDIView*> mdis = getMDIViews();

    // check whether the active view is part of this document
    bool ok=false;
    for (std::list<MDIView*>::const_iterator it = mdis.begin(); it != mdis.end(); ++it) {
        if ((*it) == active) {
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
 *  This method opens a new UNDO transaction on the active document. This transaction
 *  will later appear in the UNDO/REDO dialog with the name of the command. If the user 
 *  recall the transaction everything changed on the document between OpenCommand() and 
 *  CommitCommand will be undone (or redone). You can use an alternetive name for the 
 *  operation default is the command name.
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

/// Get a string vector with the 'Undo' actions
std::vector<std::string> Document::getUndoVector(void) const
{
    return getDocument()->getAvailableUndoNames();
}

/// Get a string vector with the 'Redo' actions
std::vector<std::string> Document::getRedoVector(void) const
{
    return getDocument()->getAvailableRedoNames();
}

/// Will UNDO  one or more steps
void Document::undo(int iSteps)
{
    for (int i=0;i<iSteps;i++) {
        getDocument()->undo();
    }
}

/// Will REDO  one or more steps
void Document::redo(int iSteps)
{
    for (int i=0;i<iSteps;i++) {
        getDocument()->redo();
    }
}

PyObject* Document::getPyObject(void)
{
    _pcDocPy->IncRef();
    return _pcDocPy;
}
