/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <TDF_ChildIterator.hxx>
# include <TDF_Tool.hxx>
# include <TCollection_AsciiString.hxx>
# include <TDF_ListIteratorOfAttributeList.hxx>
# include <TFunction_Logbook.hxx>
# include <TFunction_DriverTable.hxx>
# include <TFunction_Function.hxx>
# include <Standard_GUID.hxx>
# include <TNaming_Builder.hxx>
# include <TDataStd_Name.hxx>
# include <sstream>
#endif

#include <Base/PyExportImp.h>
#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;

#include "Document.h"
#include "FeatureDataProviderPy.h"

using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FeatureDataProviderPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FeatureDataProviderPy",				/*tp_name*/
	sizeof(FeatureDataProviderPy),			/*tp_basicsize*/
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
PyMethodDef FeatureDataProviderPy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},
//	PYMETHODEDEF(AddFeatureDataProvider)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FeatureDataProviderPy::Parents[] = {&PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
FeatureDataProviderPy::FeatureDataProviderPy(FeatureDataProvider *pcFeatureDataProvider, PyTypeObject *T)
: PyObjectBase( T), _pcFeatureDataProvider(pcFeatureDataProvider)
{
	Base::Console().Log("Create FeatureDataProviderPy: %p \n",this);
}

PyObject *FeatureDataProviderPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FeatureDataProviderPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
FeatureDataProviderPy::~FeatureDataProviderPy()						// Everything handled in parent
{
	Base::Console().Log("Destroy FeatureDataProviderPy: %p \n",this);
}


//--------------------------------------------------------------------------
// FeatureDataProviderPy representation
//--------------------------------------------------------------------------
PyObject *FeatureDataProviderPy::_repr(void)
{
  std::stringstream a;
  a << "FeatureDataProvider: [ ";
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// FeatureDataProviderPy Attributes
//--------------------------------------------------------------------------
PyObject *FeatureDataProviderPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	PY_TRY{
		if (Base::streq(attr, "XXXX"))						
			return Py_BuildValue("i",1); 
		else
		  _getattr_up(PyObjectBase); 						
	}PY_CATCH;
} 

int FeatureDataProviderPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (Base::streq(attr, "XXXX")){						// settable new state
		return 1;
  }else
	  return PyObjectBase::_setattr(attr, value); 						
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
/*
PYFUNCIMP_D(FeatureDataProviderPy,AddFeatureDataProvider)
{
	//char *pstr;
    //if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    //    return NULL;                             // NULL triggers exception 

	Py_Error(PyExc_Exception,"Not implemented!");
}
*/

