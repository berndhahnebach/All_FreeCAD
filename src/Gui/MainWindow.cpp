/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qlabel.h>
# include <qmenubar.h>
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
#include "../Base/EnvMacros.h"
#include "../Base/Factory.h"
#include "../App/Application.h"

#include "MainWindow.h"
#include "Document.h"
#include "View.h"
//#include "CommandStd.h"
//#include "CommandView.h"
#include "../Base/Documentation.h"

#include "Icons/developers.h"
#include "Icons/FCIcon.xpm"
#include "Splashscreen.h"
#include "WidgetFactory.h"
#include "CustomWidgets.h"
#include "Command.h"
#include "Tree.h"
#include "PropertyView.h"

#include "CommandLine.h"
#include "DlgDocTemplatesImp.h"
#include "DlgTipOfTheDayImp.h"
#include "DlgUndoRedo.h"
#include "DlgOnlineHelpImp.h"
#include "ToolBox.h"
#include "HelpView.h"
#include "ReportView.h"
#include "Macro.h"
#include "ProgressBar.h"
#include "Window.h" 

#include "Inventor/Qt/SoQt.h"

#include "Language/Translator.h"

#include "GuiInitScript.h"

using Base::Console;
using Base::Interpreter;
using namespace Gui;
using namespace Gui::DockWnd;
using Gui::Dialog::DlgOnlineHelpImp;

static MainWindow* stApp;
//static QWorkspace* stWs;


MainWindow* MainWindow::Instance = 0L;

namespace Gui {

// Pimpl class
struct MainWindowP
{
  MainWindowP()
    : toolbars(0L), viewbar(0L), _pcActiveDocument(0L),
    _bIsClosing(false), _bControlButton(false)
  {
    // create the macro manager
//TODO    _pcMacroMngr = new MacroManager();
  }

  ~MainWindowP()
  {
    viewbar = 0L;
    delete toolbars;
    delete _pcWidgetMgr;
//TODO    delete _pcMacroMngr;
  }

  QPopupMenu* toolbars;
  Gui::CustomPopupMenu* viewbar;
  Gui::CustomPopupMenu* windows;
  QValueList<int> wndIDs;
  std::map<int, QWidget*> mCheckBars;
  /// list of all handled documents
  std::list<Gui::Document*>         lpcDocuments;
  /// list of windows
  /// Active document
  Gui::Document*   _pcActiveDocument;
  Gui::CustomWidgetManager*		 _pcWidgetMgr;
  Gui::DockWindowManager* _pcDockMgr;
  MacroManager*  _pcMacroMngr;
  QLabel *         _pclSizeLabel, *_pclActionLabel;
  ToolBox*        _pcStackBar;
  /// workbench python dictionary
  PyObject*		 _pcWorkbenchDictionary;
  QString			 _cActiveWorkbenchName;
  QTimer *		 _pcActivityTimer; 
  /// List of all registered views
  std::list<Gui::BaseView*>					_LpcViews;
  bool _bIsClosing;
  // store it if the CTRL button is pressed or released
  bool _bControlButton;
  /// Handels all commands 
  CommandManager _cCommandManager;
  QWorkspace* _pWorkspace;
  QTabBar* _tabs;
  QMap<int, MDIView*> _mdiIds;
};

} // namespace Gui


/* TRANSLATOR Gui::MainWindow */

