/***************************************************************************
                          Command.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2002
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif

#include "../Base/Console.h"

#include "Command.h"
#include "Application.h"
#include "Document.h"
#include "ButtonGroup.h"
#include "../Base/Exception.h"
#include "../Base/Interpreter.h"

#include "Icons/x.xpm"
#include <qobjcoll.h>

//===========================================================================
// FCAction 
//===========================================================================
FCAction::FCAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle)
:QAction(parent, name, toggle),_pcCmd(pcCmd)
{
	connect( this, SIGNAL( activated() ) , this, SLOT( Activated() ) );
	connect( this, SIGNAL( toggled(bool) )   , this, SLOT( Toggled(bool) )   );

}

FCAction::~FCAction()
{
}

bool FCAction::addTo(QWidget *w)
{
  if (QAction::addTo(w) == true)
  {
    if (w->inherits("FCToolboxBar"))
    {
  		((FCToolboxBar*)w)->addedButton(menuText());
    }
    else if (w->inherits("FCOutlookBar"))
    {
  		((FCOutlookBar*)w)->addedButton(menuText());
    }
  }
  else
    return false;

  return true;
}


void FCAction::Activated () 
{
	_pcCmd->activated();
}
void FCAction::Toggled ( bool b)
{
	_pcCmd->toggled(b);
} 

//===========================================================================
// FCCommand 
//===========================================================================


FCCommand::FCCommand(const char* name,CMD_Type eType)
	:_eType(eType),sName(name),_pcAction(0)
{
	sAppModule		= "";
	sGroup			= "";

}


bool FCCommand::addTo(QWidget *pcWidget)
{
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction->addTo(pcWidget);
}

 
ApplicationWindow *FCCommand::GetAppWnd(void)
{
	return ApplicationWindow::Instance;
}

FCGuiDocument* FCCommand::GetActiveDocument(void)
{
	return GetAppWnd()->GetActiveDocument();
}

FCDocument*	   FCCommand::GetActiveOCCDocument(void)
{
	FCGuiDocument * pcDoc = GetAppWnd()->GetActiveDocument();
	if(pcDoc)
		return pcDoc->GetDocument();
	else
		return 0l;
}

FCAction* FCCommand::GetAction() 
{ 
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction; 
}

bool FCCommand::IsToggle(void)
{
	return _eType&Cmd_Toggle != 0; 
}


void FCCommand::activated ()
{
	GetConsole().Log("Activate %s\n",_pcAction->text().latin1());
	Activated(0);
}
void FCCommand::toggled (bool b)
{
	GetConsole().Log("Toggled %s\n",_pcAction->text().latin1());
	if(b)
		Activated(1);
	else
		Activated(0);
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
	sMenuText		="Not set!!";
	sToolTipText	="Not set!!";
	sWhatsThis		="Not set!!";
	sStatusTip		="Not set!!";
	sPixmap			=0;
	iAccel			=0;

}

std::string FCCppCommand::GetResource(const char* sName)
{

	return "";

}


FCAction * FCCppCommand::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCAction(this,ApplicationWindow::Instance,sName.c_str(),_eType&Cmd_Toggle != 0);
	pcAction->setText(sName.c_str());
	pcAction->setMenuText(_pcAction->tr(sMenuText));
	pcAction->setToolTip(_pcAction->tr(sToolTipText));
	pcAction->setStatusTip(_pcAction->tr(sStatusTip));
	pcAction->setWhatsThis(_pcAction->tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
}




//===========================================================================
// FCPythonCommand 
//===========================================================================


FCPythonCommand::FCPythonCommand(const char* name,PyObject * pcPyCommand)
	:FCCommand(name),_pcPyCommand(pcPyCommand)
{
	Py_INCREF(_pcPyCommand);

	// call the methode "GetResources()" of the command object
	_pcPyResourceDict = GetInterpreter().RunMethodObject(_pcPyCommand, "GetResources");
	// check if the "GetResources()" methode returns a Dict object
	if(! PyDict_Check(_pcPyResourceDict) ) 
		throw FCException("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns the wrong type (has to be Py Dictonary)");	

}

std::string FCPythonCommand::GetResource(const char* sName)
{
	PyObject* pcTemp;
	PyBuf ResName(sName);


	// get the "MenuText" resource string 
	pcTemp = PyDict_GetItemString(_pcPyResourceDict,ResName.str);
	if(! pcTemp ) 
		return std::string();
	if(! PyString_Check(pcTemp) ) 
		throw FCException("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns a dictionary which holds not only strings");

	return std::string(PyString_AsString(pcTemp) );

}


void FCPythonCommand::Activated(int iMsg)
{
	GetInterpreter().RunMethodVoid(_pcPyCommand, "Activated");
}

bool FCPythonCommand::IsActive(void)
{
	return true;
}

std::string FCPythonCommand::CmdHelpURL(void)
{
	PyObject* pcTemp;

	pcTemp = GetInterpreter().RunMethodObject(_pcPyCommand, "CmdHelpURL"); 

	if(! pcTemp ) 
		return std::string();
	if(! PyString_Check(pcTemp) ) 
		throw FCException("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
	
	return std::string( PyString_AsString(pcTemp) );
}

void FCPythonCommand::CmdHelpPage(std::string &rcHelpPage)
{
	PyObject* pcTemp;

	pcTemp = GetInterpreter().RunMethodObject(_pcPyCommand, "CmdHelpPage"); 

	if(! pcTemp ) 
		return ;
	if(! PyString_Check(pcTemp) ) 
		throw FCException("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
	
	rcHelpPage = PyString_AsString(pcTemp) ;


}

FCAction * FCPythonCommand::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCAction(this,ApplicationWindow::Instance,sName.c_str(),_eType&Cmd_Toggle != 0);
	pcAction->setText(sName.c_str());
	pcAction->setMenuText(GetResource("MenuText").c_str());
	pcAction->setToolTip(GetResource("ToolTip").c_str());
	pcAction->setStatusTip(GetResource("StatusTip").c_str());
	pcAction->setWhatsThis(GetResource("WhatsThis").c_str());
	if(GetResource("Pixmap") != "")
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(GetResource("Pixmap").c_str()));

	return pcAction;
}



//===========================================================================
// FCCommandManager 
//===========================================================================


void FCCommandManager::AddCommand(FCCommand* pCom)
{
	_sCommands[pCom->GetName()] = pCom;//	pCom->Init();
}

void FCCommandManager::AddTo(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) == _sCommands.end())
  {
		GetConsole().Error("FCCommandManager::AddTo() try to add a unknown command (%s) to a widget!\n",Name);
  }
  else
  {
	  FCCommand* pCom = _sCommands[Name];
	  pCom->addTo(pcWidget);
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

std::vector <FCCommand*> FCCommandManager::GetAllCommands(void)
{
	std::vector <FCCommand*> vCmds;

	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		vCmds.push_back(It->second);
	}

	return vCmds;
}


void FCCommandManager::TestActive(void)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		It->second->TestActive();
	}
}



#include "moc_Command.cpp"

