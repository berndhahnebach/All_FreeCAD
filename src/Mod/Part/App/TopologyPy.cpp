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
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <Interface_Static.hxx>
#endif

#include <Base/Exception.h>
#include <Base/FileInfo.h>
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
	"TopoShape",				/*tp_name*/
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
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_HAVE_CLASS,                            /*tp_flags */
  "About shape",                               /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  TopoShapePy::Methods,                             /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &Base::PyObjectBase::Type,                        /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  PyInit,                                           /*tp_init */
  0,                                                /*tp_alloc */
  PyMake,                                           /*tp_new */
  0,                                                /*tp_free   Low-level free-memory routine */
  0,                                                /*tp_is_gc  For PyObject_IS_GC */
  0,                                                /*tp_bases */
  0,                                                /*tp_mro    method resolution order */
  0,                                                /*tp_cache */
  0,                                                /*tp_subclasses */
  0                                                 /*tp_weaklist */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef TopoShapePy::Methods[] = {
  {"hasChild",         (PyCFunction) shasChild,         Py_NEWARGS},
  {"readIges",         (PyCFunction) sreadIges,         Py_NEWARGS},
  {"writeIges",        (PyCFunction) swriteIges,        Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

PyObject *TopoShapePy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)  // Python wrapper
{
  return new TopoShapePy();
}

int TopoShapePy::PyInit(PyObject* self, PyObject* args, PyObject*)
{
  PyObject *pcObj=0;
  if (!PyArg_ParseTuple(args, "|O!", &(TopoShapePy::Type), &pcObj))     // convert args: Python->C 
    return -1;                             // NULL triggers exception 

  if ( pcObj )
  {
    TopoShapePy* pcShape = (TopoShapePy*)pcObj;
    ((TopoShapePy*)self)->_cTopoShape = pcShape->_cTopoShape;
  }

  return 0;
}

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject TopoShapePy::Parents[] = {&Base::PyObjectBase::Type,&TopoShapePy::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
TopoShapePy::TopoShapePy(PyTypeObject *T)
: Base::PyObjectBase(T)
{
  Base::Console().Log("Create TopoShape: %p \n",this);
}

TopoShapePy::TopoShapePy(const TopoDS_Shape &cShape, PyTypeObject *T) 
 : PyObjectBase( T), _cTopoShape(cShape)
{
	Console().Log("Create TopoShape %p\n",this);
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

PyObject *TopoShapePy::hasChild(PyObject *args)
{ 
  if (!PyArg_ParseTuple(args, "" ))   
    return NULL;                      
	Py_Return; 
}

PyObject *TopoShapePy::readIges(PyObject *args)
{
  char* filename;
  if (!PyArg_ParseTuple(args, "s", &filename ))   
    return NULL;

  PY_TRY {
 
    // checking for the file
    Base::FileInfo File(filename);
    if(!File.isReadable()) {
      PyErr_SetString(PyExc_Exception,"File to read does not exist or is not readable");
      return NULL;
    }

    // read iges-file
    IGESControl_Reader aReader;

    if (aReader.ReadFile((const Standard_CString)filename) != IFSelect_RetDone) {
      PyErr_SetString(PyExc_Exception,"Reading IGES failed");
      return NULL;
    }

    //Standard_Integer val = Interface_Static::IVal("read.iges.bspline.continuity");
    //Interface_Static::SetIVal("read.iges.bspline.continuity", 2);
    //Standard_Integer ic = Interface_Static::IVal("read.precision.mode");
    //Standard_Real rp = Interface_Static::RVal("read.precision.val");
    
    //Handle(TColStd_HSequenceOfTransient) aList; 
    //aList = aReader.GiveList ("iges-type(114)");
    //int ct = aList->Length();
    //Reader.TransferList(aList);
    //ct = aReader.NbShapes();

    // one shape that contains all subshapes
    aReader.TransferRoots();
    //ct = aReader.NbShapes();
    
    _cTopoShape = aReader.OneShape();
  } PY_CATCH;

  Py_Return; 
}

PyObject *TopoShapePy::writeIges(PyObject *args)
{
  char* filename;
  if (!PyArg_ParseTuple(args, "s", &filename ))   
    return NULL;

  PY_TRY {

    // write iges-file
    IGESControl_Writer aWriter;

    //Standard_CString byvalue = Interface_Static::CVal("write.iges.header.author");
    //Interface_Static::SetCVal ("write.iges.header.author", "FreeCAD");
    //Interface_Static::SetCVal ("write.iges.header.company", "FreeCAD");

    aWriter.AddShape(_cTopoShape);

    if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone) {
      PyErr_SetString(PyExc_Exception,"Writing IGES failed");
      return NULL;
    }

  } PY_CATCH;

  Py_Return; 
}

