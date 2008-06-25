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

#include "TopoShape.h"
#include "FeaturePartBox.h"
#include "FeaturePartBoolean.h"
#include "FeaturePartCommon.h"
#include "FeaturePartCut.h"
#include "FeaturePartFuse.h"
#include "FeaturePartSection.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "FeaturePartCurveNet.h"
#include "FeaturePartCircle.h"
#include "FeaturePartPolygon.h"
#include "PrimitiveFeature.h"
#include "TopoShapePy.h"
#include "TopoShapeVertexPy.h"
#include "TopoShapeFacePy.h"
#include "TopoShapeWirePy.h"
#include "TopoShapeEdgePy.h"
#include "TopoShapeSolidPy.h"
#include "TopoShapeCompoundPy.h"
#include "TopoShapeCompSolidPy.h"
#include "TopoShapeShellPy.h"
#include "LinePy.h"
#include "CirclePy.h"
#include "ArcPy.h"

extern struct PyMethodDef Part_methods[];

PyDoc_STRVAR(module_part_doc,
"This is a module working with shapes.");

inline void AddType(PyTypeObject* Type,PyObject* Module, const char * Name){
    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    if(PyType_Ready(Type) < 0) return;
    union PyType_Object pyPartType = {Type};
    PyModule_AddObject(Module, Name, pyPartType.o);
}

extern "C" {
void AppPartExport initPart()
{
    PyObject* partModule = Py_InitModule3("Part", Part_methods, module_part_doc);   /* mod name, table ptr */
    Base::Console().Log("Loading Part module... done\n");

    // Add Types to module
    AddType(&Part::TopoShapePy         ::Type,partModule,"Shape");
    AddType(&Part::TopoShapeVertexPy   ::Type,partModule,"Vertex");
    AddType(&Part::TopoShapeWirePy     ::Type,partModule,"Wire");
    AddType(&Part::TopoShapeEdgePy     ::Type,partModule,"Edge");
    AddType(&Part::TopoShapeSolidPy    ::Type,partModule,"Solid");
    AddType(&Part::TopoShapeFacePy     ::Type,partModule,"Face");
    AddType(&Part::TopoShapeCompoundPy ::Type,partModule,"Compound");
    AddType(&Part::TopoShapeCompSolidPy::Type,partModule,"CompSolid");
    AddType(&Part::TopoShapeShellPy    ::Type,partModule,"Shell");

    AddType(&Part::LinePy   ::Type,partModule,"Line");
    AddType(&Part::CirclePy ::Type,partModule,"Circle");
    AddType(&Part::ArcPy    ::Type,partModule,"Arc");

#if 1
    Py::Module mod(partModule);
    mod.setAttr(std::string("circle"),mod.getAttr("Circle"));
#endif

    Part::TopoShape           ::init();
    Part::PropertyPartShape   ::init();
#if 1
    Part::PropertyCircle      ::init();
#endif

    Part::Feature             ::init();
    Part::Box                 ::init();
    Part::Boolean             ::init();
    Part::Common              ::init();
    Part::Cut                 ::init();
    Part::Fuse                ::init();
    Part::Section             ::init();
    Part::ImportStep          ::init();
    Part::ImportIges          ::init();
    Part::ImportBrep          ::init();
    Part::CurveNet            ::init();
    Part::Polygon             ::init();
    Part::Circle              ::init();
    Part::Plane               ::init();
    Part::Sphere              ::init();
    Part::Cylinder            ::init();
    Part::Cone                ::init();
    Part::Torus               ::init();
}

} // extern "C"
