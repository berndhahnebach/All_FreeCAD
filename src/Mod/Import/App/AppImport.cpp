/** \file AppImport.cpp
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
#	include <stdio.h>
#	include <Python.h>
#endif

#include "../../../App/Application.h"
#include "../../../App/Topology.h"

#include "../../../Base/Console.h"


/* module functions */
static PyObject *                                 /* returns object */
Info(PyObject *self, PyObject *args)              /* self unused in modules */
{                                                 /* args from python call */
	std::string strResult;

    if (! PyArg_ParseTuple(args, ""))			  /* convert Python -> C */
        return NULL;                              /* null=raise exception */
    
	strResult += "The Import module\n";

	Base::PyBuf bufTemp(strResult.c_str());
	return Py_BuildValue("s", bufTemp.str);   /* convert C -> Python */
    
}

/* module functions */
static PyObject *                                 /* returns object */
ReadBREP(PyObject *self, PyObject *args)          /* self unused in modules */
{                                                 /* args from python call */
	char* str;

    if (! PyArg_ParseTuple(args, "s",&str))		  /* convert Python -> C */
        return NULL;                              /* null=raise exception */
  
	TopoDS_Shape ResultShape;
	BRep_Builder aBuilder;

	BRepTools::Read(ResultShape,(const Standard_CString)str,aBuilder);

	return new FCTopoShape(ResultShape);		  /* convert C -> Python */
    
}



/* registration table  */
static struct PyMethodDef hello_methods[] = {
    {"Info", Info, 1},				/* method name, C func ptr, always-tuple */
    {"ReadBREP", ReadBREP, 1},

    {NULL, NULL}                   /* end of table marker */
};






// python entry
#ifdef FC_OS_WIN32
#	define ModuleExport __declspec(dllexport)
#else
#	define ModuleExport
#endif
extern "C" {
void ModuleExport initImport() {

	(void) Py_InitModule("Import", hello_methods);   /* mod name, table ptr */

	GetApplication();

	Base::Console().Log("Import loaded\n");

	return;
}


} // extern "C" {
