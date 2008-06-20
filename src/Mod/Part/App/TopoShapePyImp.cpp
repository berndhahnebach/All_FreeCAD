
#include "PreCompiled.h"
#ifndef _PreComp_
# include <sstream>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepCheck_Analyzer.hxx>
# include <BRepCheck_ListIteratorOfListOfStatus.hxx>
# include <BRepCheck_Result.hxx>
# include <BRepTools.hxx>
# include <gp_Ax1.hxx>
# include <gp_Trsf.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <TopLoc_Location.hxx>
#endif


#include <Base/PyCXX/Objects.hxx>
#include <Base/Matrix.h>
#include <Base/Rotation.h>
#include <Base/MatrixPy.h>
#include "TopoShape.h"

// inclusion of the generated files (generated out of TopoShapePy.xml)
#include "TopoShapePy.h"
#include "TopoShapePy.cpp"

#include "TopoShapeFacePy.h"
#include "TopoShapeEdgePy.h"
#include "TopoShapeWirePy.h"
#include "TopoShapeVertexPy.h"
#include "TopoShapeSolidPy.h"
#include "TopoShapeShellPy.h"
#include "TopoShapeCompSolidPy.h"
#include "TopoShapeCompoundPy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapePy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Shape object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

int TopoShapePy::PyInit(PyObject* args, PyObject*)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "|O!", &(TopoShapePy::Type), &pcObj))     // convert args: Python->C 
        return -1;                             // NULL triggers exception

    // if a shape is given
    if (pcObj) {
        TopoDS_Shape sh = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
        getTopoShapePtr()->_Shape = sh;
    }

    return 0;
}

PyObject*  TopoShapePy::read(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    getTopoShapePtr()->read(filename);
    Py_Return;
}

PyObject*  TopoShapePy::exportIges(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    // write iges file
    if (!getTopoShapePtr()->exportIges(filename)) {
        PyErr_SetString(PyExc_Exception,"Writing IGES failed");
        return NULL;
    }

    Py_Return;
}

PyObject*  TopoShapePy::exportStep(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    // write step file
    if (!getTopoShapePtr()->exportStep(filename)) {
        PyErr_SetString(PyExc_Exception,"Writing STEP failed");
        return NULL;
    }

    Py_Return;
}

PyObject*  TopoShapePy::exportBrep(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    // write brep file
    if (!getTopoShapePtr()->exportBrep(filename)) {
        PyErr_SetString(PyExc_Exception,"Writing BREP failed");
        return NULL;
    }

    Py_Return;
}

PyObject*  TopoShapePy::exportStl(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    // write stl file
    getTopoShapePtr()->exportStl(filename);
    Py_Return;
}

