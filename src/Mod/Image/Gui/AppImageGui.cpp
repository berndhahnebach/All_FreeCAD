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
# include <Python.h>
#endif

#include <Base/Console.h>

#include "Workbench.h"

// use a different name to CreateCommand()
void CreateImageCommands(void);

/* registration table  */
static struct PyMethodDef ImageGui_methods[] = {
    {NULL, NULL}                   /* end of table marker */
};


/* Python entry */
extern "C" {
void ImageGuiExport initImageGui() {

  (void) Py_InitModule("ImageGui", ImageGui_methods);   /* mod name, table ptr */
  Base::Console().Log("AppImageGui loaded\n");

  // instanciating the commands
  CreateImageCommands();
  ImageGui::Workbench::init();

  return;
}
} // extern "C" {
