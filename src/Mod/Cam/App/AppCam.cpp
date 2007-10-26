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
#include <Base/Interpreter.h>


extern struct PyMethodDef Cam_methods[];

PyDoc_STRVAR(module_part_doc,
             "This module is the playground for the CAM stuff.");

extern "C"
{
    void AppCamExport initCam()
    {

        Base::Console().Log("Mod: Loading Cam module... done\n");
        Py_InitModule3("Cam", Cam_methods, module_part_doc);   /* mod name, table ptr */

        // load dependend module
        Base::Interpreter().loadModule("Part");
        Base::Interpreter().loadModule("Mesh");

        // NOTE: To finish the initialization of our own type objects we must
        // call PyType_Ready, otherwise we run into a segmentation fault, later on.
        // This function is responsible for adding inherited slots from a type's base class.

        return;
    }

} // extern "C"
