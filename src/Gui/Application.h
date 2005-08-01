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


#ifndef APPLICATION_H
#define APPLICATION_H


#ifndef _PreComp_
# include <string>
# include <vector>
# include <qstringlist.h>
#endif

#define  putpix()

#include "../Base/Console.h"
#include "../App/Application.h"

#include <qmainwindow.h>
#include <qworkspace.h>

class QComboBox;
class QToolBar;
class QPopupMenu;
class QToolBar;
class QSplashScreen;

namespace Gui{
class BaseView;
class CustomWidgetManager;
class CommandManager;
class Document;
class MacroManager;
class MDIView;
namespace DockWnd {
class HelpView;
} //namespace DockWnd

/** The Applcation main class
 * This is the central class of the GUI 
 * @author Jürgen Riegel, Werner Mayer
 */
class GuiExport ApplicationWindow: public QMainWindow, public App::ApplicationObserver
{
    Q_OBJECT
 
public:
  /// construction
  ApplicationWindow();
  /// destruction
  ~ApplicationWindow();

  /// open a file
  void open(const char* FileName);
  /// import a file in the active document
  void import(const char* FileName);


  // Observer
  virtual void OnDocNew(App::Document* pcDoc);
  virtual void OnDocDelete(App::Document* pcDoc);

  void addWindow( Gui::MDIView* view );
  void removeWindow( Gui::MDIView* view );
  QWidgetList windows( QWorkspace::WindowOrder order = QWorkspace::CreationOrder ) const;

  /// message when a GuiDocument is about to vanish
  void onLastWindowClosed(Gui::Document* pcDoc);

  /// some kind of singelton
  static ApplicationWindow* Instance;

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
  QStringList workbenches(void);
  //@}

  /// MRU: recent files
  void appendRecentFile(const char* file);

  /// Get macro manager
  Gui::MacroManager *macroManager(void);

  /// helper which create the commands
  void createStandardOperations();

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


public:
  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------

  // static python wrapper of the exported functions
  PYFUNCDEF_S(sMenuAppendItems); // append items
  PYFUNCDEF_S(sMenuRemoveItems); // remove items
  PYFUNCDEF_S(sMenuDelete);      // delete the whole menu

  PYFUNCDEF_S(sToolbarAppendItems);
  PYFUNCDEF_S(sToolbarRemoveItems);
  PYFUNCDEF_S(sToolbarDelete);

  PYFUNCDEF_S(sCommandbarAppendItems);
  PYFUNCDEF_S(sCommandbarRemoveItems);
  PYFUNCDEF_S(sCommandbarDelete);

  PYFUNCDEF_S(sWorkbenchAdd);
  PYFUNCDEF_S(sWorkbenchDelete);
  PYFUNCDEF_S(sWorkbenchActivate);
  PYFUNCDEF_S(sWorkbenchGet);

  PYFUNCDEF_S(sSendActiveView);

  PYFUNCDEF_S(sUpdateGui);
  PYFUNCDEF_S(sCreateDialog);

  PYFUNCDEF_S(sRunCommand);
  PYFUNCDEF_S(sCommandAdd);

  static PyMethodDef    Methods[]; 
 

signals:
  void sendQuit();
  void timeEvent();

public:
  void onPolish();
  bool isCustomizable () const;
  void customize ();

public slots:
  void tileHorizontal();
  void tile();
  void cascade();
  void closeActiveWindow ();
  void closeAllWindows ();
  void activateNextWindow ();
  void activatePrevWindow ();

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
protected slots:
  virtual void languageChange();

private slots:
  void onShowView();
  void onWindowActivated( QWidget* );
  void onWindowsMenuAboutToShow();
  void onWindowsMenuActivated( int id );
  void onWindowDestroyed();
  void onToggleStatusBar();
  void onTabSelected( int i);

private:
  struct ApplicationWindowP* d;
};



/** The message box observer opens an appropriate dialog for warnings and errors
 * and writes the text also to the status bar of the main window. Normal text message
 * are written only to the status bar and log messages are ignored completely.
 * @see Console
 * @see ConsoleObserver
 * @author Jürgen Riegel
 */
class MessageBoxObserver: public Base::ConsoleObserver
{
public:
  MessageBoxObserver(ApplicationWindow *pcAppWnd);

  /// get calles when a Warning is issued
  virtual void Warning(const char *m);
  /// get calles when a Message is issued
  virtual void Message(const char * m);
  /// get calles when a Error is issued
  virtual void Error  (const char *m);
  /// get calles when a Log Message is issued
  virtual void Log    (const char *);
  /// name of the observer
  virtual const char *Name(void){return "MessageBox";}

protected:
  ApplicationWindow* _pcAppWnd;
};

} //namespace Gui

#endif
