
#include "PreCompiled.h"

#include "Mod/Sketcher/App/SketchObjectSF.h"

// inclusion of the generated files (generated out of SketchObjectSFPy.xml)
#include "SketchObjectSFPy.h"
#include "SketchObjectSFPy.cpp"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string SketchObjectSFPy::representation(void) const
{
    return "<SketchObjectSF object>";
}



PyObject *SketchObjectSFPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchObjectSFPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


