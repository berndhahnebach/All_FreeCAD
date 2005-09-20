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

namespace Gui{
class BaseView;
class CommandManager;
class Document;
class MacroManager;
class MDIView;
class MainWindow;

/** The Applcation main class
 * This is the central class of the GUI 
 * @author Jürgen Riegel, Werner Mayer
 */
class GuiExport Application : public App::ApplicationObserver
{
public:
  /// construction
  Application();
  /// destruction
  ~Application();

  /// open a file
  void open(const char* FileName);
  /// import a file in the active document
  void import(const char* FileName);

  // Observer
  void OnDocNew(App::Document* pcDoc);
  void OnDocDelete(App::Document* pcDoc);

  /// message when a GuiDocument is about to vanish
  void onLastWindowClosed(Gui::Document* pcDoc);

  /** @name methodes for View handling */
  //@{
  /// send Messages to the active view
  bool sendMsgToActiveView(const char* pMsg, const char** ppReturn=0);
  /// send Messages test to the active view
  bool sendHasMsgToActiveView(const char* pMsg);
  /// Getter for the active document
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
  //@}

  /// Set the active document
  void setActiveDocument(Gui::Document* pcDocument);

  /// true when the application shuting down
  bool isClosing(void);

  /// Reference to the command manager
  Gui::CommandManager &commandManager(void);

  /** @name workbench handling */
  //@{	
  /// Activate a named workbench
  bool activateWorkbench( const char* name );
  void refreshWorkbenchList();
  QPixmap workbenchIcon( const QString& ) const;
  QStringList workbenches(void);
  //@}

  /// Get macro manager
  Gui::MacroManager *macroManager(void);

  /// helper which create the commands
  void createStandardOperations();

  /** @name Init, Destruct an Access methodes */
  //@{
  /// some kind of singelton
  static Application* Instance;
  static void initApplication(void);
  static void runApplication(void);
  static void destruct(void);
  void tryClose( QCloseEvent * e );
  //@}

public:
  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  // static python wrapper of the exported functions
  PYFUNCDEF_S(sAddWorkbenchHandler);     // adds a new workbench handler to a list
  PYFUNCDEF_S(sRemoveWorkbenchHandler);  // removes a workbench handler from the list
  PYFUNCDEF_S(sGetWorkbenchHandler);     // retrieves the workbench handler
  PYFUNCDEF_S(sListWorkbenchHandlers);   // retrieves a list of all workbench handlers
  PYFUNCDEF_S(sCreateWorkbench);         // creates a new workbench object
  PYFUNCDEF_S(sActiveWorkbench);         // retrieves the active workbench object
  PYFUNCDEF_S(sActivateWorkbench);       // activates a workbench object
  PYFUNCDEF_S(sListWorkbenches);         // retrieves a list of all workbench objects
  PYFUNCDEF_S(sGetWorkbench);            // retrieves a workbench object
  PYFUNCDEF_S(sHasWorkbench);            // checks for a workbench object

  PYFUNCDEF_S(sSendActiveView);

  PYFUNCDEF_S(sUpdateGui);
  PYFUNCDEF_S(sCreateDialog);

  PYFUNCDEF_S(sRunCommand);
  PYFUNCDEF_S(sAddCommand);

  PYFUNCDEF_S(shide);
  PYFUNCDEF_S(sshow);

  static PyMethodDef    Methods[]; 

private:
  struct ApplicationP* d;
  static  QApplication* _pcQApp ;
  /// workbench python dictionary
  PyObject*		 _pcWorkbenchDictionary;
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
  MessageBoxObserver(MainWindow *pcAppWnd);

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
  MainWindow* _pcAppWnd;
};

} //namespace Gui

#endif
