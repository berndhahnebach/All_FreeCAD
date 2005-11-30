/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_COMMAND_H
#define GUI_COMMAND_H

#ifndef _PreComp_
# include <qaction.h>
# include <qstringlist.h>
# include <string>
# include <vector>
#endif

#include "../Base/PyExport.h"

namespace App
{
  class Document;
  class Feature;
}

class QAction;

namespace Gui {

class Application;
class CommandManager;
class Command;
class ActionGroup;
class Document;
class SelectionSingelton;
class MDIView;


void CreateStdCommands(void);
void CreateViewStdCommands(void);
void CreateWindowStdCommands(void);
void CreateTestCommands(void);


/** The CommandBase class
 * This lightweigt class is the base class of all commands in FreeCAD. It represents the link between the FreeCAD
 * command framework and the QAction world of Qt.
 * @author Werner Mayer
 */
class GuiExport CommandBase 
{
public:
  CommandBase( const char* sMenu, const char* sToolTip=0, const char* sWhat=0, 
               const char* sStatus=0, const char* sPixmap=0, int accel=0);
  virtual ~CommandBase();

  /**
   * Returns the QAction object of this command, or 0 if it doesn't exist.
   */
  QAction*  getAction();

  /** @name Methods to override when creating a new command */
  //@{
  /// Creates the used QAction when adding to a widget. The default implementation does nothing.
  virtual QAction * createAction(void);
  /// Reassigns QAction stuff after the language has changed. 
  virtual void languageChange();
  //@}

  /** @name Methods to get the properties of the command */
  //@{
  virtual const char* getMenuText   () const { return sMenuText;    }
  virtual const char* getToolTipText() const { return sToolTipText; }
  virtual const char* getStatusTip  () const { return sStatusTip;   }
  virtual const char* getWhatsThis  () const { return sWhatsThis;   }
  virtual const char* getPixmap     () const { return sPixmap;      }
  virtual int         getAccel      () const { return iAccel;       }
  //@}

protected:
  /** @name Attributes 
   *  Set by the inherited constructor to set up the most important properties 
   *  of the command. In the constructor are set default values. 
   *  The real values should be set in the constructor of the inheriting class.
   */
  //@{
  const char* sMenuText;
  const char* sToolTipText;
  const char* sWhatsThis;
  const char* sStatusTip;
  const char* sPixmap;
  int         iAccel;
  //@}
protected:
  QAction *_pcAction; /**< The QAction item. */
};

/** The Command class
 * This class is mostly used for commands implemented directly in C++ (@see PythonCommand).
 * It contains also a lot of helper methods to make implementing commands for FreeCAD as easy as possible.
 *
 * @note This class is intended to handle the GUI interaction like:
 * - starting a dialog
 * - doing view and window stuff
 * - anything else, especially altering the document must be done on application level @see doCommand() for details.
 *
 * @see CommandManager
 * @author Jürgen Riegel
 */
class GuiExport Command : public CommandBase
{
public:
  Command( const char* name );
  virtual ~Command();

protected:
  /** @name Methods to override when creating a new command
   */
  //@{
  /// Methods which gets called when activated, needs to be reimplemented!
  virtual void activated(int iMsg)=0;
  /// Overite this methode if your Cmd is not always active
  virtual bool isActive(void){return true;} 
  /// Creates the used QAction
  virtual QAction * createAction(void);
  //@}

public:
  /** @name interface used by the CommandManager and the QAction */
  //@{
  /// CommandManager is a friend
  friend class CommandManager;
  /// Get somtile called to check the state of the command
  void testActive(void);
  /// get called by the QAction
  void invoke (int); 
  /// adds this command to arbitrary widgets
  virtual bool addTo(QWidget *);
  /// removes this command from arbitrary widgets
  virtual bool removeFrom(QWidget *pcWidget);
  //@}


