
#include "PreCompiled.h"

#include "Mod/Sketcher/App/Constraint.h"

// inclusion of the generated files (generated out of ConstraintPy.xml)
#include "ConstraintPy.h"
#include "ConstraintPy.cpp"

using namespace Sketcher;

// returns a string which represents the object e.g. when printed in python
std::string ConstraintPy::representation(void) const
{
    return std::string("<Constraint object>");
}





PyObject *ConstraintPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ConstraintPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


