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
#include <Gui/Application.h>
#include <Gui/Language/Translator.h>
#include "Workbench.h"
#include "ViewProvider.h"
//#include "resources/qrc_Drawing.cpp"

// use a different name to CreateCommand()
void CreateDrawingCommands(void);

void loadDrawingResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Drawing);
    Gui::Translator::instance()->refresh();
}

/* registration table  */
extern struct PyMethodDef DrawingGui_Import_methods[];


/* Python entry */
extern "C" {
void DrawingGuiExport initDrawingGui()
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        return;
    }

    (void) Py_InitModule("DrawingGui", DrawingGui_Import_methods);   /* mod name, table ptr */
    Base::Console().Log("Loading GUI of Drawing module... done\n");

    // instanciating the commands
    CreateDrawingCommands();
    DrawingGui::Workbench::init();

    DrawingGui::ViewProviderDrawing::init();

    // add resources and reloads the translators
    loadDrawingResource();
}

} // extern "C" {
