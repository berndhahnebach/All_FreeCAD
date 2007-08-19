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

// FreeCAD Base header
#include <Base/Parameter.h>
#include <Base/FileInfo.h>
#include <App/Application.h>

#include "MainWindow.h"
#include "Application.h"

#include "Action.h"
#include "Command.h"

#include "CommandBarManager.h"
#include "DockWindowManager.h"
#include "WorkbenchManager.h"
#include "Workbench.h"

#include "Window.h" 
#include "View.h"
#include "Macro.h"
#include "ProgressBar.h"

#include "Icons/background.xpm"
#include "WidgetFactory.h"
#include "BitmapFactory.h"
#include "Splashscreen.h"

#include "Tree.h"
#include "PropertyView.h"
#include "MenuManager.h"
#include "ToolBox.h"
#include "HelpView.h"
#include "ReportView.h"

#include "DlgTipOfTheDayImp.h"
#include "DlgUndoRedo.h"
#include "DlgOnlineHelpImp.h"

#include "Language/Translator.h"
#include "GuiInitScript.h"


using namespace Gui;
using namespace Gui::DockWnd;
using namespace std;


MainWindow* MainWindow::instance = 0L;

namespace Gui {

// Pimpl class
struct MainWindowP
{
  QLabel* sizeLabel;
  QLabel* actionLabel;
  QTimer* actionTimer;
  QTimer* activityTimer; 
  QWorkspace* workspace;
  QTabBar* tabs;
  QSignalMapper* windowMapper;
  QSplashScreen* splashscreen;
  StatusBarObserver* status;
};

class MDITabbar : public QTabBar
{
public:
  MDITabbar( QWidget * parent = 0 ) : QTabBar(parent)
  {
    menu = new QMenu(this);
    // For Qt 4.2.x the tabs might be very wide
#if QT_VERSION >= 0x040200
    setDrawBase(false);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
#endif
  }

  ~MDITabbar()
  {
    delete menu;
  }

protected:
  void contextMenuEvent ( QContextMenuEvent * e )
  {
    menu->clear();
    CommandManager& cMgr = Application::Instance->commandManager();
    if ( tabRect(currentIndex()).contains(e->pos()) )
      cMgr.getCommandByName("Std_CloseActiveWindow")->addTo(menu);
    cMgr.getCommandByName("Std_CloseAllWindows")->addTo(menu);
    menu->addSeparator();
    cMgr.getCommandByName("Std_CascadeWindows")->addTo(menu);
    cMgr.getCommandByName("Std_ArrangeIcons")->addTo(menu);
    cMgr.getCommandByName("Std_TileWindows")->addTo(menu);
    menu->addSeparator();
    cMgr.getCommandByName("Std_Windows")->addTo(menu);
    menu->popup(e->globalPos());
  }

private:
  QMenu* menu;
};

} // namespace Gui


/* TRANSLATOR Gui::MainWindow */