PyObject*  TopoShapePy::check(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    if (!getTopoShapePtr()->_Shape.IsNull()) {
        BRepCheck_Analyzer aChecker(getTopoShapePtr()->_Shape);
        if (!aChecker.IsValid()) {
            TopoDS_Iterator it(getTopoShapePtr()->_Shape);
            for (;it.More(); it.Next()) {
                if (!aChecker.IsValid(it.Value())) {
                    const Handle_BRepCheck_Result& result = aChecker.Result(it.Value());
                    const BRepCheck_ListOfStatus& status = result->StatusOnShape(it.Value());

                    BRepCheck_ListIteratorOfListOfStatus it(status);
                    while (it.More()) {
                        BRepCheck_Status& val = it.Value();
                        switch (val)
                        {
                        case BRepCheck_NoError:
                            PyErr_SetString(PyExc_StandardError, "No error");
                            break;
                        case BRepCheck_InvalidPointOnCurve:
                            PyErr_SetString(PyExc_StandardError, "Invalid point on curve");
                            break;
                        case BRepCheck_InvalidPointOnCurveOnSurface:
                            PyErr_SetString(PyExc_StandardError, "Invalid point on curve on surface");
                            break;
                        case BRepCheck_InvalidPointOnSurface:
                            PyErr_SetString(PyExc_StandardError, "Invalid point on surface");
                            break;
                        case BRepCheck_No3DCurve:
                            PyErr_SetString(PyExc_StandardError, "No 3D curve");
                            break;
                        case BRepCheck_Multiple3DCurve:
                            PyErr_SetString(PyExc_StandardError, "Multiple 3D curve");
                            break;
                        case BRepCheck_Invalid3DCurve:
                            PyErr_SetString(PyExc_StandardError, "Invalid 3D curve");
                            break;
                        case BRepCheck_NoCurveOnSurface:
                            PyErr_SetString(PyExc_StandardError, "No curve on surface");
                            break;
                        case BRepCheck_InvalidCurveOnSurface:
                            PyErr_SetString(PyExc_StandardError, "Invalid curve on surface");
                            break;
                        case BRepCheck_InvalidCurveOnClosedSurface:
                            PyErr_SetString(PyExc_StandardError, "Invalid curve on closed surface");
                            break;
                        case BRepCheck_InvalidSameRangeFlag:
                            PyErr_SetString(PyExc_StandardError, "Invalid same-range flag");
                            break;
                        case BRepCheck_InvalidSameParameterFlag:
                            PyErr_SetString(PyExc_StandardError, "Invalid same-parameter flag");
                            break;
                        case BRepCheck_InvalidDegeneratedFlag:
                            PyErr_SetString(PyExc_StandardError, "Invalid degenerated flag");
                            break;
                        case BRepCheck_FreeEdge:
                            PyErr_SetString(PyExc_StandardError, "Free edge");
                            break;
                        case BRepCheck_InvalidMultiConnexity:
                            PyErr_SetString(PyExc_StandardError, "Invalid multi-connexity");
                            break;
                        case BRepCheck_InvalidRange:
                            PyErr_SetString(PyExc_StandardError, "Invalid range");
                            break;
                        case BRepCheck_EmptyWire:
                            PyErr_SetString(PyExc_StandardError, "Empty wire");
                            break;
                        case BRepCheck_RedundantEdge:
                            PyErr_SetString(PyExc_StandardError, "Redundant edge");
                            break;
                        case BRepCheck_SelfIntersectingWire:
                            PyErr_SetString(PyExc_StandardError, "Self-intersecting wire");
                            break;
                        case BRepCheck_NoSurface:
                            PyErr_SetString(PyExc_StandardError, "No surface");
                            break;
                        case BRepCheck_InvalidWire:
                            PyErr_SetString(PyExc_StandardError, "Invalid wires");
                            break;
                        case BRepCheck_RedundantWire:
                            PyErr_SetString(PyExc_StandardError, "Redundant wires");
                            break;
                        case BRepCheck_IntersectingWires:
                            PyErr_SetString(PyExc_StandardError, "Intersecting wires");
                            break;
                        case BRepCheck_InvalidImbricationOfWires:
                            PyErr_SetString(PyExc_StandardError, "Invalid imbrication of wires");
                            break;
                        case BRepCheck_EmptyShell:
                            PyErr_SetString(PyExc_StandardError, "Empty shell");
                            break;
                        case BRepCheck_RedundantFace:
                            PyErr_SetString(PyExc_StandardError, "Redundant face");
                            break;
                        case BRepCheck_UnorientableShape:
                            PyErr_SetString(PyExc_StandardError, "Unorientable shape");
                            break;
                        case BRepCheck_NotClosed:
                            PyErr_SetString(PyExc_StandardError, "Not closed");
                            break;
                        case BRepCheck_NotConnected:
                            PyErr_SetString(PyExc_StandardError, "Not connected");
                            break;
                        case BRepCheck_SubshapeNotInShape:
                            PyErr_SetString(PyExc_StandardError, "Sub-shape not in shape");
                            break;
                        case BRepCheck_BadOrientation:
                            PyErr_SetString(PyExc_StandardError, "Bad orientation");
                            break;
                        case BRepCheck_BadOrientationOfSubshape:
                            PyErr_SetString(PyExc_StandardError, "Bad orientation of sub-shape");
                            break;
                        case BRepCheck_InvalidToleranceValue:
                            PyErr_SetString(PyExc_StandardError, "Invalid tolerance value");
                            break;
                        case BRepCheck_CheckFail:
                            PyErr_SetString(PyExc_StandardError, "Check failed");
                            break;
                        }

                        if (PyErr_Occurred())
                            PyErr_Print();
                        it.Next();
                    }
                }
            }
        }
    }

    Py_Return; 
}

PyObject*  TopoShapePy::fuse(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    try {
        // Let's call algorithm computing a fuse operation:
        TopoDS_Shape fusShape = this->getTopoShapePtr()->fuse(shape);
        return new TopoShapePy(new TopoShape(fusShape));
    }
    catch (const Standard_Failure&) {
        PyErr_SetString(PyExc_Exception, "fuse operation failed");
        return NULL;
    }
}

PyObject*  TopoShapePy::common(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    try {
        // Let's call algorithm computing a common operation:
        TopoDS_Shape comShape = this->getTopoShapePtr()->common(shape);
        return new TopoShapePy(new TopoShape(comShape));
    }
    catch (const Standard_Failure&) {
        PyErr_SetString(PyExc_Exception, "common operation failed");
        return NULL;
    }
}

