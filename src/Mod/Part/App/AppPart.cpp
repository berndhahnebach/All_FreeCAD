/** \file AppPart.cpp
 *  \brief
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


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

#include <stdio.h>

# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
#include <Python.h>

#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>

//#include "PartDocType.h"
#include "FeaturePartBox.h"
#include "FeaturePartCut.h"
#include "FeaturePartImportStep.h"
#include "FeaturePartImportIges.h"
#include "FeaturePartImportBrep.h"
#include "FeaturePartCurveNet.h"

using Base::Console;
using namespace Part;

extern struct PyMethodDef Part_methods[];


// python entry
#ifdef FC_OS_WIN32
#	define ModuleExport __declspec(dllexport)
#else
#	define ModuleExport
#endif

extern "C" {

void ModuleExport initPart() {

  Base::Console().Log("Mod : Load AppPart\n");
	(void) Py_InitModule("Part", Part_methods);   /* mod name, table ptr */

  Part::Feature   ::init();
  Part::Box       ::init();
  Part::Cut       ::init();
  Part::ImportStep::init();
  Part::ImportIges::init();
  Part::ImportBrep::init();
  Part::CurveNet  ::init();

 //new App::FeatureProducer<Part::PartBoxFeature>;
  /*
  App::FeatureFactory().AddProducer("Part"          ,new App::FeatureProducer<Part::PartFeature>);
  App::FeatureFactory().AddProducer("PartBox"       ,new App::FeatureProducer<Part::PartBoxFeature>);
	App::FeatureFactory().AddProducer("PartCut"       ,new App::FeatureProducer<Part::PartCutFeature>);
	App::FeatureFactory().AddProducer("PartImportStep",new App::FeatureProducer<Part::FeaturePartImportStep>);
	App::FeatureFactory().AddProducer("PartImportIges",new App::FeatureProducer<Part::FeaturePartImportIges>);
	App::FeatureFactory().AddProducer("PartImportBrep",new App::FeatureProducer<Part::FeaturePartImportBrep>);
	App::FeatureFactory().AddProducer("PartCurveNet"  ,new App::FeatureProducer<Part::FeaturePartCurveNet>);
*/
	return;
}

} // extern "C" 