MainWindow::MainWindow(QWidget * parent, const char * name, WFlags f)
    : QMainWindow( parent, "Main window", f/*WDestructiveClose*/ )
{
  ParameterGrp::handle hPGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
  hPGrp = hPGrp->GetGroup("Preferences")->GetGroup("General");

  std::string language = hPGrp->GetASCII("Language", "English");
  Gui::Translator::setLanguage( language.c_str() );
  GetWidgetFactorySupplier();

  // init the Inventor subsystem
  SoQt::init(this);


  SoDB::init();

  d = new MainWindowP;
  QVBox* vb = new QVBox( this );
  vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  d->_pWorkspace = new QWorkspace( vb, "workspace" );

  QPixmap backgnd(( const char** ) background );
  d->_pWorkspace->setPaletteBackgroundPixmap( backgnd );
  d->_tabs = new QTabBar( vb, "tabBar" );
  d->_tabs->setShape( QTabBar::RoundedBelow );
  d->_pWorkspace->setScrollBarsEnabled( true );
  setCentralWidget( vb );
  connect( d->_pWorkspace, SIGNAL( windowActivated ( QWidget * ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
  connect( d->_tabs, SIGNAL( selected( int) ), this, SLOT( onTabSelected(int) ) );

//	setCaption( "Qt FreeCAD" );
  setCaption( "FreeCAD" );
  setIcon(QPixmap(FCIcon));

  d->_cActiveWorkbenchName="<none>";

  // global access 
  Instance = this;

  stApp = this;

  // instanciate the workbench dictionary
  d->_pcWorkbenchDictionary = PyDict_New();

    // attach the console observer
//TODO  Base::Console().AttacheObserver( new GuiConsoleObserver(this) );


  // setting up the Bitmap manager
//	QString tmpWb = _cActiveWorkbenchName;
//	_cBmpFactory.GetPixmap("Function");
/*
  QDir dir(GetApplication().GetHomePath()); 
  QString root = dir.path();
  GetDocumentationManager().AddProvider(new FCDocProviderDirectory("FCDoc:/"          ,(root + "/Doc/Online\\"   ).latin1()));
  GetDocumentationManager().AddProvider(new FCDocProviderDirectory("FCDoc:/Framework/",(root + "/Doc/FrameWork\\").latin1()));

  std::string test =  GetDocumentationManager().Retrive("FCDoc:/index", Html );

  test =  GetDocumentationManager().Retrive("FCDoc:/Framework/index", Html );
*/

  // labels and progressbar
  d->_pclActionLabel = new QLabel("", statusBar(), "Action");
  d->_pclActionLabel->setFixedWidth(120);
  statusBar()->addWidget(d->_pclActionLabel,0,true);
  statusBar()->addWidget(ProgressBar::instance(),0,true);
  //ProgressBar::Instance().setFixedWidth(200);
  d->_pclSizeLabel = new QLabel("Dimension", statusBar(), "Dimension");
  d->_pclSizeLabel->setFixedWidth(120);
  statusBar()->addWidget(d->_pclSizeLabel,0,true);

  // update gui timer
  d->_pcActivityTimer = new QTimer( this );
    connect( d->_pcActivityTimer, SIGNAL(timeout()),this, SLOT(updateCmdActivity()) );
    d->_pcActivityTimer->start( 300, TRUE );


  // Command Line +++++++++++++++++++++++++++++++++++++++++++++++++++
  CommandLine().reparent(statusBar());
  statusBar()->addWidget(&CommandLine(), 0, true);
  statusBar()->message( tr("Ready"), 2001 );

  // Cmd Button Group +++++++++++++++++++++++++++++++++++++++++++++++
  d->_pcStackBar = new ToolBox(this,"Cmd_Group");
  d->_pcWidgetMgr = new Gui::CustomWidgetManager( d->_pcStackBar );
  d->_pcDockMgr = new Gui::DockWindowManager();
  d->_pcDockMgr->addDockWindow( "Toolbox",d->_pcStackBar, Qt::DockRight );

  // Help View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
  QString home = DlgOnlineHelpImp::getStartpage();
  HelpView* pcHelpView = new HelpView( home, this, "HelpViewer" );
  d->_pcDockMgr->addDockWindow("Help view", pcHelpView, Qt::DockRight );


  // Tree Bar  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
  TreeView* pcTree = new TreeView(0,this,"Raw_tree");
  pcTree->setMinimumWidth(210);
  d->_pcDockMgr->addDockWindow("Tree view", pcTree, Qt::DockLeft );

  // PropertyView  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
  PropertyView* pcPropView = new PropertyView(0,0,"PropertyView");
  pcPropView->setMinimumWidth(210);
  d->_pcDockMgr->addDockWindow("Property editor", pcPropView, Qt::DockLeft );

  // Report View
  Gui::DockWnd::ReportView* pcOutput = new Gui::DockWnd::ReportView(this,"ReportView");
  d->_pcDockMgr->addDockWindow("Report View", pcOutput, Qt::DockBottom );

  createStandardOperations();
  MacroCommand::load();

  // accept drops on the window, get handled in dropEvent, dragEnterEvent   
  setAcceptDrops(true);

  // misc stuff
  loadWindowSettings();
}

MainWindow::~MainWindow()
{
  // save macros
  MacroCommand::save();
  // save recent file list
  // Note: the recent files are restored in StdCmdMRU::createAction(), because we need
  //       an valid instance of StdCmdMRU to do this
  StdCmdMRU::save();
  saveWindowSettings();
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void MainWindow::createStandardOperations()
{
  // register the application Standard commands from CommandStd.cpp
  Gui::CreateStdCommands();
  Gui::CreateViewStdCommands();
  Gui::CreateTestCommands();
}

void MainWindow::onPolish()
{
  d->viewbar  = d->_pcWidgetMgr->getPopupMenu("&View");
  d->viewbar->setCanModify(true);
  d->toolbars = new QPopupMenu(d->viewbar, "Toolbars");

  connect(d->viewbar,  SIGNAL(aboutToShow (   )), this, SLOT(onShowView(     )));

  d->windows = d->_pcWidgetMgr->getPopupMenu("&Windows");
  connect(d->windows, SIGNAL( aboutToShow()), this, SLOT( onWindowsMenuAboutToShow() ) );
}

bool MainWindow::isCustomizable () const
{
  return true;
}

void MainWindow::customize ()
{
  commandManager().runCommandByName("Std_DlgCustomize");
}

void MainWindow::tileHorizontal()
{
  // primitive horizontal tiling
  QWidgetList windows = d->_pWorkspace->windowList();
  if ( !windows.count() )
    return;
    
  int heightForEach = d->_pWorkspace->height() / windows.count();
  int y = 0;
  for ( int i = 0; i < int(windows.count()); ++i ) 
  {
    QWidget *window = windows.at(i);
    if ( window->testWState( WState_Maximized ) ) 
    {
      // prevent flicker
      window->hide();
      window->showNormal();
    }

    int preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
    int actHeight = QMAX(heightForEach, preferredHeight);
  
    window->parentWidget()->setGeometry( 0, y, d->_pWorkspace->width(), actHeight );
    y += actHeight;
  }
}

void MainWindow::tile()
{
  d->_pWorkspace->tile();
}

void MainWindow::cascade()
{
  d->_pWorkspace->cascade();
}

void MainWindow::onShowView()
{
  Gui::CustomPopupMenu* menu = d->viewbar;
  // get a copy of the items an reassign to force the rebuild
  QStringList items = menu->getCustomItems();
  menu->setCustomItems( items );
  menu->setCheckable(true);
  d->mCheckBars.clear();

  // toolbars
  menu->insertItem(tr("Toolbars"), createDockWindowMenu( OnlyToolBars ));
  menu->insertSeparator();

  connect( menu, SIGNAL( aboutToShow() ), this, SLOT( menuAboutToShow() ) );

  QPtrList<QDockWindow> wnds = dockWindows ();
  QDockWindow* dw;
  for ( dw = wnds.first(); dw; dw = wnds.next() )
  {
    if ( !dw->inherits("QToolBar") )
    {
      QString label = dw->caption();
      int id = menu->insertItem( label, dw, SLOT( toggleVisible() ) );
      menu->setItemChecked( id, dw->isVisible() );
    }
  }

  // status bar
  menu->insertSeparator();
  QWidget* w = statusBar();
  int id = menu->insertItem( tr("Status bar"), this, SLOT( onToggleStatusBar() ) );
  d->mCheckBars[id] = w;
  menu->setItemChecked(id, w->isVisible());
}

void MainWindow::onToggleStatusBar()
{
  QWidget* w = statusBar();
  w->isVisible() ? w->hide() : w->show();
}

void MainWindow::addWindow( MDIView* view )
{
  // make workspace parent of view
  view->reparent( d->_pWorkspace, QPoint() );
  connect( view, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( message(const QString&, int )) );
  // show the very first window in maximized mode
  if ( d->_pWorkspace->windowList().isEmpty() )
    view->showMaximized();
  else
    view->show();

  // being informed when the view is destroyed
  connect( view, SIGNAL( destroyed() ), this, SLOT( onWindowDestroyed() ) );

  // add a new tab to our tabbar
  QTab* tab = new QTab;

  // extract file name if possible
  QFileInfo fi( view->caption() );
  if ( fi.isFile() && fi.exists() )
    tab->setText( fi.fileName() );
  else
    tab->setText( view->caption() );
  if ( view->icon() )
    tab->setIconSet( *view->icon() );
  d->_tabs->setToolTip( d->_tabs->count(), view->caption() );

  int id = d->_tabs->addTab( tab );
  d->_mdiIds[ id ] = view;
  if ( d->_tabs->count() == 1 )
    d->_tabs->show(); // invoke show() for the first tab
  d->_tabs->update();
  d->_tabs->setCurrentTab( tab );
}

void MainWindow::onWindowDestroyed()
{
  QObject* obj = (QObject*)sender();
  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data() == obj )
    {
      QTab* tab = d->_tabs->tab( it.key() );
      d->_tabs->removeTab( tab );
      d->_mdiIds.remove( it );
      if ( d->_tabs->count() == 0 )
        d->_tabs->hide(); // no view open any more
      break;
    }
  }
}

void MainWindow::onWindowActivated( QWidget* w )
{
  MDIView* mdi = dynamic_cast<MDIView*>(w);
  if ( !mdi ) return; // either no MDIView or no valid object
  mdi->setActive();

  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data() == mdi )
    {
      d->_tabs->blockSignals( true );
      d->_tabs->setCurrentTab( it.key() );
      d->_tabs->blockSignals( false );
      break;
    }
  }
}

