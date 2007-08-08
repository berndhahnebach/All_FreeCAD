
#include "PreCompiled.h"

#include "BaseClass.h"

// inclusion of the generated files (generated out of BaseClassPy.xml)
#include "BaseClassPy.h"
#include "BaseClassPy.cpp"

using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *BaseClassPy::representation(void)
{
	return "BaseClassPy";
}


PyObject*  BaseClassPy::isDerivedFrom(PyObject *args)
{
	return 0;
}

PyObject*  BaseClassPy::getAllDerivedFrom(PyObject *args)
{
	return 0;
}

Py::String BaseClassPy::getType(void) const
{
	return Py::String();
}

Py::Int BaseClassPy::getModule(void) const
{
	return Py::Int();
}

PyObject *BaseClassPy::getCustomAttributes(const char* attr) const
{
	return 0;
}

int BaseClassPy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}


