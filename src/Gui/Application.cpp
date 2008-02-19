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
# include "InventorAll.h"
# include <boost/signals.hpp>
# include <boost/bind.hpp>
#endif

// FreeCAD Base header
#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Factory.h>
#include <Base/FileInfo.h>
#include <App/Document.h>

#include "Application.h"
#include "MainWindow.h"
#include "Document.h"
#include "View.h"
#include "View3DPy.h"
#include "WidgetFactory.h"
#include "Command.h"
#include "Macro.h"
#include "ProgressBar.h"
#include "Workbench.h"
#include "WorkbenchManager.h"
#include "CommandBarManager.h"
#include "WaitCursor.h"
#include "MenuManager.h"
#include "Window.h"
#include "Selection.h"
#include "BitmapFactory.h"
#include "SoFCDB.h"

#include "View3DInventor.h"
#include "ViewProvider.h"
#include "ViewProviderExtern.h"
#include "ViewProviderFeature.h"
#include "ViewProviderPythonFeature.h"
#include "ViewProviderDocumentObjectGroup.h"
#include "ViewProviderGeometryObject.h"
#include "ViewProviderInventorObject.h"

#include "Language/Translator.h"
#include "GuiInitScript.h"


using namespace Gui;
using namespace Gui::DockWnd;
using namespace std;


Application* Application::Instance = 0L;

namespace Gui {

// Pimpl class
struct ApplicationP
{
  ApplicationP() : _pcActiveDocument(0L), _bIsClosing(false), _bStartingUp(true)
  {
    // create the macro manager
    _pcMacroMngr = new MacroManager();
  }

  ~ApplicationP()
  {
    delete _pcMacroMngr;
  }

  /// list of all handled documents
  map<App::Document*, Gui::Document*> lpcDocuments;
  /// Active document
  Gui::Document*   _pcActiveDocument;
  MacroManager*  _pcMacroMngr;
  /// List of all registered views
  list<Gui::BaseView*>					_LpcViews;
  bool _bIsClosing;
  bool _bStartingUp;
  /// Handles all commands 
  CommandManager _cCommandManager;
};

} // namespace Gui

Application::Application()
{
  //App::GetApplication().Attach(this);
  App::GetApplication().signalNewDocument.connect(boost::bind(&Gui::Application::slotNewDocument, this, _1));
  App::GetApplication().signalDeleteDocument.connect(boost::bind(&Gui::Application::slotDeleteDocument, this, _1));
  App::GetApplication().signalRenameDocument.connect(boost::bind(&Gui::Application::slotRenameDocument, this, _1));
  App::GetApplication().signalActiveDocument.connect(boost::bind(&Gui::Application::slotActiveDocument, this, _1));
  App::GetApplication().signalRelabelDocument.connect(boost::bind(&Gui::Application::slotRelabelDocument, this, _1));


  // install the last active language
  ParameterGrp::handle hPGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
  hPGrp = hPGrp->GetGroup("Preferences")->GetGroup("General");
  Translator::instance()->activateLanguage(hPGrp->GetASCII("Language", "English").c_str());
  GetWidgetFactorySupplier();

  // setting up Python binding
  PyObject* module = Py_InitModule3("FreeCADGui", Application::Methods,
    "The functions in the FreeCADGui module allow working with GUI documents,\n"
    "view providers, views, workbenches and much more.\n\n"
    "The FreeCADGui instance provides a list of references of GUI documents which\n"
    "can be addressed by a string. These documents contain the view providers for\n"
    "objects in the associated App document. An App and GUI document can be\n"
    "accessed with the same name.\n\n"
    "The FreeCADGui module also provides a set of functions to work with so called\n"
    "workbenches.");
  Py::Module(module).setAttr(std::string("ActiveDocument"),Py::None());

  d = new ApplicationP;

  // global access 
  Instance = this;

  // instanciate the workbench dictionary
  _pcWorkbenchDictionary = PyDict_New();

  createStandardOperations();
  MacroCommand::load();
}

