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


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#ifndef _PreComp_
# include <string>
# include <vector>
# include <qmainwindow.h>
# include <qworkspace.h>
#endif

#include "../Base/Console.h"
#include "../App/Application.h"

class QComboBox;
class QToolBar;
class QPopupMenu;
class QToolBar;
class QSplashScreen;

namespace Gui {
class BaseView;
class CustomWidgetManager;
class CommandManager;
class Document;
class MacroManager;
class MDIView;
namespace DockWnd {
class HelpView;
} //namespace DockWnd


/** 
 * The MainWindow class provides a main window with menu bar, toolbars, dockable windows,
 * a status bar and mainly a workspace for the MDI windows.
 * @author Werner Mayer
 */
class GuiExport MainWindow: public QMainWindow
{
    Q_OBJECT
 
public:
  /**
   * Constructs an empty main window. For default \a parent is 0, as there usually is
   * no toplevel window there.
   */
  MainWindow( QWidget * parent = 0, const char * name = 0, WFlags f = WType_TopLevel );
  /** Destroys the object and frees any allocated resources. */
  ~MainWindow();

  /** 
   * Adds an MDI window \a view to the main window's workspace and adds a new tab
   * to the tab bar.
   */
  void addWindow( MDIView* view );

  /**
   * Returns a list of all MDI windows in the worpspace.
   */
  QWidgetList windows( QWorkspace::WindowOrder order = QWorkspace::CreationOrder ) const;

protected slots:
  /**
   * This method is called from the Qt framework automatically whenever a
   * QTranslator object has been installed. This allows to translate all
   * relevant user visible text.
   */
  virtual void languageChange();

private:
  void createStandardOperations();

private slots:
  void onShowView();
  void onWindowActivated( QWidget* );
  void onWindowsMenuAboutToShow();
  void onWindowsMenuActivated( int id );
  void onWindowDestroyed();
  void onToggleStatusBar();
  void onTabSelected( int i);

private:
  struct MainWindowP* d;











public:

  /// message when a GuiDocument is about to vanish
  void onLastWindowClosed(Gui::Document* pcDoc);

  /// some kind of singelton
  static MainWindow* Instance;

  /** @name methodes for View handling */
  //@{
  /// send Messages to the active view
  bool sendMsgToActiveView(const char* pMsg, const char** ppReturn=0);
  /// send Messages test to the active view
  bool sendHasMsgToActiveView(const char* pMsg);
  /// returns the active view or NULL
  Gui::MDIView* activeView(void);
  /// Geter for the Active View
  Gui::Document* activeDocument(void);
  /// Attach a view (get called by the FCView constructor)
  void attachView(Gui::BaseView* pcView);
  /// Detach a view (get called by the FCView destructor)
  void detachView(Gui::BaseView* pcView);
  /// get called if a view gets activated, this manage the whole activation scheme
  void viewActivated(Gui::MDIView* pcView);
  /// call update to all docuemnts an all views (costly!)
  void onUpdate(void);
  /// call update to all views of the active document
  void updateActive(void);
  /// call update to the pixmaps' size
  void updatePixmapsSize(void);
  /// call update to style
  void updateStyle(void);
  //@}

  /// Set the active document
  void setActiveDocument(Gui::Document* pcDocument);

  /// true when the application shuting down
  bool isClosing(void);

  /// Reference to the command manager
  Gui::CommandManager &commandManager(void);

  /// Returns the widget manager
  Gui::CustomWidgetManager* customWidgetManager(void);

  /** @name status bar handling */
  //@{	
  /// set text to the pane
  void setPaneText(int i, QString text);
  //@}

  /** @name workbench handling */
  //@{	
  /// Activate a named workbench
  void activateWorkbench(const char* name);
  /// update the combo box when there are changes in the workbenches
  void appendWorkbench(const char* name);
  void removeWorkbench(const char* name);
  /// returns the name of the active workbench
  QString activeWorkbench(void);
  std::vector<std::string> workbenches(void);
  //@}

  /// MRU: recent files
  void appendRecentFile(const char* file);

  /// Get macro manager
  Gui::MacroManager *macroManager(void);


public:
  /** @name Init, Destruct an Access methodes */
  //@{
  static void initApplication(void);
  static void runApplication(void);
  static void startSplasher(void);
  static void stopSplasher(void);
  static void showTipOfTheDay(bool force=false);
  static void destruct(void);

  //@}

private:
  static  QApplication* _pcQApp ;
  static  QSplashScreen *_splash;

signals:
  void sendQuit();
  void timeEvent();

public:
  void onPolish();
  bool isCustomizable () const;
  void customize ();
  void tileHorizontal();
  void tile();
  void cascade();

protected: // Protected methods
  virtual void closeEvent ( QCloseEvent * e );
  /// waiting cursor stuff 
  void timerEvent( QTimerEvent * e){emit timeEvent();}
  void dropEvent        ( QDropEvent        * e );
  void dragEnterEvent   ( QDragEnterEvent   * e );

  // windows stuff
  void loadWindowSettings();
  void saveWindowSettings();
  void loadDockWndSettings();
  void saveDockWndSettings();

public slots:
  /// this slot get frequently activatet and test the commands if they are still active
  void updateCmdActivity();
  /** @name methodes for the UNDO REDO handling 
   *  this methodes are usaly used by the GUI document! Its not intended
   *  to use them directly. If the GUI is not up, there is usaly no UNDO / REDO 
   *  nececary.
   */
  //@{
  void onUndo();
  void onRedo();
  //@}

};


} // namespace Gui

#endif // MAIN_WINDOW_H