MainWindow::MainWindow(QWidget * parent, Qt::WFlags f)
  : QMainWindow( parent, f/*WDestructiveClose*/ )
{
  d = new MainWindowP;
  d->splashscreen = 0;

  // global access 
  instance = this;

  // Create the layout containing the workspace and a tab bar
  QFrame* vbox = new QFrame(this);
  vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(1);
  vbox->setLayout(layout);

  d->workspace = new QWorkspace();
  d->workspace->setScrollBarsEnabled( true );
  QPixmap backgnd(( const char** ) background );
  d->workspace->setBackground( backgnd );
  d->tabs = new MDITabbar();
  d->tabs->setShape( QTabBar:: RoundedSouth );

  layout->addWidget(d->workspace);
  layout->addWidget(d->tabs);
  setCentralWidget( vbox );

  // window title and icon of the main window
  setWindowTitle( App::Application::Config()["ExeName"].c_str() );
  setWindowIcon(Gui::BitmapFactory().pixmap(App::Application::Config()["AppIcon"].c_str()));

  // labels and progressbar
  d->status = new StatusBarObserver();
  d->actionLabel = new QLabel(statusBar());
  d->actionLabel->setMinimumWidth(120);
  d->sizeLabel = new QLabel(tr("Dimension"), statusBar());
  d->sizeLabel->setMinimumWidth(120);
  statusBar()->addWidget(d->actionLabel, 0);
  QProgressBar* progressBar = Gui::Sequencer::instance()->getProgressBar(statusBar());
  statusBar()->addPermanentWidget(progressBar, 0);
  statusBar()->addPermanentWidget(d->sizeLabel, 0);

  // clears the action label
  d->actionTimer = new QTimer( this );
  connect(d->actionTimer, SIGNAL(timeout()), d->actionLabel, SLOT(clear()));

  // update gui timer
  d->activityTimer = new QTimer( this );
  connect( d->activityTimer, SIGNAL(timeout()),this, SLOT(updateActions()) );
  d->activityTimer->setSingleShot(true);
  d->activityTimer->start(300);

  d->windowMapper = new QSignalMapper(this);

  // connection between workspace, window menu and tab bar
  connect( d->windowMapper, SIGNAL( mapped(QWidget *) ), d->workspace, SLOT( setActiveWindow( QWidget* ) ) );
  connect( d->workspace, SIGNAL( windowActivated ( QWidget * ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
  connect( d->tabs, SIGNAL( currentChanged( int) ), this, SLOT( onTabSelected(int) ) );

  DockWindowManager* pDockMgr = DockWindowManager::instance();

  // Show all dockable windows over the workbench facility
  static const char* dockWindows[] = {
    QT_TRANSLATE_NOOP( "QDockWidget", "Toolbox" ),
    QT_TRANSLATE_NOOP( "QDockWidget", "Help view" ),
    QT_TRANSLATE_NOOP( "QDockWidget", "Tree view" ),
    QT_TRANSLATE_NOOP( "QDockWidget", "Property view" ),
    QT_TRANSLATE_NOOP( "QDockWidget", "Report View" )
  };

  // Toolbox
  ToolBox* toolBox = new ToolBox(this);
  toolBox->setWindowTitle("Toolbox");
  pDockMgr->registerDockWindow("Std_ToolBox", toolBox);
  CommandBarManager::getInstance()->setToolBox( toolBox );

  // Help View
  //QString home = Gui::Dialog::DlgOnlineHelpImp::getStartpage();
  //HelpView* pcHelpView = new HelpView( home, this );
  //pDockMgr->registerDockWindow("Std_HelpView", pcHelpView);

  // Tree view
  TreeDockWidget* tree = new TreeDockWidget(0, this);
  tree->setWindowTitle("Tree view");
  tree->setMinimumWidth(210);
  pDockMgr->registerDockWindow("Std_TreeView", tree);
/*
  // Tree view
  pcTree = new TreeView(0,this);
  pcTree->setWindowTitle("Tree view");
  pcTree->setMinimumWidth(210);
  pDockMgr->registerDockWindow("Std_TreeView", pcTree);
*/
  // Property view
  PropertyView* pcPropView = new PropertyView(0, this);
  pcPropView->setWindowTitle("Property view");
  pcPropView->setMinimumWidth(210);
  pDockMgr->registerDockWindow("Std_PropertyView", pcPropView);

  // Report view
  Gui::DockWnd::ReportView* pcReport = new Gui::DockWnd::ReportView(this);
  pcReport->setWindowTitle("Report view");
  pDockMgr->registerDockWindow("Std_ReportView", pcReport);

  // accept drops on the window, get handled in dropEvent, dragEnterEvent
  setAcceptDrops(true);
  statusBar()->showMessage( tr("Ready"), 2001 );
}

MainWindow::~MainWindow()
{
  delete d->status;
  delete d;
  instance = 0;
}

MainWindow* MainWindow::getInstance()
{
  // MainWindow has a public constructor
  return instance;
}

QMenu* MainWindow::createPopupMenu ()
{
  Command* cmd = Application::Instance->commandManager().getCommandByName("Std_DlgCustomize");
  QMenu* menu = QMainWindow::createPopupMenu();
  if (cmd) { 
    menu->addSeparator();
    cmd->addTo(menu);
  }

  return menu;
}

void MainWindow::arrangeIcons()
{
  d->workspace->arrangeIcons();
}

void MainWindow::tile()
{
  d->workspace->tile();
}

void MainWindow::cascade()
{
  d->workspace->cascade();
}

void MainWindow::closeActiveWindow ()
{
  d->workspace->closeActiveWindow();
}

void MainWindow::closeAllWindows ()
{
  d->workspace->closeAllWindows();
}

void MainWindow::activateNextWindow ()
{
  d->workspace->activateNextWindow();
}

void MainWindow::activatePreviousWindow ()
{
  d->workspace->activatePreviousWindow();
}

void MainWindow::whatsThis()
{
  QWhatsThis::enterWhatsThisMode();
}

bool MainWindow::eventFilter ( QObject* o, QEvent* e )
{
  if ( o != this ) {
    if ( e->type() == QEvent::WindowStateChange ) {
      // notify all mdi views when the active view receives a show normal, show minimized or show maximized event 
      MDIView * view = (MDIView*)o->qt_metacast("Gui::MDIView");
      if ( view )
        showActiveView( view );
    }
  }
#if 0 //TODO Check eventFilter
  if ( o != this ) {
    if ( e->type() == QEvent::ShowMaximized || e->type() == QEvent::ShowMinimized || e->type() == QEvent::ShowNormal || e->type() == QEvent::ShowFullScreen ) {
      MDIView * view = (MDIView*)o->qt_metacast("Gui::MDIView");

      // notify all mdi views when the active view receives a show normal, show minimized or show maximized event 
      if ( view )
        emit showActiveView( view );
    }
  }
#endif

  return QMainWindow::eventFilter( o, e );
}

void MainWindow::addWindow( MDIView* view )
{
  // make workspace parent of view
  d->workspace->addWindow(view);
  connect( view, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( showMessage(const QString&, int )) );
  connect( this, SIGNAL( showActiveView(MDIView*) ), view, SLOT( showActiveView(MDIView*) ) );

  // listen to the incoming events of the view
  view->installEventFilter(this);

  // show the very first window in maximized mode
  if ( d->workspace->windowList().isEmpty() )
    view->showMaximized();
  else
    view->show();

  // look if the window was already inserted
  for ( int i=0; i < d->tabs->count(); i++ ) {
    if ( d->tabs->tabData(i).value<QWidget*>() == view )
      return;
  }

  // being informed when the view is destroyed
  connect( view, SIGNAL( destroyed() ), this, SLOT( onWindowDestroyed() ) );

  // add a new tab to our tabbar
  int index=-1;
  index = d->tabs->addTab(view->windowIcon(), view->windowTitle());
  d->tabs->setTabToolTip( index, view->windowTitle() );
  QVariant var; var.setValue<QWidget*>(view);
  d->tabs->setTabData( index, var);

  tabChanged( view );
  if ( d->tabs->count() == 1 )
    d->tabs->show(); // invoke show() for the first tab
  d->tabs->update();
  d->tabs->setCurrentIndex( index );
}

void MainWindow::removeWindow( Gui::MDIView* view )
{
  // free all connections
  disconnect( view, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( message(const QString&, int )) );
  disconnect( this, SIGNAL( showActiveView(MDIView*) ), view, SLOT( showActiveView(MDIView*) ) );
  view->removeEventFilter(this);

  for ( int i = 0; i < d->tabs->count(); i++ ) {
    if ( d->tabs->tabData(i).value<QWidget*>() == view ) {
      d->tabs->removeTab( i );
      if ( d->tabs->count() == 0 )
        d->tabs->hide(); // no view open any more
      break;
    }
  }

  // this view is not under control of the main window any more
  disconnect( view, SIGNAL( destroyed() ), this, SLOT( onWindowDestroyed() ) );
}

void MainWindow::tabChanged( MDIView* view )
{
    for ( int i = 0; i < d->tabs->count(); i++ ) {
        if ( d->tabs->tabData(i).value<QWidget*>() == view ) {
            // remove file separators
            QString cap = view->windowTitle();
            int pos = cap.lastIndexOf('/');
            cap = cap.mid( pos+1 );
            pos = cap.lastIndexOf('\\');
            cap = cap.mid( pos+1 );

            QString placeHolder(QLatin1String("[*]"));

            int index = cap.indexOf(placeHolder);

            while (index != -1) {
                index += placeHolder.length();
                int count = 1;
                while (cap.indexOf(placeHolder, index) == index) {
                    ++count;
                    index += placeHolder.length();
                }

                if (count%2) { // odd number of [*] -> replace last one
                    int lastIndex = cap.lastIndexOf(placeHolder, index - 1);
                    if (view->isWindowModified() && view->style()->styleHint(QStyle::SH_TitleBar_ModifyNotification, 0, view))
                        cap.replace(lastIndex, 3, QWidget::tr("*"));
                    else
                        cap.replace(lastIndex, 3, QLatin1String(""));
                }

                index = cap.indexOf(placeHolder, index);
            }

            cap.replace(QLatin1String("[*][*]"), QLatin1String("[*]"));
            d->tabs->setTabText(i, cap );
            break;
        }
    }
}

MDIView* MainWindow::getWindowWithCaption( const QString& cap ) const
{
  for ( int i = 0; i < d->tabs->count(); i++ ) {
    MDIView* view = qobject_cast<MDIView*>(d->tabs->tabData(i).value<QWidget*>());
    if ( view->windowTitle() == cap ) {
      return view;
    }
  }

  return 0;
}

void MainWindow::onWindowDestroyed()
{
  QObject* view = (QObject*)sender();
  for ( int i = 0; i < d->tabs->count(); i++ )
  {
    if ( d->tabs->tabData(i).value<QWidget*>() == view )
    {
      d->tabs->removeTab( i );
      if ( d->tabs->count() == 0 )
        d->tabs->hide(); // no view open any more
      break;
    }
  }
}

void MainWindow::onWindowActivated( QWidget* w )
{
  MDIView* view = dynamic_cast<MDIView*>(w);

  // Even if windowActivated() signal is emitted mdi doesn't need to be a top-level window.
  // This happens e.g. if two windows are top-level and one of them gets docked again.
  // QWorkspace emits the signal then even though the other window is in front.
  // The consequence is that the docked window becomes the active window and not the undocked
  // window on top. This means that all accel events, menu and toolbar actions get redirected
  // to the (wrong) docked window.
  // But just testing whether the view is active and ignore it if not leads to other more serious problems -
  // at least under Linux. It seems to be a problem with the window manager.
  // Under Windows it seems to work though it's not really sure that it works reliably.
  // Result: So, we accept the first problem to be sure to avoid the second one.
  if ( !view /*|| !mdi->isActiveWindow()*/ ) 
    return; // either no MDIView or no valid object or no top-level window

  // set active the appropriate window (it needs not to be part of mdiIds, e.g. directly after creation)
  view->setActiveView();

  // set the appropriate tab to the new active window
  for ( int i = 0; i < d->tabs->count(); i++ )
  {
    if ( d->tabs->tabData(i).value<QWidget*>() == view )
    {
      d->tabs->blockSignals( true );
      d->tabs->setCurrentIndex( i );
      d->tabs->blockSignals( false );
      break;
    }
  }
}

void MainWindow::onTabSelected( int i)
{
  QVariant var = d->tabs->tabData(i);
  if ( var.isValid() && var.canConvert<QWidget*>() ) {
    QWidget* view = var.value<QWidget*>();
    if ( view && !view->hasFocus() )
      view->setFocus();
  }
}

void MainWindow::onWindowsMenuAboutToShow()
{
  QList<QWidget*> windows = d->workspace->windowList( QWorkspace::CreationOrder );
  QWidget* active = d->workspace->activeWindow();

  // We search for the 'Std_WindowsMenu' command that provides the list of actions
  CommandManager& cMgr = Application::Instance->commandManager();
  Command* cmd = cMgr.getCommandByName("Std_WindowsMenu");
  QList<QAction*> actions = qobject_cast<ActionGroup*>(cmd->getAction())->actions();

  int numWindows = std::min<int>(actions.count()-1, windows.count());
  for ( int index = 0; index < numWindows; index++ ) {
    QWidget* child = windows.at(index);
    QAction* action = actions.at(index);
    QString text;
    if (index < 9)
      text = QString("&%1 %2").arg( index+1 ).arg(child->windowTitle());
    else
      text = QString("%1 %2").arg( index+1 ).arg(child->windowTitle());
    action->setText(text);
    action->setVisible(true);
    action->setChecked(child == active);
    connect(action, SIGNAL(triggered()), d->windowMapper, SLOT(map()));
    d->windowMapper->setMapping(action, child);
  }

  // if less windows than actions
  for (int index = numWindows; index < actions.count(); index++)
    actions[index]->setVisible(false);
  // show the separator
  if ( numWindows > 0 )
    actions.last()->setVisible(true);
}

QList<QWidget*> MainWindow::windows( QWorkspace::WindowOrder order ) const
{
  return d->workspace->windowList( order );
}

// set text to the pane
void MainWindow::setPaneText(int i, QString text)
{
  if (i==1) {
    d->actionLabel->setText(text);
    d->actionTimer->setSingleShot(true);
    d->actionTimer->start(5000);
  } else if (i==2) {
    d->sizeLabel->setText(text);
  }
}

//TODO Redo the active window stuff
MDIView* MainWindow::activeWindow(void) const
{/*
  // redirect all meesages to the view in fullscreen mode, if so
  MDIView* pView = 0;
  MDIView* pTmp = 0;
  for ( QMap<int, MDIView*>::Iterator it = d->mdiIds.begin(); it != d->mdiIds.end(); it++ )
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
    pView = reinterpret_cast <MDIView *> ( d->workspace->activeWindow() );
    if ( !pView && pTmp )
      pView = pTmp;
  }

  return pView;*/

  // the corresponding window to the current tab is active
  int id = d->tabs->currentIndex();
  if ( id < 0 )
    return 0;
  return qobject_cast<MDIView*>(d->tabs->tabData(id).value<QWidget*>());
}

void MainWindow::setActiveWindow( MDIView* view )
{
  d->workspace->setActiveWindow(view);
}

void MainWindow::closeEvent ( QCloseEvent * e )
{
  //TODO Reimplement the fullscreen mode stuff
  Application::Instance->tryClose( e );
  if( e->isAccepted() )
  {
    // Before closing the main window we must make sure that all views are in 'Normal' mode otherwise the 'lastWindowClosed()' signal
    // doesn't get emitted from QApplication later on. Just destroying these views doesn't work either.
    for ( int i = 0; i < d->tabs->count(); i++ )
    {
      MDIView* view = qobject_cast<MDIView*>(d->tabs->tabData(i).value<QWidget*>());
      if ( view->currentViewMode() != MDIView::Normal )
        view ->setCurrentViewMode(MDIView::Normal);
    }

    d->activityTimer->stop();
    QMainWindow::closeEvent( e );
  }
}

void MainWindow::appendRecentFile(const QString& filename)
{
    RecentFilesAction *recent = getMainWindow()->findChild<RecentFilesAction *>(QString("recentFiles"));
    if (recent) {
        recent->appendFile(filename);
    }
}

void MainWindow::updateActions()
{
  static QTime cLastCall;

  if ( cLastCall.elapsed() > 250 && isVisible() )
  {
    Application::Instance->commandManager().testActive();
    //pcTree->testStatus();
    // remember last call
    cLastCall.start();
  }

  d->activityTimer->setSingleShot(true);
  d->activityTimer->start(300);	
}

void MainWindow::switchToTopLevelMode()
{
  QList<QDockWidget*> dw = this->findChildren<QDockWidget*>();
  for (QList<QDockWidget*>::Iterator it = dw.begin(); it != dw.end(); ++it) {
    (*it)->setParent(0, Qt::Window);
    (*it)->show();
  }
  QList<QWidget*> mdi = getMainWindow()->windows();
  for (QList<QWidget*>::Iterator it = mdi.begin(); it != mdi.end(); ++it) {
    (*it)->setParent(0, Qt::Window);
    (*it)->show();
  }
}

void MainWindow::switchToDockedMode()
{
  // TODO
  QMessageBox::critical(getMainWindow(), "Not implemented yet", "Not implemented yet");
}

void MainWindow::loadWindowSettings()
{
  loadLayoutSettings();

  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("MainWindow");
  int w = hGrp->GetInt("Width", 1024);
  int h = hGrp->GetInt("Height", 768);
  int x = hGrp->GetInt("PosX", pos().x());
  int y = hGrp->GetInt("PosY", pos().y());
  bool max = hGrp->GetBool("Maximized", false);
  resize( w, h );
  move(x, y);
  if (max) showMaximized();
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

  saveLayoutSettings();
}

void MainWindow::loadLayoutSettings()
{
  std::string path = App::GetApplication().Config()["UserAppData"];
  QByteArray state;
  QDir dir(path.c_str());
  if (dir.exists() && WorkbenchManager::instance()->active()) {
    QString name = WorkbenchManager::instance()->active()->name();
    for (int i=0; i<name.length(); i++) {
      if (!name[i].isLetterOrNumber())
        name[i] = '_';
    }
    name += ".bin";
    QString filename = dir.filePath(name);
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
      QDataStream str(&file);
      str >> state;
      file.close();
    }
  }

  QString hidden = App::Application::Config()["HiddenDockWindow"].c_str();
  QStringList hiddenDW = hidden.split(";");
  DockWindowManager::instance()->hideDockWindows( hiddenDW );

  restoreState(state);
}

void MainWindow::saveLayoutSettings()
{
  std::string path = App::GetApplication().Config()["UserAppData"];
  QDir dir(path.c_str());

  if (dir.exists() && WorkbenchManager::instance()->active()) {
    QString name = WorkbenchManager::instance()->active()->name();
    for (int i=0; i<name.length(); i++) {
      if (!name[i].isLetterOrNumber())
        name[i] = '_';
    }
    name += ".bin";
    QString filename = dir.filePath(name);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
      QByteArray state = saveState();
      QDataStream str(&file);
      str << state;
      file.close();
    }
  }
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
      d->splashscreen = new SplashScreen( Gui::BitmapFactory().pixmap(App::Application::Config()["SplashPicture"].c_str()) );
      d->splashscreen->show();
    }else
      d->splashscreen = 0;


  }
}

