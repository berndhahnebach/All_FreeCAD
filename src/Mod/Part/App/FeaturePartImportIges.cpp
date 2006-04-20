/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <TopTools_HSequenceOfShape.hxx>
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <TopoDS_Shape.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeaturePartImportIges.h"


using namespace Part;

PROPERTY_SOURCE(Part::ImportIges, Part::Feature)


ImportIges::ImportIges(void)
{
	ADD_PROPERTY(FileName,(""));

}

Standard_Integer ImportIges::execute(void)
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
  Base::SequencerLauncher seq("Load IGES", 1);
  Base::Sequencer().next();

    // read iges-file
  if (aReader.ReadFile((const Standard_CString)FileName.getValue()) != IFSelect_RetDone)
    throw Base::Exception("IGES read failed (load file)");
  
  // check iges-file (memory)
  //if (!aReader.Check(Standard_True))
  //  Base::Console().Warning( "IGES model contains errors! try loading anyway....\n" );
  
  // make brep
  aReader.TransferRoots();
  // one shape, who contain's all subshapes
  aShape = aReader.OneShape();

	setShape(aShape);

  return 0;
}