Application::~Application()
{
  Base::Console().Log("Destruct Gui::Application\n");
  WorkbenchManager::destruct();
  SelectionSingleton::destruct();
  Translator::destruct();
  WidgetFactorySupplier::destruct();
  BitmapFactoryInst::destruct();

  // finish also Inventor subsystem
  View3DPy::cleanupSWIG();
  SoFCDB::finish();
  SoQt::done();

#if (COIN_MAJOR_VERSION >= 2) && (COIN_MINOR_VERSION >= 4)
  SoDB::finish();
#elif (COIN_MAJOR_VERSION >= 3)
  SoDB::finish();
#else
  SoDB::cleanup();
#endif

  // save macros
  MacroCommand::save();
  //App::GetApplication().Detach(this);

  delete d;
  Instance = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Application::open(const char* FileName)
{
  WaitCursor wc;
  Base::FileInfo File(FileName);
  string te = File.extension();
  const char* Mod = App::GetApplication().hasOpenType( te.c_str() );

  if (Mod != 0) {
    // issue module loading
    Command::doCommand(Command::App, "import %s", Mod);

    // issue gui module loading
    try{
      Command::doCommand(Command::Gui, "import %sGui", Mod);
    } catch (const Base::PyException&){
      // ignore this type of exception (e.g. if Mod is already a Gui module)
    }

    try {
      // load the file with the module
      Command::doCommand(Command::App, "%s.open(\"%s\")", Mod, File.filePath().c_str());
      if ( activeDocument() )
        activeDocument()->setModified(false);
      // ViewFit
      if ( !File.hasExtension("FCStd") && sendHasMsgToActiveView("ViewFit") )
        //Command::doCommand(Command::Gui, "Gui.activeDocument().activeView().fitAll()");
          Command::doCommand(Command::Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
      // the original file name is required
      getMainWindow()->appendRecentFile(QString::fromUtf8(File.filePath().c_str()));
    } catch (const Base::PyException& e){
      // Usually thrown if the file is invalid somehow
      e.ReportException();
    }
  } else {
    wc.restoreCursor();
    QMessageBox::warning(getMainWindow(), QObject::tr("Unknown file type"), QObject::tr("Cannot open unknown file type: %1").arg(te.c_str()));
    wc.setWaitCursor();
    return;
  }
}

void Application::import(const char* FileName, const char* DocName)
{
  WaitCursor wc;
  Base::FileInfo File(FileName);
  string te = File.extension();
  const char* Mod = App::GetApplication().hasOpenType( te.c_str() );

  if (Mod != 0) {
    // issue module loading
    Command::doCommand(Command::App, "import %s", Mod);

    // issue gui module loading
    try{
      Command::doCommand(Command::Gui, "import %sGui", Mod);
    } catch (const Base::PyException&){
      // ignore this type of exception (e.g. if Mod is already a Gui module)
    }

    try {
      // load the file with the module
      if ( File.hasExtension("FCStd") )
      {
        Command::doCommand(Command::App, "%s.open(\"%s\")", Mod, File.filePath().c_str());
        if ( activeDocument() )
          activeDocument()->setModified(false);
      }
      else
      {
        Command::doCommand(Command::App, "%s.insert(\"%s\",\"%s\")", Mod, File.filePath().c_str(), DocName);
        if ( activeDocument() )
          activeDocument()->setModified(true);
      }

      // the original file name is required
      getMainWindow()->appendRecentFile( File.filePath().c_str() );
    } catch (const Base::PyException& e){
      // Usually thrown if the file is invalid somehow
      e.ReportException();
    }
  } else {
    wc.restoreCursor();
    QMessageBox::warning(getMainWindow(), QObject::tr("Unknown file type"), QObject::tr("Cannot open unknown file type: %1").arg(te.c_str()));
    wc.setWaitCursor();
    return;
  }
}

void Application::createStandardOperations()
{
  // register the application Standard commands from CommandStd.cpp
  Gui::CreateStdCommands();
  Gui::CreateDocCommands();
  Gui::CreateFeatCommands();
  Gui::CreateViewStdCommands();
  Gui::CreateWindowStdCommands();
  Gui::CreateTestCommands();
}

void Application::slotNewDocument(App::Document& Doc)
{
#ifdef FC_DEBUG
  std::map<App::Document*, Gui::Document*>::const_iterator it = d->lpcDocuments.find(&Doc);
  assert(it==d->lpcDocuments.end());
#endif
  Gui::Document* pDoc = new Gui::Document(&Doc,this);
  d->lpcDocuments[&Doc] = pDoc;
  signalNewDocument(*pDoc);
  pDoc->createView("View3DIv");
}

void Application::slotDeleteDocument(App::Document& Doc)
{
  std::map<App::Document*, Gui::Document*>::iterator doc = d->lpcDocuments.find(&Doc);
#ifdef FC_DEBUG
  assert(doc!=d->lpcDocuments.end());
#endif
  

  // We must clear the selection here to notify all observers
  Gui::Selection().clearSelection(doc->second->getDocument()->getName());
  signalDeleteDocument(*doc->second);

  // If the active document gets destructed we must set it to 0. If there are further existing documents then the 
  // view that becomes active sets the active document again. So, we needn't worry about this.
  if ( d->_pcActiveDocument == doc->second )
    setActiveDocument(0);

  delete doc->second; // destroy the Gui document
  d->lpcDocuments.erase(doc);
}

void Application::slotRelabelDocument(App::Document& Doc)
{
  std::map<App::Document*, Gui::Document*>::iterator doc = d->lpcDocuments.find(&Doc);
#ifdef FC_DEBUG
  assert(doc!=d->lpcDocuments.end());
#endif

  signalRelabelDocument(*doc->second);
  doc->second->onRename();
}

void Application::slotRenameDocument(App::Document& Doc)
{
  std::map<App::Document*, Gui::Document*>::iterator doc = d->lpcDocuments.find(&Doc);
#ifdef FC_DEBUG
  assert(doc!=d->lpcDocuments.end());
#endif

  signalRenameDocument(*doc->second);
}

void Application::slotActiveDocument(App::Document& Doc)
{
  std::map<App::Document*, Gui::Document*>::iterator doc = d->lpcDocuments.find(&Doc);
#ifdef FC_DEBUG
  assert(doc!=d->lpcDocuments.end());
#endif

  signalActiveDocument(*doc->second);
}

void Application::onLastWindowClosed(Gui::Document* pcDoc)
{
  if(!d->_bIsClosing && pcDoc)
  {
    try {
      // Call the closing mechanism from Python. This also checks whether pcDoc is the last open document.
      Command::doCommand(Command::Doc, "App.closeDocument(\"%s\")", pcDoc->getDocument()->getName());
    } catch (const Base::PyException& e) {
      e.ReportException();
    }
  }
}

/// send Messages to the active view
bool Application::sendMsgToActiveView(const char* pMsg, const char** ppReturn)
{
  MDIView* pView = getMainWindow()->activeWindow();

  if(pView){
    return pView->onMsg(pMsg,ppReturn);
  }else
    return false;
}

bool Application::sendHasMsgToActiveView(const char* pMsg)
{
  MDIView* pView = getMainWindow()->activeWindow();

  if(pView){
    return pView->onHasMsg(pMsg);
  }else
    return false;
}

/// Geter for the Active View
Gui::Document* Application::activeDocument(void) const
{
  return d->_pcActiveDocument;
  /*
  MDIView* pView = GetActiveView();

  if(pView)
    return pView->GetGuiDocument();
  else
    return 0l;*/
}

void Application::setActiveDocument(Gui::Document* pcDocument)
{
  d->_pcActiveDocument = pcDocument;
  std::string name;
 
  // This adds just a line to the macro file but does not set the active document
  if(pcDocument){
    name += "App.setActiveDocument(\"";
    name += pcDocument->getDocument()->getName(); 
    name +=  "\")\n";
    name += "App.ActiveDocument=App.getDocument(\"";
    name += pcDocument->getDocument()->getName(); 
    name +=  "\")\n";
    name += "Gui.ActiveDocument=Gui.getDocument(\"";
    name += pcDocument->getDocument()->getName(); 
    name +=  "\")";
    macroManager()->addLine(MacroManager::Gui,name.c_str());
  }else{
    name += "App.setActiveDocument(\"\")\n";
    name += "App.ActiveDocument=None\n";
    name += "Gui.ActiveDocument=None";
    macroManager()->addLine(MacroManager::Gui,name.c_str());
  }

  // Sets the currently active document
  try {
    Base::Interpreter().runString(name.c_str());
  } catch (const Base::Exception& e) {
    Base::Console().Warning(e.what());
    return;
  }

#ifdef FC_LOGUPDATECHAIN
  Console().Log("Acti: Gui::Document,%p\n",d->_pcActiveDocument);
#endif

  // notify all views attached to the application (not views belong to a special document)
  for(list<Gui::BaseView*>::iterator It=d->_LpcViews.begin();It!=d->_LpcViews.end();It++)
    (*It)->setDocument(pcDocument);
}

Gui::Document* Application::getDocument( const char* name ) const
{
  App::Document* pDoc = App::GetApplication().getDocument( name );
  std::map<App::Document*, Gui::Document*>::const_iterator it = d->lpcDocuments.find(pDoc);
  if ( it!=d->lpcDocuments.end() )
    return it->second;
  else
    return 0;
}

Gui::Document* Application::getDocument(App::Document* pDoc) const
{
  std::map<App::Document*, Gui::Document*>::const_iterator it = d->lpcDocuments.find(pDoc);
  if ( it!=d->lpcDocuments.end() )
    return it->second;
  else
    return 0;
}

void Application::attachView(Gui::BaseView* pcView)
{
  d->_LpcViews.push_back(pcView);
}

void Application::detachView(Gui::BaseView* pcView)
{
  d->_LpcViews.remove(pcView);
}

void Application::onUpdate(void)
{
  // update all documents
  for(map<App::Document*, Gui::Document*>::iterator It = d->lpcDocuments.begin();It != d->lpcDocuments.end();It++)
  {
    It->second->onUpdate();
  }
  // update all the independed views
  for(list<Gui::BaseView*>::iterator It2 = d->_LpcViews.begin();It2 != d->_LpcViews.end();It2++)
  {
    (*It2)->onUpdate();
  }
}

/// Gets called if a view gets activated, this manages the whole activation scheme
void Application::viewActivated(MDIView* pcView)
{
#ifdef FC_LOGUPDATECHAIN
  Console().Log("Acti: %s,%p\n",pcView->getName(),pcView);
#endif

  // set the new active document
  if(pcView->isPassive())
    setActiveDocument(0);
  else
    setActiveDocument(pcView->getGuiDocument());
}


void Application::updateActive(void)
{
  activeDocument()->onUpdate();
}

void Application::tryClose ( QCloseEvent * e )
{
  if(d->lpcDocuments.size() == 0)
  {
    e->accept();
  }else{
    // ask all documents if closable
    for (map<App::Document*, Gui::Document*>::iterator It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
#ifndef FC_DEBUG
      MDIView* active = It->second->getActiveView();
      active->setFocus(); // raises the view to front
#endif

      It->second->canClose ( e );
      if(! e->isAccepted() ) return;
    }
  }

  // ask all passive views if closable
  for (list<Gui::BaseView*>::iterator It2 = d->_LpcViews.begin();It2!=d->_LpcViews.end();It2++)
  {
    if((*It2)->canClose() )
      e->accept();
    else 
      e->ignore();

    if(! e->isAccepted() ) return;
  }

  if( e->isAccepted() )
  {
    d->_bIsClosing = true;

    map<App::Document*, Gui::Document*>::iterator It;

    // close all views belonging to a document
    for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
      It->second->closeAllViews();
    }

    //detach the passive views
    //SetActiveDocument(0);
    list<Gui::BaseView*>::iterator It2 = d->_LpcViews.begin();
    while (It2!=d->_LpcViews.end())
    {
      (*It2)->onClose();
      It2 = d->_LpcViews.begin();
    }

    // remove all documents
    size_t cnt = d->lpcDocuments.size();
    while ( d->lpcDocuments.size() > 0 && cnt > 0 )
    {
      // destroys also the Gui document
      It = d->lpcDocuments.begin();
      App::GetApplication().closeDocument(It->second->getDocument()->getName());
      --cnt; // avoid infinite loop
    }
//    for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
//    {
//      delete It->second;
//    }
  }
}

/**
 * Activate the matching workbench to the registered workbench handler with name \a name.
 * The handler must be an instance of a class written in Python.
 * Normally, if a handler gets activated a workbench with the same name gets created unless it
 * already exists. 
 *
 * The old workbench gets deactivated before. If the workbench to the handler is already
 * active or if the switch fails false is returned. 
 */
bool Application::activateWorkbench(const char* name)
{
    bool ok = false;
    WaitCursor wc;
    Workbench* oldWb = WorkbenchManager::instance()->active();
    if (oldWb && oldWb->name() == name)
        return false; // already active

    // we check for the currently active workbench and call its 'Deactivated'
    // method, if available
    PyObject* pcOldWorkbench = 0;
    if (oldWb) {
        pcOldWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, oldWb->name().toAscii());
    }

    // get the python workbench object from the dictionary
    PyObject* pcWorkbench = 0;
    pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, name);
    // test if the workbench exists
    if (!pcWorkbench)
        return false;

    try {
        QString type;
        Base::PyGILStateLocker lock;
        Py::Object handler(pcWorkbench);
        if (!handler.hasAttr(std::string("__Workbench__"))) {
            // call its GetClassName method if possible
            Py::Callable method(handler.getAttr(std::string("GetClassName")));
            Py::Tuple args;
            Py::String result(method.apply(args));
            type = result.as_std_string().c_str();
            if (type == "Gui::PythonWorkbench") {
                Workbench* wb = WorkbenchManager::instance()->createWorkbench(name, type);
                handler.setAttr(std::string("__Workbench__"), Py::Object(wb->getPyObject()));
            }

            // import the matching module first
            Py::Callable activate(handler.getAttr(std::string("Initialize")));
            activate.apply(args);
        }

        // the Python workbench handler has changed the workbench
        Workbench* newWb = WorkbenchManager::instance()->active();
        if (newWb && newWb->name() == name)
            ok = true; // already active
        // now try to create and activate the matching workbench object
        else if (WorkbenchManager::instance()->activate(name, type)) {
            getMainWindow()->activateWorkbench(QString(name));
            this->signalActivateWorkbench(name);
            ok = true;
        }

        // if we still not have this member then it must be built-in C++ workbench
        // which could be created after loading the appropriate module
        if (!handler.hasAttr(std::string("__Workbench__"))) {
            Workbench* wb = WorkbenchManager::instance()->getWorkbench(name);
            if (wb) handler.setAttr(std::string("__Workbench__"), Py::Object(wb->getPyObject()));
        }

        // If the method Deactivate is available we call it
        if (pcOldWorkbench) {
            Py::Object handler(pcOldWorkbench);
            if (handler.hasAttr(std::string("Deactivated"))) {
                Py::Object method(handler.getAttr(std::string("Deactivated")));
                if (method.isCallable()) {
                    Py::Tuple args;
                    Py::Callable activate(method);
                    activate.apply(args);
                }
            }
        }

        // If the method Activate is available we call it
        if (handler.hasAttr(std::string("Activated"))) {
            Py::Object method(handler.getAttr(std::string("Activated")));
            if (method.isCallable()) {
                Py::Tuple args;
                Py::Callable activate(method);
                activate.apply(args);
            }
        }
    }
    catch (Py::Exception& e) {
        Py::Object o = Py::type(e);
        e.clear();
        if (o.isString()) {
            Py::String s(o);
            QString msg = s.as_std_string().c_str();
            QRegExp rx;
            // ignore '<type 'exceptions.ImportError'>' prefixes
            rx.setPattern("^\\s*<type 'exceptions.ImportError'>:\\s*");
            int pos = rx.indexIn(msg);
            while ( pos != -1 ) {
                msg = msg.mid(rx.matchedLength());
                pos = rx.indexIn(msg);
            }

            Base::Console().Error("The workbench %s couldn't be loaded due to following error:%s\n",
                                  name, (const char*)msg.toAscii());
            if (!d->_bStartingUp) {
                wc.restoreCursor();
                QMessageBox::critical(getMainWindow(), QObject::tr("Cannot load workbench"), 
                    QObject::tr("The workbench %1 couldn't be loaded due to following error:\n\n%2").
                    arg(name).arg(msg));
                wc.setWaitCursor();
            }
        }
    }

    return ok;
}