void MainWindow::stopSplasher(void)
{
  if ( d->splashscreen )
  {
    d->splashscreen->finish( this );
    delete d->splashscreen;
    d->splashscreen = 0;
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
    Gui::Dialog::DlgTipOfTheDayImp dlg(instance);
    dlg.exec();
  }
}

/**
 * Drops the event \a e and tries to open the files.
 */
void MainWindow::dropEvent ( QDropEvent* e )
{
  const QMimeData* data = e->mimeData();
  if ( data->hasUrls() ) {
    QList<QUrl> uri = data->urls();
    for ( QList<QUrl>::ConstIterator it = uri.begin(); it != uri.end(); ++it ) {
      QFileInfo info((*it).toLocalFile());
      if ( info.exists() && info.isFile() ) {
        // First check the complete extension
        if ( App::GetApplication().hasOpenType( info.completeSuffix().toAscii() ) )
          Application::Instance->open(info.absoluteFilePath().toUtf8());
        // Don't get the complete extension
        else if ( App::GetApplication().hasOpenType( info.suffix().toAscii() ) )
          Application::Instance->open(info.absoluteFilePath().toUtf8());
      }
    }
  } else {
    QMainWindow::dropEvent(e);
  }
}

void MainWindow::dragEnterEvent ( QDragEnterEvent * e )
{
  // Here we must allow uri drafs and check them in dropEvent
  const QMimeData* data = e->mimeData();
  if ( data->hasUrls() )
    e->accept();
  else
    e->ignore();
}

