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

//#include "PartDocType.h"
#include "TopologyPy.h"
#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "PartAlgos.h"

using Base::Console;
using namespace Part;
using namespace std;


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
        sprintf(szBuf, "Import called to the non-existing document '%s'", DocName);
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



/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"info"   , info,    1},       
    {"open"   , open,    1},       
    {"insert" , insert,  1},       
    {"read"   , read,  1},       
    {NULL     , NULL      }        /* end of table marker */
};