QPixmap Application::workbenchIcon(const QString& wb) const
{
    // get the python workbench object from the dictionary
    PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, wb.toAscii());
    // test if the workbench exists
    if (pcWorkbench) {
        // make a unique icon name
        QString iconName;
        iconName.sprintf("%p", static_cast<const void *>(pcWorkbench));
        QPixmap icon;
        if (BitmapFactory().findPixmapInCache(iconName, icon))
            return icon;

        // get its Icon member if possible
        try {
            Py::Object handler(pcWorkbench);
            Py::Object member = handler.getAttr(std::string("Icon"));
            Py::String data(member);
            std::string content = data.as_std_string();

            // test if in XPM format
            QByteArray ary;
            int strlen = (int)content.size();
            ary.resize(strlen);
            for (int j=0; j<strlen; j++)
                ary[j]=content[j];
            icon.loadFromData(ary, "XPM");

            if (icon.isNull()) {
                // is if a file name...
                QString file = QString::fromUtf8(content.c_str());
                icon.load(file);
                if (icon.isNull()) {
                    // ... or the name of another icon?
                    icon = BitmapFactory().pixmap(file);
                }
            }

            if (!icon.isNull()) {
                BitmapFactory().addPixmapToCache(iconName, icon);
            }

            return icon;
        }
        catch (Py::Exception& e) {
            e.clear();
        }
    }

    return QPixmap();
}