void MainWindow::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    d->sizeLabel->setText(tr("Dimension"));
    
    CommandManager& rclMan = Application::Instance->commandManager();
    vector<Command*> cmd = rclMan.getAllCommands();
    for ( vector<Command*>::iterator it = cmd.begin(); it != cmd.end(); ++it )
      (*it)->languageChange();

    // reload current workbench to retranslate all actions and winow titles
    Workbench* wb = WorkbenchManager::instance()->active();
    if (wb) {
      this->saveLayoutSettings();
      wb->activate();
      this->loadLayoutSettings();
    }
  } else {
    QMainWindow::changeEvent(e);
  }
}

// -------------------------------------------------------------

namespace Gui {

/**
 * The CustomMessageEvent class is used to send messages as events in the methods  
 * Error(), Warning() and Message() of the StatusBarObserver class to the main window 
 * to display them on the status bar instead of printing them directly to the status bar.
 *
 * This makes the usage of StatusBarObserver thread-safe.
 * @author Werner Mayer
 */
class CustomMessageEvent : public QEvent
{
public:
  CustomMessageEvent(const QString& s)
    : QEvent(QEvent::User), msg(s)
  { }
  ~CustomMessageEvent()
  { }
  const QString& message() const
  { return msg; }
private:
  QString msg;
};
}

