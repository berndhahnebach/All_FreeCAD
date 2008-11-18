/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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

# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
#include <Python.h>

#include <Base/Console.h>
#include <Base/PyObjectBase.h>
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

    if (!PyArg_ParseTuple(args, "dd", &length, &width))
        return NULL;

    if (length < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "length of plane too small");
        return NULL;
    }
    if (width < Precision::Confusion()) {
        PyErr_SetString(PyExc_Exception, "width of plane too small");
        return NULL;
    }

    PY_TRY {
        gp_Pnt aPlanePnt(0,0,0);
        gp_Dir aPlaneDir(0,0,1);
        Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
        BRepBuilderAPI_MakeFace Face(aPlane, 0.0, length, 0.0, width);
        return new TopoShapeFacePy(new TopoShape((Face.Face()))); 
    } PY_CATCH;
}

static PyObject * createBox(PyObject *self, PyObject *args)
{
    double X, Y, Z , L, H, W ;

    //const char* Name;
    if (! PyArg_ParseTuple(args, "dddddd", &X, &Y, &Z , &L, &H, &W ))
        return NULL;                         

    try {
        // Build a box using the dimension and position attributes
        BRepPrimAPI_MakeBox mkBox(gp_Pnt( X, Y, Z ), L, H, W);
        TopoDS_Shape ResultShape = mkBox.Shape();
        return new TopoShapeSolidPy(new TopoShape(ResultShape)); 
    }
    catch (Standard_Failure) {
        PyErr_SetString(PyExc_StandardError, "cannot create flat box");
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

static PyObject * makeCircle(PyObject *self, PyObject *args)
{
    double radius, angle1=0.0, angle2=2.0*Standard_PI;
    if (!PyArg_ParseTuple(args, "d|dd", &radius, &angle1, &angle2))
        return NULL;

    try {
        gp_Pnt loc = gp_Pnt(0,0,0);
        gp_Dir dir = gp_Dir(0,0,1);
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
    if (!PyArg_ParseTuple(args, "d|ddd", &radius, &angle1, &angle2, &angle3))
        return NULL;

    try {
        BRepPrimAPI_MakeSphere mkSphere(radius, angle1, angle2, angle3);
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
    PyObject *pPnt, *pDir;
    if (!PyArg_ParseTuple(args, "O!O!dd|d", &(Base::VectorPy::Type), &pPnt,
                                            &(Base::VectorPy::Type), &pDir,
                                            &radius, &height, &angle))
        return NULL;

    try {
        Base::Vector3d pnt = static_cast<Base::VectorPy*>(pPnt)->value();
        Base::Vector3d vec = static_cast<Base::VectorPy*>(pDir)->value();
        gp_Pnt p(pnt.x, pnt.y, pnt.z);
        gp_Dir d(vec.x, vec.y, vec.z);
        BRepPrimAPI_MakeCylinder mkCyl(gp_Ax2(p,d),radius, height, angle);
        TopoDS_Shape shape = mkCyl.Shape();
        return new TopoShapeSolidPy(new TopoShape(shape));
    }
    catch (Standard_DomainError) {
        PyErr_SetString(PyExc_Exception, "creation of cylinder failed");
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
     "show(shape) -- Add the shape to the active document or create one if it does not exist."},
    {"makeCompound"  ,makeCompound ,METH_VARARGS,
     "makeCompound(list) -- Create a compound out of a list of geometries."},
    {"makePlane"  ,makePlane ,METH_VARARGS,
     "makePlane(lenght,width) -- Make a plane"},
    {"createBox"  ,createBox ,METH_VARARGS}, // obsolete
    {"makeBox"    ,createBox ,METH_VARARGS,
     "makeBox(x,y,z,l,w,h) -- Make a box located in (x,y,z) with the dimensions (l,w,h)"},
    {"makeLine"   ,makeLine  ,METH_VARARGS,
     "makeLine((x1,y1,z1),(x2,y2,z2)) -- Make a line of two points"},
    {"makePolygon"   ,makePolygon  ,METH_VARARGS,
     "makePolygon(list) -- Make a polygon of a list of points"},
    {"makeCircle" ,makeCircle,METH_VARARGS,
     "makeCircle(radius,[angle1,angle2]) -- Make a circle with a given radius\n"
     "By default angle1=0 and angle2=2*PI"},
    {"makeSphere" ,makeSphere,METH_VARARGS,
     "makeSphere(radius,[angle1,angle2,angle3]) -- Make a sphere with a given radius\n"
     "By default angle1=0, angle2=0.5*PI and angle3=2*PI"},
    {"makeCylinder" ,makeCylinder,METH_VARARGS,
     "makeCylinder(radius,height,[angle]) -- Make a cylinder with a given radius and height\n"
     "By default angle=2*PI"},
    {"__sortEdges__" ,sortEdges,METH_VARARGS,
     "__sortEdges__(list of edges) -- Helper method to sort an unsorted list of edges so that afterwards\n"
     "two adjacent edges share a common vertex"},
    {NULL, NULL}        /* end of table marker */
};
