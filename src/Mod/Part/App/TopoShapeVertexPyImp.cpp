
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeVertexPy.xml)
#include "TopoShapeVertexPy.h"
#include "TopoShapeVertexPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeVertexPy::representation(void) const
{
    return "<TopoShape object>";
}



PyObject *TopoShapeVertexPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeVertexPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