void MainWindow::customEvent( QEvent* e )
{
  if (e->type() == QEvent::User) {
    Gui::CustomMessageEvent* ce = (Gui::CustomMessageEvent*)e;
    d->actionLabel->setText(ce->message());
    d->actionTimer->setSingleShot(true);
    d->actionTimer->start(5000);
  }
}

// ----------------------------------------------------------

StatusBarObserver::StatusBarObserver()
{
  Base::Console().AttachObserver(this);
}

StatusBarObserver::~StatusBarObserver()
{
  Base::Console().DetachObserver(this);
}

/** Get called when a message is issued. 
 * The message is displayed on the ststus bar. 
 */
void StatusBarObserver::Message(const char * m)
{
  // Send the event to the main window to allow thread-safety. Qt will delete it when done.
    QString msg = QString("<font color=\"#000000\">%1</font>").arg(QString::fromUtf8(m));
  CustomMessageEvent* ev = new CustomMessageEvent(msg);
  QApplication::postEvent(getMainWindow(), ev);
}

/** Get called when a warning is issued. 
 * The message is displayed on the ststus bar. 
 */
void StatusBarObserver::Warning(const char *m)
{
  // Send the event to the main window to allow thread-safety. Qt will delete it when done.
  QString msg = QString("<font color=\"#ffaa00\">%1</font>").arg(QString::fromUtf8(m));
  CustomMessageEvent* ev = new CustomMessageEvent(msg);
  QApplication::postEvent(getMainWindow(), ev);
}

/** Get called when an error is issued. 
 * The message is displayed on the ststus bar. 
 */
void StatusBarObserver::Error  (const char *m)
{
  // Send the event to the main window to allow thread-safety. Qt will delete it when done.
  QString msg = QString("<font color=\"#ff0000\">%1</font>").arg(QString::fromUtf8(m));
  CustomMessageEvent* ev = new CustomMessageEvent(msg);
  QApplication::postEvent(getMainWindow(), ev);
}

/** Get called when a log message is issued. 
 * Log messages are completely ignored.
 */
void StatusBarObserver::Log(const char *log)
{
}


#include "moc_MainWindow.cpp"