  /** @name Helper methods to get important classes */
  //@{
  /// Get pointer to the Application Window
  static Application*  getGuiApplication(void);   
  /// Get a referenc to the selection 
  Gui::SelectionSingelton&  getSelection(void);
  /// Get pointer to the active gui document
  Gui::Document*  getActiveGuiDocument(void);
  /** Get pointer to the named or active App document
   *  Returns a pointer to the named docuement or the active
   *  document when no name is given. NULL is returnd
   *  when the name does not exist or no document is active!
   */
  App::Document*  getDocument(const char* Name=0);
  /// returns the named feature or the active one from the active document or NULL
  App::Feature* getFeature(const char* Name=0);
  /// Get unique Feature name from the active document 
  std::string getUniqueFeatureName(const char *BaseName);
  //@}

  /** @name Helper methods for the Undo/Redo and Update handling */
  //@{
  /// Open a new Undo transaction on the active document
  void openCommand(const char* sName=0);
  /// Commit the Undo transaction on the active document
  void commitCommand(void);
  /// Abort the Undo transaction on the active document
  void abortCommand(void);
  /// Updates the (active) document (propagate changes)
  void updateActive(void);
  /// Updates the (all or listed) documents (propagate changes)
  void updateAll(std::list<Gui::Document*> cList);
  //@}

  /** @name Helper methods for issuing commands to the Python interpreter */
  //@{
  /// types of application level actions for DoCommand()
  enum DoCmd_Type {
    /// Action alters the document
    Doc,
    /// Action alters only the application
    App,
    /// Action alters only the Gui
    Gui
  };
  /// Run a App level Action 
  static void doCommand(DoCmd_Type eType,const char* sCmd,...);
  /// translate a string to a python string literal (needed e.g. in file names for windows...)
  const std::string strToPython(const char* Str);
  const std::string strToPython(const std::string &Str){return strToPython(Str.c_str());};
  //@}

  /** @name Helper methods to generate help pages */
  //@{
  /// returns the begin of a online help page
  const char * beginCmdHelp(void);
  /// returns the end of a online help page
  const char * endCmdHelp(void);
  /// Get the help URL
  virtual const char* getHelpUrl(void) const { return sHelpUrl; }
  //@}

  /** @name Helper methods for the Active tests */
  //@{
  /// true when there is a document
  bool hasActiveDocument(void);
  /// true when there is a document and a Feature with Name
  bool hasFeature(const char* Name);

  //@}

