/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <BRepPrimAPI_MakeBox.hxx>
# include <BRepPrimAPI_MakeCone.hxx>
# include <BRepPrimAPI_MakeCylinder.hxx>
# include <BRepPrimAPI_MakeSphere.hxx>
# include <BRep_Builder.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <BRepBuilderAPI_MakeWire.hxx>
# include <BRepBuilderAPI_MakePolygon.hxx>
# include <gp_Circ.hxx>
# include <gp_Pnt.hxx>
# include <gp_Lin.hxx>
# include <Geom_Circle.hxx>
# include <Geom_Line.hxx>
# include <Geom_Plane.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Geom_OffsetSurface.hxx>
# include <GeomAPI_PointsToBSplineSurface.hxx>
# include <Handle_Geom_Circle.hxx>
# include <Handle_Geom_Plane.hxx>
# include <Standard_ConstructionError.hxx>
# include <Standard_DomainError.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopoDS_Compound.hxx>
# include <TopExp_Explorer.hxx>
# include <TColgp_HArray2OfPnt.hxx>
# include <TColStd_Array1OfReal.hxx>
# include <TColStd_Array1OfInteger.hxx>
# include <Precision.hxx>
#endif

#include <stdio.h>

#if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
#endif // (re-)defined in pyconfig.h
#include <Python.h>

#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include <Base/Interpreter.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/GeometryPyCXX.h>
#include <Base/VectorPy.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObjectPy.h>

#include "TopoShape.h"
#include "TopoShapePy.h"
#include "TopoShapeEdgePy.h"
#include "TopoShapeWirePy.h"
#include "TopoShapeFacePy.h"
#include "TopoShapeCompoundPy.h"
#include "TopoShapeSolidPy.h"
#include "GeometryPy.h"
#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "ImportStep.h"

using Base::Console;
using namespace Part;
using namespace std;

extern const char* BRepBuilderAPI_FaceErrorText(BRepBuilderAPI_FaceError fe);