void MainWindow::onTabSelected( int i)
{
  QMap<int, MDIView*>::Iterator it = d->_mdiIds.find( i );
  if ( it != d->_mdiIds.end() )
  {
    if ( !it.data()->hasFocus() )
      it.data()->setFocus();
  }
}

void MainWindow::onWindowsMenuAboutToShow()
{
  QPopupMenu* windowsMenu = d->windows;
  QWidgetList windows = d->_pWorkspace->windowList( QWorkspace::CreationOrder );

  // remove old window items first
  while ( d->wndIDs.size() > 0 )
  {
    int id = d->wndIDs.front();
    d->wndIDs.pop_front();
    windowsMenu->removeItem( id );
  }

  // append new window items
  if ( windows.count() > 0 )
  {
    // insert separator before last item 
    int idx = windowsMenu->count() - 2;
    int pos = windowsMenu->insertSeparator( idx );
    d->wndIDs.push_back( pos );

    bool act = false;
    for ( int i = 0; i < int(windows.count()); ++i ) 
    {
      QString txt = QString("&%1 %2").arg( i+1 ).arg( windows.at(i)->caption() );
      idx = windowsMenu->count() - 2;

      act |= (d->_pWorkspace->activeWindow() == windows.at(i));

      if ( (act && d->wndIDs.size() < 10) || (!act && d->wndIDs.size() < 9))
      {
        int id = windowsMenu->insertItem( txt, this, SLOT( onWindowsMenuActivated( int ) ), 0, -1, idx );
        windowsMenu->setItemParameter( id, i );
        windowsMenu->setItemChecked( id, d->_pWorkspace->activeWindow() == windows.at(i) );
        d->wndIDs.push_back( id );
      }
    }
  }
}