QString Application::workbenchToolTip(const QString& wb) const
{
    // get the python workbench object from the dictionary
    PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, wb.toAscii());
    // test if the workbench exists
    if (pcWorkbench) {
        // get its ToolTip member if possible
        try {
            Py::Object handler(pcWorkbench);
            Py::Object member = handler.getAttr(std::string("ToolTip"));
            if (member.isString()) {
                Py::String tip(member);
                return QString::fromUtf8(tip.as_std_string().c_str());
            }
        }
        catch (Py::Exception& e) {
            e.clear();
        }
    }

    return QString();
}

QString Application::workbenchMenuText(const QString& wb) const
{
    // get the python workbench object from the dictionary
    PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, wb.toAscii());
    // test if the workbench exists
    if (pcWorkbench) {
        // get its ToolTip member if possible
        try {
            Py::Object handler(pcWorkbench);
            Py::Object member = handler.getAttr(std::string("MenuText"));
            if (member.isString()) {
                Py::String tip(member);
                return QString::fromUtf8(tip.as_std_string().c_str());
            }
        }
        catch (Py::Exception& e) {
            e.clear();
        }
    }

    return QString();
}

QStringList Application::workbenches(void) const
{
  // If neither 'HiddenWorkbench' nor 'ExtraWorkbench' is set then all workbenches are returned.
  const std::map<std::string,std::string>& config = App::Application::Config();
  std::map<std::string, std::string>::const_iterator ht = config.find("HiddenWorkbench");
  std::map<std::string, std::string>::const_iterator et = config.find("ExtraWorkbench");
  std::map<std::string, std::string>::const_iterator st = config.find("StartWorkbench");
  const char* start = (st != config.end() ? st->second.c_str() : "<none>");
  QStringList hidden, extra;
  if (ht != config.end()) { 
    QString items = ht->second.c_str();
    hidden = items.split(';', QString::SkipEmptyParts);
    if (hidden.isEmpty())
      hidden.push_back("");
  }
  if (et != config.end()) { 
    QString items = et->second.c_str();
    extra = items.split(';', QString::SkipEmptyParts);
    if (extra.isEmpty())
      extra.push_back("");
  }

  PyObject *key, *value;
  Py_ssize_t pos = 0;
  QStringList wb;
  // insert all items
  while (PyDict_Next(_pcWorkbenchDictionary, &pos, &key, &value)) {
    /* do something interesting with the values... */
    const char* wbName = PyString_AsString(key);
    // add only allowed workbenches
    bool ok = true;
    if (!extra.isEmpty()&&ok) {
      ok = (extra.indexOf(wbName) != -1);
    }
    if (!hidden.isEmpty()&&ok) {
      ok = (hidden.indexOf(wbName) == -1);
    }
    
    // okay the item is visible
    if (ok)
      wb.push_back( wbName );
    else if ( strcmp(wbName, start) == 0 ) // also allow start workbench in case it is hidden
      wb.push_back( wbName );
  }

  return wb;
}

