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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <stdlib.h>
#endif

#include "PyExportImp.h"

using namespace Base;

/*------------------------------
 * PyObjectBase Type		-- Every class, even the abstract one should have a Type
------------------------------*/

PyTypeObject PyObjectBase::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"PyObjectBase",			/*tp_name*/
	sizeof(PyObjectBase),	/*tp_basicsize*/
	0,						/*tp_itemsize*/
			/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,			        /*tp_repr*/
	0,						/*tp_as_number*/
	0,		 				/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
};

/*------------------------------
 * PyObjectBase Methods 	-- Every class, even the abstract one should have a Methods
------------------------------*/
PyMethodDef PyObjectBase::Methods[] = {
  {"isA",		 (PyCFunction) sPy_isA,			Py_NEWARGS},
  {NULL, NULL}		/* Sentinel */
};

/*------------------------------
 * PyObjectBase Parents		-- Every class, even the abstract one should have parents
------------------------------*/
PyParentObject PyObjectBase::Parents[] = {&PyObjectBase::Type, NULL};

/*------------------------------
 * PyObjectBase attributes	-- attributes
------------------------------*/
PyObject *PyObjectBase::_getattr(char *attr)
{
  if (streq(attr, "type"))
    return Py_BuildValue("s", (*(GetParents()))->tp_name);

  return Py_FindMethod(Methods, this, attr);    
}

int PyObjectBase::_setattr(char *attr, PyObject *value)
{
	//FCcerr << "Unknown attribute" << FCendl;
  return 1;
}

/*------------------------------
 * PyObjectBase repr		-- representations
------------------------------*/
PyObject *PyObjectBase::_repr(void)
{
  std::stringstream a;
  a << "PyObjectBase: [ ";
  a << "]" << std::endl;
# ifdef FCDebug
    Console().Log("PyObjectBase::_repr() not overwriten representation!");
# endif
	return Py_BuildValue("s", a.str().c_str());
}

/*------------------------------
 * PyObjectBase isA		-- the isA functions
------------------------------*/
bool PyObjectBase::isA(PyTypeObject *T)		// if called with a Type, use "typename"
{
  return isA(T->tp_name);
}

bool PyObjectBase::isA(const char *type_name)		// check typename of each parent
{
  int i;
  PyParentObject  P;
  PyParentObject *Ps = GetParents();

  for (P = Ps[i=0]; P != NULL; P = Ps[i++])
      if (streq(P->tp_name, type_name))
	return true;
  return false;
}

PyObject *PyObjectBase::Py_isA(PyObject *args)		// Python wrapper for isA
{
  char *type_name;
  Py_Try(PyArg_ParseTuple(args, "s", &type_name));
  if(isA(type_name))
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;};
}


