/***************************************************************************
                          Command.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2002
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
# include <qdir.h>
# include <qfileinfo.h>
# include <qobjectlist.h>
# include <qstatusbar.h>
# include <qwhatsthis.h>
#endif

#include "Command.h"
#include "Action.h"
#include "Application.h"
#include "Document.h"
#include "ToolBoxBar.h"
#include "HelpView.h"
#include "Macro.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"

#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "../Base/Interpreter.h"

using Base::Interpreter;
using namespace Gui;
using namespace Gui::Dialog;
using namespace Gui::DockWnd;

//===========================================================================
// FCCommand
//===========================================================================


FCCommand::FCCommand(const char* name,CMD_Type eType)
	: sName(name),_pcAction(0),_eType(eType)
{
	sAppModule		= "";
	sGroup			= "";

}

FCCommand::~FCCommand()
{
  delete _pcAction;
}

bool FCCommand::addTo(QWidget *pcWidget)
{
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction->addTo(pcWidget);
}

bool FCCommand::removeFrom(QWidget *pcWidget)
{
	if (!_pcAction)
    return false;
	return _pcAction->removeFrom(pcWidget);
}

 
ApplicationWindow *FCCommand::GetAppWnd(void)
{
	return ApplicationWindow::Instance;
}

FCGuiDocument* FCCommand::GetActiveDocument(void)
{
	return GetAppWnd()->GetActiveDocument();
}

App::Document*	   FCCommand::GetActiveOCCDocument(void)
{
	FCGuiDocument * pcDoc = GetAppWnd()->GetActiveDocument();
	if(pcDoc)
		return pcDoc->GetDocument();
	else
		return 0l;
}

QAction* FCCommand::GetAction() 
{ 
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction; 
}

bool FCCommand::IsToggle(void)
{
	return (_eType&Cmd_Toggle) != 0; 
}


void FCCommand::activated ()
{
	if(_eType == Cmd_Normal)
	{
		Base::Console().Log("Activate %s\n",_pcAction->text().latin1());
		Activated(0);
	}
}
void FCCommand::toggled (bool b)
{
	if(_eType == Cmd_Toggle)
	{
		Base::Console().Log("Toggled %s\n",_pcAction->text().latin1());
		if(b)
			Activated(1);
		else
			Activated(0);
	}
}

void FCCommand::TestActive(void)
{
	if(!_pcAction) return;
	
	bool bActive = IsActive();
	_pcAction->setEnabled ( bActive );
}


//--------------------------------------------------------------------------
// UNDO REDO transaction handling  
//--------------------------------------------------------------------------
/** Open a new Undo transaction on the active document
 *  This methode open a new UNDO transaction on the active document. This transaction
 *  will later apear in the UNDO REDO dialog with the name of the command. If the user 
 *  recall the transaction everything changed on the document between OpenCommand() and 
 *  CommitCommand will be undone (or redone). You can use an alternetive name for the 
 *  operation default is the Command name.
 *  @see CommitCommand(),AbortCommand()
 */
void FCCommand::OpenCommand(const char* sCmdName)
{
	// Using OpenCommand with no active document !
	assert(GetAppWnd()->GetActiveDocument());

	if(sCmdName)
		GetAppWnd()->GetActiveDocument()->OpenCommand(sCmdName);
	else
		GetAppWnd()->GetActiveDocument()->OpenCommand(sName.c_str());

}

void FCCommand::CommitCommand(void)
{
	GetAppWnd()->GetActiveDocument()->CommitCommand();
}

void FCCommand::AbortCommand(void)
{
	GetAppWnd()->GetActiveDocument()->AbortCommand();
}

