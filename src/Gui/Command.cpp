/***************************************************************************
                          Command.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2002
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif

#include "../Base/Console.h"

#include "Command.h"
#include "Application.h"

#include "Icons/x.xpm"


//===========================================================================
// FCCommand 
//===========================================================================


FCCommand::FCCommand(const char* name,CMD_Type eType)
	:_eType(eType),_pcName(name)
{

}

void FCCommand::Init(void)
{
	// create a action with the Application as parent (shortcuts)
	_pcAction = new QAction(ApplicationWindow::Instance,_pcName,_eType&Cmd_Toggle != 0);

	// set standard profile here
	char*  sMenuText,*sToolTipText,*sWhatsThis,*sStatusTip;
	sMenuText		= "No menu text! see CmdProfile()";
	sToolTipText	= "No Tooltip text! see CmdProfile()";
	sWhatsThis		= "";
	sStatusTip		= "";
	QPixmap cPixmap(px);
	int iAccel = 0;

	// Get the informations from the derifed class
	CmdProfile(&sMenuText,&sToolTipText,&sWhatsThis,&sStatusTip,cPixmap,iAccel);

	// set the information and do Internationalization ( tr() )
	_pcAction->setText(_pcName);
	_pcAction->setMenuText(_pcAction->tr(sMenuText));
	_pcAction->setToolTip(_pcAction->tr(sToolTipText));
	_pcAction->setStatusTip(_pcAction->tr(sStatusTip));
	_pcAction->setWhatsThis(_pcAction->tr(sWhatsThis));
	_pcAction->setIconSet(cPixmap);
	_pcAction->setAccel(iAccel);

	connect( _pcAction, SIGNAL( activated() ) , this, SLOT( activated() ) );
	connect( _pcAction, SIGNAL( toggled(bool) )   , this, SLOT( toggled(bool) )   );

}

ApplicationWindow *FCCommand::AppWnd(void)
{
	return ApplicationWindow::Instance;
}


bool FCCommand::IsToggle(void)
{
	return _eType&Cmd_Toggle != 0; 
}

const char* FCCommand::Name(void)
{
	return _pcAction->text().latin1(); 
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
	pCom->Init();
	_sCommands[pCom->Name()] = pCom;	
}

void FCCommandManager::AddTo(const char* Name,QWidget *pcWidget)
{
	_sCommands[Name]->_pcAction->addTo(pcWidget);
}







#include "Command_moc.cpp"