void Application::setupContextMenu(const char* recipient, MenuItem* items) const
{
    Workbench* actWb = WorkbenchManager::instance()->active();
    if (actWb) {
        // when populating the context-menu of a Python workbench invoke the method 
        // 'ContextMenu' of the handler object
        if (actWb->getTypeId().isDerivedFrom(PythonWorkbench::getClassTypeId())) {
            static_cast<PythonWorkbench*>(actWb)->clearContextMenu();
            PyObject* pWorkbench = 0;
            pWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, actWb->name().toAscii());

            try {
                // call its GetClassName method if possible
                Py::Object handler(pWorkbench);
                Py::Callable method(handler.getAttr(std::string("ContextMenu")));
                Py::Tuple args(1);
                args.setItem(0, Py::String(recipient));
                method.apply(args);
            }
            catch (Py::Exception& e) {
                e.clear();
            }
        }
        actWb->setupContextMenu(recipient, items);
    }
}

bool Application::isClosing(void)
{
  return d->_bIsClosing;
}

MacroManager *Application::macroManager(void)
{
  return d->_pcMacroMngr;
}

CommandManager &Application::commandManager(void)
{
  return d->_cCommandManager;
}

void Application::runCommand(bool bForce, const char* sCmd,...)
{
  // temp buffer
  size_t format_len = strlen(sCmd)+4024;
  char* format = (char*) malloc(format_len);
  va_list namelessVars;
  va_start(namelessVars, sCmd);  // Get the "..." vars
  vsnprintf(format, format_len, sCmd, namelessVars);
  va_end(namelessVars);

  if (bForce)
    d->_pcMacroMngr->addLine(MacroManager::Base,format);
  else
    d->_pcMacroMngr->addLine(MacroManager::Gui,format);
  Base::Console().Log("CmdC: %s\n",format);

  try { 
    Base::Interpreter().runString(format);
  } catch (...) {
    // free memory to avoid a leak if an exception occurred
    free (format);
    throw;
  }

  free (format);
}

