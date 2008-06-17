
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeEdgePy.xml)
#include "TopoShapeEdgePy.h"
#include "TopoShapeEdgePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeEdgePy::representation(void) const
{
    return "<Edge object>";
}

PyObject *TopoShapeEdgePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeEdgePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