  /** @name checking of internal state */
  //@{
  /// returns the name to which the command belongs
  const char* getAppModuleName(void) const {return sAppModule;}	
  /// Get the command name
  const char* getName() const { return sName; }
  /// Get the name of the grouping of the command
  const char* getGroupName() const { return sGroup; }
  //@}

protected:
  /** @name Attributes 
   *  Set by the inherited constructor to set up the most important properties 
   *  of the command. In the Command constructor are set default values! 
   *  The real values should be set in the constructor of the inhereting class.
   */
  //@{
  const char* sAppModule;
  const char* sGroup;
  const char* sName;
  const char* sHelpUrl;
  //@}
};

/** The ToggleCommand class
 * This class does basically the same as its base class Command unless that it is intended to perform toggle actions.
 * E.g. the command to hide or show the status bar of an application is done by a toggle action. If such a command is
 * added to a popup menu then the menu item is checked if the status bar is visible, otherwise the item is unchecked.
 * @author Werner Mayer
 */
class GuiExport ToggleCommand : public Command
{
public:
  ToggleCommand(const char* name);
  virtual ~ToggleCommand() {}

protected:
  /** @name Methodes to override when creating a new command  */
  //@{
  /// Creates the used QAction
  virtual QAction* createAction(void);
  //@}
};

/** \brief The CommandGroup class groups command items together.
 *
 * In some situations it is useful to group command items together. For example, if you have a
 * command to set the resolution of a geometry object with several predefined values, such as
 * 10%, 20%, ..., 100%, only one of these resolutions should be active at any one time, and one
 * simple way of achieving this is to group the command items together in one command group.
 *
 * @note The different states must be known at creation time of the action group (@ref createAction()) to create
 * the correct number of command items. Later on it is not possible to append or remove items to this group.
 *
 * @note The corresponding actions to the command items are always toggle actions. If you want to group non-toogle
 * commands together then you should use the Command class instead of the CommandGroup class by adding the wanted
 * number of Command objects to a sub-widget. for more details refer to the @ref workbench.
 *
 * A command group can be added to a menu or a toolbar as a single unit, with all the command items
 * within the command group appearing as separate menu options and toolbar buttons.
 * @author Werner Mayer
 */
class GuiExport CommandGroup : public Command
{
public:
  /** If \a exclusive is true only one action be active at any one time, otherwise several actions can be active.
   * If \a dropdown is true then a subwidget for the command items gets created, otherwise they will be added
   * to the same widget the action group is added to.
   */
  CommandGroup( const char* name, bool exclusive=true, bool dropdown=false );
  virtual ~CommandGroup();

protected:
  /** @name Methodes to override when creating a new command */
  //@{
  /// Creates the used QAction
  virtual QAction* createAction(void);
  //@}

protected:
  /**
   * A list of subcommands where the first parameter is the menu text
   * and the second parameter defines the pixmap.
   */
  std::vector<CommandBase*> _aCommands;
  bool _exclusive; /**< This property holds whether the action group does exclusive toggling. */
  /** This property holds whether the group's actions are displayed in a subwidget of the widgets the action group is added to. */
  bool _dropdown; 
};


/** The Python command class
 * This is a special type of command class. It's used to bind a Python command class into the 
 * FreeCAD command framework.
 * An object of this class gets a reference to the Python command object and manages all the 
 * passing between the C++ and the Python world. This includes everything like setting resources such as
 * bitmaps, activation or bindings to the user interface.
 * @see CommandManager
 * @author Jürgen Riegel
 */
class PythonCommand: public Command
{
public:
  PythonCommand(const char* name,PyObject * pcPyCommand);
  virtual ~PythonCommand() {}

protected:
  /** @name Methods reimplemented for Command Framework */
  //@{
  /// Method which gets called when activated
  virtual void activated(int iMsg);
  /// if the command is not always active
  virtual bool isActive(void);
  /// Get the help URL
  const char* getHelpUrl(void);
  /// Creates the used QAction
  virtual QAction * createAction(void);
  //@}

public:
  /** @name Methods to get the properties of the command */
  //@{
  const char* getWhatsThis  () const;
  const char* getMenuText   () const;
  const char* getToolTipText() const;
  const char* getStatusTip  () const;
  const char* getPixmap     () const;
  int         getAccel      () const;
  //@}

protected:
  /// Returns the resource values
  const char* getResource(const char* sName) const;
  /// a pointer to the Python command object
  PyObject * _pcPyCommand;
  /// the command object resource dictionary
  PyObject * _pcPyResourceDict;
};


/** The script command class
 * This is a special type of command class. Its used to bind a macro or Python script to the 
 * FreeCAD command framework.
 * An object of this class gets a string to the place where the script is in the file system.
 * Unlike the other commands the resources can be set by several methods.
 * @see Command
 * @see CommandManager
 * @author Werner Mayer
 */
class MacroCommand: public Command
{
public:
  MacroCommand(const char* name);
  virtual ~MacroCommand() {}

protected:
  /** @name Methodes reimplemented for Command Framework */
  //@{
  /// Method which get called when activated
  void activated(int iMsg);
  /// Creates the used QAction
  QAction * createAction(void);
  //@}

public:
  /// Returns the script name
  QString getScriptName () const { return scriptName;   }
  /// Sets the script name
  void setScriptName ( const QString& s ) { scriptName = s; }
  /// Ignore when language has changed. 
  void languageChange() {}
 
  /** @name Methods to set the properties of the Script Command */
  //@{
  void setWhatsThis  ( const char* );
  void setMenuText   ( const char* );
  void setToolTipText( const char* );
  void setStatusTip  ( const char* );
  void setPixmap     ( const char* );
  void setAccel      ( int         );
  //@}

