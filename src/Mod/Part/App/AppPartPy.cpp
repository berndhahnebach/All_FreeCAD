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

#include "TopologyPy.h"
#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "PartAlgos.h"

#include <Geom_BSplineSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

using Base::Console;
using namespace Part;
using namespace std;


/* module functions */
static PyObject *                        
open(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
    
  PY_TRY {

    //Base::Console().Log("Open in Part with %s",Name);
    Base::FileInfo file(Name);

    // extract ending
    if(file.extension() == "")
      Py_Error(PyExc_Exception,"no file ending");

    if(file.hasExtension("stp") || file.hasExtension("step"))
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().newDocument("Unnamed");
      Part::ImportStep *pcFeature = (Part::ImportStep *)pcDoc->addObject("Part::ImportStep",file.fileNamePure().c_str());
      pcFeature->FileName.setValue(Name);
      pcDoc->recompute();

    }else if(file.hasExtension("igs") || file.hasExtension("iges"))
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().newDocument(file.fileNamePure().c_str());
      Part::ImportIges *pcFeature = (Part::ImportIges*) pcDoc->addObject("Part::ImportIges",file.fileNamePure().c_str());
      pcFeature->FileName.setValue(Name);
      pcDoc->recompute();

    }else if(file.hasExtension("brp") || file.hasExtension("brep"))
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().newDocument(file.fileNamePure().c_str());
      Part::ImportBrep *pcFeature = (Part::ImportBrep *)pcDoc->addObject("Part::ImportBrep",file.fileNamePure().c_str());
      pcFeature->FileName.setValue(Name);
      pcDoc->recompute();

    }
    else
    {
      Py_Error(PyExc_Exception,"unknown file ending");
    }


  } PY_CATCH;

	Py_Return;    
}


/* module functions */
static PyObject *                        
insert(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  const char* DocName;
  if (! PyArg_ParseTuple(args, "ss",&Name,&DocName))	 		 
    return NULL;                         
    
  PY_TRY {

    //Base::Console().Log("Insert in Part with %s",Name);
    Base::FileInfo file(Name);

    // extract ending
    if(file.extension() == "")
      Py_Error(PyExc_Exception,"no file ending");

    if(file.hasExtension("stp") || file.hasExtension("step"))
    {
      // add Import feature
      App::Document *pcDoc = App::GetApplication().getDocument(DocName);
      if (!pcDoc)
      {
        char szBuf[200];
        snprintf(szBuf, 200, "Import called to the non-existing document '%s'", DocName);
        Py_Error(PyExc_Exception,szBuf);
      }

      Part::ImportStep *pcFeature = (Part::ImportStep *)pcDoc->addObject("Part::ImportStep",file.fileNamePure().c_str());
      pcFeature->FileName.setValue(Name);
      pcDoc->recompute();

    }else if(file.hasExtension("igs") || file.hasExtension("iges"))
    {
      App::Document *pcDoc = App::GetApplication().getActiveDocument();
      if (!pcDoc)
        throw "Import called without a active document??";
      Part::ImportIges *pcFeature = (Part::ImportIges *)pcDoc->addObject("Part::ImportIges",file.fileNamePure().c_str());
      pcFeature->FileName.setValue(Name);
      pcDoc->recompute();

    }else if(file.hasExtension("brp") || file.hasExtension("brep"))
    {
      App::Document *pcDoc = App::GetApplication().getActiveDocument();
      if (!pcDoc)
        throw "Import called without a active document??";
      Part::ImportBrep *pcFeature = (Part::ImportBrep *) pcDoc->addObject("Part::ImportBrep",file.fileNamePure().c_str());
      pcFeature->FileName.setValue(Name);
      pcDoc->recompute();
    }
    else
    {
      Py_Error(PyExc_Exception,"unknown file ending");
    }


  } PY_CATCH;

	Py_Return;    
}

/* module functions */
static PyObject *                        
read(PyObject *self, PyObject *args)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
  PY_TRY {
    return new TopoShapePy(PartAlgos::Load(Name)); 
  } PY_CATCH;
}

/* BREP test function */
static PyObject *                        
createTestBSPLINE(PyObject *self, PyObject *args)
{
  //const char* Name;
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  PY_TRY {
    TColgp_Array2OfPnt Poles(1,4,1,4);
    // [0,0,0, 1,0,0, 2,0,0, 3,0,0,
    //  0,1,0, 1,1,2, 2,1,2, 3,1,0,
    //  0,2,0, 1,2,2, 2,2,2, 3,2,0,
    //  0,3,0, 1,3,0, 2,3,0, 3,3,0],
    Poles.SetValue(1,1,gp_Pnt(0,0,0));
    Poles.SetValue(2,1,gp_Pnt(1,0,0));
    Poles.SetValue(3,1,gp_Pnt(2,0,0));
    Poles.SetValue(4,1,gp_Pnt(3,0,0));
    Poles.SetValue(1,2,gp_Pnt(0,1,0));
    Poles.SetValue(2,2,gp_Pnt(1,1,2));
    Poles.SetValue(3,2,gp_Pnt(2,1,2));
    Poles.SetValue(4,2,gp_Pnt(3,1,0));
    Poles.SetValue(1,3,gp_Pnt(0,2,0));
    Poles.SetValue(2,3,gp_Pnt(1,2,2));
    Poles.SetValue(3,3,gp_Pnt(2,2,2));
    Poles.SetValue(4,3,gp_Pnt(3,2,0));
    Poles.SetValue(1,4,gp_Pnt(0,3,0));
    Poles.SetValue(2,4,gp_Pnt(1,3,0));
    Poles.SetValue(3,4,gp_Pnt(2,3,0));
    Poles.SetValue(4,4,gp_Pnt(3,3,0));

    TColStd_Array1OfReal UKnots(1,2);
    UKnots.SetValue(1,0);
    UKnots.SetValue(2,1);

    TColStd_Array1OfReal VKnots(1,2);
    VKnots.SetValue(1,0);
    VKnots.SetValue(2,1);

    TColStd_Array1OfInteger 	UMults(1,2);
    UMults.SetValue(1,4);
    UMults.SetValue(2,4);

    TColStd_Array1OfInteger 	VMults(1,2);
    VMults.SetValue(1,4);
    VMults.SetValue(2,4);


    // Creating the BSpline Surface
    Handle(Geom_BSplineSurface) Surface = new Geom_BSplineSurface(  	
                                      Poles,        // const TColgp_Array2OfPnt &  	 Poles,
	                                    UKnots,       // const TColStd_Array1OfReal &  	UKnots,
	                                    VKnots,       // const TColStd_Array1OfReal &  	VKnots,
	                                    UMults,       // const TColStd_Array1OfInteger &  	UMults,
	                                    VMults,       // const TColStd_Array1OfInteger &  	VMults,
	                                    3,            // const Standard_Integer  	UDegree,
	                                    3             // const Standard_Integer  	VDegree,
	                                                  // const Standard_Boolean  	UPeriodic = Standard_False,
	                                                  // const Standard_Boolean  	VPeriodic = Standard_False*/
	                          );  

    BRepBuilderAPI_MakeFace 	Face(Surface);

    return new TopoShapePy(Face.Face()); 
  } PY_CATCH;
}


/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"open"   , open,    1},       
    {"insert" , insert,  1},       
    {"read"   , read,  1},       
    {"createTestBSPLINE"   , createTestBSPLINE,  1},       
    {NULL     , NULL      }        /* end of table marker */
};