void MainWindow::onWindowsMenuActivated( int id )
{
  QWidget* w = d->_pWorkspace->windowList().at( id );
  if ( w )
    w->showNormal();
  w->setFocus();
}

QWidgetList MainWindow::windows( QWorkspace::WindowOrder order ) const
{
  return d->_pWorkspace->windowList( order );
}

void MainWindow::onLastWindowClosed(Gui::Document* pcDoc)
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

// set text to the pane
void MainWindow::setPaneText(int i, QString text)
{
  if (i==1)
    d->_pclActionLabel->setText(text);
  else if (i==2)
    d->_pclSizeLabel->setText(text);
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// view handling
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



/// send Messages to the active view
bool MainWindow::sendMsgToActiveView(const char* pMsg, const char** ppReturn)
{
  MDIView* pView = activeView();

  if(pView){
    return pView->onMsg(pMsg,ppReturn);
  }else
    return false;
}

bool MainWindow::sendHasMsgToActiveView(const char* pMsg)
{
  MDIView* pView = activeView();

  if(pView){
    return pView->onHasMsg(pMsg);
  }else
    return false;
}


MDIView* MainWindow::activeView(void)
{
  MDIView * pView = reinterpret_cast <MDIView *> ( d->_pWorkspace->activeWindow() );
  return pView;
}


/// Geter for the Active View
Gui::Document* MainWindow::activeDocument(void)
{
  return d->_pcActiveDocument;
  /*
  MDIView* pView = GetActiveView();

  if(pView)
    return pView->GetGuiDocument();
  else
    return 0l;*/
}

void MainWindow::setActiveDocument(Gui::Document* pcDocument)
{
  d->_pcActiveDocument=pcDocument;

  Console().Log("Activate Document (%p) \n",d->_pcActiveDocument);

  // notify all views attached to the application (not views belong to a special document)
  for(std::list<Gui::BaseView*>::iterator It=d->_LpcViews.begin();It!=d->_LpcViews.end();It++)
    (*It)->setDocument(pcDocument);
}

void MainWindow::attachView(Gui::BaseView* pcView)
{
  d->_LpcViews.push_back(pcView);
}


void MainWindow::detachView(Gui::BaseView* pcView)
{
  d->_LpcViews.remove(pcView);
}

void MainWindow::onUpdate(void)
{
  // update all documents
  for(std::list<Gui::Document*>::iterator It = d->lpcDocuments.begin();It != d->lpcDocuments.end();It++)
  {
    (*It)->onUpdate();
  }
  // update all the independed views
  for(std::list<Gui::BaseView*>::iterator It2 = d->_LpcViews.begin();It2 != d->_LpcViews.end();It2++)
  {
    (*It2)->onUpdate();
  }
}

/// get calld if a view gets activated, this manage the whole activation scheme
void MainWindow::viewActivated(MDIView* pcView)
{
  Console().Log("Activate View (%p) Type=\"%s\" \n",pcView,pcView->getName());
  // set the new active document
  if(pcView->isPassiv())
    setActiveDocument(0);
  else
    setActiveDocument(pcView->getGuiDocument());
}


void MainWindow::updateActive(void)
{
  activeDocument()->onUpdate();
}

void MainWindow::onUndo()
{
  puts("MainWindow::slotUndo()");
}

void MainWindow::onRedo()
{
  puts("MainWindow::slotRedo()");
}

void MainWindow::closeEvent ( QCloseEvent * e )
{
  if(d->lpcDocuments.size() == 0)
  {
    e->accept();
  }else{
    // ask all documents if closable
    for (std::list<Gui::Document*>::iterator It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
      (*It)->canClose ( e );
  //			if(! e->isAccepted() ) break;
      if(! e->isAccepted() ) return;
    }
  }

  // ask all passiv views if closable
  for (std::list<Gui::BaseView*>::iterator It2 = d->_LpcViews.begin();It2!=d->_LpcViews.end();It2++)
  {
    if((*It2)->canClose() )
      e->accept();
    else 
      e->ignore();

//		if(! e->isAccepted() ) break;
    if(! e->isAccepted() ) return;
  }

  if( e->isAccepted() )
  {
    d->_bIsClosing = true;

    std::list<Gui::Document*>::iterator It;

    // close all views belonging to a document
    for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
      (*It)->closeAllViews();
    }

    //detache the passiv views
    //SetActiveDocument(0);
    std::list<Gui::BaseView*>::iterator It2 = d->_LpcViews.begin();
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

    d->_pcActivityTimer->stop();

    activateWorkbench("<none>");
    QMainWindow::closeEvent( e );
  }
}

