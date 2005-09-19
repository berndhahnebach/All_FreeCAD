/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#include "../Base/FileInfo.h"
#include "../App/Application.h"
#include "../App/Feature.h"
#include "../App/Document.h"

#include "MainWindow.h"
#include "Document.h"
#include "View.h"

#include "Icons/developers.h"
#include "Icons/FCIcon.xpm"
#include "WidgetFactory.h"
#include "Command.h"
#include "Tree.h"
#include "PropertyView.h"
#include "BitmapFactory.h"
#include "Splashscreen.h"
#include "MenuManager.h"
#include "WorkbenchFactory.h"

#include "CommandLine.h"
#include "DlgTipOfTheDayImp.h"
#include "DlgUndoRedo.h"
#include "DlgOnlineHelpImp.h"
#include "ToolBox.h"
#include "HelpView.h"
#include "ReportView.h"
#include "Macro.h"
#include "ProgressBar.h"
#include "Window.h" 
#include "Workbench.h"
#include "WorkbenchManager.h"
#include "CommandBarManager.h"
#include "SoFCSelection.h"

#include "Inventor/Qt/SoQt.h"
#include "Language/Translator.h"
#include "GuiInitScript.h"


using Base::Console;
using Base::Interpreter;
using namespace Gui;
using namespace Gui::DockWnd;
using Gui::Dialog::DlgOnlineHelpImp;
using namespace std;


MainWindow* MainWindow::Instance = 0L;

namespace Gui {

// Pimpl class
struct MainWindowP
{
  MainWindowP()
    : _pcActiveDocument(0L), _bIsClosing(false)
  {
    // create the macro manager
//    _pcMacroMngr = new MacroManager();
  }

  ~MainWindowP()
  {
//    delete _pcMacroMngr;
  }

  QValueList<int> wndIDs;
  /// list of all handled documents
  list<Gui::Document*>         lpcDocuments;
  /// Active document
  Gui::Document*   _pcActiveDocument;
  Gui::DockWindowManager* _pcDockMgr;
  MacroManager*  _pcMacroMngr;
  QLabel *         _pclSizeLabel, *_pclActionLabel;
  ToolBox*        _pcStackBar;
  QTimer *		 _pcActivityTimer; 
  /// List of all registered views
  list<Gui::BaseView*>					_LpcViews;
  bool _bIsClosing;
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
  Gui::Translator::installLanguage();
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

  // caption and icon of the main window
  setCaption( App::Application::Config()["ExeName"].c_str() );
  setIcon(Gui::BitmapFactory().pixmap(App::Application::Config()["AppIcon"].c_str()));

  // global access 
  Instance = this;

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
  CommandBarManager::getInstance()->setToolBox( d->_pcStackBar );
  d->_pcDockMgr = new Gui::DockWindowManager();
  d->_pcDockMgr->addDockWindow( "Toolbox",d->_pcStackBar, Qt::DockRight );

  // Help View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
  QString home = DlgOnlineHelpImp::getStartpage();
  HelpView* pcHelpView = new HelpView( home, this, "HelpViewer" );
  d->_pcDockMgr->addDockWindow("Help view", pcHelpView, Qt::DockRight );

#ifdef FC_DEBUG
  // Tree Bar  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
  TreeView* pcTree = new TreeView(0,this,"Raw_tree");
  pcTree->setMinimumWidth(210);
  d->_pcDockMgr->addDockWindow("Tree view", pcTree, Qt::DockLeft );

  // PropertyView  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
  PropertyView* pcPropView = new PropertyView(0,0,"PropertyView");
  pcPropView->setMinimumWidth(210);
  d->_pcDockMgr->addDockWindow("Property editor", pcPropView, Qt::DockLeft );

#endif

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

void MainWindow::open(const char* FileName)
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
    Cmd =  "import ";
    Cmd += Mod;
    Cmd += "Gui";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Gui,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());

    // load the file with the module
    Cmd = Mod;
    Cmd += ".open(\"";
    Cmd += File.filePath().c_str();
    Cmd += "\")";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Base,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());
    sendMsgToActiveView("ViewFit");
  }else{
    Base::Console().Error("MainWindow::open() try to open unknown file type .%s\n",te.c_str());
    return;
  }

  // the original file name is required
  appendRecentFile( File.filePath().c_str() );
}

void MainWindow::import(const char* FileName)
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
    Cmd =  "import ";
    Cmd += Mod;
    Cmd += "Gui";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Gui,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());

    // load the file with the module
    Cmd = Mod;
    Cmd += ".insert(\"";
    Cmd += File.filePath().c_str();
    Cmd += "\")";
    Base::Interpreter().runString(Cmd.c_str());
    macroManager()->addLine(MacroManager::Base,Cmd.c_str());
    Base::Console().Log("CmdO: %s\n",Cmd.c_str());

  }else{
    Base::Console().Error("MainWindow::import() try to open unknowne file type .%s\n",te.c_str());
    return;
  }

  // the original file name is required
  appendRecentFile( File.filePath().c_str() );
}

