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
# include <BRep_Builder.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <gp_Circ.hxx>
# include <gp_Pnt.hxx>
# include <gp_Lin.hxx>
# include <Geom_Circle.hxx>
# include <Geom_Line.hxx>
# include <Geom_Plane.hxx>
# include <Handle_Geom_Circle.hxx>
# include <Handle_Geom_Plane.hxx>
# include <TopoDS_Edge.hxx>
# include <TopoDS_Face.hxx>
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
#include <App/Application.h>
#include <App/Document.h>

#include "TopoShape.h"
#include "TopoShapePy.h"
#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "ImportStep.h"

#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

using Base::Console;
using namespace Part;
using namespace std;


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

static PyObject * makePlane(PyObject *self, PyObject *args)
{
    double length, width;

    if (!PyArg_ParseTuple(args, "dd", &length, &width))
        return NULL;

    if (length < gp::Resolution()) {
        PyErr_SetString(PyExc_Exception, "length of plane too small");
        return NULL;
    }
    if (width < gp::Resolution()) {
        PyErr_SetString(PyExc_Exception, "width of plane too small");
        return NULL;
    }

    PY_TRY {
        gp_Pnt aPlanePnt(0,0,0);
        gp_Dir aPlaneDir(0,0,1);
        Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
        BRepBuilderAPI_MakeFace Face(aPlane, 0.0, length, 0.0, width);
        return new TopoShapePy(new TopoShape((Face.Face()))); 
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
        return new TopoShapePy(new TopoShape(ResultShape)); 
    }
    catch (const Standard_DomainError&) {
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
        return new TopoShapePy(new TopoShape(edge)); 
    }
    catch (const Py::Exception&) {
        return NULL;
    }
}

static PyObject * makeCircle(PyObject *self, PyObject *args)
{
    PyObject *obj1, *obj2;
    double radius, angle0=0.0, angle1=2.0*F_PI;
    if (!PyArg_ParseTuple(args, "OOd|dd", &obj1, &obj2, &radius, &angle0, &angle1))
        return NULL;

    try {
        Py::Tuple p1(obj1), p2(obj2);
        // Convert into OCC representation
        gp_Pnt loc = gp_Pnt((double)Py::Float(p1[0]),
                            (double)Py::Float(p1[1]),
                            (double)Py::Float(p1[2]));
        gp_Dir dir = gp_Dir((double)Py::Float(p2[0]),
                            (double)Py::Float(p2[1]),
                            (double)Py::Float(p2[2]));

        gp_Ax1 axis(loc, dir);
        gp_Circ circle;
        circle.SetAxis(axis);
        circle.SetRadius(radius);

        Handle_Geom_Circle hCircle = new Geom_Circle (circle);
        BRepBuilderAPI_MakeEdge aMakeEdge(hCircle, angle0, angle1);
        TopoDS_Edge edge = aMakeEdge.Edge();
        return new TopoShapePy(new TopoShape(edge)); 
    }
    catch (const Py::Exception&) {
        return NULL;
    }
}


/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"open"   , open,    1},
    {"insert" , insert,  1},
    {"read"   , read,  1},
    {"show"   , show,  1},
    {"makePlane" , makePlane, 1},
    {"createBox" , createBox, 1}, // obsolete
    {"makeBox" , createBox, 1},
    {"makeLine" , makeLine, 1},
    {"makeCircle" , makeCircle, 1},
    {NULL     , NULL      }        /* end of table marker */
};