/**
 *  Activate the named workbench by calling the methodes in the 
 *  python workbench object. If the workbench is allready active
 *  nothing get called!
 */
void MainWindow::activateWorkbench(const char* name)
{
  // net buffer because of char* <-> const char*
  Base::PyBuf Name(name);

  // get the python workbench object from the dictionary
  PyObject* pcWorkbench = PyDict_GetItemString(d->_pcWorkbenchDictionary, Name.str);

  // test if the workbench exists
  if ( !pcWorkbench )
  {
    QString exc = tr("Workbench '%1' does not exist").arg( name );
    Console().Error("%s\n", exc.latin1());
    return;
  }

  // close old workbench
  if(d->_cActiveWorkbenchName != "")
  {
    Base::PyBuf OldName ( d->_cActiveWorkbenchName.latin1());
    PyObject* pcOldWorkbench = PyDict_GetItemString(d->_pcWorkbenchDictionary, OldName.str);
    assert(pcOldWorkbench);
    customWidgetManager()->hideToolBox();
    Interpreter().runMethodVoid(pcOldWorkbench, "Stop");
  }

  try{
    // rename with new workbench before(!!!) calling "Start"
    d->_cActiveWorkbenchName = name;

    // running the start of the workbench object
    Interpreter().runMethodVoid(pcWorkbench, "Start");
    d->_pcWidgetMgr->update(name);
    customWidgetManager()->showToolBox();

    // update the Std_Workbench command and its action object
    Command* pCmd = d->_cCommandManager.getCommandByName("Std_Workbench");
    if (pCmd)
    {
      ((StdCmdWorkbench*)pCmd)->activate( name );
    }

    show();
  }
  catch (const Base::Exception&)
  {
    // do nothing here
  }
}

