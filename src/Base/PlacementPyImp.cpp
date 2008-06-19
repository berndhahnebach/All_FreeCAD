
#include "PreCompiled.h"

#include "Base/Placement.h"

// inclusion of the generated files (generated out of PlacementPy.xml)
#include "PlacementPy.h"
#include "PlacementPy.cpp"

using namespace Base;

// returns a string which represents the object e.g. when printed in python
const char *PlacementPy::representation(void) const
{
    return "<Placement object>";
}

// constructor method
int PlacementPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* PlacementPy::move(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

Py::Object PlacementPy::getBase(void) const
{
    return Py::Object();
}

void PlacementPy::setBase(Py::Object /*arg*/)
{

}

Py::List PlacementPy::getRotation(void) const
{
    return Py::List();
}

void PlacementPy::setRotation(Py::List /*arg*/)
{

}

PyObject *PlacementPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int PlacementPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
