
#include "PreCompiled.h"
#ifndef _PreComp_
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
#endif

#include "Mod/Part/App/TopoShape.h"
#include <ShapeAnalysis.hxx>

// inclusion of the generated files (generated out of TopoShapeFacePy.xml)
#include "TopoShapeFacePy.h"
#include "TopoShapeFacePy.cpp"

using namespace Part;

// returns a string which represent the object e.g. when printed in python
const char *TopoShapeFacePy::representation(void) const
{
    return "<TopoShape object>";
}

Py::Object TopoShapeFacePy::getWire(void) const
{
    TopoDS_Shape clSh = getTopoShapePtr()->_Shape;
    if (clSh.ShapeType() == TopAbs_FACE) {
        TopoDS_Face clFace = (TopoDS_Face&)clSh;
        TopoDS_Wire clWire = ShapeAnalysis::OuterWire(clFace);
        return Py::Object(new TopoShapePy(new TopoShape(clWire)),true);
    }
    else
        throw "Internal error, TopoDS_Shape is not a face!";

    return Py::Object();
}

PyObject *TopoShapeFacePy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int TopoShapeFacePy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