  /** @name Methods to load and save macro commands. */
  //@{
  /** Loads all macros command from the preferences. */
  static void load();
  /** Saves all macros command to the preferences. */
  static void save();
  //@}

protected:
  /** @name Attributes 
   *  set by the inherited constructor to set up the most important propertys 
   *  of the command. In the Command constructor are set default values! 
   *  The real values should be set with the Seter.
   */
  //@{
  QString scriptName;
  //@}
private:
  const char* createStringCopy( const char* ) const;
};

/** The CommandManager class
 *  This class manage all available commands in FreeCAD. All 
 *  Commands will registered here, also commands from Application
 *  modules. Also activation / deactivation, Icons Tooltips and so
 *  on are handles here. Further the Building of Toolbars and (Context) 
 *  menus (connecting to a QAction) is done here.
 *  @see Command
 *  @author Jürgen Riegel
 */
class GuiExport CommandManager
{
public:
  /// Insert a new command into the manager
  void addCommand(Command* pCom);
  /// Remove a command from the manager
  void removeCommand(Command* pCom);

  /// Adds the given command to a given widget
  void addTo(const char* Name,QWidget *pcWidget);
  /// Removes the given command from the widget
  void removeFrom(const char* Name,QWidget *pcWidget);

  /** Returns all commands of a special App Module
   *  delivers a vector of all comands in the given application module. When no 
   *  name is given the standard commands (build in ) are returned.
   *  @see Command
   */
  std::vector <Command*> getModuleCommands(const char *sModName);

  /** Returns the module name of the corresponding Command
   * object. If the command cannot be found the message 'Not found'
   * is returned.
   */
  std::string getAppModuleName(QAction* pActions);

  /** Returns the module name of the corresponding Command
   * object. If the command cannot be found the message 'Not found'
   * is returned.
   */
  std::string getAppModuleNameByName(const char* sName);

  /** Returns all commands registered in the manager
   *  delivers a vector of all comands. If you intereted in commands of 
   *  of a special app module use GetModuleCommands()
   *  @see Command
   */
  std::vector <Command*> getAllCommands(void);

  /** Returns all commands of a group
   *  delivers a vector of all comands in the given group. 
   */
  std::vector <Command*> getGroupCommands(const char *sGrpName);

  /** Returns the command registered in the manager with the name sName
   *  If nothing is found it returns a null pointer 
   *  @see Command
   */
  Command* getCommandByName(const char* sName);

  /**  
   * Runs the command
   */
  void runCommandByName (const char* sName);

  /// method is OBSOLET use GetModuleCommands() or GetAllCommands() 
  const std::map<std::string, Command*>& getCommands() { return _sCommands; }
  /// get frequently called by the AppWnd to check the commands are active
  void testActive(void); 
private:
  std::map<std::string,Command*> _sCommands;
};

/**
 * Shows information about the application.
 * @author Werner Mayer
 */
class StdCmdAbout : public Command
{
public:
  StdCmdAbout();

  /** i18n stuff of the command. */
  void languageChange();

protected:
  /** Creates the action object. */
  QAction* createAction();
  /** Invokes the about-dialog. */
  void activated(int iMsg);
};

/**
 *  The workbench command
 *  @author Werner Mayer
 */
class StdCmdWorkbench : public Command
{
public:
  StdCmdWorkbench();

  /** Refreshes the list of available workbenches. */
  void refresh ();

  /** Activates the workbench \a item. */
  void activate( const QString& item );

  /** Notifies this command when workbench has been changed from 
   * outside, e.g. via Python command line. 
   */
  void notify( const QString& item );
  
  /** Adds the workbench command to a widget. */
  bool addTo(QWidget *);

protected:
  /** The item at position \a iMsg is activated. */
  void activated(int iMsg);

  /** Creates the accompanying QAction object to the command. */
  QAction * createAction(void);

private:
  /** Appends a new workbench \a item. */
  void addWorkbench ( const QString& item );

