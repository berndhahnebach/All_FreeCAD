
#include "PreCompiled.h"

#include "Mod/Robot/App/Waypoint.h"

// inclusion of the generated files (generated out of WaypointPy.xml)
#include "WaypointPy.h"
#include "WaypointPy.cpp"

using namespace Robot;

// returns a string which represents the object e.g. when printed in python
const char *WaypointPy::representation(void) const
{
    return "<Waypoint object>";
}

PyObject *WaypointPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of WaypointPy and the Twin object 
    return new WaypointPy(new Waypoint);
}

// constructor method
int WaypointPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}




Py::String WaypointPy::getName(void) const
{
    return Py::String();
}

void WaypointPy::setName(Py::String /*arg*/)
{

}

Py::String WaypointPy::getType(void) const
{
    return Py::String();
}

void WaypointPy::setType(Py::String /*arg*/)
{

}


Py::Object WaypointPy::getPos(void) const
{
    return Py::Object();
}

void WaypointPy::setPos(Py::Object /*arg*/)
{

}

Py::Boolean WaypointPy::getCont(void) const
{
    return Py::Boolean();
}

void WaypointPy::setCont(Py::Boolean /*arg*/)
{

}

Py::Int WaypointPy::getTool(void) const
{
    return Py::Int();
}

void WaypointPy::setTool(Py::Int /*arg*/)
{

}

Py::Int WaypointPy::getBase(void) const
{
    return Py::Int();
}

void WaypointPy::setBase(Py::Int /*arg*/)
{

}

PyObject *WaypointPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int WaypointPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


