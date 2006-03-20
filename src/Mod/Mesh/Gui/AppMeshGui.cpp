/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Base/Interpreter.h>
#include <Base/Console.h>

#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>

#include "images.h"
#include "ViewProvider.h"
#include "ViewProviderCurvature.h"
#include "ViewProviderTransform.h"
#include "ViewProviderTransformDemolding.h"
#include "Workbench.h"


// use a different name to CreateCommand()
void CreateMeshCommands(void);

/* registration table  */
static struct PyMethodDef MeshGui_methods[] = {
    {NULL, NULL}                   /* end of table marker */
};

/* Python entry */
extern "C" {
void GuiMeshExport initMeshGui() {
  if ( !Gui::Application::Instance )
  {
    PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
    return;
  }

  (void) Py_InitModule("MeshGui", MeshGui_methods);   /* mod name, table ptr */

  // load needed modules
  Base::Console().Log("Mod : Load AppMeshGui\n");
  Base::Interpreter().loadModule("Mesh");

  // Register icons
  Gui::BitmapFactory().addXPM("curv_info", curv_info);
  Gui::BitmapFactory().addXPM("import_mesh", import_mesh);
  Gui::BitmapFactory().addXPM("export_mesh", export_mesh);

  // instanciating the commands
  CreateMeshCommands();

  MeshGui::KernelEditorItem                  ::init();

  MeshGui::ViewProviderMesh                  ::init();
  MeshGui::ViewProviderMeshCurvature         ::init();
  MeshGui::ViewProviderMeshTransform         ::init();
  MeshGui::ViewProviderMeshTransformDemolding::init();

  MeshGui::Workbench                         ::init();

  return;
}
} // extern "C" {
