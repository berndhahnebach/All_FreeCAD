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

#include "ViewProvider.h"
#include "ViewProviderBox.h"
#include "ViewProviderCurveNet.h"
#include "ViewProviderImport.h"
#include "DlgSettings3DViewPartImp.h"
#include "Workbench.h"

#include "Part_de.h"
#include <Gui/Language/LanguageFactory.h>

#include "Icons/PartFeature.xpm"
#include "Icons/PartFeatureImport.xpm"

// use a different name to CreateCommand()
void CreatePartCommands(void);

/* registration table  */
static struct PyMethodDef PartGui_methods[] = {
    {NULL, NULL}                   /* end of table marker */
};

extern "C" {
void AppPartGuiExport initPartGui() {

  Base::Console().Log("Mod : Load AppPartGui\n");
	(void) Py_InitModule("PartGui", PartGui_methods);   /* mod name, table ptr */

  // load needed modules
  Base::Interpreter().loadModule("Part");

  PartGui::ViewProviderPart    ::init();
  PartGui::ViewProviderBox     ::init();
  PartGui::ViewProviderImport  ::init();
  PartGui::ViewProviderCurveNet::init();

  PartGui::Workbench           ::init();

  // instanciating the commands
	CreatePartCommands();

  // register preferences pages
  new Gui::PrefPageProducer<PartGui::DlgSettings3DViewPartImp> ( "Part design" );
  new Gui::LanguageProducer("Deutsch", GetPart_de());

  // register bitmaps
  Gui::BitmapFactoryInst& rclBmpFactory = Gui::BitmapFactory();
  rclBmpFactory.addXPM("PartFeature",(const char**) PartFeature_xpm);
  rclBmpFactory.addXPM("PartFeatureImport",(const char**) PartFeatureImport_xpm);

	return;
}
} // extern "C"