/* module functions */
static PyObject * open(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        //Base::Console().Log("Open in Part with %s",Name);
        Base::FileInfo file(Name);

        // extract ending
        if (file.extension() == "")
            Py_Error(PyExc_Exception,"no file ending");

        if (file.hasExtension("stp") || file.hasExtension("step")) {
            // create new document and add Import feature
            App::Document *pcDoc = App::GetApplication().newDocument("Unnamed");
#if 1
            ImportStepParts(pcDoc,Name);
#else
            Part::ImportStep *pcFeature = (Part::ImportStep *)pcDoc->addObject("Part::ImportStep",file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
#endif 
            pcDoc->recompute();
        }
        else if (file.hasExtension("igs") || file.hasExtension("iges")) {
            // create new document and add Import feature
            App::Document *pcDoc = App::GetApplication().newDocument(file.fileNamePure().c_str());
            Part::ImportIges *pcFeature = (Part::ImportIges*) pcDoc->addObject("Part::ImportIges",file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
            pcDoc->recompute();
        }
        else if (file.hasExtension("brp") || file.hasExtension("brep")) {
            // create new document and add Import feature
            App::Document *pcDoc = App::GetApplication().newDocument(file.fileNamePure().c_str());
            Part::ImportBrep *pcFeature = (Part::ImportBrep *)pcDoc->addObject("Part::ImportBrep",file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
            pcDoc->recompute();
        }
        else {
            Py_Error(PyExc_Exception,"unknown file ending");
        }
    } PY_CATCH;

    Py_Return;
}


/* module functions */
static PyObject * insert(PyObject *self, PyObject *args)
{
    const char* Name;
    const char* DocName;
    if (!PyArg_ParseTuple(args, "ss",&Name,&DocName))
        return NULL;                         

    PY_TRY {
        //Base::Console().Log("Insert in Part with %s",Name);
        Base::FileInfo file(Name);

        // extract ending
        if (file.extension() == "")
            Py_Error(PyExc_Exception,"no file ending");
        App::Document *pcDoc = App::GetApplication().getDocument(DocName);
        if (!pcDoc) {
            PyErr_Format(PyExc_Exception, "Import called to the non-existing document '%s'", DocName);
            return NULL;
        }

        if (file.hasExtension("stp") || file.hasExtension("step")) {
#if 1
            ImportStepParts(pcDoc,Name);
#else
            // add Import feature
            Part::ImportStep *pcFeature = (Part::ImportStep *)pcDoc->addObject("Part::ImportStep",file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
#endif 
            pcDoc->recompute();
        }
        else if (file.hasExtension("igs") || file.hasExtension("iges")) {
            Part::ImportIges *pcFeature = (Part::ImportIges *)pcDoc->addObject("Part::ImportIges",file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
            pcDoc->recompute();
        }
        else if (file.hasExtension("brp") || file.hasExtension("brep")) {
            Part::ImportBrep *pcFeature = (Part::ImportBrep *) pcDoc->addObject("Part::ImportBrep",file.fileNamePure().c_str());
            pcFeature->FileName.setValue(Name);
            pcDoc->recompute();
        }
        else {
            Py_Error(PyExc_Exception,"unknown file ending");
        }
    } PY_CATCH;

    Py_Return;
}

/* module functions */
static PyObject * exporter(PyObject *self, PyObject *args)
{
    PyObject* object;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os",&object,&filename))
        return NULL;

    BRep_Builder builder;
    TopoDS_Compound comp;
    builder.MakeCompound(comp);

    PY_TRY {
        Py::List list(object);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            PyObject* item = (*it).ptr();
            if (PyObject_TypeCheck(item, &(App::DocumentObjectPy::Type))) {
                App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(item)->getDocumentObjectPtr();
                if (obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
                    Part::Feature* part = static_cast<Part::Feature*>(obj);
                    builder.Add(comp, part->Shape.getValue());
                }
            }
        }
    } PY_CATCH;

    TopoShape shape(comp);
    shape.write(filename);

    Py_Return;
}

/* module functions */
static PyObject * read(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         
    PY_TRY {
        TopoShape* shape = new TopoShape();
        shape->read(Name);
        return new TopoShapePy(shape); 
    } PY_CATCH;
}

static PyObject * 
show(PyObject *self, PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        App::Document *pcDoc = App::GetApplication().getActiveDocument(); 	 
        if (!pcDoc)
            pcDoc = App::GetApplication().newDocument();
        TopoShapePy* pShape = static_cast<TopoShapePy*>(pcObj);
        Part::Feature *pcFeature = (Part::Feature *)pcDoc->addObject("Part::Feature", "Shape");
        // copy the data
        //TopoShape* shape = new MeshObject(*pShape->getTopoShapeObjectPtr());
        pcFeature->Shape.setValue(pShape->getTopoShapePtr()->_Shape);
        pcDoc->recompute();
    } PY_CATCH;

    Py_Return;
}

static PyObject * 
makeCompound(PyObject *self, PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(PyList_Type), &pcObj))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        BRep_Builder builder;
        TopoDS_Compound Comp;
        builder.MakeCompound(Comp);
        
        try {
            Py::List list(pcObj);
            for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
                if (PyObject_TypeCheck((*it).ptr(), &(Part::GeometryPy::Type))) {
                    TopoDS_Shape sh = static_cast<GeometryPy*>((*it).ptr())->
                        getGeometryPtr()->toShape();
                    builder.Add(Comp, sh);
                }
            }
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }

        return new TopoShapeCompoundPy(new TopoShape(Comp));
    } PY_CATCH;
}

static PyObject * makePlane(PyObject *self, PyObject *args)
{
    double length, width;
    PyObject *pPnt=0, *pDir=0;
    if (!PyArg_ParseTuple(args, "dd|O!O!", &length, &width,
                                           &(Base::VectorPy::Type), &pPnt,
                                           &(Base::VectorPy::Type), &pDir))
        return NULL;

    if (length < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "length of plane too small");
        return NULL;
    }
    if (width < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "width of plane too small");
        return NULL;
    }

    try {
        gp_Pnt p(0,0,0);
        gp_Dir d(0,0,1);
        if (pPnt) {
            Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
            p.SetCoord(pnt.x, pnt.y, pnt.z);
        }
        if (pDir) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
            d.SetCoord(vec.x, vec.y, vec.z);
        }
        Handle_Geom_Plane aPlane = new Geom_Plane(p, d);
        BRepBuilderAPI_MakeFace Face(aPlane, 0.0, length, 0.0, width);
        return new TopoShapeFacePy(new TopoShape((Face.Face()))); 
    }
    catch (Standard_DomainError) {
        PyErr_SetString(PyExc_Exception, "creation of plane failed");
        return NULL;
    }
}

