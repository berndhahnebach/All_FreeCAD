
#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRepBuilderAPI_MakeFace.hxx>
# include <ShapeAnalysis.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
#endif

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeFacePy.xml)
#include "TopoShapeFacePy.h"
#include "TopoShapeFacePy.cpp"

using namespace Part;

// returns a string which represent the object e.g. when printed in python
const char *TopoShapeFacePy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Face object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeFacePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TopoShapeFacePy and the Twin object 
    return new TopoShapeFacePy(new TopoShape);
}

// constructor method
int TopoShapeFacePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        getTopoShapePtr()->_Shape = TopoDS_Face();
        return 0;
    }

    PyErr_Clear();
    PyObject *pW;
    if (PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &pW)) {
        try {
            TopoDS_Shape sh = static_cast<Part::TopoShapePy*>(pW)->getTopoShapePtr()->_Shape;
            if (sh.ShapeType() == TopAbs_WIRE) {
                BRepBuilderAPI_MakeFace mkFace(TopoDS::Wire(sh));
                getTopoShapePtr()->_Shape = mkFace.Face();
                return 0;
            }
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return -1;
        }
    }

    PyErr_SetString(PyExc_Exception, "wire expected");
    return -1;
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
