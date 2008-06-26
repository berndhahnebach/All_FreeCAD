
#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRepBuilderAPI_MakeWire.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Wire.hxx>
#endif

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeWirePy.xml)
#include "TopoShapeWirePy.h"
#include "TopoShapeWirePy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeWirePy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Wire object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeWirePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TopoShapeWirePy and the Twin object 
    return new TopoShapeWirePy(new TopoShape);
}

// constructor method
int TopoShapeWirePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        getTopoShapePtr()->_Shape = TopoDS_Wire();
        return 0;
    }

    PyErr_Clear();
    PyObject *pcObj;
    if (PyArg_ParseTuple(args, "O!", &(PyList_Type), &pcObj)) {
        BRepBuilderAPI_MakeWire mkWire;
        Py::List list(pcObj);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            PyObject* item = (*it).ptr();
            if (PyObject_TypeCheck(item, &(Part::TopoShapePy::Type))) {
                TopoDS_Shape sh = static_cast<Part::TopoShapePy*>(item)->getTopoShapePtr()->_Shape;
                if (sh.ShapeType() == TopAbs_EDGE)
                    mkWire.Add(TopoDS::Edge(sh));
                else if (sh.ShapeType() == TopAbs_WIRE)
                    mkWire.Add(TopoDS::Wire(sh));
                else {
                    PyErr_SetString(PyExc_TypeError, "shape is neither edge nor wire");
                    return -1;
                }
            }
            else {
                PyErr_SetString(PyExc_TypeError, "shape is not a shape");
                return -1;
            }
        }

        try {
            getTopoShapePtr()->_Shape = mkWire.Wire();
            return 0;
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return -1;
        }
    }

    PyErr_Clear();
    PyObject *pE1, *pE2, *pE3;
    if (PyArg_ParseTuple(args, "O!O!O!", &(Part::TopoShapePy::Type), &pE1,
                                         &(Part::TopoShapePy::Type), &pE2,
                                         &(Part::TopoShapePy::Type), &pE3)) {
        TopoDS_Shape sh1 = static_cast<Part::TopoShapePy*>(pE1)->getTopoShapePtr()->_Shape;
        TopoDS_Shape sh2 = static_cast<Part::TopoShapePy*>(pE2)->getTopoShapePtr()->_Shape;
        TopoDS_Shape sh3 = static_cast<Part::TopoShapePy*>(pE3)->getTopoShapePtr()->_Shape;
        if (sh1.ShapeType() == TopAbs_EDGE && 
            sh2.ShapeType() == TopAbs_EDGE && 
            sh3.ShapeType() == TopAbs_EDGE) {
            BRepBuilderAPI_MakeWire mkWire(TopoDS::Edge(sh1), 
                TopoDS::Edge(sh2) , TopoDS::Edge(sh3));
            getTopoShapePtr()->_Shape = mkWire.Wire();
            return 0;
        }
    }

    return 0;
}

PyObject *TopoShapeWirePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeWirePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


