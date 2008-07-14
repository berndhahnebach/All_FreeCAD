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
# include <BRepPrimAPI_MakeCylinder.hxx>
# include <BRepPrimAPI_MakeSphere.hxx>
# include <BRep_Builder.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <BRepBuilderAPI_MakeWire.hxx>
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
#include <Base/VectorPy.h>
#include <App/Application.h>
#include <App/Document.h>

#include "TopoShape.h"
#include "TopoShapePy.h"
#include "TopoShapeEdgePy.h"
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

    try {
        Py::Tuple p1(obj1), p2(obj2);
        // Convert into OCC representation
        gp_Pnt pnt1 = gp_Pnt((double)Py::Float(p1[0]),
                             (double)Py::Float(p1[1]),
                             (double)Py::Float(p1[2]));
        gp_Pnt pnt2 = gp_Pnt((double)Py::Float(p2[0]),
                             (double)Py::Float(p2[1]),
                             (double)Py::Float(p2[2]));
        // Create directly the underlying line geometry
        BRepBuilderAPI_MakeEdge makeEdge(pnt1,pnt2);

        const char *error=0;
        switch ( makeEdge.Error() )
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
    catch (const Py::Exception&) {
        return NULL;
    }
}

static PyObject * makeCircle(PyObject *self, PyObject *args)
{
    double radius, angle1=0.0, angle2=2.0*F_PI;
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
    double radius, angle1=-F_PI/2.0, angle2=F_PI/2.0, angle3=2.0*F_PI;
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
    double radius, height, angle=2.0*F_PI;
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

/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"open"       ,open      ,METH_VARARGS,
     "open(string) -- Create a new document and load the file into the document."},
    {"insert"     ,insert    ,METH_VARARGS,
     "insert(string,string) -- Insert the file into the given document."},
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
    {"makeCircle" ,makeCircle,METH_VARARGS,
     "makeCircle(radius,[angle1,angle2]) -- Make a circle with a given radius\n"
     "By default angle1=0 and angle2=2*PI"},
    {"makeSphere" ,makeSphere,METH_VARARGS,
     "makeSphere(radius,[angle1,angle2,angle3]) -- Make a sphere with a given radius\n"
     "By default angle1=0, angle2=0.5*PI and angle3=2*PI"},
    {"makeCylinder" ,makeCylinder,METH_VARARGS,
     "makeCylinder(radius,height,[angle]) -- Make a cylinder with a given radius and height\n"
     "By default angle=2*PI"},
    {NULL, NULL}        /* end of table marker */
};