PyObject*  TopoShapePy::section(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    try {
        // Let's call algorithm computing a section operation:
        TopoDS_Shape secShape = this->getTopoShapePtr()->section(shape);
        return new TopoShapePy(new TopoShape(secShape));
    }
    catch (const Standard_Failure&) {
        PyErr_SetString(PyExc_Exception, "section operation failed");
        return NULL;
    }
}

PyObject*  TopoShapePy::cut(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    try {
        // Let's call algorithm computing a cut operation:
        TopoDS_Shape cutShape = this->getTopoShapePtr()->cut(shape);
        return new TopoShapePy(new TopoShape(cutShape));
    }
    catch (const Standard_Failure&) {
        PyErr_SetString(PyExc_Exception, "cut operation failed");
        return NULL;
    }
}

PyObject*  TopoShapePy::translate(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

    try {
        Py::Tuple p(obj);
        // Convert into OCC representation
        gp_Vec vec = gp_Vec((double)Py::Float(p[0]),
                            (double)Py::Float(p[1]),
                            (double)Py::Float(p[2]));
        gp_Trsf mov;
        mov.SetTranslation(vec);
        TopLoc_Location loc(mov);
            getTopoShapePtr()->_Shape.Move(loc);
        Py_Return;
    }
    catch (const Py::Exception&) {
        return NULL;
    }
}

PyObject*  TopoShapePy::rotate(PyObject *args)
{
    PyObject *obj1, *obj2;
    double angle;
    if (!PyArg_ParseTuple(args, "OOd", &obj1, &obj2, &angle))
        return NULL;

    try {
        Py::Tuple p1(obj1), p2(obj2);
        // Convert into OCC representation
        gp_Pnt pos = gp_Pnt((double)Py::Float(p1[0]),
                            (double)Py::Float(p1[1]),
                            (double)Py::Float(p1[2]));
        gp_Dir dir = gp_Dir((double)Py::Float(p2[0]),
                            (double)Py::Float(p2[1]),
                            (double)Py::Float(p2[2]));

        gp_Ax1 axis(pos, dir);
        gp_Trsf mov;
        mov.SetRotation(axis, angle);
        TopLoc_Location loc(mov);
        getTopoShapePtr()->_Shape.Move(loc);
        Py_Return;
    }
    catch (const Py::Exception&) {
        return NULL;
    }
}

PyObject*  TopoShapePy::scale(PyObject *args)
{
    double factor;
    if (!PyArg_ParseTuple(args, "d", &factor))
        return NULL;

    if (fabs(factor) < gp::Resolution()) {
        PyErr_SetString(PyExc_Exception, "scale factor too small");
        return NULL;
    }

    try {
        gp_Trsf scl;
        scl.SetScaleFactor(factor);
        TopLoc_Location loc(scl);
        getTopoShapePtr()->_Shape.Move(loc);
        Py_Return;
    }
    catch (const Standard_Failure&) {
        PyErr_SetString(PyExc_Exception, "scaling failed");
        return NULL;
    }
}

PyObject*  TopoShapePy::reverse(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    getTopoShapePtr()->_Shape.Reverse();
    Py_Return;
}

PyObject*  TopoShapePy::complement(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    getTopoShapePtr()->_Shape.Complement();
    Py_Return;
}

PyObject*  TopoShapePy::nullify(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    getTopoShapePtr()->_Shape.Nullify();
    Py_Return;
}

PyObject*  TopoShapePy::isNull(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool null = getTopoShapePtr()->isNull();
    return Py_BuildValue("O", (null ? Py_True : Py_False));
}

PyObject*  TopoShapePy::isClosed(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    TopoDS_Shape shape = getTopoShapePtr()->_Shape;
    if (shape.IsNull()) {
        PyErr_SetString(PyExc_Exception, "shape is empty");
        return NULL;
    }

    Standard_Boolean test = shape.Closed();
    return Py_BuildValue("O", (test ? Py_True : Py_False));
}

PyObject*  TopoShapePy::isEqual(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    Standard_Boolean test = (getTopoShapePtr()->_Shape == shape);
    return Py_BuildValue("O", (test ? Py_True : Py_False));
}

PyObject*  TopoShapePy::isSame(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    Standard_Boolean test = getTopoShapePtr()->_Shape.IsSame(shape);
    return Py_BuildValue("O", (test ? Py_True : Py_False));
}

PyObject*  TopoShapePy::isValid(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    try {
        return Py_BuildValue("O", (getTopoShapePtr()->isValid() ? Py_True : Py_False));
    }
    catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "check failed, shape may be empty");
        return NULL;
    }
}

