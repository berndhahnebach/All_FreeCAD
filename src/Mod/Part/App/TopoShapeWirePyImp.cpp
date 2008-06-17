
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeWirePy.xml)
#include "TopoShapeWirePy.h"
#include "TopoShapeWirePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeWirePy::representation(void) const
{
    return "<Wire object>";
}

PyObject *TopoShapeWirePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeWirePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