void MainWindow::appendWorkbench(const char* name)
{
  StdCmdWorkbench* pCmd = dynamic_cast<StdCmdWorkbench*>(d->_cCommandManager.getCommandByName("Std_Workbench"));

  if ( pCmd && pCmd->getAction(false) )
  {
    pCmd->appendItem( name );
  }
}

void MainWindow::removeWorkbench(const char* name)
{
//	PyObject *key, *value;
//	int pos = 0;
//     
//  Command* pCmd = d->_cCommandManager.GetCommandByName("Std_Workbench");
//  if (pCmd)
//  {
//  	// remove all items from the command
//    ((StdCmdWorkbench*)pCmd)->Clear();
//
//  	// insert all items
//    while (PyDict_Next(d->_pcWorkbenchDictionary, &pos, &key, &value)) {
//		  /* do something interesting with the values... */
//      ((StdCmdWorkbench*)pCmd)->AddItem(PyString_AsString(key));
//	  }
//  }
}

std::vector<std::string> MainWindow::workbenches(void)
{
  PyObject *key, *value;
  int pos = 0;
  std::vector<std::string> wb;
  // insert all items
  while (PyDict_Next(d->_pcWorkbenchDictionary, &pos, &key, &value)) {
    /* do something interesting with the values... */
    wb.push_back(PyString_AsString(key));
  }
  return wb;
}

void MainWindow::appendRecentFile(const char* file)
{
  StdCmdMRU* pCmd = dynamic_cast<StdCmdMRU*>(d->_cCommandManager.getCommandByName("Std_MRU"));
  if (pCmd)
  {
    pCmd->addRecentFile( file );
    pCmd->refreshRecentFileWidgets();
  }
}

void MainWindow::updateCmdActivity()
{
  static QTime cLastCall;

  if(cLastCall.elapsed() > 250 && isVisible () )
  {
    //puts("testActive");
    d->_cCommandManager.testActive();
    // remember last call
    cLastCall.start();
  }

  d->_pcActivityTimer->start( 300, TRUE );	
}

void MainWindow::loadWindowSettings()
{
  loadDockWndSettings();

  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("MainWindow");
  int w = hGrp->GetInt("Width", 1024);
  int h = hGrp->GetInt("Height", 768);
  int x = hGrp->GetInt("PosX", pos().x());
  int y = hGrp->GetInt("PosY", pos().y());
  bool max = hGrp->GetBool("Maximized", false);
  resize( w, h );
  move(x, y);
  if (max) showMaximized();
  //setBackgroundPixmap(QPixmap((const char*)FCBackground));

  updatePixmapsSize();
  updateStyle();
}

void MainWindow::updatePixmapsSize(void)
{
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
  hGrp = hGrp->GetGroup("Preferences")->GetGroup("General");
  bool bigPixmaps = hGrp->GetBool("BigPixmaps", false);
  if (bigPixmaps != usesBigPixmaps())
    setUsesBigPixmaps (bigPixmaps);
}

void MainWindow::updateStyle(void)
{
  QStyle& curStyle = QApplication::style();
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
  hGrp = hGrp->GetGroup("Preferences")->GetGroup("General");

  QString style = hGrp->GetASCII( "WindowStyle", curStyle.name() ).c_str();
  if ( style == QString( curStyle.name() ) )
    return; // already set
  
  QStyle* newStyle = QStyleFactory::create( style );
  if ( newStyle != 0 )
  {
    QApplication::setStyle( newStyle );
  }
}

