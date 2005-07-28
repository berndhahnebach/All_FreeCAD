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
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <TFunction_Logbook.hxx>
#	include <TFunction_DriverTable.hxx>
#	include <TFunction_Function.hxx>
# include <TNaming_Builder.hxx>
# include <TNaming_NamedShape.hxx>
# include <TopoDS_Shape.hxx>
#	include <Standard_GUID.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;
#include <App/Topology.h>


#include "PartFeature.h"
#include "PartFeaturePy.h"
//#include "Function.h"
//#include "Property.h"
//#include "PropertyAttr.h"

using namespace Part;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject PartFeaturePy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"PartFeaturePy",				/*tp_name*/
	sizeof(PartFeaturePy),			/*tp_basicsize*/
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
PyMethodDef PartFeaturePy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},
	PYMETHODEDEF(getShape)
	PYMETHODEDEF(setShape)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PartFeaturePy::Parents[] = {&PartFeaturePy::Type ,&App::FeaturePy::Type,&PyObjectBase::Type,  NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
PartFeaturePy::PartFeaturePy(PartFeature *pcFeature, PyTypeObject *T)
: App::FeaturePy(pcFeature, T), _pcFeature(pcFeature)
{
	Base::Console().Log("Create PartFeaturePy: %p \n",this);
}

PyObject *PartFeaturePy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new PartFeaturePy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
PartFeaturePy::~PartFeaturePy()						// Everything handled in parent
{
	Base::Console().Log("Destroy PartFeaturePy: %p \n",this);
} 


//--------------------------------------------------------------------------
// PartFeaturePy representation
//--------------------------------------------------------------------------
PyObject *PartFeaturePy::_repr(void)
{
  std::stringstream a;
  a << "PartFeature: [ ";
//  for(std::map<std::string,int>::const_iterator It = _pcFeature->_PropertiesMap.begin();It!=_pcFeature->_PropertiesMap.end();It++)
//  {
//    a << It->first << "=" << _pcFeature->GetProperty(It->first.c_str()).GetAsString() << "; ";
//  }
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// PartFeaturePy Attributes
//--------------------------------------------------------------------------
PyObject *PartFeaturePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
			  _getattr_up(FeaturePy); 						
} 

int PartFeaturePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
  return FeaturePy::_setattr(attr, value); 						
} 



//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(PartFeaturePy,getShape)
{
	//char *pstr;
    //if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    //    return NULL;                             // NULL triggers exception 

  return new App::TopoShapePy(_pcFeature->getShape());
	//Py_Error(PyExc_Exception,"Not implemented!");
}

PYFUNCIMP_D(PartFeaturePy,setShape)
{
  App::TopoShapePy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(App::TopoShapePy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (App::TopoShapePy*)pcObj;

  // copy in the Feature Mesh
  _pcFeature->setShape(pcObject->getShape());

  Py_Return;
}
