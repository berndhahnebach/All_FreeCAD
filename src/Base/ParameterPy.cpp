/** \file ParameterPy.cpp
 *  \brief The base::parameter module python bindings
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here is the implementation of the parameter mimic.
 *  @see Parameter.h FCParameter FCParameterManager
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/



/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <fcntl.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	ifdef FC_OS_WIN32
#		include <io.h>
#   include <xercesc/sax/SAXParseException.hpp>
#	endif
#	include <stdio.h>
#endif


#include <fcntl.h>
#ifdef FC_OS_LINUX
#	include <unistd.h>
#endif

#include "Parameter.h"
#include "Exception.h"
#include "Console.h"
#include "PyExportImp.h"

using namespace Base;

//**************************************************************************
//**************************************************************************
// FCPyParametrGrp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



/** The ParameterGrp wrapper class
 */
class FCPyParameterGrp :public Base::FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

public:


	//---------------------------------------------------------------------
	// construction / destruction +++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	/// Constructor 
	FCPyParameterGrp(const FCHandle<FCParameterGrp> &rcParamGrp, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCPyParameterGrp();

	//---------------------------------------------------------------------
	// python exports  ++++++++++++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PYFUNCDEF_D (FCPyParameterGrp,PyGetGrp);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemGrp);
	PYFUNCDEF_D (FCPyParameterGrp,PyClear);
	PYFUNCDEF_D (FCPyParameterGrp,PyHasGroup);
	PYFUNCDEF_D (FCPyParameterGrp,PyIsEmpty);

	PYFUNCDEF_D (FCPyParameterGrp,PySetBool);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetBool);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemBool);

	PYFUNCDEF_D (FCPyParameterGrp,PySetInt);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetInt);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemInt);

	PYFUNCDEF_D (FCPyParameterGrp,PySetFloat);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetFloat);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemFloat);

	PYFUNCDEF_D (FCPyParameterGrp,PySetString);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetString);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemString);