/// Run a App level Action 
void FCCommand::DoCommand(DoCmd_Type eType,const char* sCmd,...)
{
	// temp buffer
	char* format = (char*) malloc(strlen(sCmd)+1024);
    va_list namelessVars;
    va_start(namelessVars, sCmd);  // Get the "..." vars
    vsprintf(format, sCmd, namelessVars);
    va_end(namelessVars);

	if(eType == Gui)
		GetAppWnd()->GetMacroMngr()->AddLine(FCMacroManager::Gui,format);
	else
		GetAppWnd()->GetMacroMngr()->AddLine(FCMacroManager::Base,format);
	Interpreter().RunFCCommand(format);

	free (format);

}

/// Activate an other Commands
void FCCommand::ActivateCommand(const char* sCmdName)
{
	FCCommand* pcCmd = GetAppWnd()->GetCommandManager().GetCommandByName(sCmdName);
	if(pcCmd)
	{
		assert(!(pcCmd->IsToggle()));
		pcCmd->Activated(0);
	}

}

/// Toggles other Commands
void FCCommand::ToggleCommand(const char* sCmdName,bool bToggle)
{
	FCCommand* pcCmd = GetAppWnd()->GetCommandManager().GetCommandByName(sCmdName);
	if(pcCmd)
	{
		assert(pcCmd->IsToggle());
		pcCmd->_pcAction->setOn(bToggle?1:0);
	}

}

/// Updates the (active) document (propagate changes)
void FCCommand::UpdateActive(void)
{

	GetAppWnd()->UpdateActive();
}

/// Updates the (all or listed) documents (propagate changes)
void FCCommand::UpdateAll(std::list<FCGuiDocument*> cList)
{
	if(cList.size()>0)
	{
		for(std::list<FCGuiDocument*>::iterator It= cList.begin();It!=cList.end();It++)
			(*It)->Update();
	}else{
		GetAppWnd()->Update();
	}

}

//--------------------------------------------------------------------------
// Online help handling  
//--------------------------------------------------------------------------

/// returns the begin of a online help page
const char * FCCommand::BeginCmdHelp(void)
{
	return  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
			"<html>\n"
			"<head>\n"
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n"
			"<title>FreeCAD Main Index</title>\n"
			"</head>\n"
			"<body bgcolor=\"#ffffff\">\n\n";
}
/// returns the end of a online help page
const char * FCCommand::EndCmdHelp(void)
{
	return "</body></html>\n\n";
}

//===========================================================================
// FCCppCommand 
//===========================================================================

FCCppCommand::FCCppCommand(const char* name,CMD_Type eType)
	:FCCommand(name,eType)
{
	sMenuText		  = "";
	sToolTipText	= "";
	sWhatsThis		= "";
	sStatusTip		= "";
  sPixmap			  = QString::null;
	iAccel			  = 0;
}

std::string FCCppCommand::GetResource(const char* sName)
{

	return "";

}


