
#include "PreCompiled.h"

#include "Base/Rotation.h"

// inclusion of the generated files (generated out of RotationPy.xml)
#include "RotationPy.h"
#include "RotationPy.cpp"

using namespace Base;

// returns a string which represents the object e.g. when printed in python
const char *RotationPy::representation(void) const
{
    return "<Rotation object>";
}

// constructor method
int RotationPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}


PyObject* RotationPy::invert(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

Py::List RotationPy::getQ(void) const
{
    return Py::List();
}

void RotationPy::setQ(Py::List /*arg*/)
{

}

PyObject *RotationPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int RotationPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