void MainWindow::createStandardOperations()
{
  // register the application Standard commands from CommandStd.cpp
  Gui::CreateStdCommands();
  Gui::CreateViewStdCommands();
  Gui::CreateWindowStdCommands();
  Gui::CreateTestCommands();
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

void MainWindow::closeActiveWindow ()
{
  d->_pWorkspace->closeActiveWindow();
}

void MainWindow::closeAllWindows ()
{
  d->_pWorkspace->closeAllWindows();
}

void MainWindow::activateNextWindow ()
{
  d->_pWorkspace->activateNextWindow();
}

void MainWindow::activatePrevWindow ()
{
  d->_pWorkspace->activatePrevWindow();
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

  // look if the window was already inserted
  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data() == view )
      return;
  }

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

void MainWindow::removeWindow( Gui::MDIView* view )
{
  // free all connections
  disconnect( view, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( message(const QString&, int )) );

  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data() == view )
    {
      QTab* tab = d->_tabs->tab( it.key() );
      d->_tabs->removeTab( tab );
      d->_mdiIds.remove( it );
      if ( d->_tabs->count() == 0 )
        d->_tabs->hide(); // no view open any more
      break;
    }
  }

  // this view is not under control of the main window any more
  disconnect( view, SIGNAL( destroyed() ), this, SLOT( onWindowDestroyed() ) );
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
//  QPopupMenu* windowsMenu = d->windows;
  QPopupMenu* windowsMenu = (QPopupMenu*)sender();
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
  // redirect all meesages to the view in fullscreen mode, if so
  MDIView* pView = 0;
  MDIView* pTmp = 0;
  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data()->isFullScreen() )
    {
      // store this in case we have a non-acitve view in fullscreen (e.g. opened dialog that is active)
      pTmp = it.data(); 
      if ( it.data()->isActiveWindow() )
        pView = it.data();
      break;
    }
  }
  // if no fullscreen window then look in workspace
  if ( !pView )
  {
    pView = reinterpret_cast <MDIView *> ( d->_pWorkspace->activeWindow() );
    if ( !pView && pTmp )
      pView = pTmp;
  }

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
  App::GetApplication().setActiveDocument( pcDocument ? pcDocument->getDocument() : 0 );

#ifdef FC_LOGUPDATECHAIN
  Console().Log("Acti: Gui::Document,%p\n",d->_pcActiveDocument);
#endif

  // notify all views attached to the application (not views belong to a special document)
  for(list<Gui::BaseView*>::iterator It=d->_LpcViews.begin();It!=d->_LpcViews.end();It++)
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
void MainWindow::viewActivated(MDIView* pcView)
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
    for (list<Gui::Document*>::iterator It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
    {
      (*It)->canClose ( e );
  //			if(! e->isAccepted() ) break;
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

//		if(! e->isAccepted() ) break;
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

    //detache the passiv views
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

    d->_pcActivityTimer->stop();

    QMainWindow::closeEvent( e );
  }
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
  string FileName(GetApplication().GetHomePath());
  FileName += "FreeCAD.xml";
  QFile* datafile = new QFile(FileName.c_str());
  if (!datafile->open(IO_ReadOnly)) 
  {
    // error opening file
    bool bMute = MessageBoxObserver::bMute;
    MessageBoxObserver::bMute = true;
    Console().Warning((tr("Error: Cannot open file '%1' "
                             "(Maybe you're running FreeCAD the first time)\n").arg(FileName.c_str())));
    MessageBoxObserver::bMute = bMute;
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
  string FileName(GetApplication().GetHomePath());
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
  vector<Command*> cmd = rclMan.getAllCommands();
  for ( vector<Command*>::iterator it = cmd.begin(); it != cmd.end(); ++it )
  {
    (*it)->languageChange();
  }

  MenuManager::getInstance()->languageChange();
}

//**************************************************************************
// Init, Destruct and singelton

QApplication* MainWindow::_pcQApp = NULL ;

QSplashScreen *MainWindow::_splash = NULL;


void MainWindow::initApplication(void)
{
  new Base::ScriptProducer( "FreeCADGuiInit", FreeCADGuiInit );
}
/*
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
*/
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
      _splash = new SplashScreen( Gui::BitmapFactory().pixmap(App::Application::Config()["SplashPicture"].c_str()) );
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
          open(info.absFilePath().latin1());
      }
    }
  }else
    MainWindow::dropEvent(e);
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
