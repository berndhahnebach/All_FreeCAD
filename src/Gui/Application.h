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


#include <string>
#include <vector>

#define  putpix()

#include <App/Application.h>

class QCloseEvent;

namespace Gui{
class BaseView;
class CommandManager;
class Document;
class MacroManager;
class MDIView;
class MainWindow;
class MenuItem;

/** transport the changes of the Application
 *  This class transport closer information what was changed in the
 *  application. 
 *@see Application
 *@see Observer
*/
class GuiExport AppChanges
{
public:
  enum {
    New,
    Del
  } Why;
  Gui::Document* Doc;
};

/** The Applcation main class
 * This is the central class of the GUI 
 * @author Jürgen Riegel, Werner Mayer
 */
class GuiExport Application :public App::Application::ObserverType, public Base::Subject<const AppChanges&>
{
public:
  /// construction
  Application();
  /// destruction
  ~Application();

  /** @name methods for support of files */
  //@{
  /// open a file
  void open(const char* FileName);
  /// import a file into the document DocName
  void import(const char* FileName, const char* DocName);
  //@}


  /** @name methods for View handling */
  //@{
  /// send Messages to the active view
  bool sendMsgToActiveView(const char* pMsg, const char** ppReturn=0);
  /// send Messages test to the active view
  bool sendHasMsgToActiveView(const char* pMsg);
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

  /** @name methods for Document handling */
  //@{
  /// Observer message from the Application
  virtual void OnChange(App::Application::SubjectType &rCaller,App::Application::MessageType Reason);
  void OnDocNew(App::Document* pcDoc);
  void OnDocDelete(App::Document* pcDoc);
  /// message when a GuiDocument is about to vanish
  void onLastWindowClosed(Gui::Document* pcDoc);
  /// Getter for the active document
  Gui::Document* activeDocument(void) const;
  /// Set the active document
  void setActiveDocument(Gui::Document* pcDocument);
  /** Retrieves a pointer to the Gui::Document whose App::Document has the name \a name.
   * If no such document exists 0 is returned.
   */
  Gui::Document* getDocument(const char* name) const;
  /** Retrieves a pointer to the Gui::Document whose App::Document matches to \a pDoc.
   * If no such document exists 0 is returned.
   */
  Gui::Document* getDocument(App::Document* pDoc) const;
  //@}

  /// true when the application shuting down
  bool isClosing(void);

  /** @name workbench handling */
  //@{	
  /// Activate a named workbench
  bool activateWorkbench( const char* name );
  void refreshWorkbenchList();
  QPixmap workbenchIcon( const QString& ) const;
  QStringList workbenches(void) const;
  void setupContextMenu(const char* recipient, MenuItem*) const;
  //@}

  /** @name User Commands */
  //@{
  /// Get macro manager
  Gui::MacroManager *macroManager(void);
  /// Reference to the command manager
  Gui::CommandManager &commandManager(void);
  /// Run a Python command
  void runCommand(bool bForce, const char* sCmd,...);
  /// helper which create the commands
  void createStandardOperations();
  //@}

  /** @name Init, Destruct an Access methods */
  //@{
  /// some kind of singelton
  static Application* Instance;
  static void initApplication(void);
  static void initTypes(void);
  static void runApplication(void);
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
  PYFUNCDEF_S(sAddIconPath);             // adds a path to an icon file

  PYFUNCDEF_S(sSendActiveView);

  PYFUNCDEF_S(sUpdateGui);
  PYFUNCDEF_S(sCreateDialog);

  PYFUNCDEF_S(sRunCommand);
  PYFUNCDEF_S(sAddCommand);

  PYFUNCDEF_S(shide);
  PYFUNCDEF_S(sshow);
  
  PYFUNCDEF_S(sopen);                     // open Python scripts
  PYFUNCDEF_S(sinsert);                   // open Python scripts

  PYFUNCDEF_S(sdocument);                 // deprecated (use activeDocument or getDocument(<string>) instead)
  PYFUNCDEF_S(sActiveDocument);
  PYFUNCDEF_S(sGetDocument);

  static PyMethodDef    Methods[]; 

private:
  struct ApplicationP* d;
  /// workbench python dictionary
  PyObject*		 _pcWorkbenchDictionary;
};

} //namespace Gui

#endif
