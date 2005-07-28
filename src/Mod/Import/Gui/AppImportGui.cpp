/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <App/Application.h>
#include <Base/Console.h>
#include <Base/Interpreter.h>

#include <Gui/Application.h>
#include <Gui/Macro.h>
#include <Gui/Application.h>
#include <Gui/ViewProvider.h>
#include <Gui/Macro.h>

#include <Gui/WidgetFactory.h>
#include <Mod/Part/Gui/ViewProvider.h>

// use a different name to CreateCommand()
void CreateImportCommands(void);


/* module functions */
static PyObject *                                 /* returns object */
message(PyObject *self, PyObject *args)           /* self unused in modules */
{                                                 /* args from python call */
    char *fromPython, result[64];
    if (! PyArg_ParseTuple(args, "(s)", &fromPython))  /* convert Python -> C */
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





// python intry
// python entry
#ifdef FC_OS_WIN32
#	define ModuleExport __declspec(dllexport)
#else
#	define ModuleExport
#endif
extern "C" {
void ModuleExport initImportGui() {

	(void) Py_InitModule("ImportGui", hello_methods);   /* mod name, table ptr */

  Base::Interpreter().loadModule("Import");
  Base::Interpreter().loadModule("PartGui");

	Base::Console().Log("ImportGui loaded\n");

  Gui::ViewProviderInventorFeatureFactory().AddProducer("ImportStep",new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFeatureFactory().AddProducer("ImportIges",new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);


	// instanciating the commands
	CreateImportCommands();


	return;
}
} // extern "C" {



