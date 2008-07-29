
#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRep_Tool.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <ShapeAnalysis.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <Geom_BezierSurface.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Geom_Plane.hxx>
# include <Geom_CylindricalSurface.hxx>
# include <Geom_ConicalSurface.hxx>
# include <Geom_SphericalSurface.hxx>
# include <Geom_ToroidalSurface.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <gp_Pln.hxx>
# include <gp_Cylinder.hxx>
# include <gp_Cone.hxx>
# include <gp_Sphere.hxx>
# include <gp_Torus.hxx>
#endif

#include <Base/VectorPy.h>

#include "TopoShape.h"
#include "TopoShapeSolidPy.h"
#include "TopoShapeFacePy.h"
#include "TopoShapeFacePy.cpp"

#include "BezierSurfacePy.h"
#include "BSplineSurfacePy.h"
#include "PlanePy.h"
#include "CylinderPy.h"
#include "ConePy.h"
#include "SpherePy.h"
#include "OffsetSurfacePy.h"
#include "SurfaceOfRevolutionPy.h"
#include "SurfaceOfExtrusionPy.h"
#include "ToroidPy.h"

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
    PyObject *pW;
    if (PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &pW)) {
        try {
            TopoDS_Shape sh = static_cast<Part::TopoShapePy*>(pW)->getTopoShapePtr()->_Shape;
            if (sh.IsNull()) {
                PyErr_SetString(PyExc_Exception, "cannot create face out of empty wire");
                return -1;
            }

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

PyObject* TopoShapeFacePy::extrude(PyObject *args)
{
    PyObject *pVec;
    if (PyArg_ParseTuple(args, "O!", &(Base::VectorPy::Type), &pVec)) {
        try {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pVec)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->makePrism(gp_Vec(vec.x,vec.y,vec.z));
            return new TopoShapeSolidPy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

PyObject* TopoShapeFacePy::revolve(PyObject *args)
{
    PyObject *pPos,*pDir;
    double d=2.0*Standard_PI;
    if (PyArg_ParseTuple(args, "O!O!|d", &(Base::VectorPy::Type), &pPos, &(Base::VectorPy::Type), &pDir,&d)) {
        try {
            Base::Vector3d pos = static_cast<Base::VectorPy*>(pPos)->value();
            Base::Vector3d dir = static_cast<Base::VectorPy*>(pDir)->value();
            TopoDS_Shape shape = this->getTopoShapePtr()->revolve(
                gp_Ax1(gp_Pnt(pos.x,pos.y,pos.z), gp_Dir(dir.x,dir.y,dir.z)),d);
            return new TopoShapeSolidPy(new TopoShape(shape));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    }

    return 0;
}

Py::Object TopoShapeFacePy::getSurface() const
{
    TopoDS_Face f = TopoDS::Face(getTopoShapePtr()->_Shape);
    BRepAdaptor_Surface adapt(f);
    switch(adapt.GetType())
    {
    case GeomAbs_Plane:
        {
            GeomPlane* plane = new GeomPlane();
            Handle_Geom_Plane this_surf = Handle_Geom_Plane::DownCast
                (plane->handle());
            this_surf->SetPln(adapt.Plane());
            return Py::Object(new PlanePy(plane));
        }
    case GeomAbs_Cylinder:
        {
            GeomCylinder* cylinder = new GeomCylinder();
            Handle_Geom_CylindricalSurface this_surf = Handle_Geom_CylindricalSurface::DownCast
                (cylinder->handle());
            this_surf->SetCylinder(adapt.Cylinder());
            return Py::Object(new CylinderPy(cylinder));
        }
    case GeomAbs_Cone:
        {
            GeomCone* cone = new GeomCone();
            Handle_Geom_ConicalSurface this_surf = Handle_Geom_ConicalSurface::DownCast
                (cone->handle());
            this_surf->SetCone(adapt.Cone());
            return Py::Object(new ConePy(cone));
        }
    case GeomAbs_Sphere:
        {
            GeomSphere* sphere = new GeomSphere();
            Handle_Geom_SphericalSurface this_surf = Handle_Geom_SphericalSurface::DownCast
                (sphere->handle());
            this_surf->SetSphere(adapt.Sphere());
            return Py::Object(new SpherePy(sphere));
        }
    case GeomAbs_Torus:
        {
            GeomToroid* toroid = new GeomToroid();
            Handle_Geom_ToroidalSurface this_surf = Handle_Geom_ToroidalSurface::DownCast
                (toroid->handle());
            this_surf->SetTorus(adapt.Torus());
            return Py::Object(new ToroidPy(toroid));
        }
    case GeomAbs_BezierSurface:
        {
            GeomBezierSurface* surf = new GeomBezierSurface(adapt.Bezier());
            return Py::Object(new BezierSurfacePy(surf));
        }
    case GeomAbs_BSplineSurface:
        {
            GeomBSplineSurface* surf = new GeomBSplineSurface(adapt.BSpline());
            return Py::Object(new BSplineSurfacePy(surf));
        }
    case GeomAbs_SurfaceOfRevolution:
        {
            Handle_Geom_Surface s = BRep_Tool::Surface(f);
            Handle_Geom_SurfaceOfRevolution rev = Handle_Geom_SurfaceOfRevolution::DownCast(s);
            if (!rev.IsNull()) {
                GeomSurfaceOfRevolution* surf = new GeomSurfaceOfRevolution(rev);
                return Py::Object(new SurfaceOfRevolutionPy(surf));
            }
        }
    case GeomAbs_SurfaceOfExtrusion:
        {
            Handle_Geom_Surface s = BRep_Tool::Surface(f);
            Handle_Geom_SurfaceOfLinearExtrusion ext = Handle_Geom_SurfaceOfLinearExtrusion::DownCast(s);
            if (!ext.IsNull()) {
                GeomSurfaceOfExtrusion* surf = new GeomSurfaceOfExtrusion(ext);
                return Py::Object(new SurfaceOfExtrusionPy(surf));
            }
        }
    case GeomAbs_OffsetSurface:
        {
            Handle_Geom_Surface s = BRep_Tool::Surface(f);
            Handle_Geom_OffsetSurface off = Handle_Geom_OffsetSurface::DownCast(s);
            if (!off.IsNull()) {
                GeomOffsetSurface* surf = new GeomOffsetSurface(off);
                return Py::Object(new OffsetSurfacePy(surf));
            }
        }
    case GeomAbs_OtherSurface:
        break;
    }

    throw Py::TypeError("undefined surface type");
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
