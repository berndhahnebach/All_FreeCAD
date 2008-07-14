
#include "PreCompiled.h"

#ifndef _PreComp_
# include <gp_Pnt.hxx>
# include <BRep_Tool.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Vertex.hxx>
# include <BRepBuilderAPI_MakeVertex.hxx>
#endif

#include <Mod/Part/App/TopoShape.h>
#include <Base/VectorPy.h>
#include <Base/Vector3D.h>

#include "TopoShapeEdgePy.h"
#include "TopoShapeVertexPy.h"
#include "TopoShapeVertexPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeVertexPy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Vertex object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeVertexPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TopoShapeVertexPy and the Twin object 
    return new TopoShapeVertexPy(new TopoShape);
}

// constructor method
int TopoShapeVertexPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    double x=0.0,y=0.0,z=0.0;
    PyObject *object;
    if (PyArg_ParseTuple(args, "|ddd", &x,&y,&z)) {
        // do nothing here
    }
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object)) {
        PyErr_Clear(); // set by PyArg_ParseTuple()
        // Note: must be static_cast, not reinterpret_cast
        Base::Vector3d* ptr = static_cast<Base::VectorPy*>(object)->getVectorPtr();
        x = ptr->x;
        y = ptr->y;
        z = ptr->z;
    }
    else if (PyArg_ParseTuple(args,"O!",&(PyTuple_Type), &object)) {
        PyErr_Clear(); // set by PyArg_ParseTuple()
        try {
            Py::Tuple tuple(object);
            x = (float)Py::Float(tuple.getItem(0));
            y = (float)Py::Float(tuple.getItem(1));
            z = (float)Py::Float(tuple.getItem(2));
        }
        catch (const Py::Exception&) {
            return -1;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Either three floats, tuple or vector expected");
        return -1;
    }

    TopoShapeVertexPy::PointerType ptr = reinterpret_cast<TopoShapeVertexPy::PointerType>(_pcTwinPointer);
    BRepBuilderAPI_MakeVertex aBuilder(gp_Pnt(x,y,z));
    TopoDS_Shape s = aBuilder.Vertex();
    ptr->_Shape = s;

    return 0;
}

PyObject* TopoShapeVertexPy::extrude(PyObject *args)
{
    PyObject *pVec;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type), &pVec)) {
        try {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pVec)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->makePrism(gp_Vec(vec.x,vec.y,vec.z));
            return new TopoShapeEdgePy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

Py::Float TopoShapeVertexPy::getX(void) const
{
    TopoDS_Vertex v = TopoDS::Vertex(getTopoShapePtr()->_Shape);
    return Py::Float(BRep_Tool::Pnt(v).X());
}

Py::Float TopoShapeVertexPy::getY(void) const
{
    TopoDS_Vertex v = TopoDS::Vertex(getTopoShapePtr()->_Shape);
    return Py::Float(BRep_Tool::Pnt(v).Y());
}

Py::Float TopoShapeVertexPy::getZ(void) const
{
    TopoDS_Vertex v = TopoDS::Vertex(getTopoShapePtr()->_Shape);
    return Py::Float(BRep_Tool::Pnt(v).Z());
}

Py::Object TopoShapeVertexPy::getPoint(void) const
{
    TopoDS_Vertex v = TopoDS::Vertex(getTopoShapePtr()->_Shape);
    gp_Pnt p = BRep_Tool::Pnt(v);
    return Py::Object(new Base::VectorPy(new Base::Vector3d(p.X(),p.Y(),p.Z())));
}

PyObject *TopoShapeVertexPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeVertexPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
