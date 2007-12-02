
#include "PreCompiled.h"

#include "ComplexGeoData.h"

// inclusion of the generated files (generated out of ComplexGeoDataPy.xml)
#include "ComplexGeoDataPy.h"
#include <Base/BoundBoxPy.h>
#include "ComplexGeoDataPy.cpp"

using namespace Data;
using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *ComplexGeoDataPy::representation(void) const
{
    return "<ComplexGeoData object>";
}

PyObject*  ComplexGeoDataPy::setPlacement(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

Py::Object ComplexGeoDataPy::getBoundBox(void) const
{
    return Py::Object(new BoundBoxPy(new BoundBox3d(getComplexGeoDataPtr()->getBoundBox())));
}

PyObject *ComplexGeoDataPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int ComplexGeoDataPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


