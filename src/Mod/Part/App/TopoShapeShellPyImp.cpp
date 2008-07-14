
#include "PreCompiled.h"

#include <Base/VectorPy.h>

#include "TopoShape.h"
#include "TopoShapeCompSolidPy.h"
#include "TopoShapeShellPy.h"
#include "TopoShapeShellPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeShellPy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Shell object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject* TopoShapeShellPy::extrude(PyObject *args)
{
    PyObject *pVec;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type), &pVec)) {
        try {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pVec)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->makePrism(gp_Vec(vec.x,vec.y,vec.z));
            return new TopoShapeCompSolidPy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

Py::Object TopoShapeShellPy::getWire(void) const
{
    return Py::Object();
}

PyObject *TopoShapeShellPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeShellPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
