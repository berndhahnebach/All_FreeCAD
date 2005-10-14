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
# include <qeventloop.h>
# include <qlabel.h>
# include <qmenubar.h>
# include <qmessagebox.h>
# include <qstatusbar.h>
# include <qstyle.h>
# include <qstylefactory.h>
# include <qtabbar.h>
# include <qtimer.h>
# include <qvbox.h>
#endif

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
#include "Icons/FCIcon.xpm"
#include "WidgetFactory.h"
#include "Command.h"
#include "Macro.h"
#include "ProgressBar.h"
#include "Workbench.h"
#include "WorkbenchFactory.h"
#include "WorkbenchManager.h"
#include "CommandBarManager.h"
#include "SoFCSelection.h"

#include "Language/Translator.h"
#include "GuiInitScript.h"


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
  list<Gui::Document*>         lpcDocuments;
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
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Application::open(const char* FileName)
{
 Base::FileInfo File(FileName);
 string te = File.extension();
 const char* Mod = App::GetApplication().hasOpenType( te.c_str() );

 if ( Mod != 0 )
 {
    // issue module loading
    string Cmd = "import ";
    Cmd += Mod;
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Base,Cmd.c_str());

    // issue gui module loading
    try{
      Cmd =  "import ";
      Cmd += Mod;
      Cmd += "Gui";
      Base::Interpreter().runString(Cmd.c_str());
      macroManager()->addLine(MacroManager::Gui,Cmd.c_str());
      Base::Console().Log("CmdO: %s\n",Cmd.c_str());
    } catch (const Base::PyException&){
      // ignore this type of exception (e.g. if Mod is already a Gui module)
    }

    // load the file with the module
    Cmd = Mod;
    Cmd += ".open(\"";
    Cmd += File.filePath().c_str();
    Cmd += "\")";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Base,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());

    // ViewFit
    Cmd = "FreeCADGui.SendMsgToActiveView(\"ViewFit\")";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Gui,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());
  }else{
    Base::Console().Error("Application::open() try to open unknown file type .%s\n",te.c_str());
    return;
  }

  // the original file name is required
  getMainWindow()->appendRecentFile( File.filePath().c_str() );
}

void Application::import(const char* FileName)
{
 Base::FileInfo File(FileName);
 string te = File.extension();
 const char* Mod = App::GetApplication().hasOpenType( te.c_str() );

 if ( Mod != 0 )
 {
    // issue module loading
    string Cmd = "import ";
    Cmd += Mod;
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Base,Cmd.c_str());

    // issue gui module loading
    try{
      Cmd =  "import ";
      Cmd += Mod;
      Cmd += "Gui";
      Base::Interpreter().runString(Cmd.c_str());
      macroManager()->addLine(MacroManager::Gui,Cmd.c_str());
      Base::Console().Log("CmdO: %s\n",Cmd.c_str());
    } catch (const Base::PyException&){
      // ignore this type of exception (e.g. if Mod is already a Gui module)
    }

    // load the file with the module
    Cmd = Mod;
    Cmd += ".insert(\"";
    Cmd += File.filePath().c_str();
    Cmd += "\")";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Base,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());

  }else{
    Base::Console().Error("Application::import() try to open unknowne file type .%s\n",te.c_str());
    return;
  }

  // the original file name is required
  getMainWindow()->appendRecentFile( File.filePath().c_str() );
}

void Application::createStandardOperations()
{
  // register the application Standard commands from CommandStd.cpp
  Gui::CreateStdCommands();
  Gui::CreateViewStdCommands();
  Gui::CreateWindowStdCommands();
  Gui::CreateTestCommands();
}

void Application::OnDocNew(App::Document* pcDoc)
{
  d->lpcDocuments.push_back( new Gui::Document(pcDoc,this) );
}

void Application::OnDocDelete(App::Document* pcDoc)
{
  Gui::Document* pcGDoc;

  for(list<Gui::Document*>::iterator It = d->lpcDocuments.begin();It != d->lpcDocuments.end();It++)
  {
    if( ((*It)->getDocument()) == pcDoc)
    {
      pcGDoc = *It;
      d->lpcDocuments.erase(It);
      delete pcGDoc;
    }
  }

}

