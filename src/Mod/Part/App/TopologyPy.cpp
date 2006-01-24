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


#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <sstream>
#endif

#include <Base/Exception.h>
#include <Base/Console.h>
#include "TopologyPy.h"

using Base::Console;

using namespace Part;


//===========================================================================
// TopoShapePy - Warpper for the TopoDS classes
//===========================================================================

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject TopoShapePy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"TopoShapePy",				/*tp_name*/
	sizeof(TopoShapePy),	/*tp_basicsize*/
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
PyMethodDef TopoShapePy::Methods[] = {
  {"HasChild",         (PyCFunction) sPyHasChild,         Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject TopoShapePy::Parents[] = {&Base::PyObjectBase::Type,&TopoShapePy::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
TopoShapePy::TopoShapePy(const TopoDS_Shape &cShape, PyTypeObject *T) 
 : PyObjectBase( T), _cTopoShape(cShape)
{
	Console().Log("Create TopoShape %p\n",this);
}

PyObject *TopoShapePy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  return new TopoShapePy(TopoDS_Shape());			// Make new Python-able object
}

//--------------------------------------------------------------------------
//  TopoShapePy destructor 
//--------------------------------------------------------------------------
TopoShapePy::~TopoShapePy()						// Everything handled in parent
{
	Console().Log("Destroy TopoShape %p\n",this);
} 

//--------------------------------------------------------------------------
// TopoShapePy Attributes
//--------------------------------------------------------------------------
PyObject *TopoShapePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		// Access the number of attributes at this label
		if (Base::streq(attr, "AttributeCount"))
		{
			return Py_BuildValue("i", 1); 
		}else
			_getattr_up(PyObjectBase); 						// send to parent
	}catch(...){
		Console().Log("Exception in TopoShapePy::_getattr()\n");
		return 0;
	}
} 

int TopoShapePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (Base::streq(attr, "Real"))						// settable new state
		; 
	else  
		return PyObjectBase::_setattr(attr, value);	// send up to parent
	return 0;
} 

//--------------------------------------------------------------------------
// PartFeaturePy representation
//--------------------------------------------------------------------------
PyObject *TopoShapePy::_repr(void)
{
  std::stringstream a;
  a << "TopoShape: [ ";
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *TopoShapePy::PyHasChild(PyObject *args)
{ 
  if (!PyArg_ParseTuple(args, "" ))   
    return NULL;                      
	Py_Return; 
}
 




//**************************************************************************
//**************************************************************************
// Seperator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



