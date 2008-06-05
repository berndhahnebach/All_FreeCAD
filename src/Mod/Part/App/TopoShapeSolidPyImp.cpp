
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeSolidPy.xml)
#include "TopoShapeSolidPy.h"
#include "TopoShapeSolidPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeSolidPy::representation(void) const
{
    return "<TopoShapeSolid>";
}



PyObject *TopoShapeSolidPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeSolidPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


