
#include "PreCompiled.h"

#include "Geom_TrimmedCurve.hxx"

// inclusion of the generated files (generated out of ArcPy.xml)
#include "ArcPy.h"
#include "ArcPy.cpp"
#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *ArcPy::representation(void) const
{
    return "<Arc object>";
}

PyObject *ArcPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(PyExc_RuntimeError, "You cannot create directly an instance of 'Arc'.\n"
        "Create a curve (e.g. Line, Circle, Ellipse, ...) and use the 'trim' method.");
 
    return 0;
}

// constructor method
int ArcPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject *ArcPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ArcPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