//**************************************************************************
// Init, Destruct and ingleton

void Application::initApplication(void)
{
  try {
    initTypes();
    new Base::ScriptProducer( "FreeCADGuiInit", FreeCADGuiInit );
  } catch (...) {
    // force to flush the log
    App::Application::destructObserver();
    throw;
  }
}

void Application::initTypes(void)
{
  // views
  Gui::BaseView                              ::init();
  Gui::MDIView                               ::init();
  Gui::View3DInventor                        ::init();
  // View Provider
  Gui::ViewProvider                          ::init();
  Gui::ViewProviderExtern                    ::init();
  Gui::ViewProviderDocumentObject            ::init();
  Gui::ViewProviderFeature                   ::init();
  Gui::ViewProviderPythonFeature             ::init();
  Gui::ViewProviderDocumentObjectGroup       ::init();
  Gui::ViewProviderGeometryObject            ::init();
  Gui::ViewProviderInventorObject            ::init();

  // Workbench
  Gui::Workbench                             ::init();
  Gui::StdWorkbench                          ::init();
  Gui::NoneWorkbench                         ::init();
  Gui::TestWorkbench                         ::init();
  Gui::PythonWorkbench                       ::init();
}

void messageHandler( QtMsgType type, const char *msg )
{
#ifdef FC_DEBUG
  switch ( type )
  {
    case QtDebugMsg:
      Base::Console().Message( msg );
      break;
    case QtWarningMsg:
      Base::Console().Warning( msg );
      break;
    case QtFatalMsg:
      Base::Console().Error( msg );
      abort();                    // deliberately core dump
  }
#else
  // do not stress user with Qt internals but write to log file if enabled
  Base::Console().Log( "%s\n", msg );
#endif
}

