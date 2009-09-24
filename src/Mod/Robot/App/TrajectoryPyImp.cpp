
#include "PreCompiled.h"

#include "Mod/Robot/App/Trajectory.h"

// inclusion of the generated files (generated out of TrajectoryPy.xml)
#include "TrajectoryPy.h"
#include "TrajectoryPy.cpp"

using namespace Robot;

// returns a string which represents the object e.g. when printed in python
std::string TrajectoryPy::representation(void) const
{
    return std::string("<Trajectory object>");
}

PyObject *TrajectoryPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TrajectoryPy and the Twin object 
    return new TrajectoryPy(new Trajectory);
}

// constructor method
int TrajectoryPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}


PyObject* TrajectoryPy::insertWaypoint(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* TrajectoryPy::position(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* TrajectoryPy::velocity(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}



Py::Float TrajectoryPy::getDuration(void) const
{
    return Py::Float();
}

Py::List TrajectoryPy::getWaypoints(void) const
{
    return Py::List();
}

PyObject *TrajectoryPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TrajectoryPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


