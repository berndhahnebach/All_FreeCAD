/** \file AppPart.cpp
 *  \brief 
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
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
#include <Base/PyExportImp.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/Topology.h>

//#include "PartDocType.h"
#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"

using Base::Console;
using namespace Part;


/* module functions */
static PyObject *                                 /* returns object */
info(PyObject *self, PyObject *args)           /* self unused in modules */
{                                                 /* args from python call */
    char *fromPython, result[64];
    if (! PyArg_ParseTuple(args, "s", &fromPython))  /* convert Python -> C */
        return NULL;                              /* null=raise exception */
    else {
        strcpy(result, "Hello, ");                /* build up C string */
        strcat(result, fromPython);               /* add passed Python string */
        return Py_BuildValue("s", result);        /* convert C -> Python */
    }
}



/* module functions */
static PyObject *                        
open(PyObject *self, PyObject *args)     
{                                        
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
    
  PY_TRY {

    Base::Console().Log("Open in Part with %s",Name);

    // extract ending
    std::string cEnding(Name);
    unsigned int pos = cEnding.find_last_of('.');
    if(pos == cEnding.size())
      Py_Error(PyExc_Exception,"no file ending");
    cEnding.erase(0,pos+1);

    if(cEnding == "stp" || cEnding == "step" || cEnding == "STP" || cEnding == "STEP")
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().New();
      App::Feature *pcFeature = pcDoc->AddFeature("PartImportStep","Step open");
      pcFeature->setPropertyString(Name,"FileName");
      pcFeature->TouchProperty("FileName");
      pcDoc->Recompute();

    }else if(cEnding == "igs" || cEnding == "iges" || cEnding == "IGS" || cEnding == "IGES")
    {
      // create new document and add Import feature
      App::Document *pcDoc = App::GetApplication().New();
      App::Feature *pcFeature = pcDoc->AddFeature("PartImportIges","Iges open");
      pcFeature->setPropertyString(Name,"FileName");
      pcFeature->TouchProperty("FileName");
      pcDoc->Recompute();

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
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
    
  PY_TRY {

    Base::Console().Log("Insert in Part with %s",Name);

    // extract ending
    std::string cEnding(Name);
    unsigned int pos = cEnding.find_last_of('.');
    if(pos == cEnding.size())
      Py_Error(PyExc_Exception,"no file ending");
    cEnding.erase(0,pos+1);

    if(cEnding == "stp" || cEnding == "step" || cEnding == "STP" || cEnding == "STEP")
    {
      App::Document *pcDoc = App::GetApplication().Active();
      if (!pcDoc)
        throw "Import called without a active document??";
      App::Feature *pcFeature = pcDoc->AddFeature("PartImportStep","Step open");
      pcFeature->setPropertyString(Name,"FileName");
      pcFeature->TouchProperty("FileName");
      pcDoc->Recompute();

    }else if(cEnding == "igs" || cEnding == "iges" || cEnding == "IGS" || cEnding == "IGES")
    {
      App::Document *pcDoc = App::GetApplication().Active();
      if (!pcDoc)
        throw "Import called without a active document??";
      App::Feature *pcFeature = pcDoc->AddFeature("PartImportIges","Iges open");
      pcFeature->setPropertyString(Name,"FileName");
      pcFeature->TouchProperty("FileName");
      pcDoc->Recompute();

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

  Base::FileInfo File(Name);
  
  // checking on the file
  if(!File.isReadable())
    Py_Error(PyExc_Exception,"File to load not existing or not readable");
    
  TopoDS_Shape aShape;

  PY_TRY {
    if(File.extension() == "igs" ||File.extension() == "IGS" ||File.extension() == "iges" ||File.extension() == "IGES" )
    {
      
      IGESControl_Reader aReader;

        // read iges-file
      if (aReader.ReadFile((const Standard_CString)File.filePath().c_str()) != IFSelect_RetDone)
        Py_Error(PyExc_Exception,"Error in reading IGES");
  
      // make brep
      aReader.TransferRoots();
      // one shape, who contain's all subshapes
      aShape = aReader.OneShape();

    }else if(File.extension() == "stp" ||File.extension() == "STP" ||File.extension() == "step" ||File.extension() == "STEP" )
    {
      STEPControl_Reader aReader;

      Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
      if (aReader.ReadFile((const Standard_CString)File.filePath().c_str()) != IFSelect_RetDone)
        Py_Error(PyExc_Exception,"Error in reading STEP");
      

    
      // Root transfers
      Standard_Integer nbr = aReader.NbRootsForTransfer();
      //aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
      for ( Standard_Integer n = 1; n<= nbr; n++)
      {
        printf("STEP: Transfering Root %d\n",n);
        /*Standard_Boolean ok =*/ aReader.TransferRoot(n);
        // Collecting resulting entities
        Standard_Integer nbs = aReader.NbShapes();
        if (nbs == 0) {
          aHSequenceOfShape.Nullify();
          Py_Return;
        } else {
          for (Standard_Integer i =1; i<=nbs; i++) 
          {
            printf("STEP:   Transfering Shape %d\n",n);
            aShape=aReader.Shape(i);
            aHSequenceOfShape->Append(aShape);
          }
        }
      }

    }else{
      Py_Error(PyExc_Exception,"Unknown ending");
    }
    
    return new App::TopoShapePy(aShape); 
  } PY_CATCH;
}



/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"info"   , info,    1},       
    {"open"   , open,    1},       
    {"insert" , insert,  1},       
    {"read"   , read,  1},       
    {NULL     , NULL      }        /* end of table marker */
};







