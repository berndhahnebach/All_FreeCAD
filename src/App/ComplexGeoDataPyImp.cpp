
#include "PreCompiled.h"

#include "ComplexGeoData.h"

// inclusion of the generated files (generated out of ComplexGeoDataPy.xml)
#include "ComplexGeoDataPy.h"
#include "ComplexGeoDataPy.cpp"

using namespace Data;

// returns a string which represent the object e.g. when printed in python
const char *ComplexGeoDataPy::representation(void)
{
	return "ComplexGeoDataPy";
}

PyObject*  ComplexGeoDataPy::setPlacement(PyObject *args)
{
	return 0;
}

PyObject *ComplexGeoDataPy::getCustomAttributes(const char* attr) const
{
	return 0;
}

int ComplexGeoDataPy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}


