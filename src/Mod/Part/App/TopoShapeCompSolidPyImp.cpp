
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeCompSolidPy.xml)
#include "TopoShapeCompSolidPy.h"
#include "TopoShapeCompSolidPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeCompSolidPy::representation(void) const
{
    return "<TopoShape object>";
}

PyObject *TopoShapeCompSolidPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeCompSolidPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
