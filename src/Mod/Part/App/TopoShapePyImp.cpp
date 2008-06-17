
#include "PreCompiled.h"
#ifndef _PreComp_
# include <IGESControl_Controller.hxx>
# include <IGESControl_Writer.hxx>
# include <STEPControl_Writer.hxx>
# include <BRepTools.hxx>
# include <BRepCheck_Analyzer.hxx>
# include <BRepCheck_ListIteratorOfListOfStatus.hxx>
# include <BRepCheck_Result.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <BRepAlgoAPI_Common.hxx>
# include <BRepAlgoAPI_Cut.hxx>
# include <BRepAlgoAPI_Fuse.hxx>
# include <BRepAlgoAPI_Section.hxx>
#endif


#include <Base/PyCXX/Objects.hxx>
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
    return "<shape object>";
}

int TopoShapePy::PyInit(PyObject* args, PyObject*)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "|O", &pcObj))     // convert args: Python->C 
        return -1;                             // NULL triggers exception

    // if no shape is given
    if (!pcObj) return 0;
    if (PyObject_TypeCheck(pcObj, &(TopoShapePy::Type))) {
        TopoDS_Shape sh = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
        getTopoShapePtr()->_Shape = sh;
    }
    else {
        PyErr_SetString(PyExc_Exception, "shape expected");
        return -1;
    }

    return 0;
}

PyObject*  TopoShapePy::exportIges(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename ))   
        return NULL;
    // write iges file
    IGESControl_Controller::Init();
    IGESControl_Writer aWriter;
    aWriter.AddShape(getTopoShapePtr()->_Shape);

    if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone) {
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
    STEPControl_Writer aWriter;

    //FIXME: Does not work this way!!!
    if (aWriter.Transfer(getTopoShapePtr()->_Shape, STEPControl_AsIs)) {
        PyErr_SetString(PyExc_Exception,"Transferring STEP failed");
        return NULL;
    }

    if (aWriter.Write((const Standard_CString)filename) != IFSelect_RetDone) {
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

    // read brep file
    if (!BRepTools::Write(getTopoShapePtr()->_Shape,(const Standard_CString)filename)) {
        PyErr_SetString(PyExc_Exception,"Writing BREP failed");
        return NULL;
    }

    Py_Return; 
}

PyObject*  TopoShapePy::exportStl(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
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

    TopoDS_Shape self = this->getTopoShapePtr()->_Shape;
    TopoDS_Shape that = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;

    // Let's call algorithm computing a fuse operation:
    BRepAlgoAPI_Fuse mkFuse(self, that);
    // Let's check if the cut has been successful
    if (!mkFuse.IsDone()) {
        PyErr_SetString(PyExc_Exception, "operation failed");
        return 0;
    }

    return new TopoShapePy(new TopoShape(mkFuse.Shape()));
}

PyObject*  TopoShapePy::common(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape self = this->getTopoShapePtr()->_Shape;
    TopoDS_Shape that = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;

    // Let's call algorithm computing a intersection operation:
    BRepAlgoAPI_Common mkCommon(self, that);
    // Let's check if the cut has been successful
    if (!mkCommon.IsDone()) {
        PyErr_SetString(PyExc_Exception, "operation failed");
        return 0;
    }

    return new TopoShapePy(new TopoShape(mkCommon.Shape()));
}

PyObject*  TopoShapePy::section(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape self = this->getTopoShapePtr()->_Shape;
    TopoDS_Shape that = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;

    // Let's call algorithm computing a section operation:
    BRepAlgoAPI_Section mkSection(self, that);
    // Let's check if the cut has been successful
    if (!mkSection.IsDone()) {
        PyErr_SetString(PyExc_Exception, "operation failed");
        return 0;
    }

    return new TopoShapePy(new TopoShape(mkSection.Shape()));
}

PyObject*  TopoShapePy::cut(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape self = this->getTopoShapePtr()->_Shape;
    TopoDS_Shape that = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;

    // Let's call algorithm computing a cut operation:
    BRepAlgoAPI_Cut mkCut(self, that);
    // Let's check if the cut has been successful
    if (!mkCut.IsDone()) {
        PyErr_SetString(PyExc_Exception, "operation failed");
        return 0;
    }

    return new TopoShapePy(new TopoShape(mkCut.Shape()));
}

Py::Bool TopoShapePy::getNull(void) const
{
    Standard_Boolean null = getTopoShapePtr()->_Shape.IsNull();
    return Py::Bool(null ? true : false);
}

Py::Bool TopoShapePy::getValid(void) const
{
    throw Py::Exception(PyExc_NotImplementedError, "Valid not implemented");
    //return Py::Bool(false);
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
