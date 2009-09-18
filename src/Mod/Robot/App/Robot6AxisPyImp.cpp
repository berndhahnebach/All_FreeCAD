
#include "PreCompiled.h"

#include "Mod/Robot/App/Robot6Axis.h"

// inclusion of the generated files (generated out of Robot6AxisPy.xml)
#include "Robot6AxisPy.h"
#include "Robot6AxisPy.cpp"

using namespace Robot;

// returns a string which represents the object e.g. when printed in python
const char *Robot6AxisPy::representation(void) const
{
    return "<Robot6Axis object>";
}

PyObject *Robot6AxisPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of Robot6AxisPy and the Twin object 
    return new Robot6AxisPy(new Robot6Axis);
}

// constructor method
int Robot6AxisPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}


PyObject* Robot6AxisPy::check(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}



Py::Float Robot6AxisPy::getAxis1(void) const
{
    return Py::Float();
}

void Robot6AxisPy::setAxis1(Py::Float /*arg*/)
{

}

Py::Float Robot6AxisPy::getAxis2(void) const
{
    return Py::Float();
}

void Robot6AxisPy::setAxis2(Py::Float /*arg*/)
{

}

Py::Float Robot6AxisPy::getAxis3(void) const
{
    return Py::Float();
}

void Robot6AxisPy::setAxis3(Py::Float /*arg*/)
{

}

Py::Float Robot6AxisPy::getAxis4(void) const
{
    return Py::Float();
}

void Robot6AxisPy::setAxis4(Py::Float /*arg*/)
{

}

Py::Float Robot6AxisPy::getAxis5(void) const
{
    return Py::Float();
}

void Robot6AxisPy::setAxis5(Py::Float /*arg*/)
{

}

Py::Float Robot6AxisPy::getAxis6(void) const
{
    return Py::Float();
}

void Robot6AxisPy::setAxis6(Py::Float /*arg*/)
{

}

Py::Object Robot6AxisPy::getTcp(void) const
{
    return Py::Object();
}

void Robot6AxisPy::setTcp(Py::Object /*arg*/)
{

}

Py::Object Robot6AxisPy::getBase(void) const
{
    return Py::Object();
}

void Robot6AxisPy::setBase(Py::Object /*arg*/)
{

}

PyObject *Robot6AxisPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int Robot6AxisPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


