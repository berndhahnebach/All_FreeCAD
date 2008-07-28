
#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRep_Tool.hxx>
# include <BRepAdaptor_Curve.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <Geom_Circle.hxx>
# include <Geom_Curve.hxx>
# include <Geom_Ellipse.hxx>
# include <Geom_Hyperbola.hxx>
# include <Geom_Parabola.hxx>
# include <Geom_Line.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <Geom_BezierCurve.hxx>
# include <Geom_BSplineCurve.hxx>
# include <gp_Circ.hxx>
# include <gp_Elips.hxx>
# include <gp_Hypr.hxx>
# include <gp_Parab.hxx>
# include <gp_Lin.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Shape.hxx>
# include <TopoDS_Edge.hxx>
# include <Standard_Failure.hxx>
#endif

#include <Base/VectorPy.h>

#include "TopoShape.h"
#include "TopoShapeFacePy.h"
#include "TopoShapeEdgePy.h"
#include "TopoShapeEdgePy.cpp"

#include "Geometry.h"
#include "GeometryPy.h"
#include "LinePy.h"
#include "CirclePy.h"
#include "EllipsePy.h"
#include "HyperbolaPy.h"
#include "ParabolaPy.h"
#include "BezierCurvePy.h"
#include "BSplineCurvePy.h"

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

PyObject* TopoShapeEdgePy::extrude(PyObject *args)
{
    PyObject *pVec;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type), &pVec)) {
        try {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pVec)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->makePrism(gp_Vec(vec.x,vec.y,vec.z));
            return new TopoShapeFacePy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

Py::Object TopoShapeEdgePy::getCurve() const
{
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
            return Py::Object(new LinePy(line));
        }
    case GeomAbs_Circle:
        {
            GeomCircle* circle = new GeomCircle();
            Handle_Geom_Circle this_curv = Handle_Geom_Circle::DownCast
                (circle->handle());
            this_curv->SetCirc(adapt.Circle());
            //Standard_Real dd = adapt.FirstParameter();
            //Standard_Real ee = adapt.LastParameter();
            return Py::Object(new CirclePy(circle));
        }
    case GeomAbs_Ellipse:
        {
            GeomEllipse* elips = new GeomEllipse();
            Handle_Geom_Ellipse this_curv = Handle_Geom_Ellipse::DownCast
                (elips->handle());
            this_curv->SetElips(adapt.Ellipse());
            return Py::Object(new EllipsePy(elips));
        }
    case GeomAbs_Hyperbola:
        {
            GeomHyperbola* hyper = new GeomHyperbola();
            Handle_Geom_Hyperbola this_curv = Handle_Geom_Hyperbola::DownCast
                (hyper->handle());
            this_curv->SetHypr(adapt.Hyperbola());
            return Py::Object(new HyperbolaPy(hyper));
        }
    case GeomAbs_Parabola:
        {
            GeomParabola* hyper = new GeomParabola();
            Handle_Geom_Parabola this_curv = Handle_Geom_Parabola::DownCast
                (hyper->handle());
            this_curv->SetParab(adapt.Parabola());
            return Py::Object(new ParabolaPy(hyper));
        }
    case GeomAbs_BezierCurve:
        {
            GeomBezierCurve* curve = new GeomBezierCurve(adapt.Bezier());
            return Py::Object(new BezierCurvePy(curve));
        }
    case GeomAbs_BSplineCurve:
        {
            GeomBSplineCurve* curve = new GeomBSplineCurve(adapt.BSpline());
            return Py::Object(new BSplineCurvePy(curve));
        }
    case GeomAbs_OtherCurve:
        break;
    }

    throw Py::TypeError("undefined curve type");
}

PyObject *TopoShapeEdgePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeEdgePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
