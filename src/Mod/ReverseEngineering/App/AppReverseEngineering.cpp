/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   J�rgen Riegel 2007                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>
 

extern struct PyMethodDef ReverseEngineering_methods[];

PyDoc_STRVAR(module_ReverseEngineering_doc,
"This module is the ReverseEngineering module.");


/* Python entry */
extern "C" {
void AppReverseEngineeringExport initReverseEngineering()
{
    // load dependend module
    try {
        Base::Interpreter().loadModule("Part");
        //Base::Interpreter().loadModule("Mesh");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }
    Py_InitModule3("ReverseEngineering", ReverseEngineering_methods, module_ReverseEngineering_doc);   /* mod name, table ptr */
    Base::Console().Log("Loading ReverseEngineering module... done\n");


    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
 
    //ReverseEngineering::FeatureViewPart        ::init();
}

} // extern "C"
