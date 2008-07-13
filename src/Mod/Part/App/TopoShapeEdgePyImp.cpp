
#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRep_Tool.hxx>
# include <BRepAdaptor_Curve.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <Geom_Curve.hxx>
# include <TopoDS_Shape.hxx>
# include <TopoDS_Edge.hxx>
# include <Standard_Failure.hxx>
#endif

#include "Mod/Part/App/TopoShape.h"

// inclusion of the generated files (generated out of TopoShapeEdgePy.xml)
#include "TopoShapeEdgePy.h"
#include "TopoShapeEdgePy.cpp"

#include "Geometry.h"
#include "GeometryPy.h"
#include "LinePy.h"
#include "CirclePy.h"
#include "EllipsePy.h"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
const char *TopoShapeEdgePy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary
    // char array neither on the stack because the array would be freed when
    // leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class.
    // This, however, is not a problem as long as we only use this method in
    // _repr().

    std::stringstream str;
    str << "<Edge object at " << getTopoShapePtr() << ">";
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *TopoShapeEdgePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TopoShapeEdgePy and the Twin object 
    return new TopoShapeEdgePy(new TopoShape);
}

// constructor method
int TopoShapeEdgePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pcObj;
    double first=DBL_MAX, last=DBL_MAX;
    if (PyArg_ParseTuple(args, "O!|dd", &(Part::GeometryPy::Type), &pcObj, &first, &last)) {
        Geometry* geom = static_cast<GeometryPy*>(pcObj)->getGeometryPtr();
        Handle_Geom_Curve curve = Handle_Geom_Curve::DownCast(geom->handle());
        if (curve.IsNull()) {
            PyErr_SetString(PyExc_Exception, "geometry is not a curve type");
            return -1;
        }

        if (first==DBL_MAX)
            first = curve->FirstParameter();
        if (last==DBL_MAX)
            last = curve->LastParameter();

        try {
            BRepBuilderAPI_MakeEdge mkEdge(curve, first, last);
            getTopoShapePtr()->_Shape = mkEdge.Edge();
            return 0;
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return -1;
        }
    }

    PyErr_SetString(PyExc_Exception, "a curve expected");
    return -1;
}

PyObject* TopoShapeEdgePy::curve(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    TopoDS_Edge e = TopoDS::Edge(getTopoShapePtr()->_Shape);
    BRepAdaptor_Curve adapt(e);
    switch(adapt.GetType())
    {
    case GeomAbs_Line:
        {
            GeomLineSegment* line = new GeomLineSegment();
            Handle_Geom_TrimmedCurve this_curv = Handle_Geom_TrimmedCurve::DownCast
                (line->handle());
            Handle_Geom_Line this_line = Handle_Geom_Line::DownCast
                (this_curv->BasisCurve());
            this_line->SetLin(adapt.Line());
            this_curv->SetTrim(adapt.FirstParameter(), adapt.LastParameter());
            return new LinePy(line);
        }
    case GeomAbs_Circle:
        {
            GeomCircle* circle = new GeomCircle();
            Handle_Geom_Circle this_curv = Handle_Geom_Circle::DownCast
                (circle->handle());
            this_curv->SetCirc(adapt.Circle());
            Standard_Real dd = adapt.FirstParameter();
            Standard_Real ee = adapt.LastParameter();
            return new CirclePy(circle);
        }
    case GeomAbs_Ellipse:
        {
            GeomEllipse* elips = new GeomEllipse();
            Handle_Geom_Ellipse this_curv = Handle_Geom_Ellipse::DownCast
                (elips->handle());
            this_curv->SetElips(adapt.Ellipse());
            return new EllipsePy(elips);
        }
    case GeomAbs_Hyperbola:
        break;
    case GeomAbs_Parabola:
        break;
    case GeomAbs_BezierCurve:
        break;
    case GeomAbs_BSplineCurve:
        break;
    case GeomAbs_OtherCurve:
        break;
    }

    PyErr_SetString(PyExc_TypeError, "undefined curve type");
    return 0;
}

PyObject *TopoShapeEdgePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeEdgePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