QAction * FCCppCommand::CreateAction(void)
{
	QAction *pcAction;

	pcAction = new Action(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(QObject::tr(sMenuText));
	pcAction->setMenuText(QObject::tr(sMenuText));
	pcAction->setToolTip(QObject::tr(sToolTipText));
	pcAction->setStatusTip(QObject::tr(sStatusTip));
	pcAction->setWhatsThis(QObject::tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
}

void FCCppCommand::languageChange()
{
  if ( _pcAction )
  {
	  _pcAction->setText(QObject::tr(sMenuText));
	  _pcAction->setMenuText(QObject::tr(sMenuText));
	  _pcAction->setToolTip(QObject::tr(sToolTipText));
	  _pcAction->setStatusTip(QObject::tr(sStatusTip));
	  _pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  }
}


//===========================================================================
// MacroCommand 
//===========================================================================

MacroCommand::MacroCommand(const char* name)
	:FCCppCommand(name,Cmd_Normal)
{
  sAppModule    = "";
  sGroup        = "Macros";
}

void MacroCommand::Activated(int iMsg)
{
//	OpenCommand("Excecute Macro");
//
//	DoCommand(Doc,"execfile(%s)",_sScriptName.c_str());
//
//	void CommitCommand(void);
  std::string cMacroPath = App::GetApplication().GetParameterGroupByPath
    ("User parameter:BaseApp/Preferences/Macro")->GetASCII("MacroPath",
    App::GetApplication().GetHomePath());

  QDir d( cMacroPath.c_str() );
  QFileInfo fi( d, scriptName );
  ApplicationWindow::Instance->GetMacroMngr()->Run(FCMacroManager::File,( fi.filePath() ).latin1());
}

void MacroCommand::SetScriptName ( const QString& s )
{
  scriptName = s;
}

void MacroCommand::SetWhatsThis( const QString& s )
{
  sWhatsThis = s;
  if ( _pcAction )
    _pcAction->setWhatsThis(QObject::tr(sWhatsThis));
}

void MacroCommand::SetMenuText( const QString& s )
{
  sMenuText = s;
  if ( _pcAction )
  {
    _pcAction->setText    (QObject::tr(sMenuText));
    _pcAction->setMenuText(QObject::tr(sMenuText));
  }
}

void MacroCommand::SetToolTipText( const QString& s )
{
  sToolTipText = s;
  if ( _pcAction )
    _pcAction->setToolTip(QObject::tr(sToolTipText));
}

void MacroCommand::SetStatusTip( const QString& s )
{
  sStatusTip = s;
  if ( _pcAction )
    _pcAction->setStatusTip(QObject::tr(sStatusTip));
}

void MacroCommand::SetPixmap( const QString& s )
{
  sPixmap = s;
  if ( _pcAction )
  {
    if ( sPixmap )
      _pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));
    else
      _pcAction->setIconSet(QPixmap());
  }
}

void MacroCommand::SetAccel(int i)
{
  iAccel = i;
  if ( _pcAction )
    _pcAction->setAccel(iAccel);
}

//===========================================================================
// FCPythonCommand
//===========================================================================


FCPythonCommand::FCPythonCommand(const char* name,PyObject * pcPyCommand)
	:FCCommand(name),_pcPyCommand(pcPyCommand)
{
	Py_INCREF(_pcPyCommand);

	// call the methode "GetResources()" of the command object
	_pcPyResourceDict = Interpreter().RunMethodObject(_pcPyCommand, "GetResources");
	// check if the "GetResources()" methode returns a Dict object
	if(! PyDict_Check(_pcPyResourceDict) )
		throw Base::Exception("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns the wrong type (has to be Py Dictonary)");

}

std::string FCPythonCommand::GetResource(const char* sName)
{
	PyObject* pcTemp;
	Base::PyBuf ResName(sName);


	// get the "MenuText" resource string
	pcTemp = PyDict_GetItemString(_pcPyResourceDict,ResName.str);
	if(! pcTemp )
		return std::string();
	if(! PyString_Check(pcTemp) )
		throw Base::Exception("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns a dictionary which holds not only strings");

	return std::string(PyString_AsString(pcTemp) );

}


void FCPythonCommand::Activated(int iMsg)
{
	try{
		Interpreter().RunMethodVoid(_pcPyCommand, "Activated");
	}catch (Base::Exception e){
		Base::Console().Error("Running the python command %s failed,try to resume",sName.c_str());
	}
}

bool FCPythonCommand::IsActive(void)
{
	return true;
}

