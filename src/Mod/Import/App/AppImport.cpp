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
 
#include "../../../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif

#include "../../../App/Application.h"

#include <stdio.h>
#include <python.h>
#include "../../../Base/Console.h"


/* module functions */
static PyObject *                                 /* returns object */
Info(PyObject *self, PyObject *args)              /* self unused in modules */
{                                                 /* args from python call */
	std::string strResult;

    if (! PyArg_ParseTuple(args, ""))			  /* convert Python -> C */
        return NULL;                              /* null=raise exception */
    
	strResult += "The Import module\n";

    PyBuf bufTemp(strResult.c_str());
	return Py_BuildValue("s", bufTemp.str);   /* convert C -> Python */
    
}



/* registration table  */
static struct PyMethodDef hello_methods[] = {
    {"Info", Info, 1},				/* method name, C func ptr, always-tuple */

    {NULL, NULL}                   /* end of table marker */
};






// python intry
extern "C" {
void __declspec(dllexport) initImport() {

	(void) Py_InitModule("Import", hello_methods);   /* mod name, table ptr */

	GetApplication();

	GetConsole().Log("Import loaded\n");

	return;
}


} // extern "C" {