static PyObject * makeBox(PyObject *self, PyObject *args)
{
    double length, width, height;
    PyObject *pPnt=0, *pDir=0;
    if (!PyArg_ParseTuple(args, "ddd|O!O!", &length, &width, &height,
                                            &(Base::VectorPy::Type), &pPnt,
                                            &(Base::VectorPy::Type), &pDir))
        return NULL;

    if (length < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "length of box too small");
        return NULL;
    }
    if (width < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "width of box too small");
        return NULL;
    }
    if (height < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "height of box too small");
        return NULL;
    }

    try {
        gp_Pnt p(0,0,0);
        gp_Dir d(0,0,1);
        if (pPnt) {
            Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
            p.SetCoord(pnt.x, pnt.y, pnt.z);
        }
        if (pDir) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
            d.SetCoord(vec.x, vec.y, vec.z);
        }
        BRepPrimAPI_MakeBox mkBox(gp_Ax2(p,d), length, width, height);
        TopoDS_Shape ResultShape = mkBox.Shape();
        return new TopoShapeSolidPy(new TopoShape(ResultShape)); 
    }
    catch (Standard_DomainError) {
        PyErr_SetString(PyExc_Exception, "creation of box failed");
        return NULL;
    }
}

static PyObject * makeCircle(PyObject *self, PyObject *args)
{
    double radius, angle1=0.0, angle2=2.0*Standard_PI;
    PyObject *pPnt=0, *pDir=0;
    if (!PyArg_ParseTuple(args, "d|O!O!dd", &radius,
                                            &(Base::VectorPy::Type), &pPnt,
                                            &(Base::VectorPy::Type), &pDir,
                                            &angle1, &angle2))
        return NULL;

    try {
        gp_Pnt loc(0,0,0);
        gp_Dir dir(0,0,1);
        if (pPnt) {
            Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
            loc.SetCoord(pnt.x, pnt.y, pnt.z);
        }
        if (pDir) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
            dir.SetCoord(vec.x, vec.y, vec.z);
        }
        gp_Ax1 axis(loc, dir);
        gp_Circ circle;
        circle.SetAxis(axis);
        circle.SetRadius(radius);

        Handle_Geom_Circle hCircle = new Geom_Circle (circle);
        BRepBuilderAPI_MakeEdge aMakeEdge(hCircle, angle1, angle2);
        TopoDS_Edge edge = aMakeEdge.Edge();
        return new TopoShapeEdgePy(new TopoShape(edge)); 
    }
    catch (Standard_Failure) {
        PyErr_SetString(PyExc_Exception, "creation of circle failed");
        return NULL;
    }
}

static PyObject * makeSphere(PyObject *self, PyObject *args)
{
    double radius, angle1=-M_PI_2, angle2=M_PI_2, angle3=2.0*Standard_PI;
    PyObject *pPnt=0, *pDir=0;
    if (!PyArg_ParseTuple(args, "d|O!O!ddd", &radius,
                                             &(Base::VectorPy::Type), &pPnt,
                                             &(Base::VectorPy::Type), &pDir,
                                             &angle1, &angle2, &angle3))
        return NULL;

    try {
        gp_Pnt p(0,0,0);
        gp_Dir d(0,0,1);
        if (pPnt) {
            Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
            p.SetCoord(pnt.x, pnt.y, pnt.z);
        }
        if (pDir) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
            d.SetCoord(vec.x, vec.y, vec.z);
        }
        BRepPrimAPI_MakeSphere mkSphere(gp_Ax2(p,d), radius, angle1, angle2, angle3);
        TopoDS_Shape shape = mkSphere.Shape();
        return new TopoShapeSolidPy(new TopoShape(shape));
    }
    catch (Standard_DomainError) {
        PyErr_SetString(PyExc_Exception, "creation of sphere failed");
        return NULL;
    }
}

static PyObject * makeCylinder(PyObject *self, PyObject *args)
{
    double radius, height, angle=2.0*Standard_PI;
    PyObject *pPnt=0, *pDir=0;
    if (!PyArg_ParseTuple(args, "dd|O!O!d", &radius, &height,
                                            &(Base::VectorPy::Type), &pPnt,
                                            &(Base::VectorPy::Type), &pDir,
                                            &angle))
        return NULL;

    try {
        gp_Pnt p(0,0,0);
        gp_Dir d(0,0,1);
        if (pPnt) {
            Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
            p.SetCoord(pnt.x, pnt.y, pnt.z);
        }
        if (pDir) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
            d.SetCoord(vec.x, vec.y, vec.z);
        }
        BRepPrimAPI_MakeCylinder mkCyl(gp_Ax2(p,d),radius, height, angle);
        TopoDS_Shape shape = mkCyl.Shape();
        return new TopoShapeSolidPy(new TopoShape(shape));
    }
    catch (Standard_DomainError) {
        PyErr_SetString(PyExc_Exception, "creation of cylinder failed");
        return NULL;
    }
}