void Application::onLastWindowClosed(Gui::Document* pcDoc)
{
  if(!d->_bIsClosing)
  {
    // GuiDocument has closed the last window and get destructed
    d->lpcDocuments.remove(pcDoc);
    //lpcDocuments.erase(pcDoc);
    delete pcDoc;

    // last document closed?
    if(d->lpcDocuments.size() == 0 )
      // reset active document
      setActiveDocument(0);
    else
      setActiveDocument(d->lpcDocuments.front());
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
  Gui::Document* pDoc=0;
  for ( list<Gui::Document*>::iterator it = d->lpcDocuments.begin(); it != d->lpcDocuments.end(); ++it )
  {
    if ( strcmp(name, (*it)->getDocument()->getName()) == 0 )
    {
      pDoc = *it;
      break;
    }
  }

  return pDoc;
}

Gui::Document* Application::getDocument(App::Document* pDoc) const
{
  Gui::Document* pGuiDoc=0;
  for ( list<Gui::Document*>::iterator it = d->lpcDocuments.begin(); it != d->lpcDocuments.end(); ++it )
  {
    if ( (*it) && (*it)->getDocument() && (*it)->getDocument() == pDoc )
    {
      pGuiDoc = *it;
      break;
    }
  }

  return pGuiDoc;
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
  for(list<Gui::Document*>::iterator It = d->lpcDocuments.begin();It != d->lpcDocuments.end();It++)
  {
    (*It)->onUpdate();
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
  if(pcView->isPassiv())
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
    for (list<Gui::Document*>::iterator It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {

#ifndef FC_DEBUG
      std::list<MDIView*> mdiViews = (*It)->getMDIViews();
      if ( mdiViews.size() > 0 )
      {
        mdiViews.front()->setFocus();
      }
#endif

      (*It)->canClose ( e );
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

    list<Gui::Document*>::iterator It;

    // close all views belonging to a document
    for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
      (*It)->closeAllViews();
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
    for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
      delete(*It);
    }
  }
}

/**
 * Activates the matching workbench to the module name \a name.
 * The old workbench gets deactivated before. If \a name is already
 * active or if the switch fails false is returned. 
 */
bool Application::activateWorkbench( const char* name )
{
  Workbench* oldWb = WorkbenchManager::instance()->active();
  if ( oldWb && oldWb->name() == name )
    return false; // already active
  // net buffer because of char* <-> const char*
  Base::PyBuf Name(name);
  // get the python workbench object from the dictionary
  PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, Name.str);
  // test if the workbench exists
  if ( !pcWorkbench )
    return false;

  try{
    // import the matching module first
    Interpreter().runMethodVoid(pcWorkbench, "Activate");
  } catch (const Base::Exception&)
  {
    // clears the error flag if needed (coming from a Python file)
    if ( PyErr_Occurred() )
      PyErr_Clear();
  }

  Workbench* newWb = WorkbenchManager::instance()->active();

  // the Python workbench handler has changed the workbench
  bool ok = false;
  if ( newWb && newWb->name() == name )
    ok = true; // already active
  // now try to create and activate the matching workbench object
  else if ( WorkbenchManager::instance()->activate( name ) )
    ok = true;

  // update the Std_Workbench command and its action object
  StdCmdWorkbench* pCmd = dynamic_cast<StdCmdWorkbench*>(d->_cCommandManager.getCommandByName("Std_Workbench"));
  if ( pCmd && pCmd->getAction(false) )
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

  if ( pCmd && pCmd->getAction(false) )
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
  new Base::ScriptProducer( "FreeCADGuiInit", FreeCADGuiInit );
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
  Console().Log("Init: Showing Application Window\n");
  mw->show();

  _pcQApp->connect( _pcQApp, SIGNAL(lastWindowClosed()), _pcQApp, SLOT(quit()) );

  Console().Log("Init: Starting default Workbench\n");
  std::string hidden = App::Application::Config()["HiddenWorkbench"];
  const char* start = App::Application::Config()["StartWorkbench"].c_str();
  std::string defWb = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/General/AutoloadModule")->
                           GetASCII("currentText",App::Application::Config()["StartWorkbench"].c_str());

  // in case the user defined workbench is hidden then we take the default StartWorkbench 
  if ( hidden.find( defWb ) != std::string::npos )
    defWb = start;

  app->activateWorkbench( defWb.c_str() );

  Gui::SoFCSelection::initClass();

  
  Console().Log("Init: Processing command line files\n");
  unsigned short count = atoi(App::Application::Config()["OpenFileCount"].c_str());

  string File;
  for (unsigned short i=0; i<count; i++)
  {
    // getting file name
    ostringstream temp;
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


  // run the Application event loop
  Console().Log("Init: Entering event loop\n");
  // attach the console observer
  MessageBoxObserver* msgbox = new MessageBoxObserver(mw);
  Base::Console().AttacheObserver( msgbox );
  _pcQApp->exec();
  Base::Console().DetacheObserver( msgbox );
  Console().Log("Init: event loop left\n");
}

void Application::destruct(void)
{
  Console().Log("Destruct GuiApplication\n");
  MainWindow::destruct();
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

/// get called when a Warning is issued
void MessageBoxObserver::Warning(const char *m)
{
  bool ok = Base::Sequencer().isRunning();
  if ( ok )
    Base::Sequencer().pause();
  QMessageBox::warning( _pcAppWnd, QObject::tr("Warning"),m);
  if ( ok )
    Base::Sequencer().resume();
  _pcAppWnd->statusBar()->message( m, 2001 );
}

/// get called when a Message is issued
void MessageBoxObserver::Message(const char * m)
{
  _pcAppWnd->statusBar()->message( m, 2001 );
}

/// get called when a Error is issued
void MessageBoxObserver::Error  (const char *m)
{
  bool ok = ProgressBar::instance()->isRunning();
  if ( ok )
    ProgressBar::instance()->pause();
  QMessageBox::critical( _pcAppWnd, QObject::tr("Critical Error"),m);
  if ( ok )
    ProgressBar::instance()->resume();
  _pcAppWnd->statusBar()->message( m, 2001 );
}

/// get called when a Log Message is issued
void MessageBoxObserver::Log    (const char *)
{
}
