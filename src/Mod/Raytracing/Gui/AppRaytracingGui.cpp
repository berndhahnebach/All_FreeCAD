/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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

#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/WidgetFactory.h>
#include <Gui/Language/LanguageFactory.h>


#include "DlgSettingsRayImp.h"
#include "Workbench.h"

#include "Raytracing_de.h"

// use a different name to CreateCommand()
void CreateRaytracingCommands(void);

using namespace RaytracingGui;


/* registration table  */
static struct PyMethodDef RaytracingGui_methods[] = {
    {NULL, NULL}                   /* end of table marker */
};

extern "C" {
void AppRaytracingGuiExport initRaytracingGui() {

  Base::Console().Log("Mod : Load AppRaytracingGui\n");
  (void) Py_InitModule("RaytracingGui", RaytracingGui_methods);   /* mod name, table ptr */

  // instanciating the commands
  CreateRaytracingCommands();
  RaytracingGui::Workbench::init();
  //Gui::WorkbenchFactory().AddProducer("Raytracing", new Gui::WorkbenchProducer<RaytracingGui::Workbench>);

  // register preferences pages
  new Gui::PrefPageProducer<DlgSettingsRayImp> ( "Raytracing" );
  new Gui::LanguageProducer("Deutsch", Raytracing_de_h_data, Raytracing_de_h_len);

  return;
}
} // extern "C" {
