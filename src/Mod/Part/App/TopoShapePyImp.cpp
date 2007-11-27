
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
#endif


#include <Base/PyCXX/Objects.hxx>
#include "TopoShape.h"

// inclusion of the generated files (generated out of TopoShapePy.xml)
#include "TopoShapePy.h"
#include "TopoShapePy.cpp"

using namespace Part;

// returns a string which represent the object e.g. when printed in python
const char *TopoShapePy::representation(void) const
{
	return "<shape object>";
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
  if (!PyArg_ParseTuple(args, "s", &filename ))   
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
  if (!PyArg_ParseTuple(args, "s", &filename ))
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
	return 0;
}

PyObject*  TopoShapePy::check(PyObject *args)
{
	  if (!PyArg_ParseTuple(args, "" ))   
    return NULL;
  if ( !getTopoShapePtr()->_Shape.IsNull() )
  {
    BRepCheck_Analyzer aChecker(getTopoShapePtr()->_Shape);
    if (!aChecker.IsValid())
    {
      TopoDS_Iterator it(getTopoShapePtr()->_Shape);
      for (;it.More(); it.Next())
      {
        if (!aChecker.IsValid(it.Value()))
        {
          const Handle_BRepCheck_Result& result = aChecker.Result(it.Value());
          const BRepCheck_ListOfStatus& status = result->StatusOnShape(it.Value());

          BRepCheck_ListIteratorOfListOfStatus it(status);
          while ( it.More() )
          {
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
	Py::List ret;

  TopExp_Explorer Ex(getTopoShapePtr()->_Shape,TopAbs_VERTEX);
  while (Ex.More()) {
    //BRepBuilderAPI_Copy copy(Ex.Current());
    //TopoDS_Shape shape = copy.Shape();
    TopoDS_Shape shape = Ex.Current();
    ret.append(Py::Object(new TopoShapePy(new TopoShape(shape)),true));
    Ex.Next();
  }

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


