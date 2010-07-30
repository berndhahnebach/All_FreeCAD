
#include "PreCompiled.h"

#include "Mod/Sketcher/App/SketchObject.h"
#include <Mod/Part/App/LinePy.h>
#include <Mod/Part/App/Geometry.h>

// inclusion of the generated files (generated out of SketchObjectSFPy.xml)
#include "SketchObjectPy.h"
#include "SketchObjectPy.cpp"
// other python types
#include "ConstraintPy.h"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchObjectPy::representation(void) const
{
    return "<SketchObjectSF object>";
}


PyObject* SketchObjectPy::solve(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* SketchObjectPy::addGeometry(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return 0;

    if (PyObject_TypeCheck(pcObj, &(Part::GeometryPy::Type))) {
        Part::Geometry *geo = static_cast<Part::GeometryPy*>(pcObj)->getGeometryPtr();
        return Py::new_reference_to(Py::Int(this->getSketchObjectPtr()->addGeometry(geo)));
    }
    Py_Return; 
}

PyObject* SketchObjectPy::addConstraint(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return 0;

    if (PyObject_TypeCheck(pcObj, &(Sketcher::ConstraintPy::Type))) {
        Sketcher::Constraint *constr = static_cast<Sketcher::ConstraintPy*>(pcObj)->getConstraintPtr();
        return Py::new_reference_to(Py::Int(this->getSketchObjectPtr()->addConstraints(constr)));
    }
    Py_Return; 
}

PyObject* SketchObjectPy::movePoint(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}


Py::Int SketchObjectPy::getConstraintCount(void) const
{
    //return Py::Int();
    throw Py::AttributeError("Not yet implemented");
}

Py::Int SketchObjectPy::getGeometryCount(void) const
{
    //return Py::Int();
    throw Py::AttributeError("Not yet implemented");
}

PyObject *SketchObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