  QActionGroup *pcAction;
};


/**
 *  The MRU command which does the handling of recent files.
 *  @author Werner Mayer
 */
class StdCmdMRU : public Command
{
public:
  StdCmdMRU();
  
  /** Adds the new item to the recent files. */
  void addRecentFile ( const QString& item );
  /** Removes \a item from the recent files. */
  void removeRecentFile ( const QString& item );

  /** Refreshes the recent file list. */
  void refreshRecentFileWidgets();

  int  maxCount() const { return _nMaxItems; }
  void setMaxCount (int i) { _nMaxItems = i;    }

  QStringList recentFiles() const;

  /** @name Methods to load or save from preferences */
  //@{
  /** Loads all recent files from the preferences. */
  static void load();
  /** Saves all recent files to the preferences. */
  static void save();
  //@}

protected:
  bool isActive(void){return true;}

  /** The item at position \a iMsg is activated. */
  void activated(int iMsg);

  /** Creates the accompanying QAction object to the command. */
  QAction * createAction(void);

private:
  QStringList _vMRU;
  QActionGroup *pcAction;
  int _nMaxItems;
};

} // namespace Gui



/** The Command Macro Standard
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name.
 *  @author Jürgen Riegel
 */
#define DEF_STD_CMD(X) class X : public Gui::Command \
{\
public:\
  X();\
protected: \
  virtual void activated(int iMsg);\
};

/** The Command Macro Standard + isActive()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Jürgen Riegel
 */
#define DEF_STD_CMD_A(X) class X : public Gui::Command \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
};

/** The Command Macro Standard + createAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Jürgen Riegel
 */
#define DEF_STD_CMD_C(X) class X : public Gui::Command \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual QAction * createAction(void);\
};

/** The Command Macro Standard + isActive() + createAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_AC(X) class X : public Gui::Command \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
  virtual QAction * createAction(void);\
};

/** The Command Macro Standard for toggle commands.
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name.
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_TOGGLE(X) class X : public Gui::ToggleCommand \
{\
public:\
  X();\
protected: \
  virtual void activated(int iMsg);\
};

/** The Command Macro Standard for toggle commands + isActive()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_TOGGLE_A(X) class X : public Gui::ToggleCommand \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
};

/** The Command Macro Standard for toggle commands + createAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_TOGGLE_C(X) class X : public Gui::ToggleCommand \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual QAction * createAction(void);\
};

/** The Command Macro Standard for toggle commands + isActive() + createAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_TOGGLE_AC(X) class X : public Gui::ToggleCommand \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
  virtual QAction * createAction(void);\
};

/** The Command Macro Standard for group commands.
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name.
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_GROUP(X) class X : public Gui::CommandGroup \
{\
public:\
  X();\
protected: \
  virtual void activated(int iMsg);\
};

/** The Command Macro Standard for group commands + isActive()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_GROUP_A(X) class X : public Gui::CommandGroup \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
};

/** The Command Macro Standard for group commands + createAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_GROUP_C(X) class X : public Gui::CommandGroup \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual QAction * createAction(void);\
};

/** The Command Macro Standard for group commands + isActive() + createAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_GROUP_AC(X) class X : public Gui::CommandGroup \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
  virtual QAction * createAction(void);\
};

/** The Command Macro view
 *  This macro makes it easyer to define a new command for the 3D View
 *  It activate the command only when a 3DView is active.
 *  The parameters are the class name
 *  @author Jürgen Riegel
 */
#define DEF_3DV_CMD(X) class X : public Gui::Command \
{\
public:\
  X();\
  virtual ~X(){}\
protected: \
  virtual void activated(int iMsg);\
  virtual bool isActive(void)\
  {\
    return ( getMainWindow()->activeWindow() && QString( getMainWindow()->activeWindow()->getName()).contains( "View3DInventor" )  > 0)?true:false;\
  }\
};

#endif // GUI_COMMAND_H