#ifdef FC_DEBUG // redirect Coin messages to FreeCAD
void messageHandlerCoin( const SoError * error, void * userdata )
{
  if ( error && error->getTypeId() == SoDebugError::getClassTypeId() )
  {
    const SoDebugError* dbg = static_cast<const SoDebugError*>(error);
    const char* msg = error->getDebugString().getString();
    switch ( dbg->getSeverity() )
    {
    case SoDebugError::INFO:
      Base::Console().Message( msg );
      break;
    case SoDebugError::WARNING:
      Base::Console().Warning( msg );
      break;
    default: // error
      Base::Console().Error( msg );
      break;
    }
  }
  else if ( error )
  {
    const char* msg = error->getDebugString().getString();
    Base::Console().Log( msg );
  }
}

void messageHandlerSoQt( const SbString errmsg, SoQt::FatalErrors errcode, void *userdata )
{
  Base::Console().Error( errmsg.getString() );
}
#endif

/**
 * This method gets called for Coin internal processes that take a long time.
 * Unfortunately, this framework doesn't seem to work why this method is commented out at the moment.
 */
/*
SbBool progressCallbackHandler(const SbName & itemid, float fraction, SbBool interruptible, void * userdata)
{
  if ( fraction < 0.0f )
    Base::Sequencer().halt();
  else if ( fraction == 0.0f )
    Base::Sequencer().start(itemid, 100);
  else if ( fraction == 1.0f )
    Base::Sequencer().stop();
  else
    Base::Sequencer().next();
  return true;
}*/

