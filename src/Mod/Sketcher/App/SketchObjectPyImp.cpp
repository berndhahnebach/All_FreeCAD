
#include "PreCompiled.h"

#include "Mod/Sketcher/App/SketchObject.h"

// inclusion of the generated files (generated out of SketchObjectSFPy.xml)
#include "SketchObjectPy.h"
#include "SketchObjectPy.cpp"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchObjectPy::representation(void) const
{
    return "<SketchObjectSF object>";
}



PyObject *SketchObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


