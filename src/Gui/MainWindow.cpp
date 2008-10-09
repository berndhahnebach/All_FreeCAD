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
#include "Assistant.h"

#include "Action.h"
#include "Command.h"

#include "ToolBoxManager.h"
#include "DockWindowManager.h"
#include "ToolBarManager.h"
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
#include "SelectionView.h"
#include "MenuManager.h"
//#include "ToolBox.h"
#include "HelpView.h"
#include "ReportView.h"
#include "PythonConsole.h"

#include "DlgTipOfTheDayImp.h"
#include "DlgUndoRedo.h"
#include "DlgOnlineHelpImp.h"

#include "Language/Translator.h"
#include "GuiInitScript.h"

#include "Document.h"
#include "ViewProviderExtern.h"

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
    QTimer* visibleTimer;
    QWorkspace* workspace;
    QTabBar* tabs;
    QPointer<MDIView> activeView;
    QSignalMapper* windowMapper;
    QSplashScreen* splashscreen;
    StatusBarObserver* status;
    bool whatsthis;
    QString whatstext;
    Assistant* assistant;
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
        if (tabRect(currentIndex()).contains(e->pos()))
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
    d->activeView = 0;
    d->whatsthis = false;
    d->assistant = new Assistant();

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
    setWindowTitle(QString::fromAscii(App::Application::Config()["ExeName"].c_str()));
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
    d->activityTimer = new QTimer(this);
    connect(d->activityTimer, SIGNAL(timeout()),this, SLOT(updateActions()));
    d->activityTimer->setSingleShot(true);
    d->activityTimer->start(300);

    // show main window timer
    d->visibleTimer = new QTimer(this);
    connect(d->visibleTimer, SIGNAL(timeout()),this, SLOT(showMainWindow()));
    d->visibleTimer->setSingleShot(true);

    d->windowMapper = new QSignalMapper(this);

    // connection between workspace, window menu and tab bar
    connect( d->windowMapper, SIGNAL( mapped(QWidget *) ), d->workspace, SLOT( setActiveWindow( QWidget* ) ) );
    connect( d->workspace, SIGNAL( windowActivated ( QWidget * ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
    connect( d->tabs, SIGNAL( currentChanged( int) ), this, SLOT( onTabSelected(int) ) );

    DockWindowManager* pDockMgr = DockWindowManager::instance();

    // Show all dockable windows over the workbench facility
    //
    // Toolbox
 /*   ToolBox* toolBox = new ToolBox(this);
    toolBox->setObjectName(QT_TRANSLATE_NOOP("QDockWidget","Toolbox"));
    pDockMgr->registerDockWindow("Std_ToolBox", toolBox);
    ToolBoxManager::getInstance()->setToolBox( toolBox );
*/

    // Help View
    //QString home = Gui::Dialog::DlgOnlineHelpImp::getStartpage();
    //HelpView* pcHelpView = new HelpView( home, this );
    //pDockMgr->registerDockWindow("Std_HelpView", pcHelpView);

    // Tree view
    TreeDockWidget* tree = new TreeDockWidget(0, this);
    tree->setObjectName
        (QString::fromAscii(QT_TRANSLATE_NOOP("QDockWidget","Tree view")));
    tree->setMinimumWidth(210);
    pDockMgr->registerDockWindow("Std_TreeView", tree);

    // Property view
    PropertyView* pcPropView = new PropertyView(0, this);
    pcPropView->setObjectName
        (QString::fromAscii(QT_TRANSLATE_NOOP("QDockWidget","Property view")));
    pcPropView->setMinimumWidth(210);
    pDockMgr->registerDockWindow("Std_PropertyView", pcPropView);

    // Selection view
    SelectionView* pcSelectionView = new SelectionView(0, this);
    pcSelectionView->setObjectName
        (QString::fromAscii(QT_TRANSLATE_NOOP("QDockWidget","Selection view")));
    pcSelectionView->setMinimumWidth(210);
    pDockMgr->registerDockWindow("Std_SelectionView", pcSelectionView);

    // Report view
    Gui::DockWnd::ReportView* pcReport = new Gui::DockWnd::ReportView(this);
    pcReport->setObjectName
        (QString::fromAscii(QT_TRANSLATE_NOOP("QDockWidget","Report view")));
    pDockMgr->registerDockWindow("Std_ReportView", pcReport);

    // FIXME: Break into two dock windows in tabbed mode (with icon and correct tab width)
    //QWidget* pcPython = new PythonConsole(this);
    //pcPython->setObjectName(QT_TRANSLATE_NOOP("QDockWidget","Python view"));
    //pDockMgr->registerDockWindow("Std_PythonView", pcPython);

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

void MainWindow::activateWorkbench(const QString& name)
{
    // emit this signal
    workbenchActivated(name);
}

void MainWindow::whatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::showDocumentation(const char* Article)
{
#ifdef FC_OS_WIN32
	std::wstring wstr = L"mk:@MSITStore:" ;
    wstr += Base::FileInfo(App::Application::Config()["AppHomePath"] + "doc/FreeCAD.chm").toStdWString();
	if(Article){
		wstr += L"::/index.php@title="; 
		std::string s = Article;
		std::wstring ws( s.begin(), s.end() );
		wstr += ws;
	}

    bool ok = (reinterpret_cast<int>(ShellExecuteW(NULL, L"open", L"hh.exe",wstr.c_str(), NULL, SW_SHOWNORMAL)) > 32);

#else
    // Use Qt Assistant...
	bool ok = false;
#endif
   if (!ok) {
        QMessageBox::critical(getMainWindow(), QObject::tr("File not found"),
            QObject::tr("Cannot open help file"));
   }

}

bool MainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::EnterWhatsThisMode) {
        // Unfortunately, for top-level widgets such as menus or dialogs we
        // won't be notified when the user clicks the link in the hypertext of
        // the what's this text. Thus, we have to install the main window to
        // the application to observe what happens in eventFilter().
        d->whatstext.clear();
        if (!d->whatsthis) {
            d-> whatsthis = true;
            qApp->installEventFilter(this);
        }
    }
    if (e->type() == QEvent::LeaveWhatsThisMode) {
        // Here we can't do anything because this event is sent
        // before the WhatThisClicked event is sent. Thus, we handle
        // this in eventFilter().
    }

    if (e->type() == QEvent::WhatsThisClicked) {
        QWhatsThisClickedEvent* wt = static_cast<QWhatsThisClickedEvent*>(e);
        // use assistant class later on
        //d->assistant->showDocumentation(wt->href());
        showDocumentation((const char*)wt->href().toUtf8());
    }
    return QMainWindow::event(e);
}

bool MainWindow::eventFilter(QObject* o, QEvent* e)
{
    if (o != this) {
        if (e->type() == QEvent::WindowStateChange) {
            // notify all mdi views when the active view receives a show normal, show minimized 
            // or show maximized event 
            MDIView * view = qobject_cast<MDIView*>(o);
            if (view) { // emit this signal
                Qt::WindowStates oldstate = static_cast<QWindowStateChangeEvent*>(e)->oldState();
                Qt::WindowStates newstate = view->windowState();
                if (oldstate != newstate)
                    windowStateChanged(view);
            }
        }

        // We don't want to show the bubble help for the what's this text but want to
        // start the help viewer with the according key word.
        // Thus, we have to observe WhatThis events if called for a widget, use its text and
        // must avoid to make the bubble widget visible.
        if (e->type() == QEvent::WhatsThis) {
            if (!o->isWidgetType())
                return false;
            // clicked on a widget in what's this mode
            QWidget * w = static_cast<QWidget *>(o);
            d->whatstext = w->whatsThis();
        }
        if (e->type() == QEvent::WhatsThisClicked) {
            // if the widget is a top-level window
            if (o->isWidgetType() && qobject_cast<QWidget*>(o)->isWindow()) {
                // re-direct to the widget
                QApplication::sendEvent(this, e);
            }
        }
        // special treatment for menus because they directly call QWhatsThis::showText()
        // whereby we must be informed for which action the help should be shown
        if (o->inherits("QMenu") && QWhatsThis::inWhatsThisMode()) {
            bool whatthis = false;
            if (e->type() == QEvent::KeyPress) {
                QKeyEvent* ke = static_cast<QKeyEvent*>(e);
                if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_F1)
                    whatthis = true;
            }
            else if (e->type() == QEvent::MouseButtonRelease)
                whatthis = true;
            else if (e->type() == QEvent::EnterWhatsThisMode)
                whatthis = true;
            if (whatthis) {
                QAction* cur = static_cast<QMenu*>(o)->activeAction();
                if (cur) {
                    // get the help text for later usage
                    QString s = cur->whatsThis();
                    if (s.isEmpty())
                        s = static_cast<QMenu*>(o)->whatsThis();
                    d->whatstext = s;
                }
            }
        }
        if (o->inherits("QWhatsThat") && e->type() == QEvent::Show) {
            // the bubble help should become visible which we avoid by marking the widget
            // that it is out of range. Instead of, we show the help viewer
            if (!d->whatstext.isEmpty()) {
                QWhatsThisClickedEvent e(d->whatstext);
                QApplication::sendEvent(this, &e);
            }
            static_cast<QWidget *>(o)->setAttribute(Qt::WA_OutsideWSRange);
            return true;
        }
        if (o->inherits("QWhatsThat") && e->type() == QEvent::Hide) {
            // leave what's this mode
            if (d->whatsthis) {
                d->whatsthis = false;
                d->whatstext.clear();
                qApp->removeEventFilter(this);
            }
        }
    }

    return QMainWindow::eventFilter(o, e);
}

