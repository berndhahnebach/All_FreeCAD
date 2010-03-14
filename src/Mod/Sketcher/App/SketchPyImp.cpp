
#include "PreCompiled.h"

#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/GeometryCurvePy.h>
#include <Mod/Part/App/LinePy.h>

#include "Sketch.h"

// inclusion of the generated files (generated out of SketchPy.xml)
#include "SketchPy.h"
#include "SketchPy.cpp"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchPy::representation(void) const
{
    return std::string("<Sketch object>");
}

PyObject *SketchPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of SketchPy and the Twin object 
    return new SketchPy(new Sketch());
}

// constructor method
int SketchPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

// +++ methodes implementer ++++++++++++++++++++++++++++++++++++++++++++++++

PyObject* SketchPy::solve(PyObject *args)
{
    getSketchPtr()->solve();
    Py_Return; 
}

PyObject* SketchPy::addGeometry(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return 0;

    if (PyObject_TypeCheck(pcObj, &(Part::LinePy::Type))) {
        Part::GeomLineSegment *line = new Part::GeomLineSegment(*static_cast<Part::LinePy*>(pcObj)->getGeomLineSegmentPtr());
        this->getSketchPtr()->addGeometry(line);
    }
    Py_Return; 
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


