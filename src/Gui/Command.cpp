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
/*
FCAction::FCAction ( const QString & text, const QIconSet & icon, const QString & menuText, int accel, QObject * parent, const char * name, bool toggle)
:QAction(text, icon, menuText, accel, parent, name, toggle)
{
}

FCAction::FCAction ( const QString & text, const QString & menuText, int accel, QObject * parent, const char * name, bool toggle)
:QAction(text, menuText, accel, parent, name, toggle)
{
}
*/
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

}

void FCCommand::Init(void)
{
	if(_pcAction) return;
	// create a action with the Application as parent (shortcuts)
	_pcAction = new FCAction(this,ApplicationWindow::Instance,_pcName,_eType&Cmd_Toggle != 0);

	// set standard profile here
	char*  sMenuText,*sToolTipText,*sWhatsThis,*sStatusTip,*sPixMap;
	sMenuText		= "No menu text! see CmdProfile()";
	sToolTipText	= "No Tooltip text! see CmdProfile()";
	sWhatsThis		= "";
	sStatusTip		= "";
	sPixMap         = NULL;
	int iAccel = 0;

	// Get the informations from the derifed class
	CmdProfile(&sMenuText,&sToolTipText,&sWhatsThis,&sStatusTip,&sPixMap,iAccel);

	// set the information and do Internationalization ( tr() )
	_pcAction->setText(_pcName);
	_pcAction->setMenuText(_pcAction->tr(sMenuText));
	_pcAction->setToolTip(_pcAction->tr(sToolTipText));
	_pcAction->setStatusTip(_pcAction->tr(sStatusTip));
	_pcAction->setWhatsThis(_pcAction->tr(sWhatsThis));
	if(sPixMap)
		_pcAction->setIconSet(ApplicationWindow::Instance->GetBmpFactory().GetPixmap(sPixMap));
	_pcAction->setAccel(iAccel);

	//connect( _pcAction, SIGNAL( activated() ) , this, SLOT( activated() ) );
	//connect( _pcAction, SIGNAL( toggled(bool) )   , this, SLOT( toggled(bool) )   );

}

ApplicationWindow *FCCommand::AppWnd(void)
{
	return ApplicationWindow::Instance;
}

FCGuiDocument* FCCommand::GetActiveDocument(void)
{
	return AppWnd()->GetActiveDocument();
}

FCDocument*	   FCCommand::GetActiveOCCDocument(void)
{
	FCGuiDocument * pcDoc = AppWnd()->GetActiveDocument();
	if(pcDoc)
		return pcDoc->GetDocument();
	else
		return 0l;
}

bool FCCommand::IsToggle(void)
{
	return _eType&Cmd_Toggle != 0; 
}

const char* FCCommand::Name(void)
{
	return _pcName; 
}


//--------------------------------------------------------------------------
// slots from QT 
//--------------------------------------------------------------------------
void FCCommand::activated ()
{
	GetConsole().Log("Activate %s\n",_pcAction->text().latin1());
	Activated();
}
void FCCommand::toggled ( bool bState)
{
	GetConsole().Log("Toggle %s\n",_pcAction->text().latin1());
	Toogled(bState);
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



void FCCommandManager::AddCommand(FCCommand* pCom)
{
	_sCommands[pCom->Name()] = pCom;	pCom->Init();
}

void FCCommandManager::AddTo(const char* Name,QWidget *pcWidget)
{
	FCCommand* pCom = _sCommands[Name];
	if(!pCom) throw FCException("Wrong command name");
	pCom->Init();
	pCom->_pcAction->addTo(pcWidget);
}







#include "moc_Command.cpp"

