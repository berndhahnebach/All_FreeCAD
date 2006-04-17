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

#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "FeaturePartCurveNet.h"
#include "FeaturePartLine.h"
#include "FeaturePartPolygon.h"

extern struct PyMethodDef Part_methods[];

extern "C" {
void AppPartExport initPart() {

  Base::Console().Log("Mod : Load AppPart\n");
	(void) Py_InitModule("Part", Part_methods);   /* mod name, table ptr */

  Part::Feature   ::init();
  Part::Box       ::init();
  Part::Cut       ::init();
  Part::ImportStep::init();
  Part::ImportIges::init();
  Part::ImportBrep::init();
  Part::CurveNet  ::init();
  Part::Line      ::init();
  Part::Polygon   ::init();

	return;
}

} // extern "C"
