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
#include <qaction.h>
#include <string>
#include <map>
//#	pragma warning( disable : 4786 )

class FCCommandManager;
class ApplicationWindow;
class FCGuiDocument;
class FCDocument;
class FCCommand;


void CreateStdCommands(void);
void CreateViewStdCommands(void);

/**
 * Allow Actions to be added to other widgets except of toolbars and menus
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

public slots:
	void Activated ();
	void Toggled   (bool); 
private:
	FCCommand *_pcCmd;

};


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

	/** @name Methodes to override when create a new command
	 */
	//@{
	/// Method which get called when activated, needs to be reimplemented!
	virtual void Activated(int iMsg){assert(_eType&Cmd_Toggle == 0);}
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
	friend FCCommandManager;
	/// Get somtile called to check the state of the command
	void TestActive(void);
	/// get called by the FCAction
	virtual void activated (); 
	/// get called by the FCAction
	virtual void toggled ( bool ); 
	/// adds this command to abetrary widgets
	virtual bool addTo(QWidget *);
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
 *  @see FCCommandManager
 */
 class GuiExport FCCppCommand :public FCCommand
{
public:

	FCCppCommand(const char* name,CMD_Type eType=Cmd_Normal);

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

	/// Adds the given command to a given widget
	void AddTo(const char* Name,QWidget *pcWidget);

	/** Returns all commands of a special App Module
	 *  delivers a vector of all comands in the given application module. When no 
	 *  name is given the standard commands (build in ) are returned.
	 *  @see FCCommand
	 */
	std::vector <FCCommand*> GetModuleCommands(const char *sModName);

	/** Returns all commands registered in the manager
	 *  delivers a vector of all comands. If you intereted in commands of 
	 *  of a special app module use GetModuleCommands()
	 *  @see FCCommand
	 */
	std::vector <FCCommand*> GetAllCommands(void);

	/// method is OBSOLET use GetModuleCommands() or GetAllCommands() 
	const std::map<std::string, FCCommand*>& GetCommands() { return _sCommands; }
	/// get frequently called by the AppWnd to check the commands are active
	void TestActive(void); 
private:
#	pragma warning( disable : 4251 )
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

/** The Command Macro view
 *  This macro makes it easyer to define a new command for the 3D View
 *  It activate the command only when a 3DView is active.
 *  The parameters are the class name
 */
#define DEF_3DV_CMD(X) class X : public FCCppCommand \
{\
public:\
	X();\
	virtual void Activated(int iMsg);\
	virtual bool IsActive(void)\
		{\
			return GetAppWnd()->GetActiveView() && strcmp( GetAppWnd()->GetActiveView()->GetName(), "View3D" ) == 0?true:false;\
		}\
};

#endif // __Command_h__