protected:

	/// Pointer to the FCDocument where the label comes from 
	FCHandle<FCParameterGrp> _cParamGrp;
};


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCPyParameterGrp::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCParameterGrp",		/*tp_name*/
	sizeof(FCPyParameterGrp),/*tp_basicsize*/
	0,						/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,					/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef FCPyParameterGrp::Methods[] = {
  {"GetGroup",         (PyCFunction) sPyGetGrp,          Py_NEWARGS},
  {"RemGroup",         (PyCFunction) sPyRemGrp,          Py_NEWARGS},
  {"HasGroup",         (PyCFunction) sPyHasGroup,        Py_NEWARGS},
  {"IsEmpty",          (PyCFunction) sPyIsEmpty,         Py_NEWARGS},
  {"Clear",            (PyCFunction) sPyClear,           Py_NEWARGS},

  {"SetBool",          (PyCFunction) sPySetBool,         Py_NEWARGS},
  {"GetBool",          (PyCFunction) sPyGetBool,         Py_NEWARGS},
  {"RemBool",          (PyCFunction) sPyRemBool,         Py_NEWARGS},

  {"SetInt",           (PyCFunction) sPySetInt,          Py_NEWARGS},
  {"GetInt",           (PyCFunction) sPyGetInt,          Py_NEWARGS},
  {"RemInt",           (PyCFunction) sPyRemInt,          Py_NEWARGS},

  {"SetFloat",         (PyCFunction) sPySetFloat,        Py_NEWARGS},
  {"GetFloat",         (PyCFunction) sPyGetFloat,        Py_NEWARGS},
  {"RemFloat",         (PyCFunction) sPyRemFloat,        Py_NEWARGS},

  {"SetString",        (PyCFunction) sPySetString,       Py_NEWARGS},
  {"GetString",        (PyCFunction) sPyGetString,       Py_NEWARGS},
  {"RemString",        (PyCFunction) sPyRemString,       Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCPyParameterGrp::Parents[] = {&FCPyObject::Type,&FCPyParameterGrp::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCPyParameterGrp::FCPyParameterGrp(const FCHandle<FCParameterGrp> &rcParamGrp, PyTypeObject *T ) 
 : FCPyObject( T),_cParamGrp(rcParamGrp)
{
	//Console().Log("Create Param Group %p\n",this);
}

PyObject *FCPyParameterGrp::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCPyParameterGrp();			// Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  FCPyParametrGrp destructor 
//--------------------------------------------------------------------------
FCPyParameterGrp::~FCPyParameterGrp()						// Everything handled in parent
{
	//Console().Log("Destroy ParameterGrp %p\n",this);
} 

//--------------------------------------------------------------------------
// FCPyParametrGrp Attributes
//--------------------------------------------------------------------------
PyObject *FCPyParameterGrp::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	/*
	try{
		// Access the number of attributes at this FCPyParametrGrp
		if (streq(attr, "AttributeCount"))						
			return Py_BuildValue("i", _cLabel.NbAttributes()); 
		else if (streq(attr, "ChildrenCount"))					
			return Py_BuildValue("i", _cLabel.NbChildren()); 
		else if (streq(attr, "Root"))						
			return _pcDocument->HasPyLabel( _cLabel.Root()); 
		else if (streq(attr, "Father"))						
			return _pcDocument->HasPyLabel( _cLabel.Father()); 
		else if (streq(attr, "Real")){
			Handle(TDataStd_Real) RealAttr;
			if(_cLabel.FindAttribute(TDataStd_Real::GetID(),RealAttr))
				return Py_BuildValue("d",RealAttr->Get()); 
			else
				return 0;
		}else if (streq(attr, "Int")){
			Handle(TDataStd_Integer) IntAttr;
			if(_cLabel.FindAttribute(TDataStd_Integer::GetID(),IntAttr))
				return Py_BuildValue("d",IntAttr->Get()); 
			else
				return 0;
		}else if (streq(attr, "Name")){
			Handle(TDataStd_Name) NameAttr;
			if(_cLabel.FindAttribute(TDataStd_Name::GetID(),NameAttr))
				//return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#ifdef FC_OS_LINUX // u is unicode as ToExtString is!
				return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#else
				return Py_BuildValue("s",NameAttr->Get()); 
#endif				
			else
				return 0;
		}else*/
			_getattr_up(FCPyObject); 						// send to parent
/*	}catch(...){
		Console().Log("Exception in FCPyParametrGrp::_getattr()\n");
		return 0;
	}*/
		return 0;
} 

int FCPyParameterGrp::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ /*
	if (streq(attr, "Real"))						// settable new state
		TDataStd_Real::Set(_cLabel, PyFloat_AsDouble(value)); 
	else if (streq(attr, "Int"))						// settable new state
		TDataStd_Integer::Set(_cLabel, PyInt_AsLong(value)); 
	else if (streq(attr, "Name"))						// settable new state
		TDataStd_Name::Set(_cLabel, (short*)PyString_AsString(value)); 
		//TDataStd_Name::Set(_cLabel, (short*)PyUnicode_AsUnicode(value)); 
	else  
 */
		return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCPyParameterGrp::PyGetGrp(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	// get the Handle of the wanted group
	FCHandle<FCParameterGrp> handle = _cParamGrp->GetGroup(pstr);
	if(handle.IsValid()){
		// crate a python wrapper class
		FCPyParameterGrp *pcParamGrp = new FCPyParameterGrp(handle);
		// increment the reff count
		//pcParamGrp->_INCREF();
		return pcParamGrp;
	}else{
		PyErr_SetString(PyExc_IOError, "GetGroup failed");
		return 0L;
	}
} 

PyObject *FCPyParameterGrp::PySetBool(PyObject *args)
{ 
	char *pstr;
	int  Bool;
    if (!PyArg_ParseTuple(args, "si", &pstr,&Bool))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->SetBool(pstr,Bool!=0);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyGetBool(PyObject *args)
{ 
	char *pstr;
	int  Bool=0;
    if (!PyArg_ParseTuple(args, "s|i", &pstr,&Bool))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	return Py_BuildValue("i",_cParamGrp->GetBool(pstr,Bool!=0));
} 

PyObject *FCPyParameterGrp::PySetInt(PyObject *args)
{ 
	char *pstr;
	long  Int;
    if (!PyArg_ParseTuple(args, "si", &pstr,&Int))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->SetInt(pstr,Int);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyGetInt(PyObject *args)
{ 
	char *pstr;
	long  Int=0;
    if (!PyArg_ParseTuple(args, "s|i", &pstr,&Int))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	return Py_BuildValue("i",_cParamGrp->GetInt(pstr,Int));
} 

PyObject *FCPyParameterGrp::PySetFloat(PyObject *args)
{ 
	char *pstr;
	double  Float;
    if (!PyArg_ParseTuple(args, "sd", &pstr,&Float))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->SetFloat(pstr,Float);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyGetFloat(PyObject *args)
{ 
	char *pstr;
	double  Float=0.0;
    if (!PyArg_ParseTuple(args, "s|d", &pstr,&Float))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	return Py_BuildValue("d",_cParamGrp->GetFloat(pstr,Float));
} 

PyObject *FCPyParameterGrp::PySetString(PyObject *args)
{ 
	char *pstr;
	char *  str;
    if (!PyArg_ParseTuple(args, "ss", &pstr,&str))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->SetASCII(pstr,str);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyGetString(PyObject *args)
{ 
	char *pstr;
	char *  str="";
    if (!PyArg_ParseTuple(args, "s|d", &pstr,&str))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	return Py_BuildValue("s",_cParamGrp->GetASCII(pstr,str).c_str());
} 

//----

PyObject *FCPyParameterGrp::PyRemInt(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->RemoveInt(pstr);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyRemBool(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->RemoveBool(pstr);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyRemGrp(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->RemoveGrp(pstr);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyRemFloat(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->RemoveFloat(pstr);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyRemString(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->RemoveASCII(pstr);
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyClear(PyObject *args)
{ 
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	_cParamGrp->Clear();
	Py_Return; 
} 

PyObject *FCPyParameterGrp::PyIsEmpty(PyObject *args)
{ 
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return Py_BuildValue("i",_cParamGrp->IsEmpty());
} 

PyObject *FCPyParameterGrp::PyHasGroup(PyObject *args)
{ 
	char *pstr;
    if (!PyArg_ParseTuple(args, "s"),&pstr)     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	
	return Py_BuildValue("i",_cParamGrp->HasGroup(pstr));
} 



/** python wrapper function
*/
PyObject* GetPyObject( const FCHandle<FCParameterGrp> &hcParamGrp)
{

	return new FCPyParameterGrp(hcParamGrp); 
}



