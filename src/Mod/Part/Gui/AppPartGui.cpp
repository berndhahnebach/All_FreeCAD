/** \file AppPartGui.cpp
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

#include <Base/Console.h>
#include <Base/Interpreter.h>

#include <App/Application.h>

#include <Gui/Application.h>
#include <Gui/ViewProvider.h>
#include <Gui/Macro.h>

#include <Gui/WidgetFactory.h>

#include "ViewProvider.h"
#include "DlgSettings3DViewPartImp.h"

using namespace PartGui;

void CreateCommands(void);


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





// python entry
#ifdef FC_OS_WIN32
#	define ModuleExport __declspec(dllexport)
#else
#	define ModuleExport
#endif
extern "C" {
void ModuleExport initPartGui() {

	(void) Py_InitModule("PartGui", hello_methods);   /* mod name, table ptr */

  // load needed modules
  Base::Interpreter().loadModule("Part");

	Base::Console().Log("AppPartGui loaded\n");

  Gui::ApplicationWindow::Instance->macroManager()->setModule("Part");

  // Register view provider
  Gui::ViewProviderInventorFactory().AddProducer("PartBox"       ,new Gui::ViewProviderInventorProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFactory().AddProducer("PartCut"       ,new Gui::ViewProviderInventorProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFactory().AddProducer("PartImportStep",new Gui::ViewProviderInventorProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFactory().AddProducer("PartImportIges",new Gui::ViewProviderInventorProducer<PartGui::ViewProviderInventorPart>);

	// instanciating the commands
	CreateCommands();

  // register preferences pages
  new Gui::PrefPageProducer<DlgSettings3DViewPartImp> ( QObject::tr( "Part design" ) );


	return;
}
} // extern "C" {



/*
#include <boost/python/class_builder.hpp>
namespace python = boost::python;

std::string info() { return "hello, world"; }


BOOST_PYTHON_MODULE_INIT(getting_started1)
{
    // Create an object representing this extension module.
    python::module_builder this_module("getting_started1");

    // Add regular functions to the module.
    this_module.def(info, "Info");
    //this_module.def(square, "square");
}
*/
