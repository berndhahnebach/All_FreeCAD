/** \file Command.h
 *  \brief The visual command module
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    4.2002
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
#ifndef __Command_h__
#define __Command_h__


#include "../Base/PyExport.h"

class FCCommandManager;
class ApplicationWindow;
class FCGuiDocument;
class FCDocument;
class FCCommand;

namespace Gui {
namespace Dialog {

class CUndoRedoDlg;

} // namespace Dialog
} // namespace Gui


void CreateStdCommands(void);
void CreateViewStdCommands(void);
void CreateTestCommands(void);

/**
 * Allow actions to be added to other widgets except of toolbars and menus
 * @author Werner Mayer
 */
class GuiExport FCAction : public QAction
{
	Q_OBJECT
public:
	FCAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
	virtual ~FCAction();

	/// allow to add this to other widgets as 'QToolBar' or 'QPopupMenu'
	virtual bool addTo(QWidget *);
  bool removeFrom ( QWidget * w );
  FCCommand* GetCommand() { return _pcCmd; }

public slots:
	void Activated ();
	void Toggled   (bool); 
  void setEnabled ( bool ) ;

protected slots:
  void slotToolButtonToggled( bool on );
  void slotClearStatusText();
  void slotShowStatusText( const QString& text );
  void slotDestroyed();

protected:
  std::vector<QWidget*> widgets;

private:
	FCCommand *_pcCmd;
};

/**
 * Allow several actions to add to a toolbar or menu.
 * For menus a submenu will be created for toolbars a combo box.
 * @author Werner Mayer
 */
class GuiExport FCMultiAction : public FCAction
{
	Q_OBJECT
public:
	FCMultiAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
	virtual ~FCMultiAction();

	/// allow to add this to other widgets as 'QToolBar' or 'QPopupMenu'
	virtual bool addTo(QWidget *);
  void setItems(const std::vector<std::string>& items);
  void insertItem(const char* item);
  void removeItem(const char* item);
  void activate(int);
  void activate(QString);
  void clear();

public slots:
	void activated (int i);

private slots:
  void onAboutToShow();

protected:
  std::vector<std::string> mItems;
};

/**
 * Special action for the undo button
 * @author Werner Mayer
 */
class GuiExport FCUndoAction : public FCAction
{
	Q_OBJECT
public:
	FCUndoAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE )
    : FCAction(pcCmd, parent, name, toggle)
  {
    tipGroup = new QToolTipGroup(0);
  }

  ~FCUndoAction(){ delete tipGroup; }
	bool addTo(QWidget *);

private:
  QToolTipGroup* tipGroup;
};

/**
 * Special action for the redo button
 * @author Werner Mayer
 */
class GuiExport FCRedoAction : public FCAction
{
	Q_OBJECT
public:
	FCRedoAction ( FCCommand* pcCmd,QObject * parent = 0, const char * name = 0, bool toggle = FALSE )
    : FCAction(pcCmd, parent, name, toggle)
  {
    tipGroup = new QToolTipGroup(0);
  }

  ~FCRedoAction(){ delete tipGroup; }
	bool addTo(QWidget *);

private:
  QToolTipGroup* tipGroup;
};


/// Type of Commands
enum CMD_Type { 
	Cmd_Normal=0,
	Cmd_Toggle=1
};




/** The Command class
 *  This is the base class of all commands in FreeCAD. Its the single point where 
 *  new commands get implemented. It also contains a lot of helper mthodes to make 
 *  implementing commands for FreeCAD as easy as possible.
 *  @see FCCommandManager
 */
class GuiExport FCCommand 
{
public:

	FCCommand(const char* name,CMD_Type eType=Cmd_Normal);
	virtual ~FCCommand() {}

	/** @name Methodes to override when create a new command
	 */
	//@{
	/// Method which get called when activated, needs to be reimplemented!
	virtual void Activated(int iMsg){assert((_eType&Cmd_Toggle) == 0);}
	/// Overite this methode if your Cmd is not always active
	virtual bool IsActive(void){return true;} 
	/// Get the help URL
	virtual std::string CmdHelpURL(void){ return std::string("FCDoc://Msg/NoHelp.htm");}
	/// Get the help page
	virtual void CmdHelpPage(std::string &rcHelpPage){return;}
	/// Creates the used FCAction
	virtual FCAction * CreateAction(void)=0;
	/// returns the resource values
	virtual std::string GetResource(const char* sName)=0;
	//@}


