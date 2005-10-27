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
#include <Gui/WorkbenchFactory.h>

#include <Gui/WidgetFactory.h>

#include "ViewProvider.h"
#include "ViewProviderCurveNet.h"
#include "DlgSettings3DViewPartImp.h"
#include "Workbench.h"

#include "Part_de.h"
#include <Gui/Language/LanguageFactory.h>

using namespace PartGui;

// use a different name to CreateCommand()
void CreatePartCommands(void);


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

  Base::Console().Log("Mod : Load AppPartGui\n");
	(void) Py_InitModule("PartGui", hello_methods);   /* mod name, table ptr */

  // load needed modules
  Base::Interpreter().loadModule("Part");


  Gui::Application::Instance->macroManager()->setModule("Part");

  // Register view provider
  Gui::ViewProviderInventorFeatureFactory().AddProducer("PartBox"       ,new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFeatureFactory().AddProducer("PartCut"       ,new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFeatureFactory().AddProducer("PartImportStep",new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFeatureFactory().AddProducer("PartImportIges",new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFeatureFactory().AddProducer("PartImportBrep",new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderInventorPart>);
  Gui::ViewProviderInventorFeatureFactory().AddProducer("PartCurveNet"  ,new Gui::ViewProviderInventorFeatureProducer<PartGui::ViewProviderCurveNet>);

  Gui::WorkbenchFactory().AddProducer("Part design", new Gui::WorkbenchProducer<PartGui::Workbench>);

  // instanciating the commands
	CreatePartCommands();

  // register preferences pages
  new Gui::PrefPageProducer<DlgSettings3DViewPartImp> ( "Part design" );
  new Gui::LanguageProducer("Deutsch", GetPart_de());


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
