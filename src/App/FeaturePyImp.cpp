
#include "PreCompiled.h"

#include "Feature.h"

// inclusion of the generated files (generated out of FeaturePy.xml)
#include "FeaturePy.h"
#include "FeaturePy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *FeaturePy::representation(void)
{
	return "FeaturePy";
}


PyObject*  FeaturePy::recompute(PyObject *args)
{
	return 0;
}

Py::Int FeaturePy::getValid(void) const
{
	return Py::Int(getAbstractFeatureObject()->isValid());
}

PyObject *FeaturePy::getCustomAttributes(const char* attr) const
{
	return 0;
}

int FeaturePy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}