#if 0 // see ComplexGeoDataPy::Matrix which does the same
Py::Object TopoShapePy::getLocation(void) const
{
    const TopLoc_Location& loc = getTopoShapePtr()->_Shape.Location();
    gp_Trsf trf = (gp_Trsf)loc;
    Base::Matrix4D mat;
    mat[0][0] = trf.Value(1,1);
    mat[0][1] = trf.Value(1,2);
    mat[0][2] = trf.Value(1,3);
    mat[0][3] = trf.Value(1,4);
    
    mat[1][0] = trf.Value(2,1);
    mat[1][1] = trf.Value(2,2);
    mat[1][2] = trf.Value(2,3);
    mat[1][3] = trf.Value(2,4);
    
    mat[2][0] = trf.Value(3,1);
    mat[2][1] = trf.Value(3,2);
    mat[2][2] = trf.Value(3,3);
    mat[2][3] = trf.Value(3,4);
    return Py::Object(new Base::MatrixPy(mat));
}

void TopoShapePy::setLocation(Py::Object o)
{
    PyObject* p = o.ptr();
    if (PyObject_TypeCheck(p, &(Base::MatrixPy::Type))) {
        Base::Matrix4D mat = static_cast<Base::MatrixPy*>(p)->value();
        Base::Rotation rot(mat);
        Base::Vector3d axis;
        double angle;
        rot.getValue(axis, angle);
        gp_Trsf trf;
        trf.SetRotation(gp_Ax1(gp_Pnt(), gp_Dir(axis.x, axis.y, axis.z)), angle);
        trf.SetTranslationPart(gp_Vec(mat[0][3],mat[1][3],mat[2][3]));
        TopLoc_Location loc(trf);
        getTopoShapePtr()->_Shape.Location(loc);
    }
    else {
        std::string error = std::string("type must be 'Matrix', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}
#endif

Py::String TopoShapePy::getShapeType(void) const
{
    TopoDS_Shape sh = getTopoShapePtr()->_Shape;
    if (sh.IsNull())
        throw Py::Exception(PyExc_Exception, "cannot determine type of null shape");
    TopAbs_ShapeEnum type = sh.ShapeType();
    std::string name;
    switch (type)
    {
    case TopAbs_COMPOUND:
        name = "Compund";
        break;
    case TopAbs_COMPSOLID:
        name = "CompSolid";
        break;
    case TopAbs_SOLID:
        name = "Solid";
        break;
    case TopAbs_SHELL:
        name = "Shell";
        break;
    case TopAbs_FACE:
        name = "Face";
        break;
    case TopAbs_WIRE:
        name = "Wire";
        break;
    case TopAbs_EDGE:
        name = "Edge";
        break;
    case TopAbs_VERTEX:
        name = "Vertex";
        break;
    case TopAbs_SHAPE:
        name = "Shape";
        break;
    }

    return Py::String(name);
}

Py::List TopoShapePy::getFaces(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_FACE);
    while (Ex.More()) 
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeFacePy(new TopoShape(shape)),true));
    }

    return ret;
}

Py::List TopoShapePy::getVertexes(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_VERTEX);
    while (Ex.More()) 
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeVertexPy(new TopoShape(shape)),true));
    }

    return ret;
}

Py::List TopoShapePy::getShells(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_SHELL);
    while (Ex.More()) 
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeShellPy(new TopoShape(shape)),true));
    }

    return ret;
}

Py::List TopoShapePy::getSolids(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_SOLID);
    while (Ex.More()) 
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeSolidPy(new TopoShape(shape)),true));
    }

    return ret;
}

Py::List TopoShapePy::getCompSolids(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_COMPSOLID);
    while (Ex.More()) 
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeCompSolidPy(new TopoShape(shape)),true));
    }

    return ret;
}

Py::List TopoShapePy::getEdges(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_EDGE);
    while (Ex.More()) 
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeEdgePy(new TopoShape(shape)),true));
    }

    return ret;
}

Py::List TopoShapePy::getWires(void) const
{
    Py::List ret;
    TopTools_IndexedMapOfShape M;

    TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_WIRE);
    while (Ex.More())
    {
        M.Add(Ex.Current());
        Ex.Next();
    }

    for (Standard_Integer k = 1; k <= M.Extent(); k++)
    {
        const TopoDS_Shape& shape = M(k);
        ret.append(Py::Object(new TopoShapeWirePy(new TopoShape(shape)),true));
    }

    return ret;
}

PyObject *TopoShapePy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int TopoShapePy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
