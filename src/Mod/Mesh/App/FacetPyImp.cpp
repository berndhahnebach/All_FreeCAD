
#include "PreCompiled.h"
#if 0
#include "Facet.h"

// inclusion of the generated files (generated out of FacetPy.xml)
#include "FacetPy.h"
#include "FacetPy.cpp"

using namespace Mesh;

// returns a string which represent the object e.g. when printed in python
const char *FacetPy::representation(void)
{
    return "FacetPy";
}


PyObject*  FacetPy::setPlacement(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject *FacetPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int FacetPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
#endif