	/** @name Helper methodes to get importend classes */
	//@{
	/// Get pointer to the Application Window
	ApplicationWindow*	GetAppWnd(void);   
	/// Get pointer to the active gui document
	FCGuiDocument*		GetActiveDocument(void);
	/// Get ponter to the active CasCade document 
	FCDocument*			GetActiveOCCDocument(void);
	/// Get the FCAxtion object of this command
	FCAction*           GetAction();
	//@}

	/** @name Helper methodes for the UNDO REDO handling */
	//@{
	/// Open a new Undo transaction on the active document
	void OpenCommand(const char* sName=0);
	/// Commit the Undo transaction on the active document
	void CommitCommand(void);
	/// Abort the Undo transaction on the active document
	void AbortCommand(void);
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
	void DoCommand(DoCmd_Type eType,const char* sCmd,...);
	/// Activate an other Commands
	void ActivateCommand(const char* sCmdName);
	/// Toggles other Commands
	void ToggleCommand(const char* sCmdName,bool bToggle);
	/// Updates the (active) document (propagate changes)
	void UpdateActive(void);
	/// Updates the (all or listed) documents (propagate changes)
	void UpdateAll(std::list<FCGuiDocument*> cList);
	//@}

	/** @name Helper methodes to generate help pages */
	//@{
	/// returns the begin of a online help page
	const char * BeginCmdHelp(void);
	/// returns the end of a online help page
	const char * EndCmdHelp(void);
	//@}


	/** @name checking of internal state */
	//@{
	/// is it a toggle cmd
	bool IsToggle(void);
	/// returns the name to which the command belongs
	const char* GetAppModuleName(void){return sAppModule.c_str();}	
	/// Get the command name
	const char*	GetName() { return sName.c_str(); }
	/// Get the name of the grouping of the command
	const char*	GetGroupName() { return sGroup.c_str(); }
	//@}

	/** @name interface used by the FCCommandManager and the FCAction */
	//@{
	/// FCCommandManager is a friend
	friend class FCCommandManager;
	/// Get somtile called to check the state of the command
	void TestActive(void);
	/// get called by the FCAction
	virtual void activated (); 
	/// get called by the FCAction
	virtual void toggled ( bool ); 
	/// adds this command to arbitrary widgets
	virtual bool addTo(QWidget *);
	/// removes this command from arbitrary widgets
  virtual bool removeFrom(QWidget *pcWidget);
	//@}



protected:
	/** @name Attributes 
	 *  set by the inherited constructor to set up the most important propertys 
	 *  of the command. In the FCCommand constructor are set default values! 
	 *  The real values should be set in the constructor of the inhereting class.
	 */
	//@{
	std::string sAppModule;
	std::string sGroup;
	std::string sName;
	FCAction *_pcAction;
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
 *  @see FCCommandManager
 */
 class GuiExport FCCppCommand :public FCCommand
{
public:

	FCCppCommand(const char* name,CMD_Type eType=Cmd_Normal);
	virtual ~FCCppCommand() {}

	/** @name Methodes to override when create a new command
	 *  Description  
	 */
	//@{
	/// Method which get called when activated, needs to be reimplemented!
	virtual void Activated(int iMsg)=0;
	/// Overite this methode if your Cmd is not always active
	virtual bool IsActive(void){return true;}
	/// Creates the used FCAction
	virtual FCAction * CreateAction(void);
	/// returns the resource values
	virtual std::string GetResource(const char* sName);

	//@}

protected:
	/** @name Attributes 
	 *  set by the inherited constructor to set up the most important propertys 
	 *  of the command. In the FCCommand constructor are set default values! 
	 *  The real values should be set in the constructor of the inhereting class.
	 */
	//@{
	const char* sMenuText;
	const char* sToolTipText;
	const char* sWhatsThis;
	const char* sStatusTip;
	const char* sPixmap;
	int         iAccel;
	//@}

};


/** The python command class
 *  This is a special type of command class. Its used to bind
 *  a python command class into the FC command framework.
 *  A object of this class gets a reference to the python 
 *  command object and manage all the passing between the 
 *  C++ and the python world. This includes everithing like
 *  seting resources like bitmaps, aktivation or bindings 
 *  to the user interface.
 *  @see FCCommandManager
 */
class FCPythonCommand: public FCCommand
{
public:
	FCPythonCommand(const char* name,PyObject * pcPyCommand);
	virtual ~FCPythonCommand() {}

