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
# include <qapplication.h>
# include <qcursor.h>
# include <qeventloop.h>
# include <qmessagebox.h>
# include <qstatusbar.h>
# include <strstream>
# include <map>
#endif

#include <Inventor/errors/SoDebugError.h> 
#include <Inventor/Qt/SoQt.h> 

// FreeCAD Base header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"
#include "../Base/Factory.h"
#include "../Base/FileInfo.h"
#include "../App/Application.h"
#include "../App/Feature.h"
#include "../App/Document.h"

#include "Application.h"
#include "MainWindow.h"
#include "Document.h"
#include "View.h"
#include "Icons/developers.h"
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

#include "Language/Translator.h"
#include "GuiInitScript.h"

#include "ViewProvider.h"
#include "ViewProviderExtern.h"
#include "ViewProviderFeature.h"
#include "ViewProviderPythonFeature.h"
#include "ViewProviderDocumentObjectGroup.h"


using Base::Console;
using Base::Interpreter;
using namespace Gui;
using namespace Gui::DockWnd;
using namespace std;


Application* Application::Instance = 0L;

namespace Gui {

// Pimpl class
struct ApplicationP
{
  ApplicationP()
    : _pcActiveDocument(0L), _bIsClosing(false)
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
  /// Handels all commands 
  CommandManager _cCommandManager;
};

} // namespace Gui

Application::Application()
{
  App::GetApplication().Attach(this);

  Gui::Translator::installLanguage();
  GetWidgetFactorySupplier();

  // seting up Python binding
  (void) Py_InitModule("FreeCADGui", Application::Methods);

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
  // save macros
  MacroCommand::save();
  delete d;

  App::GetApplication().Detach(this);
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

  if ( Mod != 0 )
  {
    // issue module loading
    Command::doCommand(Command::App, "import %s", Mod);

    // issue gui module loading
    try{
      Command::doCommand(Command::Gui, "import %sGui", Mod);
    } catch (const Base::PyException&){
      // ignore this type of exception (e.g. if Mod is already a Gui module)
    }

    try{
      // load the file with the module
      Command::doCommand(Command::App, "%s.open(\"%s\")", Mod, File.filePath().c_str());
      if ( activeDocument() )
        activeDocument()->setModified(false);
      // ViewFit
      if ( !File.hasExtension("FCStd") && sendHasMsgToActiveView("ViewFit") )
          Command::doCommand(Command::Gui, "Gui.SendMsgToActiveView(\"ViewFit\")");
      // the original file name is required
      getMainWindow()->appendRecentFile( File.filePath().c_str() );
    } catch (const Base::PyException& e){
      // Usually thrown if the file is invalid somehow
      e.ReportException();
    }
  }else{
    Base::Console().Error("Application::open() try to open unknown file type .%s\n",te.c_str());
    return;
  }
}

