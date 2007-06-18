
#include "PreCompiled.h"

#include "DocumentObject.h"

// inclusion of the generated files (generated out of DocumentObjectPy.xml)
#include "DocumentObjectPy.h"
#include "DocumentObjectPy.cpp"

using namespace App;


PyObject*  DocumentObjectPy::touch(PyObject *args)
{
	return 0;
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


