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
# include <qdragobject.h>
# include <qeventloop.h>
# include <qfileinfo.h>
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
#include "../Base/Factory.h"
#include "../Base/FileInfo.h"
#include "../App/Application.h"
#include "../App/Feature.h"
#include "../App/Document.h"

#include "MainWindow.h"
#include "Application.h"
#include "Document.h"
#include "View.h"

#include "Icons/developers.h"
#include "WidgetFactory.h"
#include "Command.h"
#include "Tree.h"
#include "PropertyView.h"
#include "BitmapFactory.h"
#include "Splashscreen.h"
#include "MenuManager.h"

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
#include "SoFCDB.h"

#include "Inventor/Qt/SoQt.h"
#include "Language/Translator.h"
#include "GuiInitScript.h"


using Base::Console;
using Base::Interpreter;
using namespace Gui;
using namespace Gui::DockWnd;
using Gui::Dialog::DlgOnlineHelpImp;
using namespace std;


MainWindow* MainWindow::instance = 0L;

namespace Gui {

// Pimpl class
struct MainWindowP
{
  QValueList<int> wndIDs;
  QLabel *         _pclSizeLabel, *_pclActionLabel;
  ToolBox*        _pcStackBar;
  QTimer *		 _pcActivityTimer; 
  QWorkspace* _pWorkspace;
  QTabBar* _tabs;
  QMap<int, MDIView*> _mdiIds;
};

class MDITabbar : public QTabBar
{
public:
  MDITabbar( QWidget * parent = 0, const char * name = 0 ) : QTabBar(parent, name)
  {
    menu = new QPopupMenu(this);
  }

protected:
  void contextMenuEvent ( QContextMenuEvent * e )
  {
    QTab* sel = selectTab(e->pos());
    menu->clear();
    CommandManager& cMgr = Application::Instance->commandManager();
    cMgr.getCommandByName("Std_New")->addTo(menu);
    menu->insertSeparator();
    if ( sel == tab(currentTab()) )
      cMgr.getCommandByName("Std_CloseActiveWindow")->addTo(menu);
    cMgr.getCommandByName("Std_CloseAllWindows")->addTo(menu);
    menu->insertSeparator();
    cMgr.getCommandByName("Std_TilePragmatic")->addTo(menu);
    cMgr.getCommandByName("Std_TileHoricontal")->addTo(menu);
    cMgr.getCommandByName("Std_TileVertical")->addTo(menu);
    menu->insertSeparator();
    cMgr.getCommandByName("Std_Windows")->addTo(menu);
    menu->popup(e->globalPos());
  }

private:
  QPopupMenu* menu;
};

} // namespace Gui


/* TRANSLATOR Gui::MainWindow */