void Application::import(const char* FileName, const char* DocName)
{
  WaitCursor wc;
  Base::FileInfo File(FileName);
  string te = File.extension();
  const char* Mod = App::GetApplication().hasOpenType( te.c_str() );

  if ( Mod != 0 )
  {
    // issue module loading
    Command::doCommand(Command::App, "import %s", Mod);

    // issue gui module loading
    try{
      Command::doCommand(Command::Gui, "import %sGui", Mod);
    } catch (const Base::PyException&){
      // ignore this type of exception (e.g. if Mod is already a Gui module)
    }

    try{
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

  }else{
    Base::Console().Error("Application::import() try to open unknowne file type .%s\n",te.c_str());
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

void Application::OnChange(App::Application::SubjectType &rCaller,App::Application::MessageType Reason)
{
  switch(Reason.Why){
  case App::AppChanges::New:
    OnDocNew(Reason.Doc);
    break;
  case App::AppChanges::Del:
    OnDocDelete(Reason.Doc);
    break;
  }
}

void Application::OnDocNew(App::Document* pcDoc)
{
#ifdef FC_DEBUG
  std::map<App::Document*, Gui::Document*>::const_iterator it = d->lpcDocuments.find(pcDoc);
  assert(it==d->lpcDocuments.end());
#endif
  d->lpcDocuments[pcDoc] = new Gui::Document(pcDoc,this);
  AppChanges AppChange;
  AppChange.Why = AppChanges::New;
  AppChange.Doc = d->lpcDocuments[pcDoc];

  // Let the selection participate when App::Dcoument chnages
  pcDoc->Attach(&Selection());

  Notify(AppChange);
}

void Application::OnDocDelete(App::Document* pcDoc)
{
  std::map<App::Document*, Gui::Document*>::iterator doc = d->lpcDocuments.find(pcDoc);
#ifdef FC_DEBUG
  assert(doc!=d->lpcDocuments.end());
#endif
  
  AppChanges AppChange;
  AppChange.Why = AppChanges::Del;
  AppChange.Doc = doc->second;

  // Remove the selection from notification
  pcDoc->Detach(&Selection());

  Notify(AppChange);

  // We must clear the selection here to notify all observers
  Gui::Selection().clearSelection(doc->second->getDocument()->getName());

  delete doc->second; // destroy the Gui document
  d->lpcDocuments.erase(doc);

  // check if the last document has been closed?
  // Note: in case there were further existing documents then we needn't worry about it
  //       because the active view at this moment does this for us
  if (d->lpcDocuments.size() == 0 )
  {
    // there is no active document any more
    setActiveDocument(0);
  }
}

void Application::onLastWindowClosed(Gui::Document* pcDoc)
{
  if(!d->_bIsClosing && pcDoc)
  {
    try {
      // call the closing mechanism from Python
      Command::doCommand(Command::Doc, "App.Close(\"%s\")", pcDoc->getDocument()->getName());
    } catch (const Base::PyException& e) {
      e.ReportException();
    }

    // check if the last document has been closed?
    // Note: in case there were further existing documents then we needn't worry about it
    //       because the active view at this moment does this for us
    if (d->lpcDocuments.size() == 0 )
    {
      // there is no active document any more
      setActiveDocument(0);
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
  d->_pcActiveDocument=pcDocument;
  App::GetApplication().setActiveDocument( pcDocument ? pcDocument->getDocument() : 0 );

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

/// get calld if a view gets activated, this manage the whole activation scheme
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

  // ask all passiv views if closable
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
    unsigned long cnt = d->lpcDocuments.size();
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
 * Activates the matching workbench to the module name \a name.
 * The old workbench gets deactivated before. If \a name is already
 * active or if the switch fails false is returned. 
 */
bool Application::activateWorkbench( const char* name )
{
  WaitCursor wc;
  Workbench* oldWb = WorkbenchManager::instance()->active();
  if ( oldWb && oldWb->name() == name )
    return false; // already active
  // net buffer because of char* <-> const char*
  Base::PyBuf Name(name);
  // get the python workbench object from the dictionary
  PyObject* pcWorkbench = 0;
  pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, Name.str);
  // test if the workbench exists
  if ( !pcWorkbench )
    return false;

  try{
    // import the matching module first
    Interpreter().runMethodVoid(pcWorkbench, "Activate");
#ifdef FC_DEBUG
  } catch (const Base::Exception& e) {
    Base::Console().Error("%s\n", e.what() );
#else
  } catch (const Base::Exception&) {
#endif
    // clears the error flag if needed (coming from a Python file)
    if ( PyErr_Occurred() )
      PyErr_Clear();
  }

  Workbench* newWb = WorkbenchManager::instance()->active();

  // call its GetClassName method if possible
  QString className;
  try{
    PyObject* res = Interpreter().runMethodObject(pcWorkbench, "GetClassName");
    if ( PyString_Check( res) )
     className = PyString_AsString(res);
  } catch ( const Base::Exception& e ) {
    Base::Console().Log("%s\n", e.what() );
    return false;
  }

  // the Python workbench handler has changed the workbench
  bool ok = false;
  if ( newWb && newWb->name() == name )
    ok = true; // already active
  // now try to create and activate the matching workbench object
  else if ( WorkbenchManager::instance()->activate( name, className ) )
    ok = true;

  // update the Std_Workbench command and its action object
  StdCmdWorkbench* pCmd = dynamic_cast<StdCmdWorkbench*>(d->_cCommandManager.getCommandByName("Std_Workbench"));
  if ( pCmd && pCmd->getAction() )
  {
    Workbench* curWb = WorkbenchManager::instance()->active();
    QString curName = (curWb ? curWb->name() : QString(name));
    pCmd->notify( curName );
  }

  return ok;
}

void Application::refreshWorkbenchList()
{
  StdCmdWorkbench* pCmd = dynamic_cast<StdCmdWorkbench*>(d->_cCommandManager.getCommandByName("Std_Workbench"));

  if ( pCmd && pCmd->getAction() )
  {
    pCmd->refresh();
    Workbench* curWb = WorkbenchManager::instance()->active();
    QString curWbName = curWb ? curWb->name() : "<none>";
    PyObject* wb = PyDict_GetItemString(_pcWorkbenchDictionary,curWbName.latin1()); 
    if ( !wb ) // this workbench has been removed
    {
      // then just load the last workbench
      int ct = PyDict_Size( _pcWorkbenchDictionary );
      if ( ct > 0 )
      {
        PyObject* list = PyDict_Keys( _pcWorkbenchDictionary ); 
        PyObject* str = PyList_GetItem( list, ct-1 );
        Py_DECREF(list); // frees the list
        const char* name = PyString_AsString( str );
        activateWorkbench( name );
      }
    }
    else
      pCmd->notify( curWbName );
  }
}

QPixmap Application::workbenchIcon( const QString& wb ) const
{
  // net buffer because of char* <-> const char*
  Base::PyBuf Name(wb.latin1());
  // get the python workbench object from the dictionary
  PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, Name.str);
  // test if the workbench exists
  if ( pcWorkbench )
  {
    // call its GetIcon method if possible
    try{
      PyObject* res = Interpreter().runMethodObject(pcWorkbench, "GetIcon");
      if ( PyList_Check(res) )
      {
        // create temporary buffer
        int ct = PyList_Size(res);
        QByteArray ary;

        if ( ct > 0 )
        {
          PyObject* line = PyList_GetItem(res,0);
          if ( line && PyString_Check(line) )
          {
            const char* szBuf = PyString_AsString(line);
            int strlen = PyString_Size(line);
            ary.resize(strlen);
            for ( int j=0; j<strlen; j++ )
              ary[j]=szBuf[j];
          }
        }

        QPixmap px; px.loadFromData(ary, "XPM");
        return px;
      }
    } catch ( const Base::Exception& e ) {
      Base::Console().Log("%s\n", e.what() );
    }
  }

  return QPixmap();
}

QStringList Application::workbenches(void)
{
  std::string hidden = App::Application::Config()["HiddenWorkbench"];
  const char* start = App::Application::Config()["StartWorkbench"].c_str();

  PyObject *key, *value;
  int pos = 0;
  QStringList wb;
  // insert all items
  while (PyDict_Next(_pcWorkbenchDictionary, &pos, &key, &value)) {
    /* do something interesting with the values... */
    const char* wbName = PyString_AsString(key);
    // add only allowed workbenches
    if ( hidden.find( wbName ) == std::string::npos )
      wb.push_back( wbName );
    else if ( strcmp(wbName, start) == 0 ) // also allow start workbench in case it is hidden
      wb.push_back( wbName );
  }
  return wb;
}

void Application::setupContextMenu(const char* recipient, MenuItem* items) const
{
  Workbench* actWb = WorkbenchManager::instance()->active();
  if ( actWb )
  {
    actWb->setupContextMenu( recipient, items );
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

//**************************************************************************
// Init, Destruct and singelton

QApplication* Application::_pcQApp = NULL ;

void Application::initApplication(void)
{
  try{
    initTypes();
    new Base::ScriptProducer( "FreeCADGuiInit", FreeCADGuiInit );
  }
  catch (...)
  {
    // force to flush the log
    App::Application::destructObserver();
    throw;
  }

}

void Application::initTypes(void)
{
  // View Provider
  Gui::ViewProvider                          ::init();
  Gui::ViewProviderExtern                    ::init();
  Gui::ViewProviderDocumentObject            ::init();
  Gui::ViewProviderFeature                   ::init();
  Gui::ViewProviderPythonFeature             ::init();
  Gui::ViewProviderDocumentObjectGroup       ::init();

  // Workbench
  Gui::Workbench                             ::init();
  Gui::StdWorkbench                          ::init();
  Gui::TestWorkbench                         ::init();
  Gui::PythonWorkbench                       ::init();
}

void messageHandler( QtMsgType type, const char *msg )
{
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
}

#ifdef FC_DEBUG // redirect Coin messages to FreeCAD
void messageHandlerCoin( const SoError * error, void * userdata )
{
  if ( error && error->getTypeId() == SoDebugError::getClassTypeId() )
  {
    const SoDebugError* dbg = reinterpret_cast<const SoDebugError*>(error);
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
 * Unfortunately, this framework doesn't seem to work why this method is uncommented at the moment.
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

/**
 * A modal dialog has its own event loop and normally gets shown with QDialog::exec().
 * If an exception is thrown from within the dialog and this exception is caught in the calling
 * instance then the main event loop from the application gets terminated, because the implementation
 * of QDialog seems not be exception-safe..
 *
 * This class is an attempt to solve the problem with Qt's event loop. The trick is that the method
 * QEventLoop::exit() gets called when the application is about to being closed. But if the error above
 * occurs then QEventLoop::exit() is skipped. So this a possibility to determine if the application
 * should continue or not.
 * @author Werner Mayer
 */
class MainEventLoop : public QEventLoop
{
public:
  MainEventLoop ( QObject * parent = 0, const char * name = 0 )
    : QEventLoop ( parent, name ), _exited(false)
  {
  }
  virtual void exit ( int retcode = 0 )
  {
    _exited = true;
    QEventLoop::exit(retcode);
  }
  virtual int exec ()
  {
    int ret = QEventLoop::exec();
    // do we really want to exit?
    if ( !_exited )
    {
#ifdef FC_DEBUG
      Base::Console().Log("Error in event loop\n");
#endif
      exec(); // recursive call
    }
    return ret;
  }
private:
  bool _exited;
};

void Application::runApplication(void)
{
  // register own event loop
  MainEventLoop loop;
  // A new QApplication
  Console().Log("Init: Creating Gui::Application and QApplication\n");
  // if application not yet created by the splasher
  int argc = App::Application::GetARGC();
  qInstallMsgHandler( messageHandler );
  if (!_pcQApp)  _pcQApp = new QApplication (argc, App::Application::GetARGV());

  Application * app = new Application();
  MainWindow* mw = new MainWindow;
  mw->startSplasher();
  _pcQApp->setMainWidget(mw);

  // runing the Gui init script
  Interpreter().runString(Base::ScriptFactory().ProduceScript("FreeCADGuiInit"));
  // show the main window
  Console().Log("Init: Showing main window\n");
  mw->show();

  _pcQApp->connect( _pcQApp, SIGNAL(lastWindowClosed()), _pcQApp, SLOT(quit()) );

  Console().Log("Init: Activating default workbench\n");
  std::string hidden = App::Application::Config()["HiddenWorkbench"];
  const char* start = App::Application::Config()["StartWorkbench"].c_str();
  std::string defWb = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/General/AutoloadModule")->
                           GetASCII("currentText",App::Application::Config()["StartWorkbench"].c_str());

  // in case the user defined workbench is hidden then we take the default StartWorkbench 
  if ( hidden.find( defWb ) != std::string::npos )
    defWb = start;

  app->activateWorkbench( defWb.c_str() );

#ifdef FC_DEBUG // redirect Coin messages to FreeCAD
  SoDebugError::setHandlerCallback( messageHandlerCoin, 0 );
  SoQt::setFatalErrorHandler( messageHandlerSoQt, 0 );
#endif

  
  Console().Log("Init: Processing command line files\n");
  unsigned short count = 0;
  count = atoi(App::Application::Config()["OpenFileCount"].c_str());

  string File;
  for (unsigned short i=0; i<count; i++)
  {
    // getting file name
    std::stringstream temp;
    temp << "OpenFile" << i;

    File = App::Application::Config()[temp.str()];

    // try to open
    try{
      app->open(File.c_str());
    }catch(...){
      Console().Error("Can't open file %s \n",File.c_str());
    }
  }

  mw->stopSplasher();

  if(!count)
    mw->showTipOfTheDay();


  // attach the console observer
  MessageBoxObserver* msgbox = new MessageBoxObserver(mw);
  Base::Console().AttachObserver( msgbox );

  // Create new document?
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Document");
  if ( hGrp->GetBool("CreateNewDoc", false) )
  {
    App::GetApplication().newDocument();
  }

  // run the Application event loop
  Console().Log("Init: Entering event loop\n");

  try{
    _pcQApp->exec();
  }catch(...){
    // catching nasty stuff comming out of the event loop
    App::Application::destructObserver();
    //exit(1);
    throw;
  }

  Console().Log("Init: event loop left\n");

  
  Base::Console().DetachObserver( msgbox );
}

void Application::destruct(void)
{
  Console().Log("Destruct GuiApplication\n");
  MainWindow::destruct();
  SelectionSingleton::destruct();
  delete Instance;

  delete _pcQApp;
}

// -------------------------------------------------------------

MessageBoxObserver::MessageBoxObserver(MainWindow *pcAppWnd)
  :_pcAppWnd(pcAppWnd)
{
#ifdef FC_DEBUG
  this->bErr = false;
  this->bWrn = false;
#endif
}

MessageBoxObserver::~MessageBoxObserver()
{
}

/// get called when a Warning is issued
void MessageBoxObserver::Warning(const char *m)
{
  WaitCursor::lock();
  QCursor* cursor = QApplication::overrideCursor();
  bool ok = Base::Sequencer().isRunning();
  if ( ok )
    Base::Sequencer().pause();
  else if ( cursor )
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
  QMessageBox::warning( qApp->activeWindow(), QObject::tr("Warning"),m);
  if ( ok )
    Base::Sequencer().resume();
  else if ( cursor )
    QApplication::restoreOverrideCursor();
  _pcAppWnd->statusBar()->message( m, 2001 );
  WaitCursor::unlock();
}

/// get called when a Message is issued
void MessageBoxObserver::Message(const char * m)
{
  _pcAppWnd->statusBar()->message( m, 2001 );
}

/// get called when a Error is issued
void MessageBoxObserver::Error  (const char *m)
{
  WaitCursor::lock();
  QCursor* cursor = QApplication::overrideCursor();
  bool ok = ProgressBar::instance()->isRunning();
  if ( ok )
    ProgressBar::instance()->pause();
  else if ( cursor )
    QApplication::setOverrideCursor(Qt::ArrowCursor);
  QWidget* parent = qApp->activeWindow();
  if ( !parent )
    parent = getMainWindow();
  QMessageBox::critical( parent, QObject::tr("Critical Error"),m);
  if ( ok )
    ProgressBar::instance()->resume();
  else if ( cursor )
    QApplication::restoreOverrideCursor();
  _pcAppWnd->statusBar()->message( m, 2001 );
  WaitCursor::unlock();
}

/// get called when a Log Message is issued
void MessageBoxObserver::Log    (const char *log)
{
#ifdef FC_DEBUG
  if(log[0] == 'V' && log[1] == 'd' && log[2] == 'b' && log[3] == 'g' &&  log[4] == ':')
  {
    std::string str;
    str += "#Inventor V2.1 ascii \n";
    str += (log + 5);

    Gui::Document *d = Application::Instance->activeDocument();
    if(d)
    {
      ViewProviderExtern *pcExt = new ViewProviderExtern();
      pcExt->setModeByString("1",str.c_str());

      d->setAnotationViewProvider("Vdbg",pcExt);
    }
  }
#endif
}
