
#include "PreCompiled.h"

#include "DocumentObject.h"

// inclusion of the generated files (generated out of DocumentObjectPy.xml)
#include "DocumentObjectPy.h"
#include "DocumentObjectPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *DocumentObjectPy::representation(void)
{
	return "DocumentObjectPy";
}


PyObject*  DocumentObjectPy::touch(PyObject *args)
{
  getDocumentObjectObject()->touch();
	Py_Return;
}

Py::List DocumentObjectPy::getState(void) const
{
	return Py::List();
}

PyObject *DocumentObjectPy::getCustomAttributes(const char* attr) const
{
	return 0;
}

int DocumentObjectPy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}


