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

#include "../base/Console.h"

#include "Command.h"


/*


//===========================================================================
// FCAction - Warpper for the QAction classe
//===========================================================================


//--------------------------------------------------------------------------
// signals from QT 
//--------------------------------------------------------------------------
void FCAction::activated ()
{
	GetConsole().Log("Activate %s\n",sName.c_str());
}
void FCAction::toggled ( bool )
{
	GetConsole().Log("Toggle %s\n",sName.c_str());
}




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