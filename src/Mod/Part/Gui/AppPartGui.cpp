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

#include <Gui/Language/Translator.h>
#include "qrc_Part.cpp"

#include "Icons/PartFeature.xpm"
#include "Icons/PartFeatureImport.xpm"

// use a different name to CreateCommand()
void CreatePartCommands(void);

void loadPartResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Part);
    Gui::Translator::instance()->refresh();
}

/* registration table  */
static struct PyMethodDef PartGui_methods[] = {
    {NULL, NULL}                   /* end of table marker */
};

extern "C" {
void AppPartGuiExport initPartGui()
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        return;
    }

    // load needed modules
    try {
        Base::Interpreter().loadModule("Part");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }

    (void) Py_InitModule("PartGui", PartGui_methods);   /* mod name, table ptr */
    Base::Console().Log("Loading GUI of Part module... done\n");

    PartGui::ViewProviderPart    ::init();
    PartGui::ViewProviderBox     ::init();
    PartGui::ViewProviderImport  ::init();
    PartGui::ViewProviderCurveNet::init();

    PartGui::Workbench           ::init();

    // instanciating the commands
    CreatePartCommands();

    // register preferences pages
    new Gui::PrefPageProducer<PartGui::DlgSettings3DViewPartImp> ("Part design");

    // add resources and reloads the translators
    loadPartResource();

    // register bitmaps
    Gui::BitmapFactoryInst& rclBmpFactory = Gui::BitmapFactory();
    rclBmpFactory.addXPM("PartFeature",(const char**) PartFeature_xpm);
    rclBmpFactory.addXPM("PartFeatureImport",(const char**) PartFeatureImport_xpm);
}
} // extern "C"
