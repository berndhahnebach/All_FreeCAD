
#include "PreCompiled.h"

#include "Mod/Part/App/Geometry.h"

// inclusion of the generated files (generated out of GeometryPy.xml)
#include "GeometryPy.h"
#include "GeometryPy.cpp"
#include "TopoShape.h"
#include "TopoShapePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *GeometryPy::representation(void) const
{
    return "<Geometry object>";
}

PyObject *GeometryPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'Geometry'.");
    return 0;
}

// constructor method
int GeometryPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* GeometryPy::toShape(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    TopoDS_Shape sh = getGeometryPtr()->toShape();
    return new TopoShapePy(new TopoShape(sh));
}

PyObject *GeometryPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int GeometryPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


