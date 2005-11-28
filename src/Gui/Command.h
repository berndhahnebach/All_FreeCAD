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
# include <pair>
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
   * Returns the QAction object of this command. If \a create is set to true (the default) the action gets created in case
   * it's not already created.
   */
  QAction*  getAction(bool create=true);

  /** @name Methods to override when creating a new command */
  //@{
  /// Creates the used QAction. The default implementation does nothing.
  virtual QAction * createAction(void);
  /// Reassign QAction stuff
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
   *  of the command. In the Command constructor are set default values. 
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
 *  This is the base class of all commands in FreeCAD. It is the single point where 
 *  new commands get implemented. It also contains a lot of helper methods to make 
 *  implementing commands for FreeCAD as easy as possible.
 *  @see CommandManager
 * \author Jürgen Riegel
 */
class GuiExport Command : public CommandBase
{
public:
  /// Type of Commands
  enum CMD_Type { 
    Cmd_Normal=0, /** Normal command */
    Cmd_Toggle=1  /** A toogle command */
  };
  Command(const char* name,CMD_Type eType=Cmd_Normal);
  virtual ~Command();

  /** @name Methods to override when creating a new command
   */
  //@{
  /// Methods which get called when activated, needs to be reimplemented!
  virtual void activated(int iMsg){assert((_eType&Cmd_Toggle) == 0);}
  /// Overite this methode if your Cmd is not always active
  virtual bool isActive(void){return true;} 
  /// Creates the used QAction
  virtual QAction * createAction(void)=0;
  /// returns the resource values
  virtual std::string getResource(const char* sName) const=0;
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
   *  when the name not exist or no document is active!
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
  /// Activate an other Commands
  void activateCommand(const char* sCmdName);
  /// Toggles other Commands
  void toggleCommand(const char* sCmdName,bool bToggle);
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
  /// is it a toggle cmd
  bool isToggle(void) const;
  /// returns the name to which the command belongs
  const char* getAppModuleName(void) const {return sAppModule;}	
  /// Get the command name
  const char* getName() const { return sName; }
  /// Get the name of the grouping of the command
  const char* getGroupName() const { return sGroup; }
  /// Get the help URL
  const char* getHelpUrl(void) const { return sHelpUrl; }
  //@}

  /** @name interface used by the CommandManager and the QAction */
  //@{
  /// CommandManager is a friend
  friend class CommandManager;
  /// Get somtile called to check the state of the command
  void testActive(void);
  /// get called by the QAction
  virtual void activated (); 
  /// get called by the QAction
  virtual void toggled ( bool ); 
  /// adds this command to arbitrary widgets
  virtual bool addTo(QWidget *);
  /// removes this command from arbitrary widgets
  virtual bool removeFrom(QWidget *pcWidget);
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
  CMD_Type _eType;
  //@}
};

/** The cpp Command class
 *  This class is mostly used for commands implemented in C++. The resources are saved 
 *  as const char.
 *  Note!
 *  \par
 *  This class is intendet to handle the gui interaction like:
 *  - starting a dialog
 *  - doing View and Window stuff
 *  enithing else, especialy altering the document must be done on Application level!
 *  see DoCommand() for details.
 *  @see CommandManager
 * \author Jürgen Riegel
 */
class GuiExport CppCommand : public Command
{
public:
  CppCommand(const char* name,CMD_Type eType=Cmd_Normal);
  virtual ~CppCommand() {}

  /** @name Methodes to override when create a new command
   *  Description  
   */
  //@{
  /// Method which get called when activated, needs to be reimplemented!
  virtual void activated(int iMsg)=0;
  /// Overite this methode if your Cmd is not always active
  virtual bool isActive(void){return true;}
  /// Creates the used QAction
  virtual QAction * createAction(void);
  /// returns the resource values
  virtual std::string getResource(const char* sName) const;
  //@}
};

class GuiExport ToggleCommand : public CppCommand
{
public:
  ToggleCommand(const char* name,CMD_Type eType=Cmd_Normal);
  virtual ~ToggleCommand() {}

  /** @name Methodes to override when create a new command
   *  Description  
   */
  //@{
  /// Creates the used QAction
  virtual QAction* createAction(void);
  //@}
};

/**
 * The CommandGroup class groups command items together. 
 * In some situations it is useful to group command items together. For example, if you have a 
 * command to set the resolution of a geometry object with several predefined values, such as 
 * 10%, 20%, ..., 100%, only one of these resolutions should be active at any one time, and one 
 * simple way of achieving this is to group the command items together in one command group. 
 *
 * A command group can also be added to a menu or a toolbar as a single unit, with all the command items 
 * within the command group appearing as separate menu options and toolbar buttons.
 * @author Werner Mayer
 */
class GuiExport CommandGroup : public CppCommand
{
public:
  CommandGroup(const char* name, bool dropdown=false,CMD_Type eType=Cmd_Normal);
  virtual ~CommandGroup();

  /** @name Methodes to override when creating a new command
   *  Description  
   */
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
  /** Does the command items appear in a subwidget or in the same widget as the command itself. */
  bool _dropdown; 
};


/** The python command class
 *  This is a special type of command class. Its used to bind
 *  a python command class into the FC command framework.
 *  A object of this class gets a reference to the python 
 *  command object and manage all the passing between the 
 *  C++ and the python world. This includes everithing like
 *  seting resources like bitmaps, aktivation or bindings 
 *  to the user interface.
 *  @see CommandManager
 * \author Jürgen Riegel
 */