void MainWindow::saveWindowSettings()
{
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("MainWindow");
  if (isMaximized())
  {
    hGrp->SetBool("Maximized", true);
  }
  else
  {
    hGrp->SetInt("Width", width());
    hGrp->SetInt("Height", height());
    hGrp->SetInt("PosX", pos().x());
    hGrp->SetInt("PosY", pos().y());
    hGrp->SetBool("Maximized", false);
  }

  saveDockWndSettings();
}

void MainWindow::loadDockWndSettings()
{
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("MainWindow");
  QString str = hGrp->GetASCII("Layout", "").c_str();

  if ( !str.isEmpty() )
  {
    QTextStream ts( &str, IO_ReadOnly );
    ts >> *this;
  }
/*
  // open file
  std::string FileName(GetApplication().GetHomePath());
  FileName += "FreeCAD.xml";
  QFile* datafile = new QFile(FileName.c_str());
  if (!datafile->open(IO_ReadOnly)) 
  {
    // error opening file
    bool bMute = GuiConsoleObserver::bMute;
    GuiConsoleObserver::bMute = true;
    Console().Warning((tr("Error: Cannot open file '%1' "
                             "(Maybe you're running FreeCAD the first time)\n").arg(FileName.c_str())));
    GuiConsoleObserver::bMute = bMute;
    datafile->close();
    delete (datafile);
    return;
  }

  // open dom document
  QDomDocument doc("DockWindows");
  if (!doc.setContent(datafile)) 
  {
    Console().Warning("Error:  is not a valid file\n");
    datafile->close();
    delete (datafile);
    return;
  }

  datafile->close();
  delete (datafile);

  // check the doc type and stuff
  if (doc.doctype().name() != "DockWindows") 
  {
    // wrong file type
    Console().Warning("Error: is not a valid file\n");
    return;
  }

  QDomElement root = doc.documentElement();
  if (root.attribute("application") != QString("FreeCAD")) 
  {
    // right file type, wrong application
    Console().Warning("Error: wrong file\n");
    return;
  }

  readDockConfig(root);*/
}

void MainWindow::saveDockWndSettings()
{
  QString str;
  QTextStream ts( &str, IO_WriteOnly );
  ts << *this;

  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("MainWindow");
  hGrp->SetASCII("Layout", str.latin1());
/*
  // save dock window settings
  QDomDocument doc("DockWindows");

  // create the root element
  QDomElement root = doc.createElement(doc.doctype().name());
  root.setAttribute("version", "0.1");
  root.setAttribute("application", "FreeCAD");

  writeDockConfig(root);
  doc.appendChild(root);

  // save into file
  std::string FileName(GetApplication().GetHomePath());
  FileName += "FreeCAD.xml";
  QFile* datafile = new QFile (FileName.c_str());
  if (!datafile->open(IO_WriteOnly)) 
  {
    // error opening file
    Console().Warning("Error: Cannot open file\n");
    datafile->close();
    delete (datafile);
    return;
  }

  // write it out
  QTextStream textstream(datafile);
  doc.save(textstream, 0);
  datafile->close();
  delete (datafile);*/
}

bool MainWindow::isClosing(void)
{
  return d->_bIsClosing;
}

Gui::CustomWidgetManager* MainWindow::customWidgetManager(void) 
{ 
  return d->_pcWidgetMgr; 
}

QString MainWindow::activeWorkbench(void)
{
  return d->_cActiveWorkbenchName;
}

MacroManager *MainWindow::macroManager(void)
{
  return d->_pcMacroMngr;
}

CommandManager &MainWindow::commandManager(void)
{
  return d->_cCommandManager;
}

void MainWindow::languageChange()
{
  CommandManager& rclMan = commandManager();
  std::vector<Command*> cmd = rclMan.getAllCommands();
  for ( std::vector<Command*>::iterator it = cmd.begin(); it != cmd.end(); ++it )
  {
    (*it)->languageChange();
  }

  // and finally update the menu bar since QMenuBar owns no "text" property
  Gui::CustomWidgetManager* cw = customWidgetManager();
  const QMap<int, QString>& mi = cw->menuBarItems();
 
  QMenuBar* mb = menuBar();
  uint cnt = mb->count();
  for ( uint i=0; i<cnt; i++ )
  {
    int id = mb->idAt( i );
    QMap<int, QString>::ConstIterator it = mi.find( id );
    if ( it != mi.end() )
      mb->changeItem( id, tr( it.data()/*mb->text( id )*/ ) );
  }
}