void Application::runApplication(void)
{
  // add resources
  Q_INIT_RESOURCE(resource);
  Q_INIT_RESOURCE(translation);

  // A new QApplication
  Base::Console().Log("Init: Creating Gui::Application and QApplication\n");
  // if application not yet created by the splasher
  int argc = App::Application::GetARGC();
  qInstallMsgHandler( messageHandler );
  QApplication mainApp(argc, App::Application::GetARGV());

  Application app;
  MainWindow mw;

  // init the Inventor subsystem
  SoDB::init();
  SoQt::init(&mw);
  SoFCDB::init();

  // show splasher while initializing the GUI
  mw.startSplasher();

  // running the GUI init script
  Base::Interpreter().runString(Base::ScriptFactory().ProduceScript("FreeCADGuiInit"));
  
  // stop splash screen and set immediately the active window that may be of interest
  // for scripts using Python binding for Qt
  mw.stopSplasher();
  mainApp.setActiveWindow(&mw);

  // Activate the correct workbench
  Base::Console().Log("Init: Activating default workbench\n");
  std::string start = App::Application::Config()["StartWorkbench"];
  start = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/General")->
                           GetASCII("AutoloadModule", start.c_str());
  app.activateWorkbench(start.c_str());

  // show the main window
  Base::Console().Log("Init: Showing main window\n");
  mw.loadWindowSettings();

#ifdef FC_DEBUG // redirect Coin messages to FreeCAD
  SoDebugError::setHandlerCallback( messageHandlerCoin, 0 );
  SoQt::setFatalErrorHandler( messageHandlerSoQt, 0 );
#endif

  // open the 'Iip of the day' dialog if needed
  mw.showTipOfTheDay();
  Instance->d->_bStartingUp = false;

  //Base::Console().Log("Init: Processing command line files\n");
  //unsigned short count = 0;
  //count = atoi(App::Application::Config()["OpenFileCount"].c_str());

  //std::string File;
  //for (unsigned short i=0; i<count; i++)
  //{
  //  // getting file name
  //  std::stringstream temp;
  //  temp << "OpenFile" << i;

  //  File = App::Application::Config()[temp.str()];

  //  // try to open
  //  try {
  //    app.open(File.c_str());
  //  } catch(...) {
  //    Base::Console().Error("Can't open file %s \n",File.c_str());
  //  }
  //}

  // processing all command line files
  App::Application::processCmdLineFiles();

  // Create new document?
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Document");
  if ( hGrp->GetBool("CreateNewDoc", false) ) {
    App::GetApplication().newDocument();
  }

  // run the Application event loop
  Base::Console().Log("Init: Entering event loop\n");

  try {
    mainApp.exec();
  } catch(...) {
    // catching nasty stuff coming out of the event loop
    App::Application::destructObserver();
    throw;
  }

  Base::Console().Log("Init: event loop left\n");
}
