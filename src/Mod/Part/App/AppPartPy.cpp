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
#include <Python.h>

#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>

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
static PyObject *                                 /* returns object */
Box(PyObject *self, PyObject *args)               /* self unused in modules */
{                                                 /* args from python call */
	double  Float1,Float2,Float3,Float4,Float5,Float6;
    if (!PyArg_ParseTuple(args, "(dddddd)",&Float1,&Float2,&Float3,&Float4,&Float5,&Float6))     // convert args: Python->C
        return NULL;                              /* null=raise exception */
    else {


		return Py_None;                           /* convert C -> Python */
    }
}
/* development function for test purpose */
static PyObject *                                 /* returns object */
Temp(PyObject *self, PyObject *args)               /* self unused in modules */
{                                                 /* args from python call */
    if (!PyArg_ParseTuple(args, ""))            // convert args: Python->C
        return NULL;                              /* null=raise exception */
    

/*		Console().Log("Part.Temp() is runing ....\n");

    App::Document *doc = App::GetApplication().Active();
		if(!doc) return Py_None;

		Console().Log("Doc init\n");
		PartDocType *pcType = new PartDocType();
		doc->InitType(pcType);

		Console().Log("Type Get\n");
		if(strcmp(doc->GetDocType()->GetTypeName(),"Part")==0)
		{
			pcType = dynamic_cast<PartDocType*>( doc->GetDocType() );
			Console().Log("Part doc detected\n");

		}
  */
		return Py_None;                           /* convert C -> Python */
  
}

/* module functions */
static PyObject *                        
open(PyObject *self, PyObject *args)     
{                                        
	std::string strResult;

  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         
    
  Base::Console().Log("Open in Part with %s",strResult);

	Py_Return;    
}

/* registration table  */
struct PyMethodDef Part_methods[] = {
    {"info"   , info,    1},       
    {"AddBox" , Box,     1},       
    {"Temp"   , Temp,    1},       
    {"open"   , open,    1},       
    {"save"   , Temp,    1},       
    {NULL     , NULL      }        /* end of table marker */
};