static PyObject * makeCone(PyObject *self, PyObject *args)
{
    double radius1, radius2,  height, angle=2.0*Standard_PI;
    PyObject *pPnt=0, *pDir=0;
    if (!PyArg_ParseTuple(args, "ddd|O!O!d", &radius1, &radius2, &height,
                                             &(Base::VectorPy::Type), &pPnt,
                                             &(Base::VectorPy::Type), &pDir,
                                             &angle))
        return NULL;

    try {
        gp_Pnt p(0,0,0);
        gp_Dir d(0,0,1);
        if (pPnt) {
            Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
            p.SetCoord(pnt.x, pnt.y, pnt.z);
        }
        if (pDir) {
            Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
            d.SetCoord(vec.x, vec.y, vec.z);
        }
        BRepPrimAPI_MakeCone mkCone(gp_Ax2(p,d),radius1, radius2, height, angle);
        TopoDS_Shape shape = mkCone.Shape();
        return new TopoShapeSolidPy(new TopoShape(shape));
    }
    catch (Standard_DomainError) {
        PyErr_SetString(PyExc_Exception, "creation of cone failed");
        return NULL;
    }
}

static PyObject * makeLine(PyObject *self, PyObject *args)
{
    PyObject *obj1, *obj2;
    if (!PyArg_ParseTuple(args, "OO", &obj1, &obj2))
        return NULL;

    Base::Vector3d pnt1, pnt2;
    if (PyObject_TypeCheck(obj1, &(Base::VectorPy::Type))) {
        pnt1 = static_cast<Base::VectorPy*>(obj1)->value();
    }
    else if (PyObject_TypeCheck(obj1, &PyTuple_Type)) {
        try {
            pnt1 = Base::getVectorFromTuple<double>(obj1);
        }
        catch (const Py::Exception&) {
            return NULL;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "first argument must either be vector or tuple");
        return 0;
    }
    if (PyObject_TypeCheck(obj2, &(Base::VectorPy::Type))) {
        pnt2 = static_cast<Base::VectorPy*>(obj2)->value();
    }
    else if (PyObject_TypeCheck(obj2, &PyTuple_Type)) {
        try {
            pnt2 = Base::getVectorFromTuple<double>(obj2);
        }
        catch (const Py::Exception&) {
            return NULL;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "second argument must either be vector or tuple");
        return 0;
    }

    // Create directly the underlying line geometry
    BRepBuilderAPI_MakeEdge makeEdge(gp_Pnt(pnt1.x, pnt1.y, pnt1.z),
                                     gp_Pnt(pnt2.x, pnt2.y, pnt2.z));

    const char *error=0;
    switch (makeEdge.Error())
    {
    case BRepBuilderAPI_EdgeDone:
        break; // ok
    case BRepBuilderAPI_PointProjectionFailed:
        error = "Point projection failed";
        break;
    case BRepBuilderAPI_ParameterOutOfRange:
        error = "Parameter out of range";
        break;
    case BRepBuilderAPI_DifferentPointsOnClosedCurve:
        error = "Different points on closed curve";
        break;
    case BRepBuilderAPI_PointWithInfiniteParameter:
        error = "Point with infinite parameter";
        break;
    case BRepBuilderAPI_DifferentsPointAndParameter:
        error = "Different point and parameter";
        break;
    case BRepBuilderAPI_LineThroughIdenticPoints:
        error = "Line through identic points";
        break;
    }
    // Error 
    if (error) {
        PyErr_SetString(PyExc_RuntimeError, error);
        return NULL;
    }

    TopoDS_Edge edge = makeEdge.Edge();
    return new TopoShapeEdgePy(new TopoShape(edge)); 
}

