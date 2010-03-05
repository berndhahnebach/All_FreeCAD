
#include "PreCompiled.h"

#include "Mod/Sketcher/App/Sketch.h"

// inclusion of the generated files (generated out of SketchPy.xml)
#include "SketchPy.h"
#include "SketchPy.cpp"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchPy::representation(void) const
{
    return std::string("<Sketch object>");
}

// +++ methodes implementer ++++++++++++++++++++++++++++++++++++++++++++++++

PyObject* SketchPy::solve(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

// +++ attributes implementer ++++++++++++++++++++++++++++++++++++++++++++++++

Py::Int SketchPy::getConstraint(void) const
{
    //return Py::Int();
    throw Py::AttributeError("Not yet implemented");
}


// +++ custom attributes implementer ++++++++++++++++++++++++++++++++++++++++


PyObject *SketchPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


