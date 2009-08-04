
#include "PreCompiled.h"

#include "Gui/SelectionObject.h"

// inclusion of the generated files (generated out of SelectionObjectPy.xml)
#include "SelectionObjectPy.h"
#include "SelectionObjectPy.cpp"

using namespace Gui;

// returns a string which represents the object e.g. when printed in python
const char *SelectionObjectPy::representation(void) const
{
    return "<SelectionObject object>";
}



PyObject* SelectionObjectPy::remove(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* SelectionObjectPy::isA(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}



Py::String SelectionObjectPy::getObjectName(void) const
{
    return Py::String();
}

Py::String SelectionObjectPy::getSubElementName(void) const
{
    return Py::String();
}

Py::String SelectionObjectPy::getFullName(void) const
{
    return Py::String();
}

PyObject *SelectionObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SelectionObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