static PyObject * makePolygon(PyObject *self, PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(PyList_Type), &pcObj))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        BRepBuilderAPI_MakePolygon mkPoly;
        try {
            Py::List list(pcObj);
            for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
                if (PyObject_TypeCheck((*it).ptr(), &(Base::VectorPy::Type))) {
                    Base::Vector3d v = static_cast<Base::VectorPy*>((*it).ptr())->value();
                    mkPoly.Add(gp_Pnt(v.x,v.y,v.z));
                }
                else if (PyObject_TypeCheck((*it).ptr(), &PyTuple_Type)) {
                    try {
                        Base::Vector3d v = Base::getVectorFromTuple<double>((*it).ptr());
                        mkPoly.Add(gp_Pnt(v.x,v.y,v.z));
                    }
                    catch (const Py::Exception&) {
                        return 0;
                    }
                }
            }

            if (!mkPoly.IsDone())
                Standard_Failure::Raise("Cannot create polygon because less than two vetices are given");

            return new TopoShapeWirePy(new TopoShape(mkPoly.Wire()));
        }
        catch (Standard_Failure) {
            Handle_Standard_Failure e = Standard_Failure::Caught();
            PyErr_SetString(PyExc_Exception, e->GetMessageString());
            return 0;
        }
    } PY_CATCH;
}

static PyObject * toPythonOCC(PyObject *self, PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))
        return NULL;

    try {
        TopoDS_Shape* shape = new TopoDS_Shape();
        (*shape) = static_cast<TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
        PyObject* proxy = 0;
        proxy = Base::Interpreter().createSWIGPointerObj("TopoDS_Shape *", (void*)shape, 1);
        return proxy;
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }
}

static PyObject * fromPythonOCC(PyObject *self, PyObject *args)
{
    PyObject *proxy;
    if (!PyArg_ParseTuple(args, "O", &proxy))
        return NULL;

    void* ptr;
    try {
        TopoShape* shape = new TopoShape();
        Base::Interpreter().convertSWIGPointerObj("TopoDS_Shape *", proxy, &ptr, 0);
        TopoDS_Shape* s = reinterpret_cast<TopoDS_Shape*>(ptr);
        shape->_Shape = (*s);
        return new TopoShapePy(shape);
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }
}

namespace Part {
struct HashEdge {
    int v1, v2;
    TopoDS_Edge edge;
};

static std::list<TopoDS_Edge> sort_Edges(const std::vector<TopoDS_Edge>& edges)
{
    std::list<HashEdge>  hash_edge;
    TopExp_Explorer xp;
    for (std::vector<TopoDS_Edge>::const_iterator it = edges.begin(); it != edges.end(); ++it) {
        HashEdge he;
        xp.Init(*it,TopAbs_VERTEX);
        he.v1 = xp.Current().HashCode(IntegerLast());
        xp.Next();
        he.v2 = xp.Current().HashCode(IntegerLast());
        he.edge = *it;
        hash_edge.push_back(he);
    }

    if (hash_edge.empty())
        return std::list<TopoDS_Edge>();

    std::list<TopoDS_Edge> sorted;
    int first, last;
    first = hash_edge.front().v1;
    last  = hash_edge.front().v2;

    sorted.push_back(hash_edge.front().edge);
    hash_edge.erase(hash_edge.begin());

    while (!hash_edge.empty()) {
        // search for adjacent edge
        std::list<HashEdge>::iterator pEI;
        for (pEI = hash_edge.begin(); pEI != hash_edge.end(); ++pEI) {
            if (pEI->v1 == last) {
                last = pEI->v2;
                sorted.push_back(pEI->edge);
                hash_edge.erase(pEI);
                break;
            }
            else if (pEI->v2 == first) {
                first = pEI->v1;
                sorted.push_front(pEI->edge);
                hash_edge.erase(pEI);
                break;
            }
            else if (pEI->v2 == last) {
                last = pEI->v1;
                sorted.push_back(pEI->edge);
                hash_edge.erase(pEI);
                break;
            }
            else if (pEI->v1 == first) {
                first = pEI->v2;
                sorted.push_front(pEI->edge);
                hash_edge.erase(pEI);
                break;
            }
        }

        if ((pEI == hash_edge.end()) || (last == first)) {
            // no adjacent edge found or polyline is closed
            return sorted;
            /*
            rclBorders.push_back(std::vector<TopoDS_Edge>(sorted.begin(), sorted.end()));
            sorted.clear();

            if (!hash_edge.empty()) {
                // new wire
                first = hash_edge.front()->v1;
                last  = hash_edge.front()->v2;
                sorted.push_back(hash_edge.front().edge);
                hash_edge.erase(hash_edge.begin());
            }*/
        }
    }

    return sorted;
}
}

