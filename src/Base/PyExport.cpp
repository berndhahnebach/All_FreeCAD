/*------------------------------
 * FCPyObject cpp
 *
 * C++ library routines for Crawl 3.2
------------------------------*/

#include "stdlib.h"
#include "PyExport.h"

/*------------------------------
 * FCPyObject Type		-- Every class, even the abstract one should have a Type
------------------------------*/

PyTypeObject FCPyObject::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCPyObject",			/*tp_name*/
	sizeof(FCPyObject),	/*tp_basicsize*/
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
 * FCPyObject Methods 	-- Every class, even the abstract one should have a Methods
------------------------------*/
PyMethodDef FCPyObject::Methods[] = {
  {"isA",		 (PyCFunction) sPy_isA,			Py_NEWARGS},
  {NULL, NULL}		/* Sentinel */
};

/*------------------------------
 * FCPyObject Parents		-- Every class, even the abstract one should have parents
------------------------------*/
PyParentObject FCPyObject::Parents[] = {&FCPyObject::Type, NULL};

/*------------------------------
 * FCPyObject attributes	-- attributes
------------------------------*/
PyObject *FCPyObject::_getattr(char *attr)
{
  if (streq(attr, "type"))
    return Py_BuildValue("s", (*(GetParents()))->tp_name);

  return Py_FindMethod(Methods, this, attr);    
}

int FCPyObject::_setattr(char *attr, PyObject *value)
{
	stlport::cerr << "Unknown attribute" << stlport::endl;
  return 1;
}

/*------------------------------
 * FCPyObject repr		-- representations
------------------------------*/
PyObject *FCPyObject::_repr(void)
{
  Py_Error(PyExc_SystemError, "Representation not overridden by object.");  
}

/*------------------------------
 * FCPyObject isA		-- the isA functions
------------------------------*/
bool FCPyObject::isA(PyTypeObject *T)		// if called with a Type, use "typename"
{
  return isA(T->tp_name);
}

bool FCPyObject::isA(const char *type_name)		// check typename of each parent
{
  int i;
  PyParentObject  P;
  PyParentObject *Ps = GetParents();

  for (P = Ps[i=0]; P != NULL; P = Ps[i++])
      if (streq(P->tp_name, type_name))
	return true;
  return false;
}

PyObject *FCPyObject::Py_isA(PyObject *args)		// Python wrapper for isA
{
  char *type_name;
  Py_Try(PyArg_ParseTuple(args, "s", &type_name));
  if(isA(type_name))
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;};
}


