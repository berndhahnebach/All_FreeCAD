
#include "PreCompiled.h"

#include "Vector3D.h"

// inclusion of the generated files (generated out of VectorPy.xml)
#include "VectorPy.h"
#include "VectorPy.cpp"

using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *VectorPy::representation(void) const
{
    return "<Vector object>";
}

// constructor methode
int VectorPy::PyInit(PyObject* args, PyObject*k)
{
    return 0;
}


PyObject*  VectorPy::normalize(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject*  VectorPy::projectToLine(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject*  VectorPy::projectToPlane(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject*  VectorPy::distanceToLine(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject*  VectorPy::distanceToPlane(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

Py::Float VectorPy::getLength(void) const
{
    return Py::Float();
}

void  VectorPy::setLength(Py::Float arg)
{

}

Py::Float VectorPy::getx(void) const
{
    return Py::Float();
}

void  VectorPy::setx(Py::Float arg)
{

}

Py::Float VectorPy::gety(void) const
{
    return Py::Float();
}

void  VectorPy::sety(Py::Float arg)
{

}

Py::Float VectorPy::getz(void) const
{
    return Py::Float();
}

void  VectorPy::setz(Py::Float arg)
{

}

PyObject *VectorPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int VectorPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


