
#include "PreCompiled.h"

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeEdgePy.xml)
#include "TopoShapeEdgePy.h"
#include "TopoShapeEdgePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeEdgePy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Edge object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeEdgePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeEdgePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
