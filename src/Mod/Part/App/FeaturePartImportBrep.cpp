/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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
# include <fcntl.h>
# include <BRep_Builder.hxx>
# include <BRepTools.hxx>
# include <TopTools_HSequenceOfShape.hxx>
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <TopoDS_Shape.hxx>
# include <TFunction_Logbook.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeaturePartImportBrep.h"


using namespace Part;

PROPERTY_SOURCE(Part::ImportBrep, Part::Feature)


ImportBrep::ImportBrep(void)
{
	ADD_PROPERTY(FileName,(""));
}

int ImportBrep::execute(void)
{

  IGESControl_Reader aReader;
  TopoDS_Shape aShape;

#if defined (__GNUC__)
  int i=open(FileName.getValue(),O_RDONLY);
	if( i != -1)
	{
	  close(i);
#else
  int i=_open(FileName.getValue(),O_RDONLY);
	if( i != -1)
	{
	  _close(i);
#endif
	}else{
    Base::Console().Log("FeaturePartImportIges::Execute() not able to open %s!\n",FileName.getValue());
	  return 1;
	}

  // just do show the wait cursor when the Gui is up
  Base::SequencerLauncher seq("Load BREP", 1);
  Base::Sequencer().next();

  BRep_Builder aBuilder;

  // read brep-file
  BRepTools::Read(aShape,(const Standard_CString)FileName.getValue(),aBuilder);

	setShape(aShape);

  return 0;
}

