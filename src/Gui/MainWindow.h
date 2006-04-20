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


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <string>
#include <vector>
#include <qstringlist.h>
#include <qmainwindow.h>
#include <qworkspace.h>

class QComboBox;
class QToolBar;
class QPopupMenu;
class QToolBar;
class QSplashScreen;

namespace Gui {

class TreeView;
class BaseView;
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
   * Removes an MDI window from the main window's workspace and its associated tab without
   * deleting the widget. If the main windows does not have such a window nothing happens.
   */
  void removeWindow( MDIView* view );
  /**
   * Can be called after the caption of an MDIView has changed to update the tab's caption.
   */
  void tabChanged( MDIView* view );
  /**
   * Returns the first window with the caption \a caption, if there is no window with this caption 0 is returned. 
   */
  MDIView* getWindowWithCaption( const QString& ) const;
  /**
   * Returns a list of all MDI windows in the worpspace.
   */
  QWidgetList windows( QWorkspace::WindowOrder order = QWorkspace::CreationOrder ) const;
  /**
   * Returns the active MDI window or 0 if there is none.
   */
  MDIView* activeWindow();
  /** Calls update to the pixmaps' size.
   */
  void updatePixmapsSize(void);
  /** Calls update to style.
   */
  void updateStyle(void);
  /**
   * Sets text to the pane in the status bar.
   */
  void setPaneText(int i, QString text);
  /**
   * MRU: Appends \a file to the list of recent files.
   */
  void appendRecentFile(const char* file);
  /**
   * Returns true that the context menu contains the 'Customize...' menu item.
   */
  bool isCustomizable () const;
  /**
   * Opens a dialog to customize the main window.
   */
  void customize ();

  /** @name Splasher and access methods */
  //@{
  /** Gets the one and only instance. */
  static MainWindow* getInstance();
  /** Destroys the main window and frees any allocated resources. */
  static void destruct();
  /** Starts the splasher at startup. */
  static void startSplasher(void);
  /** Stops the splasher after startup. */
  static void stopSplasher(void);
  /** Shows the Tip-of-the-day dialog after startup. */
  static void showTipOfTheDay(bool force=false);
  //@}

  TreeView* getTreeView(void){return pcTree;}

public slots:
  /**
   * Arranges all child windows in a horizontal tile pattern.
   */
  void tileHorizontal();
  /**
   * Arranges all child windows in a complex tile pattern.
   */
  void tileComplex();
  /**
   * Arranges all child windows in a tile pattern.
   */
  void tile();
  /**
   * Arranges all the child windows in a cascade pattern.
   */
  void cascade();
  /**
   * Closes the child window that is currently active.
   */
  void closeActiveWindow ();
  /**
   * Closes all child windows. 
   * The windows are closed in random order. The operation stops if a window does not accept the close event.
   */
  void closeAllWindows ();
  /**
   * Activates the next window in the child window chain.
   */
  void activateNextWindow ();
  /**
   * Activates the previous window in the child window chain.
   */
  void activatePrevWindow ();
  /** 
   * This method gets frequently activated and test the commands if they are still active.
   */
  void updateCmdActivity();
 
  /** @name Methods for the undo/redo handling 
   *  This methods are usally used by the GUI document! Its not intended
   *  to use them directly. If the GUI is not up, there is usaly no undo/redo 
   *  necessary.
   */
  //@{
  void onUndo();
  void onRedo();
  //@}

protected slots:
  /**
   * This method is called from the Qt framework automatically whenever a
   * QTranslator object has been installed. This allows to translate all
   * relevant user visible text.
   */
  void languageChange();

protected:
  /**
   * This method checks if the main window can be closed by checking all open documents and views.
   */
  void closeEvent ( QCloseEvent * e );
  void timerEvent( QTimerEvent * e){emit timeEvent();}
  /**
   * Try to interpret dropped elements.
   */
  void dropEvent        ( QDropEvent        * e );
  /**
   * Checks if a mime source object can be interpreted.
   */
  void dragEnterEvent   ( QDragEnterEvent   * e );

  /** @name Layout Methods 
   */
  //@{
  /// Loads the main window settings.
  void loadWindowSettings();
  /// Saves the main window settings.
  void saveWindowSettings();
  /// Loads the dock windows settings.
  void loadDockWndSettings();
  /// Saves the dock windows settings.
  void saveDockWndSettings();
  //@}

private slots:
  /**
   * Activates the associated tab to this widget.
   */
  void onWindowActivated( QWidget* );
  /**
   * Fills up the menu with the current windows in the workspace.
   */
  void onWindowsMenuAboutToShow();
  /**
   * Activates the window with \a id.
   */
  void onWindowsMenuActivated( int id );
  /**
   * Removes the associated tab to the window when it gets destroyed from outside.
   */
  void onWindowDestroyed();
  /**
   * Activates the associated window to the tab with \a id.
   */
  void onTabSelected( int i);

signals:
  void timeEvent();

private:
  /// some kind of singleton
  static MainWindow* instance;
  struct MainWindowP* d;
  static QSplashScreen *_splash;
  TreeView* pcTree;
};

inline GuiExport MainWindow* getMainWindow()
{
  return MainWindow::getInstance();
}

} // namespace Gui

#endif // MAIN_WINDOW_H