std::string FCPythonCommand::CmdHelpURL(void)
{
	PyObject* pcTemp;

	pcTemp = Interpreter().RunMethodObject(_pcPyCommand, "CmdHelpURL"); 

	if(! pcTemp ) 
		return std::string();
	if(! PyString_Check(pcTemp) ) 
		throw Base::Exception("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
	
	return std::string( PyString_AsString(pcTemp) );
}

void FCPythonCommand::CmdHelpPage(std::string &rcHelpPage)
{
	PyObject* pcTemp;

	pcTemp = Interpreter().RunMethodObject(_pcPyCommand, "CmdHelpPage"); 

	if(! pcTemp ) 
		return ;
	if(! PyString_Check(pcTemp) ) 
		throw Base::Exception("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
	
	rcHelpPage = PyString_AsString(pcTemp) ;


}

QAction * FCPythonCommand::CreateAction(void)
{
	QAction *pcAction;

	pcAction = new Action(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(sName.c_str());
	pcAction->setMenuText(GetResource("MenuText").c_str());
	pcAction->setToolTip(GetResource("ToolTip").c_str());
	pcAction->setStatusTip(GetResource("StatusTip").c_str());
	pcAction->setWhatsThis(GetResource("WhatsThis").c_str());
	if(GetResource("Pixmap") != "")
		pcAction->setIconSet(Gui::BitmapFactory().pixmap(GetResource("Pixmap").c_str()));

	return pcAction;
}



//===========================================================================
// FCCommandManager 
//===========================================================================


void FCCommandManager::AddCommand(FCCommand* pCom)
{
	_sCommands[pCom->GetName()] = pCom;//	pCom->Init();
}

void FCCommandManager::RemoveCommand(FCCommand* pCom)
{
	std::map <std::string,FCCommand*>::iterator It = _sCommands.find(pCom->GetName());
  if (It != _sCommands.end())
  {
		delete It->second;
		_sCommands.erase(It);
  }
}

void FCCommandManager::AddTo(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) == _sCommands.end())
  {
	  Base::Console().Error("FCCommandManager::AddTo() try to add an unknown command (%s) to a widget!\n",Name);
  }
  else
  {
	  FCCommand* pCom = _sCommands[Name];
	  pCom->addTo(pcWidget);
  }
}

void FCCommandManager::RemoveFrom(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) != _sCommands.end())
  {
	  FCCommand* pCom = _sCommands[Name];
	  pCom->removeFrom(pcWidget);
  }
}

std::vector <FCCommand*> FCCommandManager::GetModuleCommands(const char *sModName)
{
	std::vector <FCCommand*> vCmds;

	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		if( strcmp(It->second->GetAppModuleName(),sModName) == 0)
			vCmds.push_back(It->second);
	}

	return vCmds;
}

std::string FCCommandManager::GetAppModuleName(QAction* pAction)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();++It)
	{
    if ( It->second->GetAction() == pAction )
      return It->second->GetAppModuleName();
	}

  return "Not found";
}

std::string FCCommandManager::GetAppModuleNameByName(const char* sName)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();++It)
	{
    if ( strcmp(It->second->GetName(), sName) == 0 )
      return It->second->GetAppModuleName();
	}

  return "Not found";
}

std::vector <FCCommand*> FCCommandManager::GetAllCommands(void)
{
	std::vector <FCCommand*> vCmds;

	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		vCmds.push_back(It->second);
	}

	return vCmds;
}

std::vector <FCCommand*> FCCommandManager::GetGroupCommands(const char *sGrpName)
{
	std::vector <FCCommand*> vCmds;

	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		if( strcmp(It->second->GetGroupName(),sGrpName) == 0)
			vCmds.push_back(It->second);
	}

	return vCmds;
}

FCCommand* FCCommandManager::GetCommandByName(const char* sName)
{
  FCCommand* pCom = NULL;
  if (_sCommands.find(sName) != _sCommands.end())
  {
    pCom = _sCommands[sName];
  }

  return pCom;
}

FCCommand* FCCommandManager::GetCommandByActionText(const char* sName)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
    if (It->second->GetAction())
    {
      if (It->second->GetAction()->text() == sName)
      {
        return It->second;
      }
    }
	}

  return NULL;
}

void FCCommandManager::RunCommandByName (const char* sName)
{
  FCCommand* pCmd = GetCommandByName(sName);

  if (pCmd)
    pCmd->activated();
}

void FCCommandManager::TestActive(void)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		It->second->TestActive();
	}
}