	/** @name Methodes reimplemented for Command Framework */
	//@{
	/// Method which get called when activated
	virtual void Activated(int iMsg);
	/// if your Cmd is not always active
	virtual bool IsActive(void);
	/// Get the help URL
	virtual std::string CmdHelpURL(void);
	/// Get the help page
	virtual void CmdHelpPage(std::string &rcHelpPage);
	/// Creates the used FCAction
	virtual FCAction * CreateAction(void);
	/// returns the resource values
	virtual std::string GetResource(const char* sName);
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
 *  @see FCCommand
 *  @see FCCommandManager
 */
class FCScriptCommand: public FCCommand
{
public:
	FCScriptCommand(const char* name);
	virtual ~FCScriptCommand() {}

	/** @name Methodes reimplemented for Command Framework */
	//@{
	/// Method which get called when activated
	virtual void Activated(int iMsg);
	/// Get the help URL
	virtual std::string CmdHelpURL(void);
	/// Get the help page
	virtual void CmdHelpPage(std::string &rcHelpPage);
	/// Creates the used FCAction
	virtual FCAction * CreateAction(void);
	/// returns the resource values
	virtual std::string GetResource(const char* sName);
	//@}

	/** @name Methodes to get the propertys of the Script Command */
	//@{
	const char* GetScriptName ()	{return _sScriptName. c_str();}
	const char* GetMenuText   ()	{return _sMenuText.   c_str();}
	const char* GetToolTipText()	{return _sToolTipText.c_str();}
	const char* GetWhatsThis  ()	{return _sWhatsThis.  c_str();}
	const char* GetStatusTip  ()	{return _sStatusTip.  c_str();}
	const char* GetPixmap     ()	{return _sPixmap.     c_str();}
	const char* GetHelpPage   ()	{return _sHelpPage.   c_str();}
	const char* GetHelpURL    ()	{return _sHelpURL.    c_str();}
	int         GetAccel      ()	{return _iAccel;              }
	//@}
	/** @name Methodes to set the propertys of the Script Command */
	//@{
	void SetScriptName		(const char* sScriptName)	{_sScriptName	=sScriptName;}
	void SetMenuText		(const char* sMenuText)		{_sMenuText		=sMenuText;}
	void SetToolTipText		(const char* sToolTipText)	{_sToolTipText	=sToolTipText;}
	void SetWhatsThis		(const char* sWhatsThis)	{_sWhatsThis	=sWhatsThis;}
	void SetStatusTip		(const char* sStatusTip)	{_sStatusTip	=sStatusTip;}
	void SetPixmap		    (const char* sPixmap)		{_sPixmap		=sPixmap;}
	void SetHelpPage	    (const char* sHelpPage)		{_sHelpPage		=sHelpPage;}
	void SetHelpURL		    (const char* sHelpURL)		{_sHelpURL		=sHelpURL;}
	void SetAccel		    (int iAccel)				{_iAccel		=iAccel;}
	//@}


protected:
	/** @name Attributes 
	 *  set by the inherited constructor to set up the most important propertys 
	 *  of the command. In the FCCommand constructor are set default values! 
	 *  The real values should be set with the Seter.
	 */
	//@{
	std::string _sMenuText;
	std::string _sToolTipText;
	std::string _sWhatsThis;
	std::string _sStatusTip;
	std::string _sPixmap;
	std::string _sScriptName;
	std::string _sHelpPage;
	std::string _sHelpURL;
	int         _iAccel;
	//@}

};

/** The CommandManager class
 *  This class manage all available commands in FreeCAD. All 
 *  Commands will registered here, also commands from Application
 *  modules. Also activation / deactivation, Icons Tooltips and so
 *  on are handles here. Further the Building of Toolbars and (Context) 
 *  menus (conecting to a FCAction) is done here.
 *  @see FCCommand
 */
class GuiExport FCCommandManager
{
public:
	/// Insert a new command into the manager
	void AddCommand(FCCommand* pCom);
	/// Remove a command from the manager
	void RemoveCommand(FCCommand* pCom);

	/// Adds the given command to a given widget
	void AddTo(const char* Name,QWidget *pcWidget);
  /// Removes the given command from the widget
  void RemoveFrom(const char* Name,QWidget *pcWidget);

	/** Returns all commands of a special App Module
	 *  delivers a vector of all comands in the given application module. When no 
	 *  name is given the standard commands (build in ) are returned.
	 *  @see FCCommand
	 */
	std::vector <FCCommand*> GetModuleCommands(const char *sModName);

  /** Returns the module name of the corresponding FCCommand
   * object. If the command cannot be found the message 'Not found'
   * is returned.
   */
  std::string GetAppModuleName(QAction* pActions);

  /** Returns the module name of the corresponding FCCommand
   * object. If the command cannot be found the message 'Not found'
   * is returned.
   */
  std::string GetAppModuleNameByName(const char* sName);

