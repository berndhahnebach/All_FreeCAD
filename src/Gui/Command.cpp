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

#include "Icons/x.xpm"

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
	if ( w->inherits( "FCToolboxGroup" ) ) 
	{
		FCToolboxButton* btn = new FCToolboxButton(menuText(), iconSet().pixmap(), toolTip(), w);
		btn->setFixedHeight(32);
		
		connect( btn, SIGNAL( clicked() ), this, SIGNAL( activated() ) );
		connect( btn, SIGNAL( toggled(bool) ), this, SLOT( toolButtonToggled(bool) ) );
		connect( btn, SIGNAL( destroyed() ), this, SLOT( objectDestroyed() ) );
		// for disconnecting later
		btn->setAction(this);
		((FCToolboxGroup*)w)->addedButton();
		return true;
	}
	else
		return QAction::addTo(w);
}


void FCAction::Activated () 
{
	puts("Activeted");
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
	:_eType(eType),_pcName(name),_pcAction(0)
{
	sAppModule		= 0;
	sMenuText		= "No menu text! see CmdProfile()";
	sToolTipText	= "No Tooltip text! see CmdProfile()";
	sWhatsThis		= "";
	sStatusTip		= "";
	sPixmap         = NULL;
	int iAccel = 0;

}


bool FCCommand::addTo(QWidget *pcWidget)
{
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction->addTo(pcWidget);
}

FCAction * FCCommand::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCAction(this,ApplicationWindow::Instance,_pcName,_eType&Cmd_Toggle != 0);
	pcAction->setText(_pcName);
	pcAction->setMenuText(_pcAction->tr(sMenuText));
	pcAction->setToolTip(_pcAction->tr(sToolTipText));
	pcAction->setStatusTip(_pcAction->tr(sStatusTip));
	pcAction->setWhatsThis(_pcAction->tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
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
	if(_pcAction)
		_pcAction->setEnabled ( IsActive() );
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
void FCCommand::OpenCommand(const char* sName)
{

}

void FCCommand::CommitCommand(void)
{

}

void FCCommand::AborCommand(void)
{

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


/*


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCAction::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						
	"FCAction",				
	sizeof(FCAction),		
	0,						
	
	PyDestructor,	  		
	0,			 			
	__getattr, 				
	__setattr, 				
	0,						
	__repr,					
	0,						
	0,						
	0,						
	0,						
	0,						
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef FCAction::Methods[] = {
  {"Do",         (PyCFunction) sPyDo,         Py_NEWARGS},

  {NULL, NULL}	
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCAction::Parents[] = {&FCAction::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCAction::FCAction(const char* Name) 
 : QAction(),FCPyObject( &Type ),sName(Name)
{
	GetConsole().Log("Create Action %s\n",sName.c_str());
}

PyObject *FCAction::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return new FCAction("");			// Make new Python-able object
  //return 0;
}

//--------------------------------------------------------------------------
//  FCLabel destructor 
//--------------------------------------------------------------------------
FCAction::~FCAction()						// Everything handled in parent
{
	GetConsole().Log("Destroy Action %s\n",sName.c_str());
} 

//--------------------------------------------------------------------------
// FCLabel Attributes
//--------------------------------------------------------------------------
PyObject *FCAction::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (streq(attr, "Text"))						// accessable new state
			return Py_BuildValue("s",text()); 
		else
			_getattr_up(FCPyObject); 						// send to parent
	}catch(...){
		GetConsole().Log("Exception in FCAction::_getattr()\n");
		return 0;
	}
} 

int FCAction::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "Name"))						// settable new state
		setText(PyString_AsString(value)); 
		//TDataStd_Name::Set(_cLabel, (short*)PyUnicode_AsUnicode(value)); 
	else  
 
		return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCAction::PyDo(PyObject *args)
{ 
	int Tag;
    if (!PyArg_ParseTuple(args, "i",&Tag ))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return new FCLabel( _cLabel.FindChild(Tag) );
	return 0;
} 

*/

//===========================================================================
// FCCommandManager 
//===========================================================================


void FCCommandManager::AddCommand(FCCommand* pCom)
{
	_sCommands[pCom->GetName()] = pCom;//	pCom->Init();
}

void FCCommandManager::AddTo(const char* Name,QWidget *pcWidget)
{
	FCCommand* pCom = _sCommands[Name];

	if(!pCom) 
		GetConsole().Error("FCCommandManager::AddTo() try to add a unknown command (%s) to a widget!\n",Name);
	else
		pCom->addTo(pcWidget);
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

