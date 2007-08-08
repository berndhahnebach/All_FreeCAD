
#include "PreCompiled.h"

#include "TopoShape.h"

// inclusion of the generated files (generated out of TopoShapePy.xml)
#include "TopoShapePy.h"
#include "TopoShapePy.cpp"

using namespace Part;

// returns a string which represent the object e.g. when printed in python
const char *TopoShapePy::representation(void)
{
	return "TopoShapePy";
}


PyObject*  TopoShapePy::exportIges(PyObject *args)
{
	return 0;
}

PyObject*  TopoShapePy::exportStep(PyObject *args)
{
	return 0;
}

PyObject*  TopoShapePy::exportBrep(PyObject *args)
{
	return 0;
}

PyObject*  TopoShapePy::exportStl(PyObject *args)
{
	return 0;
}

PyObject*  TopoShapePy::check(PyObject *args)
{
	return 0;
}

Py::Int TopoShapePy::getValid(void) const
{
	return Py::Int();
}

Py::List TopoShapePy::getFaces(void) const
{
	return Py::List();
}

Py::List TopoShapePy::getVertexes(void) const
{
	return Py::List();
}

Py::List TopoShapePy::getShapes(void) const
{
	return Py::List();
}

Py::List TopoShapePy::getEdges(void) const
{
	return Py::List();
}

Py::List TopoShapePy::getWires(void) const
{
	return Py::List();
}

PyObject *TopoShapePy::getCustomAttributes(const char* attr) const
{
	return 0;
}

int TopoShapePy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}