	/** Returns all commands registered in the manager
	 *  delivers a vector of all comands. If you intereted in commands of 
	 *  of a special app module use GetModuleCommands()
	 *  @see FCCommand
	 */
	std::vector <FCCommand*> GetAllCommands(void);

	/** Returns all commands of a group
	 *  delivers a vector of all comands in the given group. 
	 */
	std::vector <FCCommand*> GetGroupCommands(const char *sGrpName);

  /** Returns the command registered in the manager with the name sName
   *  If nothing is found it returns a null pointer 
	 *  @see FCCommand
	 */
	FCCommand* GetCommandByName(const char* sName);

  /** Returns the command registered in the manager with the 
   *  action text sName (not its name! for this 
   *  @see GetCommandByName(const char*))
   *  If nothing is found it returns a null pointer 
	 *  @see FCCommand
	 */
	FCCommand* GetCommandByActionText(const char* sName);

  /**  
   * Runs the command
   */
  void RunCommandByName (const char* sName);

	/// method is OBSOLET use GetModuleCommands() or GetAllCommands() 
	const std::map<std::string, FCCommand*>& GetCommands() { return _sCommands; }
	/// get frequently called by the AppWnd to check the commands are active
	void TestActive(void); 
private:
	std::map<std::string,FCCommand*> _sCommands;

};

/** The Command Macro Standard
 *  This macro makes it easyer to define a new command.
 *  The parameters are the class name
 */
#define DEF_STD_CMD(X) class X : public FCCppCommand \
{\
public:\
	X();\
	virtual void Activated(int iMsg);\
};

/** The Command Macro Standard + IsActive()
 *  This macro makes it easyer to define a new command.
 *  The parameters are the class name
 */
#define DEF_STD_CMD_A(X) class X : public FCCppCommand \
{\
public:\
	X();\
	virtual ~X(){}\
	virtual void Activated(int iMsg);\
	virtual bool IsActive(void);\
};

/** The Command Macro view
 *  This macro makes it easyer to define a new command for the 3D View
 *  It activate the command only when a 3DView is active.
 *  The parameters are the class name
 */
#define DEF_3DV_CMD(X) class X : public FCCppCommand \
{\
public:\
	X();\
	virtual ~X(){}\
	virtual void Activated(int iMsg);\
	virtual bool IsActive(void)\
		{\
			return ( GetAppWnd()->GetActiveView() && strcmp( GetAppWnd()->GetActiveView()->GetName(), "View3D" )  == 0)?true:false;\
		}\
};


/** Some special commands
 *  The undo/redo commands
 */
class FCCmdUndo : public FCCppCommand
{
public:
	  FCCmdUndo();
	void Activated(int iMsg);
    bool IsActive(void);
    QWidget* GetWidget();
  	FCAction * CreateAction(void);

private:
	Gui::Dialog::CUndoRedoDlg*	 _pclUndoRedoWidget;
};

/**
 *  The undo/redo commands
 */
class FCCmdRedo : public FCCppCommand
{
public:
	  FCCmdRedo();
	void Activated(int iMsg);
    bool IsActive(void);
    QWidget* GetWidget();
  	FCAction * CreateAction(void);

private:
	Gui::Dialog::CUndoRedoDlg*	 _pclUndoRedoWidget;
};

/**
 *  The workbench command
 */
class FCCmdWorkbench : public FCCppCommand
{
public:
	  FCCmdWorkbench();
	void Activated(int iMsg);
  	FCAction * CreateAction(void);
    void AddItem (const char* item);
    void RemItem (const char* item);
    void Clear();
    void UpdateAction(int i);
    void UpdateAction(const char* item);
	  bool addTo(QWidget *);

private:
    FCMultiAction *pcAction;
};

/**
 *  The MRU command which does the handling of recent files
 */
class FCCmdMRU : public FCCppCommand
{
public:
	  FCCmdMRU();
	bool IsActive(void){return true;}
	void Activated(int iMsg);
  	FCAction * CreateAction(void);
    void AddItem (const char* item);
    void RemItem (const char* item);
    void Clear();
    int  GetMaxItems()      { return nMaxItems; }
    void SetMaxItems(int i) { nMaxItems = i;    }
    std::vector<std::string> GetItems() const;
	std::string GetResource(const char* sName) { return ""; }

private:
    QString GetFileName(const char* name);
    std::vector<std::string> _vMRU;
    FCMultiAction *pcAction;
    int nMaxItems;
};

#endif // __Command_h__
