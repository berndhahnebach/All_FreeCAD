
#include "PreCompiled.h"

#include "PartFeature.h"

// inclusion of the generated files (generated out of PartFeaturePy.xml)
#include "PartFeaturePy.h"
#include "PartFeaturePy.cpp"

using namespace Part;

// returns a string which represent the object e.g. when printed in python
const char *PartFeaturePy::representation(void) const
{
    return "<Part object>";
}

PyObject *PartFeaturePy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int PartFeaturePy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