MainWindow::MainWindow(QWidget * parent, const char * name, WFlags f)
    : QMainWindow( parent, "Main window", f/*WDestructiveClose*/ )
{
  d = new MainWindowP;
  // global access 
  instance = this;

  // init the Inventor subsystem
  SoQt::init(this);
  SoDB::init();
  SoFCDB::init();

  QVBox* vb = new QVBox( this );
  vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  d->_pWorkspace = new QWorkspace( vb, "workspace" );

  QPixmap backgnd(( const char** ) background );
  d->_pWorkspace->setPaletteBackgroundPixmap( backgnd );
  d->_tabs = new MDITabbar( vb, "tabBar" );
  d->_tabs->setShape( QTabBar::RoundedBelow );
  d->_pWorkspace->setScrollBarsEnabled( true );
  setCentralWidget( vb );
  connect( d->_pWorkspace, SIGNAL( windowActivated ( QWidget * ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
  connect( d->_tabs, SIGNAL( selected( int) ), this, SLOT( onTabSelected(int) ) );

  // caption and icon of the main window
  setCaption( App::Application::Config()["ExeName"].c_str() );
  setIcon(Gui::BitmapFactory().pixmap(App::Application::Config()["AppIcon"].c_str()));

  // labels and progressbar
  d->_pclActionLabel = new QLabel("", statusBar(), "Action");
  d->_pclActionLabel->setMinimumWidth(120);
  statusBar()->addWidget(d->_pclActionLabel,0,true);
  statusBar()->addWidget(ProgressBar::instance(),0,true);
  //ProgressBar::Instance().setFixedWidth(200);
  d->_pclSizeLabel = new QLabel("Dimension", statusBar(), "Dimension");
  d->_pclSizeLabel->setMinimumWidth(120);
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
  DockWindowManager* pDockMgr = DockWindowManager::instance();
  pDockMgr->addDockWindow( "Toolbox",d->_pcStackBar, Qt::DockRight );

  // Help View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
  QString home = DlgOnlineHelpImp::getStartpage();
  HelpView* pcHelpView = new HelpView( home, this, "HelpViewer" );
  pDockMgr->addDockWindow("Help view", pcHelpView, Qt::DockRight );

  // Tree Bar  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
  pcTree = new TreeView(0,this,"TreeView");
  pcTree->setMinimumWidth(210);
  pDockMgr->addDockWindow("Tree view", pcTree, Qt::DockLeft );

  // PropertyView  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
  PropertyView* pcPropView = new PropertyView(0,0,"PropertyView");
  pcPropView->setMinimumWidth(210);
  pDockMgr->addDockWindow("Property editor", pcPropView, Qt::DockLeft );

  // Report View
  Gui::DockWnd::ReportView* pcOutput = new Gui::DockWnd::ReportView(this,"ReportView");
  pDockMgr->addDockWindow("Report View", pcOutput, Qt::DockBottom );


  // accept drops on the window, get handled in dropEvent, dragEnterEvent   
  setAcceptDrops(true);

  // misc stuff
  loadWindowSettings();
}

MainWindow::~MainWindow()
{
  // save recent file list
  // Note: the recent files are restored in StdCmdMRU::createAction(), because we need
  //       a valid instance of StdCmdMRU to do this
  StdCmdMRU::save();
  saveWindowSettings();
}

MainWindow* MainWindow::getInstance()
{
  // MainWindow has a public constructor
  return instance;
}

void MainWindow::destruct()
{
  delete instance;
  instance = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool MainWindow::isCustomizable () const
{
  return true;
}

void MainWindow::customize ()
{
  Application::Instance->commandManager().runCommandByName("Std_DlgCustomize");
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

void MainWindow::tileComplex()
{
  QWorkspace* ws=d->_pWorkspace;
  QWidgetList windows = ws->windowList();
  if ( windows.count() < 2 )
  {
    // primitive vetical tiling
    getMainWindow()->tile();
    return;
  }

  // we want all windows except of the last to put at the left side with same size
  int ctWnds = windows.count() > 2 ? windows.count()-1 : 2;
  int heightForEach = ws->height() / ctWnds;
  int y = 0;
  for ( int i = 0; i < int(windows.count()); ++i ) 
  {
    QWidget *window = windows.at(i);
    if ( window->testWState( Qt::WState_Maximized ) ) 
    {
      // prevent flicker
      window->hide();
      window->showNormal();
    }

    int preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
    int actHeight = QMAX(heightForEach, preferredHeight);

    if ( i<ctWnds )
    {
  
      window->parentWidget()->setGeometry( 0, y, actHeight, actHeight );
      y += actHeight;
    }
    else
    {
      // the last window then must fillup the remaining area
      window->parentWidget()->setGeometry( actHeight, 0, ws->width()-actHeight, ws->height() );
    }
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
#ifdef FC_DEBUG
  d->_pWorkspace->closeAllWindows();
#else
  bool closed = true;
  MDIView* p = 0;
  MDIView* w = activeWindow();
  while ( w && w != p && closed )
  {
    p = w;
    closed = w->close();
    w = activeWindow();
  }
#endif
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

void MainWindow::tabChanged( MDIView* view )
{
  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data() == view )
    {
      // extract file name if possible
      QFileInfo fi( view->caption() );
      QTab* tab = d->_tabs->tab( it.key() );
      if ( fi.isFile() && fi.exists() )
        tab->setText( fi.fileName() );
      else
        tab->setText( view->caption() );
      break;
    }
  }
}

MDIView* MainWindow::getWindowWithCaption( const QString& cap ) const
{
  for ( QMap<int, MDIView*>::Iterator it = d->_mdiIds.begin(); it != d->_mdiIds.end(); it++ )
  {
    if ( it.data()->caption() == cap )
      return it.data();
  }

  return 0;
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

// set text to the pane
void MainWindow::setPaneText(int i, QString text)
{
  if (i==1)
    d->_pclActionLabel->setText(text);
  else if (i==2)
    d->_pclSizeLabel->setText(text);
}

MDIView* MainWindow::activeWindow(void)
{/*
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

  return pView;*/

  // the corresponding window to the current tab is active
  MDIView* pView = 0;
  int id = d->_tabs->currentTab();
  QMap<int, MDIView*>::Iterator it = d->_mdiIds.find( id );
  if ( it != d->_mdiIds.end() )
    pView = it.data();
  return pView;
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
  Application::Instance->tryClose( e );
  if( e->isAccepted() )
  {
    d->_pcActivityTimer->stop();
    QMainWindow::closeEvent( e );
  }
}

void MainWindow::appendRecentFile(const char* file)
{
  StdCmdMRU* pCmd = dynamic_cast<StdCmdMRU*>(Application::Instance->commandManager().getCommandByName("Std_MRU"));
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
    Application::Instance->commandManager().testActive();

    pcTree->testStatus();

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

  QString hidden = App::Application::Config()["HiddenDockWindow"].c_str();
  QStringList hiddenDW = QStringList::split ( ';', hidden, false );
  DockWindowManager::instance()->hideDockWindows( hiddenDW );
}

void MainWindow::saveDockWndSettings()
{
  QString str;
  QTextStream ts( &str, IO_WriteOnly );
  ts << *this;

  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("MainWindow");
  hGrp->SetASCII("Layout", str.latin1());
}

void MainWindow::languageChange()
{
  CommandManager& rclMan = Application::Instance->commandManager();
  vector<Command*> cmd = rclMan.getAllCommands();
  for ( vector<Command*>::iterator it = cmd.begin(); it != cmd.end(); ++it )
  {
    (*it)->languageChange();
  }

  MenuManager::getInstance()->languageChange();
}

//**************************************************************************
// Init, Destruct and singelton

QSplashScreen *MainWindow::_splash = NULL;

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
    _splash->finish( instance );
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
    Gui::Dialog::DlgTipOfTheDayImp dlg(instance, "Tipofday");
    dlg.exec();
  }
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
        Application::Instance->open(info.absFilePath().latin1());
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