class PythonCommand: public Command
{
public:
  PythonCommand(const char* name,PyObject * pcPyCommand);
  virtual ~PythonCommand() {}

  /** @name Methodes reimplemented for Command Framework */
  //@{
  /// Method which get called when activated
  virtual void activated(int iMsg);
  /// if your Cmd is not always active
  virtual bool isActive(void);
  /// Get the help URL
  virtual std::string cmdHelpURL(void);
  /// Get the help page
  virtual void cmdHelpPage(std::string &rcHelpPage);
  /// Creates the used QAction
  virtual QAction * createAction(void);
  /// returns the resource values
  virtual std::string getResource(const char* sName) const;
  //@}

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
  /// a reference to the python coammnd object
  PyObject * _pcPyCommand;
  /// the command object resource dictionary
  PyObject * _pcPyResourceDict;
};


/** The script command class
 *  This is a special type of command class. Its used to bind
 *  a macro or python script into the FC command framework.
 *  A object of this class gets a string to the place where the 
 *  script is in the file system and manage all the passing between the 
 *  C++ and the python world. Unlike the other commands the resources can
 *  be set by methodes.
 *  @see Command
 *  @see CommandManager
 *  @author Werner Mayer
 */
class MacroCommand: public CppCommand
{
public:
  MacroCommand(const char* name);
  virtual ~MacroCommand() {}

  /** @name Methodes reimplemented for Command Framework */
  //@{
  /// Method which get called when activated
  virtual void activated(int iMsg);
  //@}

  /** @name Methodes to set the propertys of the Script Command */
  //@{
  QString getScriptName () const { return scriptName;   }
  void setScriptName ( const QString& );
  void setWhatsThis  ( const QString& );
  void setMenuText   ( const QString& );
  void setToolTipText( const QString& );
  void setStatusTip  ( const QString& );
  void setPixmap     ( const QString& );
  void setAccel      ( int   i        );
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

  /** Returns the command registered in the manager with the 
   *  action text sName (not its name! for this 
   *  @see GetCommandByName(const char*))
   *  If nothing is found it returns a null pointer 
   *  @see Command
   */
  Command* getCommandByActionText(const char* sName);

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
class StdCmdAbout : public CppCommand
{
public:
  StdCmdAbout();

  /** Creates the action object. */
  QAction* createAction();
  /** Invokes the about-dialog. */
  void activated(int iMsg);
  /** i18n stuff of the command. */
  void languageChange();
};

/**
 *  The workbench command
 *  @author Werner Mayer
 */
class StdCmdWorkbench : public CppCommand
{
public:
  StdCmdWorkbench();

  /** The item at position \a iMsg is activated. */
  void activated(int iMsg);

  /** Creates the accompanying QAction object to the command. */
  QAction * createAction(void);

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

private:
  /** Appends a new workbench \a item. */
  void addWorkbench ( const QString& item );

  QActionGroup *pcAction;
};


/**
 *  The MRU command which does the handling of recent files.
 *  @author Werner Mayer
 */
class StdCmdMRU : public CppCommand
{
public:
  StdCmdMRU();
  bool isActive(void){return true;}

  /** The item at position \a iMsg is activated. */
  void activated(int iMsg);

  /** Creates the accompanying QAction object to the command. */
  QAction * createAction(void);
  
  /** Adds the new item to the recent files. */
  void addRecentFile ( const QString& item );
  /** Removes \a item from the recent files. */
  void removeRecentFile ( const QString& item );

  /** Refreshes the recent file list. */
  void refreshRecentFileWidgets();

  int  maxCount() const { return _nMaxItems; }
  void setMaxCount (int i) { _nMaxItems = i;    }

  QStringList recentFiles() const;
  std::string getResource(const char* sName) const { return ""; }

  /** @name Methods to load or save from preferences */
  //@{
  /** Loads all recent files from the preferences. */
  static void load();
  /** Saves all recent files to the preferences. */
  static void save();
  //@}

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
#define DEF_STD_CMD(X) class X : public Gui::CppCommand \
{\
public:\
  X();\
  virtual void activated(int iMsg);\
};

/** The Command Macro Standard + IsActive()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Jürgen Riegel
 */
#define DEF_STD_CMD_A(X) class X : public Gui::CppCommand \
{\
public:\
  X();\
  virtual ~X(){}\
  virtual void activated(int iMsg);\
  virtual bool isActive(void);\
};

/** The Command Macro Standard + IsActive() + CreateAction()
 *  This macro makes it easier to define a new command.
 *  The parameters are the class name
 *  @author Werner Mayer
 */
#define DEF_STD_CMD_AC(X) class X : public Gui::CppCommand \
{\
public:\
  X();\
  virtual ~X(){}\
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
#define DEF_3DV_CMD(X) class X : public Gui::CppCommand \
{\
public:\
  X();\
  virtual ~X(){}\
  virtual void activated(int iMsg);\
  virtual bool isActive(void)\
  {\
    return ( getMainWindow()->activeWindow() && QString( getMainWindow()->activeWindow()->getName()).contains( "View3DInventor" )  > 0)?true:false;\
  }\
};

#endif // GUI_COMMAND_H
