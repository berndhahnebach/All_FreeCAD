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

#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/WidgetFactory.h>

#include "Workbench.h"

// use a different name to CreateCommand()
void CreateCamCommands(void);

/* registration table  */
static struct PyMethodDef CamGui_methods[] = {
    {NULL, NULL}                   /* end of table marker */
};

extern "C" {
void AppCamGuiExport initCamGui() {
  if ( !Gui::Application::Instance )
  {
    PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
    return;
  }

  Base::Console().Log("Mod: Loading GUI of Cam module... done\n");
	(void) Py_InitModule("CamGui", CamGui_methods);   /* mod name, table ptr */

  // load needed modules
  Base::Interpreter().loadModule("Cam");

  CamGui::Workbench           ::init();

  // instanciating the commands
	CreateCamCommands();

	return;
}
} // extern "C"