static PyObject * sortEdges(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(PyList_Type), &obj)) {
        PyErr_SetString(PyExc_Exception, "list of edges expected");
        return 0;
    }

    Py::List list(obj);
    std::vector<TopoDS_Edge> edges;
    for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
        PyObject* item = (*it).ptr();
        if (PyObject_TypeCheck(item, &(Part::TopoShapePy::Type))) {
            TopoDS_Shape sh = static_cast<Part::TopoShapePy*>(item)->getTopoShapePtr()->_Shape;
            if (sh.ShapeType() == TopAbs_EDGE)
                edges.push_back(TopoDS::Edge(sh));
            else {
                PyErr_SetString(PyExc_TypeError, "shape is not an edge");
                return 0;
            }
        }
        else {
            PyErr_SetString(PyExc_TypeError, "item is not a shape");
            return 0;
        }
    }

    std::list<TopoDS_Edge> sorted = sort_Edges(edges);
    Py::List sorted_list;
    for (std::list<TopoDS_Edge>::iterator it = sorted.begin(); it != sorted.end(); ++it) {
        sorted_list.append(Py::Object(new TopoShapeEdgePy(new TopoShape(*it)),true));
    }

    return Py::new_reference_to(sorted_list);
}

/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"open"       ,open      ,METH_VARARGS,
     "open(string) -- Create a new document and load the file into the document."},
    {"insert"     ,insert    ,METH_VARARGS,
     "insert(string,string) -- Insert the file into the given document."},
    {"export"     ,exporter  ,METH_VARARGS,
     "export(list,string) -- Export a list of objects into a single file."},
    {"read"       ,read      ,METH_VARARGS,
     "read(string) -- Load the file and return the shape."},
    {"show"       ,show      ,METH_VARARGS,
     "show(shape) -- Add the shape to the active document or create one if no document exists."},
    {"makeCompound"  ,makeCompound ,METH_VARARGS,
     "makeCompound(list) -- Create a compound out of a list of geometries."},
    {"makePlane"  ,makePlane ,METH_VARARGS,
     "makePlane(length,width,[pnt,dir]) -- Make a plane\n"
     "By default pnt=Vector(0,0,0) and dir=Vector(0,0,1)"},
    {"makeBox"    ,makeBox ,METH_VARARGS,
     "makeBox(length,width,height,[pnt,dir]) -- Make a box located\n"
     "in pnt with the dimensions (length,width,height)\n"
     "By default pnt=Vector(0,0,0) and dir=Vector(0,0,1)"},
    {"makeLine"   ,makeLine  ,METH_VARARGS,
     "makeLine((x1,y1,z1),(x2,y2,z2)) -- Make a line of two points"},
    {"makePolygon"   ,makePolygon  ,METH_VARARGS,
     "makePolygon(list) -- Make a polygon of a list of points"},
    {"makeCircle" ,makeCircle,METH_VARARGS,
     "makeCircle(radius,[pnt,dir,angle1,angle2]) -- Make a circle with a given radius\n"
     "By default pnt=Vector(0,0,0), dir=Vector(0,0,1), angle1=0 and angle2=2*PI"},
    {"makeSphere" ,makeSphere,METH_VARARGS,
     "makeSphere(radius,[pnt, dir, angle1,angle2,angle3]) -- Make a sphere with a given radius\n"
     "By default pnt=Vector(0,0,0), dir=Vector(0,0,1), angle1=0, angle2=0.5*PI and angle3=2*PI"},
    {"makeCylinder" ,makeCylinder,METH_VARARGS,
     "makeCylinder(radius,height,[pnt,dir,angle]) -- Make a cylinder with a given radius and height\n"
     "By default pnt=Vector(0,0,0),dir=Vector(0,0,1) and angle=2*PI"},
    {"makeCone" ,makeCone,METH_VARARGS,
     "makeCone(radius1,radius2,height,[pnt,dir,angle]) -- Make a cone with given radii and height\n"
     "By default pnt=Vector(0,0,0), dir=Vector(0,0,1) and angle=2*PI"},
    {"__sortEdges__" ,sortEdges,METH_VARARGS,
     "__sortEdges__(list of edges) -- Helper method to sort an unsorted list of edges so that afterwards\n"
     "two adjacent edges share a common vertex"},
    {"__toPythonOCC__" ,toPythonOCC,METH_VARARGS,
     "__toPythonOCC__(shape) -- Helper method to convert an internal shape to pythonocc shape"},
    {"__fromPythonOCC__" ,fromPythonOCC,METH_VARARGS,
     "__fromPythonOCC__(occ) -- Helper method to convert a pythonocc shape to an internal shape"},
    {NULL, NULL}        /* end of table marker */
};