void MainWindow::addWindow(MDIView* view)
{
    // make workspace parent of view
    d->workspace->addWindow(view);
    connect( view, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( showMessage(const QString&, int )) );
    connect( this, SIGNAL( windowStateChanged(MDIView*) ), view, SLOT( windowStateChanged(MDIView*) ) );

    // listen to the incoming events of the view
    view->installEventFilter(this);

    // show the very first window in maximized mode
    if (d->workspace->windowList().isEmpty())
        view->showMaximized();
    else
        view->show();

    // look if the window was already inserted
    for (int i=0; i < d->tabs->count(); i++) {
        if (d->tabs->tabData(i).value<QWidget*>() == view)
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

void MainWindow::removeWindow(Gui::MDIView* view)
{
    // free all connections
    disconnect(view, SIGNAL(message(const QString&, int)),
               statusBar(), SLOT(message(const QString&, int )));
    disconnect(this, SIGNAL(windowStateChanged(MDIView*)),
               view, SLOT(windowStateChanged(MDIView*)));
    view->removeEventFilter(this);

    for (int i = 0; i < d->tabs->count(); i++) {
        if (d->tabs->tabData(i).value<QWidget*>() == view) {
            d->tabs->removeTab(i);
            if (d->tabs->count() == 0)
                d->tabs->hide(); // no view open any more
            break;
        }
    }

    // check if the focus widget is a child of the view
    QWidget* foc = this->focusWidget();
    if (foc) {
        QWidget* par = foc->parentWidget();
        while (par) {
            if (par == view) {
                foc->clearFocus();
                break;
            }
            par = par->parentWidget();
        }
    }

    // this view is not under control of the main window any more
    disconnect( view, SIGNAL( destroyed() ), this, SLOT( onWindowDestroyed() ) );
}

void MainWindow::tabChanged(MDIView* view)
{
    for (int i = 0; i < d->tabs->count(); i++) {
        if (d->tabs->tabData(i).value<QWidget*>() == view) {
            QString cap = view->windowTitle();
            int lastIndex = cap.lastIndexOf(QString::fromAscii("[*]"));
            if (lastIndex > 0) {
                cap = cap.left(lastIndex);
                if (view->isWindowModified())
                    cap = QString::fromAscii("%1*").arg(cap);
            }
            d->tabs->setTabToolTip(i, cap);
            
            // remove path separators
            int pos = cap.lastIndexOf(QLatin1Char('/'));
            cap = cap.mid( pos+1 );
            pos = cap.lastIndexOf(QLatin1Char('\\'));
            cap = cap.mid( pos+1 );

            d->tabs->setTabText(i, cap);
            break;
        }
    }
}

void MainWindow::onWindowDestroyed()
{
    QObject* view = (QObject*)sender();
    for (int i = 0; i < d->tabs->count(); i++) {
        if (d->tabs->tabData(i).value<QWidget*>() == view) {
            d->tabs->removeTab(i);
            if (d->tabs->count() == 0)
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
    d->activeView = view;
    Application::Instance->viewActivated(view);

    // set the appropriate tab to the new active window
    for (int i = 0; i < d->tabs->count(); i++) {
        if (d->tabs->tabData(i).value<QWidget*>() == view) {
            d->tabs->blockSignals(true);
            d->tabs->setCurrentIndex(i);
            d->tabs->blockSignals(false);
            break;
        }
    }
}

void MainWindow::onTabSelected( int i)
{
    QVariant var = d->tabs->tabData(i);
    if (var.isValid() && var.canConvert<QWidget*>()) {
        QWidget* view = var.value<QWidget*>();
        if (view && !view->hasFocus())
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

    // do the connection only once
    static bool firstShow = true;
    if (firstShow) {
        firstShow = false;
        QAction* last = actions.isEmpty() ? 0 : actions.last();
        for (QList<QAction*>::Iterator it = actions.begin(); it != actions.end(); ++it) {
            if (*it == last)
                break; // this is a separator
            connect(*it, SIGNAL(triggered()), d->windowMapper, SLOT(map()));
        }
    }

    int numWindows = std::min<int>(actions.count()-1, windows.count());
    for (int index = 0; index < numWindows; index++) {
        QWidget* child = windows.at(index);
        QAction* action = actions.at(index);
        QString text;
        QString title = child->windowTitle();
        int lastIndex = title.lastIndexOf(QString::fromAscii("[*]"));
        if (lastIndex > 0) {
            title = title.left(lastIndex);
            if (child->isWindowModified())
                title = QString::fromAscii("%1*").arg(title);
        }
        if (index < 9)
            text = QString::fromAscii("&%1 %2").arg( index+1 ).arg(title);
        else
            text = QString::fromAscii("%1 %2").arg( index+1 ).arg(title);
        action->setText(text);
        action->setVisible(true);
        action->setChecked(child == active);
        d->windowMapper->setMapping(action, child);
    }

    // if less windows than actions
    for (int index = numWindows; index < actions.count(); index++)
        actions[index]->setVisible(false);
    // show the separator
    if (numWindows > 0)
        actions.last()->setVisible(true);
}

void MainWindow::onToolBarMenuAboutToShow()
{
    QMenu* menu = static_cast<QMenu*>(sender());
    menu->clear();
    QList<QToolBar*> dock = this->findChildren<QToolBar*>();
    for (QList<QToolBar*>::Iterator it = dock.begin(); it != dock.end(); ++it) {
        if ((*it)->parentWidget() == this) {
            QAction* action = (*it)->toggleViewAction();
            action->setToolTip(tr("Toogles this toolbar"));
            action->setStatusTip(tr("Toogles this toolbar"));
            action->setWhatsThis(tr("Toogles this toolbar"));
            menu->addAction(action);
        }
    }
}

void MainWindow::onDockWindowMenuAboutToShow()
{
    QMenu* menu = static_cast<QMenu*>(sender());
    menu->clear();
    QList<QDockWidget*> dock = this->findChildren<QDockWidget*>();
    for (QList<QDockWidget*>::Iterator it = dock.begin(); it != dock.end(); ++it) {
        QAction* action = (*it)->toggleViewAction();
        action->setToolTip(tr("Toogles this dockable window"));
        action->setStatusTip(tr("Toogles this dockable window"));
        action->setWhatsThis(tr("Toogles this dockable window"));
        menu->addAction(action);
    }
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
    }
    else if (i==2) {
        d->sizeLabel->setText(text);
    }
}

MDIView* MainWindow::activeWindow(void) const
{
    // each activated window notifies this main window when it is activated
    return d->activeView;
}

void MainWindow::setActiveWindow(MDIView* view)
{
    d->workspace->setActiveWindow(view);
    d->activeView = view;
    Application::Instance->viewActivated(view);
}

void MainWindow::closeEvent ( QCloseEvent * e )
{
    Application::Instance->tryClose(e);
    if (e->isAccepted()) {
        // Send close event to all non-modal dialogs
        QList<QDialog*> dialogs = this->findChildren<QDialog*>();
        for (QList<QDialog*>::iterator it = dialogs.begin(); it != dialogs.end(); ++it)
            (*it)->close();
        d->activityTimer->stop();
        saveWindowSettings();
        delete d->assistant;
        d->assistant = 0;
        QMainWindow::closeEvent( e );
    }
}

void MainWindow::showEvent(QShowEvent  * /*e*/)
{
    // needed for logging
    std::clog << "Show main window" << std::endl;
    d->visibleTimer->start(15000);
}

void MainWindow::hideEvent(QHideEvent  * /*e*/)
{
    // needed for logging
    std::clog << "Hide main window" << std::endl;
    d->visibleTimer->stop();
}

void MainWindow::showMainWindow()
{
    // Under certain circumstances it can happen that at startup the main window
    // appears for a short moment and disappears immediately. The workaround
    // starts a timer to check for the visibility of the main window and call
    // ShowWindow() if needed.
    // So far, this phenomena only appeared with Qt4.1.4
#if defined(Q_WS_WIN) && (QT_VERSION == 0x040104)
    WId id = this->winId();
    ShowWindow(id, SW_SHOW);
    std::cout << "Force to show main window" << std::endl;
#endif
}

void MainWindow::appendRecentFile(const QString& filename)
{
    RecentFilesAction *recent = this->findChild<RecentFilesAction *>
        (QString::fromAscii("recentFiles"));
    if (recent) {
        recent->appendFile(filename);
    }
}

void MainWindow::updateActions()
{
    static QTime cLastCall;

    if (cLastCall.elapsed() > 250 && isVisible()) {
        Application::Instance->commandManager().testActive();
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
    // Search for all top-level MDI views
    QWidgetList toplevel = QApplication::topLevelWidgets();
    for (QWidgetList::Iterator it = toplevel.begin(); it != toplevel.end(); ++it) {
        Gui::MDIView* view = qobject_cast<MDIView*>(*it);
        if (view)
            view->setCurrentViewMode(MDIView::Child);
    }
}

void MainWindow::loadWindowSettings()
{
    QString vendor = QString::fromAscii(App::Application::Config()["ExeVendor"].c_str());
    QString application = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
    QString version = QString::fromAscii(App::Application::Config()["ExeVersion"].c_str());
    int major = (QT_VERSION >> 0x10) & 0xff;
    int minor = (QT_VERSION >> 0x08) & 0xff;
    QString qtver = QString::fromAscii("Qt%1.%2").arg(major).arg(minor);
    QSettings config(vendor, application);

    config.beginGroup(version);
    config.beginGroup(qtver);
    this->resize(config.value(QString::fromAscii("Size"), this->size()).toSize());
    QPoint pos = config.value(QString::fromAscii("Position"), this->pos()).toPoint();
    QRect rect = QApplication::desktop()->availableGeometry();
    int x1,x2,y1,y2;
    // make sure that the main window is not totally out of the visible rectangle
    rect.getCoords(&x1, &y1, &x2, &y2);
    pos.setX(qMin(qMax(pos.x(),x1-this->width()+30),x2-30));
    pos.setY(qMin(qMax(pos.y(),y1-10),y2-10));
    this->move(pos);

    // tmp. disable the report window to suppress some bothering warnings
    Base::Console().SetEnabledMsgType("ReportOutput", ConsoleMsgType::MsgType_Wrn, false);
    this->restoreState(config.value(QString::fromAscii("MainWindowState")).toByteArray());
    std::clog << "Main window restored" << std::endl;
    Base::Console().SetEnabledMsgType("ReportOutput", ConsoleMsgType::MsgType_Wrn, true);

    bool max = config.value(QString::fromAscii("Maximized"), false).toBool();
    max ? showMaximized() : show();
    config.endGroup();
    config.endGroup();

    ToolBarManager::getInstance()->restoreState();
    std::clog << "Toolbars restored" << std::endl;
}

void MainWindow::saveWindowSettings()
{
    QString vendor = QString::fromAscii(App::Application::Config()["ExeVendor"].c_str());
    QString application = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
    QString version = QString::fromAscii(App::Application::Config()["ExeVersion"].c_str());
    int major = (QT_VERSION >> 0x10) & 0xff;
    int minor = (QT_VERSION >> 0x08) & 0xff;
    QString qtver = QString::fromAscii("Qt%1.%2").arg(major).arg(minor);
    QSettings config(vendor, application);

    config.beginGroup(version);
    config.beginGroup(qtver);
    config.setValue(QString::fromAscii("Size"), this->size());
    config.setValue(QString::fromAscii("Position"), this->pos());
    config.setValue(QString::fromAscii("Maximized"), this->isMaximized());
    config.setValue(QString::fromAscii("MainWindowState"), this->saveState());
    config.endGroup();
    config.endGroup();

    DockWindowManager::instance()->saveState();
    ToolBarManager::getInstance()->saveState();
}

void MainWindow::startSplasher(void)
{
    // startup splasher
    // when running in verbose mode no splasher
    if (!(App::Application::Config()["Verbose"] == "Strict") && 
         (App::Application::Config()["RunMode"] == "Gui")) {
        ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().
            GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");
        // first search for an external imahe file
        if (hGrp->GetBool("ShowSplasher", true)) {
            d->splashscreen = new SplashScreen(this->splashImage());
            d->splashscreen->show();
        }
        else
            d->splashscreen = 0;
    }
}

void MainWindow::stopSplasher(void)
{
    if (d->splashscreen) {
        d->splashscreen->finish(this);
        delete d->splashscreen;
        d->splashscreen = 0;
    }
}

QPixmap MainWindow::splashImage() const
{
    QPixmap splash_image;
    QDir dir(QString::fromUtf8(App::Application::Config()["UserAppData"].c_str()));
    QFileInfo fi(dir.filePath(QString::fromAscii("pixmaps/splash_image.png")));
    if (fi.isFile() && fi.exists())
        splash_image.load(fi.filePath(), "PNG");
    if (splash_image.isNull())
        splash_image = Gui::BitmapFactory().pixmap(App::Application::Config()["SplashPicture"].c_str());
    return splash_image;
}

void MainWindow::showTipOfTheDay( bool force )
{
    // tip of the day?
    ParameterGrp::handle
    hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->
            GetGroup("Preferences")->GetGroup("General");

    const std::map<std::string,std::string>& config = App::Application::Config();
    std::map<std::string, std::string>::const_iterator tp = config.find("HideTipOfTheDay");
    bool tip = (tp == config.end());

    tip = hGrp->GetBool("Tipoftheday", tip);
    if (tip || force) {
        Gui::Dialog::DlgTipOfTheDayImp dlg(instance);
        dlg.exec();
    }
}

/**
 * Drops the event \a e and tries to open the files.
 */
void MainWindow::dropEvent (QDropEvent* e)
{
    const QMimeData* data = e->mimeData();
    if (data->hasUrls()) {
        QList<QUrl> uri = data->urls();
        QStringList files;
        App::Document* pDoc = 0;

        // get the files which we support
        for (QList<QUrl>::ConstIterator it = uri.begin(); it != uri.end(); ++it) {
            QFileInfo info((*it).toLocalFile());
            if (info.exists() && info.isFile()) {
                if (info.isSymLink())
                    info.setFile(info.readLink());
                if (App::GetApplication().getImportType(info.completeSuffix().toAscii()) ||
                    App::GetApplication().getImportType(info.suffix().toAscii())) {
                    // ok, we support files with this extension
                    files << info.absoluteFilePath();
                    // we load non-project files, i.e. we must create a new document
                    if (!pDoc && info.suffix().toLower() != QLatin1String("fcstd"))
                        pDoc = App::GetApplication().newDocument();
                }
            }
        }

        const char *docName = pDoc ? pDoc->getName() : "";
        for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
            Application::Instance->importFrom(it->toUtf8(), docName);
        }
    }
    else {
        QMainWindow::dropEvent(e);
    }
}

void MainWindow::dragEnterEvent ( QDragEnterEvent * e )
{
    // Here we must allow uri drafs and check them in dropEvent
    const QMimeData* data = e->mimeData();
    if (data->hasUrls())
        e->accept();
    else
        e->ignore();
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        d->sizeLabel->setText(tr("Dimension"));
    
        CommandManager& rclMan = Application::Instance->commandManager();
        std::vector<Command*> cmd = rclMan.getAllCommands();
        for (std::vector<Command*>::iterator it = cmd.begin(); it != cmd.end(); ++it)
            (*it)->languageChange();

        // reload current workbench to retranslate all actions and window titles
        Workbench* wb = WorkbenchManager::instance()->active();
        if (wb) wb->retranslate();
    }
    else {
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
     enum Type {Msg, Wrn, Err, Log};
    CustomMessageEvent(Type t, const QString& s)
      : QEvent(QEvent::User), _type(t), msg(s)
    { }
    ~CustomMessageEvent()
    { }
    Type type() const
    { return _type; }
    const QString& message() const
    { return msg; }
private:
    Type _type;
    QString msg;
};
}

void MainWindow::customEvent( QEvent* e )
{
    if (e->type() == QEvent::User) {
        Gui::CustomMessageEvent* ce = (Gui::CustomMessageEvent*)e;
        QString msg = ce->message();
        if (ce->type() == CustomMessageEvent::Log) {
            if (msg.startsWith(QLatin1String("#Inventor V2.1 ascii "))) {
                Gui::Document *d = Application::Instance->activeDocument();
                if (d) {
                    ViewProviderExtern *view = new ViewProviderExtern();
                    try {
                        view->setModeByString("1",msg.toAscii().constData());
                        d->setAnnotationViewProvider("Vdbg",view);
                    }
                    catch (...) {
                        delete view;
                    }
                }
            }
        }
        else {
            d->actionLabel->setText(msg);
            d->actionTimer->setSingleShot(true);
            d->actionTimer->start(5000);
        }
    }
}

// ----------------------------------------------------------

StatusBarObserver::StatusBarObserver()
  : WindowParameter("OutputWindow")
{
    msg = QString::fromAscii("#000000"); // black
    wrn = QString::fromAscii("#ffaa00"); // orange
    err = QString::fromAscii("#ff0000"); // red
    Base::Console().AttachObserver(this);
    getWindowParameter()->Attach(this);
    getWindowParameter()->NotifyAll();
}

StatusBarObserver::~StatusBarObserver()
{
    getWindowParameter()->Detach(this);
    Base::Console().DetachObserver(this);
}

void StatusBarObserver::OnChange(Base::Subject<const char*> &rCaller, const char * sReason)
{
    ParameterGrp& rclGrp = ((ParameterGrp&)rCaller);
    if (strcmp(sReason, "colorText") == 0) {
        unsigned long col = rclGrp.GetUnsigned( sReason );
        this->msg = QColor((col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff).name();
    }
    else if (strcmp(sReason, "colorWarning") == 0) {
        unsigned long col = rclGrp.GetUnsigned( sReason );
        this->wrn = QColor((col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff).name();
    }
    else if (strcmp(sReason, "colorError") == 0) {
        unsigned long col = rclGrp.GetUnsigned( sReason );
        this->err = QColor((col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff).name();
    }
}

/** Get called when a message is issued. 
 * The message is displayed on the ststus bar. 
 */
void StatusBarObserver::Message(const char * m)
{
    // Send the event to the main window to allow thread-safety. Qt will delete it when done.
    QString txt = QString::fromAscii("<font color=\"%1\">%2</font>").arg(this->msg).arg(QString::fromUtf8(m));
    CustomMessageEvent* ev = new CustomMessageEvent(CustomMessageEvent::Msg, txt);
    QApplication::postEvent(getMainWindow(), ev);
}

/** Get called when a warning is issued. 
 * The message is displayed on the ststus bar. 
 */
void StatusBarObserver::Warning(const char *m)
{
    // Send the event to the main window to allow thread-safety. Qt will delete it when done.
    QString txt = QString::fromAscii("<font color=\"%1\">%2</font>").arg(this->wrn).arg(QString::fromUtf8(m));
    CustomMessageEvent* ev = new CustomMessageEvent(CustomMessageEvent::Wrn, txt);
    QApplication::postEvent(getMainWindow(), ev);
}

/** Get called when an error is issued. 
 * The message is displayed on the ststus bar. 
 */
void StatusBarObserver::Error  (const char *m)
{
    // Send the event to the main window to allow thread-safety. Qt will delete it when done.
    QString txt = QString::fromAscii("<font color=\"%1\">%2</font>").arg(this->err).arg(QString::fromUtf8(m));
    CustomMessageEvent* ev = new CustomMessageEvent(CustomMessageEvent::Err, txt);
    QApplication::postEvent(getMainWindow(), ev);
}

/** Get called when a log message is issued. 
 * The message is used to create an Inventor node for debug purposes. 
 */
void StatusBarObserver::Log(const char *m)
{
    // Send the event to the main window to allow thread-safety. Qt will delete it when done.
    CustomMessageEvent* ev = new CustomMessageEvent(CustomMessageEvent::Log, QString::fromUtf8(m));
    QApplication::postEvent(getMainWindow(), ev);
}


#include "moc_MainWindow.cpp"