//**************************************************************************
// Init, Destruct and singelton

QApplication* MainWindow::_pcQApp = NULL ;

QSplashScreen *MainWindow::_splash = NULL;


void MainWindow::initApplication(void)
{
//	std::map<std::string,std::string> &Config = GetApplication().Config();


  new Base::ScriptProducer( "FreeCADGuiInit", FreeCADGuiInit );
}
/*
void messageHandler( QtMsgType type, const char *msg )
{
//  bool mute = GuiConsoleObserver::bMute;
//  GuiConsoleObserver::bMute = false;

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

//  GuiConsoleObserver::bMute = mute;
}
*/
void MainWindow::runApplication(void)
{
  // A new QApplication
  Console().Log("Creating GUI Application...\n");
  // if application not yet created by the splasher
  int argc = App::Application::GetARGC();
//  qInstallMsgHandler( messageHandler );
  if (!_pcQApp)  _pcQApp = new QApplication (argc, App::Application::GetARGV());

  startSplasher();
  MainWindow * mw = new MainWindow();
  _pcQApp->setMainWidget(mw);

  // runing the Gui init script
  Interpreter().runString(Base::ScriptFactory().ProduceScript("FreeCADGuiInit"));
  // show the main window
  Console().Log("Showing GUI Application...\n");
  mw->onPolish();
  mw->show();
  stopSplasher();
  showTipOfTheDay();

  _pcQApp->connect( _pcQApp, SIGNAL(lastWindowClosed()), _pcQApp, SLOT(quit()) );

  // run the Application event loop
  Console().Log("Running event loop...\n");
  _pcQApp->exec();
  Console().Log("event loop left\n");
}

void MainWindow::startSplasher(void)
{
  // startup splasher
  // when running in verbose mode no splasher
  if ( ! (App::Application::Config()["Verbose"] == "Strict") && (App::Application::Config()["RunMode"] == "Gui") )
  {
    ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");
#ifdef FC_DEBUG
  bool splash = false;
#else
  bool splash = true;
#endif
    if (hGrp->GetBool("AllowSplasher", splash))
    {
      QPixmap pixmap(( const char** ) splash_screen );
      _splash = new SplashScreen( pixmap );
      _splash->show();
    }
  }
}

void MainWindow::stopSplasher(void)
{
  if ( _splash )
  {
    _splash->finish( Instance );
    delete _splash;
    _splash = 0L;
  }
}

void MainWindow::showTipOfTheDay( bool force )
{
  // tip of the day?
  ParameterGrp::handle
  hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");

#ifdef FC_DEBUG
  bool tip = false;
#else
  bool tip = true;
#endif

  tip = hGrp->GetBool("Tipoftheday", tip);
  if ( tip || force)
  {
    Gui::Dialog::DlgTipOfTheDayImp dlg(Instance, "Tipofday");
    dlg.exec();
  }
}

void MainWindow::destruct(void)
{
  Console().Log("Destruct GuiApplication\n");

  delete _pcQApp;
}

/**
 * Drops the event \a e and writes the right Python command.
 */
void MainWindow::dropEvent ( QDropEvent      * e )
{
  if ( QUriDrag::canDecode(e) )
  {
    QStringList fn;
    QUriDrag::decodeLocalFiles(e, fn);

    for ( QStringList::Iterator it = fn.begin(); it != fn.end(); ++it ) {

      QFileInfo info(*it);
      if ( info.exists() && info.isFile() )
      {
//          open(info.absFilePath().latin1());
      }
    }
  }else
    QMainWindow::dropEvent(e);
}

void MainWindow::dragEnterEvent ( QDragEnterEvent * e )
{
  if ( QUriDrag::canDecode(e) )
  {
    QStringList fn;
    QUriDrag::decodeLocalFiles(e, fn);
    QString f = fn.first();
    string Ending = (f.right(f.length() - f.findRev('.')-1)).latin1();

    if ( App::GetApplication().hasOpenType( Ending.c_str() ) )
      e->accept();
  }else
    e->ignore();
}


#include "moc_MainWindow.cpp"
