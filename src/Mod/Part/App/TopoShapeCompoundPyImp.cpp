
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeCompoundPy.xml)
#include "TopoShapeCompoundPy.h"
#include "TopoShapeCompoundPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeCompoundPy::representation(void) const
{
    return "<TopoShape object>";
}

PyObject *TopoShapeCompoundPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeCompoundPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
