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
# include <sstream>
#endif

#include "../Base/PyExportImp.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
using Base::Console;

#include "Document.h"
#include "Feature.h"
#include "Property.h"
#include "FeaturePy.h"
#include "MaterialPy.h"
#include "MatrixPy.h"
#include "VectorPy.h"

using namespace App;




//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject App::FeaturePy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FeaturePy",				/*tp_name*/
	sizeof(FeaturePy),			/*tp_basicsize*/
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
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About FeaturePy",                                /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  0,                                                /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &App::DocumentObjectPy::Type,                     /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
  0,                                                /*tp_alloc */
  0,                                                /*tp_new */
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
PyMethodDef App::FeaturePy::Methods[] = {
// PyObjectBase
  PYMETHODEDEF(isA)
// DocumentObjectPy
// FeaturePy 
	PYMETHODEDEF(setModified)
	PYMETHODEDEF(setModifiedView)
	PYMETHODEDEF(isValid)

	{NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::FeaturePy::Parents[] = { &FeaturePy::Type, &DocumentObjectPy::Type, &PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
App::FeaturePy::FeaturePy(AbstractFeature *pcFeature, PyTypeObject *T)
: DocumentObjectPy(pcFeature, T), _pcFeature(pcFeature),solidMaterialPy(0),lineMaterialPy(0),pointMaterialPy(0)
{
//	Base::Console().Log("Create FeaturePy: %p \n",this);
}

PyObject *FeaturePy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FeaturePy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
FeaturePy::~FeaturePy()						// Everything handled in parent
{
//	Base::Console().Log("Destroy FeaturePy: %p \n",this);

  if(solidMaterialPy) solidMaterialPy->DecRef();
  if(lineMaterialPy) lineMaterialPy->DecRef();
  if(pointMaterialPy) pointMaterialPy->DecRef();
}

//--------------------------------------------------------------------------
// FeaturePy representation
//--------------------------------------------------------------------------
PyObject *FeaturePy::_repr(void)
{
  std::stringstream a;
  a << "Feature: [ ";
//  for(std::map<std::string,int>::const_iterator It = _pcFeature->_PropertiesMap.begin();It!=_pcFeature->_PropertiesMap.end();It++)
//  {
//    a << It->first << "=" << _pcFeature->GetProperty(It->first.c_str()).GetAsString() << "; ";
//  }
  a << "]" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// FeaturePy Attributes
//--------------------------------------------------------------------------
PyObject *FeaturePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{
  PY_TRY{
    if (Base::streq(attr, "__members__"))
    {
      PyObject *list = DocumentObjectPy::_getattr(attr);
      if (list) {
        PyList_Append(list, PyString_FromString("solidMaterial"));
        PyList_Append(list, PyString_FromString("lineMaterial"));
        PyList_Append(list, PyString_FromString("pointMaterial"));
        PyList_Append(list, PyString_FromString("lineSize"));
        PyList_Append(list, PyString_FromString("pointSize"));
        if (PyErr_Occurred()) { Py_DECREF(list);list = NULL;}
      }
      return list;
    }
    else if (Base::streq(attr, "solidMaterial"))
    {
      if(solidMaterialPy==0){
        solidMaterialPy = new MaterialPy(&(_pcFeature->_solidMaterial));
//        solidMaterialPy->IncRef();
      }
      solidMaterialPy->IncRef();
			return solidMaterialPy; 
    }
    else if (Base::streq(attr, "lineMaterial"))
    {
      if(lineMaterialPy==0){
        lineMaterialPy = new MaterialPy(&(_pcFeature->_lineMaterial));
//        lineMaterialPy->IncRef();
      }
      lineMaterialPy->IncRef();
			return lineMaterialPy; 
    }
    else if (Base::streq(attr, "pointMaterial"))
    {
      if(pointMaterialPy==0){
        pointMaterialPy = new MaterialPy(&(_pcFeature->_pointMaterial));
//        pointMaterialPy->IncRef();
      }
      pointMaterialPy->IncRef();
			return pointMaterialPy; 
    }
    else if (Base::streq(attr, "lineSize"))
    {
      return Py_BuildValue("f", _pcFeature->_lineSize);
    }
    else if (Base::streq(attr, "pointSize"))
    {
      return Py_BuildValue("f", _pcFeature->_pointSize);
    }
    else if (Base::streq(attr, "showMode"))
    {
        return Py_BuildValue("s", _pcFeature->showMode.getValue());
    }
    else{
      // search in PropertyList
      Property *prop = _pcFeature->getPropertyByName(attr);
      if(prop)
      {
        return prop->getPyObject();
      }
      else
			  _getattr_up(DocumentObjectPy); 						
    }
	}PY_CATCH;

  return Py_None;
} 

int FeaturePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (Base::streq(attr, "solidMaterial")){						// settable new state
    if( PyObject_TypeCheck( value, &(MaterialPy::Type) ))
    {
      _pcFeature->_solidMaterial = *((MaterialPy*)value)->_pcMaterial;
      _pcFeature->TouchView();
    }else if( PyString_Check( value ))
    {
      _pcFeature->_solidMaterial = Material(PyString_AsString(value));
      _pcFeature->TouchView();
    }else if( PyTuple_Check( value ))
    {
      _pcFeature->_solidMaterial.diffuseColor = MaterialPy::getColorFromPy(value);
      _pcFeature->TouchView();
    }else
      throw "material expected for that attribute";
  }else	if (Base::streq(attr, "lineMaterial")){	
    if( PyObject_TypeCheck( value, &(MaterialPy::Type) ))
    {
      _pcFeature->_lineMaterial = *((MaterialPy*)value)->_pcMaterial;
      _pcFeature->TouchView();
    }else if( PyTuple_Check( value ))
    {
      _pcFeature->_lineMaterial.diffuseColor = MaterialPy::getColorFromPy(value);
      _pcFeature->TouchView();
    }else
      throw "material expected for that attribute";
  }else	if (Base::streq(attr, "pointMaterial")){	
    if( PyObject_TypeCheck( value, &(MaterialPy::Type) ))
    {
      _pcFeature->_pointMaterial = *((MaterialPy*)value)->_pcMaterial;
      _pcFeature->TouchView();
    }else if( PyTuple_Check( value ))
    {
      _pcFeature->_pointMaterial.diffuseColor = MaterialPy::getColorFromPy(value);
      _pcFeature->TouchView();
    }else
      throw "material expected for that attribute";
  }else	if (Base::streq(attr, "lineSize")){	
    _pcFeature->_lineSize = getFloatFromPy(value);
    _pcFeature->TouchView();
  }else	if (Base::streq(attr, "pointSize")){	
    _pcFeature->_pointSize = getFloatFromPy(value);
    _pcFeature->TouchView();
  }else	if (Base::streq(attr, "transparency")){	
    _pcFeature->setTransparency(getFloatFromPy(value));
    _pcFeature->TouchView();
  }else	if (Base::streq(attr, "color")){	
    _pcFeature->setColor(MaterialPy::getColorFromPy(value));
    _pcFeature->TouchView();
  }else	if (Base::streq(attr, "showMode")){	
    _pcFeature->showMode.setValue(PyString_AsString(value));
    _pcFeature->TouchView();
  }else{
       // search in PropertyList
      Property *prop = _pcFeature->getPropertyByName(attr);
      if(prop)
      {
        prop->setPyObject(value);
      }else
			  return DocumentObjectPy::_setattr(attr, value); 						
  }
  return 0;
} 

/*
int FeaturePy::setProperty(const char *attr, PyObject *value)
{
  //char sBuf[256];
  //sprintf(sBuf,"%f",PyFloat_AsDouble(value));
  //_pcFeature->GetProperty(attr).Set(sBuf);
  if( PyObject_TypeCheck(value, &PyFloat_Type) )
    _pcFeature->setPropertyFloat(PyFloat_AsDouble(value),attr);
  else if( PyObject_TypeCheck(value, &PyInt_Type) )
    _pcFeature->setPropertyInt(PyInt_AsLong(value),attr);
  else if( PyObject_TypeCheck(value, &PyString_Type) )
    _pcFeature->setPropertyString(PyString_AsString(value),attr);
  else if( PyObject_TypeCheck(value, &(FeaturePy::Type)) )
  {
   	FeaturePy  *pcObject = (FeaturePy*)value;
    _pcFeature->setPropertyLink(pcObject->_pcFeature,attr);
  }
  else if( PyObject_TypeCheck(value, &(VectorPy::Type)) )
  {
   	VectorPy  *pcObject = (VectorPy*)value;
    _pcFeature->setPropertyVector(pcObject->value(),attr);
  }
  else if( PyObject_TypeCheck(value, &(MatrixPy::Type)) )
  {
   	MatrixPy  *pcObject = (MatrixPy*)value;
    _pcFeature->setPropertyMatrix(pcObject->value(),attr);
  }
  else
    return 0;
  
  _pcFeature->TouchProperty(attr);
  return 1;
}
*/

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(FeaturePy,setModified)
{
  _pcFeature->Touch();

	Py_Return;
}

PYFUNCIMP_D(FeaturePy,setModifiedView)
{
  _pcFeature->TouchView();

	Py_Return;
}

PYFUNCIMP_D(FeaturePy,isValid)
{
  if(_pcFeature->isValid() && !_pcFeature->mustExecute())
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;}

}

