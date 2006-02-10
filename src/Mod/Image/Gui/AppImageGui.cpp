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

#include <App/Application.h>
#include <Base/Console.h>

#include <Gui/Application.h>

#include "Workbench.h"

// use a different name to CreateCommand()
void CreateImageCommands(void);


/* module functions */
static PyObject *                                 /* returns object */
message(PyObject *self, PyObject *args)           /* self unused in modules */
{                                                 /* args from python call */
    char *fromPython, result[64];
    if (! PyArg_Parse(args, "(s)", &fromPython))  /* convert Python -> C */
        return NULL;                              /* null=raise exception */
    else {
        strcpy(result, "Hello, ");                /* build up C string */
        strcat(result, fromPython);               /* add passed Python string */
        return Py_BuildValue("s", result);        /* convert C -> Python */
    }
}

/* registration table  */
static struct PyMethodDef hello_methods[] = {
    {"message", message, 1},       /* method name, C func ptr, always-tuple */
    {NULL, NULL}                   /* end of table marker */
};





/* Python entry */
extern "C" {
void ImageGuiExport initImageGui() {

	(void) Py_InitModule("ImageGui", hello_methods);   /* mod name, table ptr */

	Base::Console().Log("AppImageGui loaded\n");

	// instanciating the commands
	CreateImageCommands();
  ImageGui::Workbench::init();
  //Gui::WorkbenchFactory().AddProducer("Image", new Gui::WorkbenchProducer<ImageGui::Workbench>);

	return;
}
} // extern "C" {
