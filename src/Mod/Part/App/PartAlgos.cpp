/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
# ifdef FC_OS_LINUX
#	  include <unistd.h>
# endif
# include <BRep_Builder.hxx>
# include <BRepTools.hxx>
# include <TopTools_HSequenceOfShape.hxx>
#endif


#include "PartAlgos.h"

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Console.h>

#include <STEPControl_Reader.hxx>
#include <IGESControl_Reader.hxx>


using namespace Part;


TopoDS_Shape PartAlgos::Load(const char *FileName)
{
  Base::FileInfo File(FileName);
  
  // checking on the file
  if(!File.isReadable())
    throw Base::Exception("File to load not existing or not readable");
    
  TopoDS_Shape aShape;

  if(File.extension() == "igs" ||File.extension() == "IGS" ||File.extension() == "iges" ||File.extension() == "IGES" )
  {
  
    IGESControl_Reader aReader;

      // read iges-file
    if (aReader.ReadFile((const Standard_CString)File.filePath().c_str()) != IFSelect_RetDone)
      throw Base::Exception("Error in reading IGES");
  
    // make brep
    aReader.TransferRoots();
    // one shape, who contain's all subshapes
    aShape = aReader.OneShape();

  }else if(File.extension() == "stp" ||File.extension() == "STP" ||File.extension() == "step" ||File.extension() == "STEP" )
  {
    STEPControl_Reader aReader;

    Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
    if (aReader.ReadFile((const Standard_CString)File.filePath().c_str()) != IFSelect_RetDone)
      throw Base::Exception("Error in reading STEP");
  

  
    // Root transfers
    Standard_Integer nbr = aReader.NbRootsForTransfer();
    //aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
    for ( Standard_Integer n = 1; n<= nbr; n++)
    {
      printf("STEP: Transfering Root %d\n",n);
      /*Standard_Boolean ok =*/ aReader.TransferRoot(n);
      // Collecting resulting entities
      Standard_Integer nbs = aReader.NbShapes();
      if (nbs == 0) {
        aHSequenceOfShape.Nullify();
        throw Base::Exception("nothing to read");
      } else {
        for (Standard_Integer i =1; i<=nbs; i++) 
        {
          printf("STEP:   Transfering Shape %d\n",n);
          aShape=aReader.Shape(i);
          aHSequenceOfShape->Append(aShape);
        }
      }
    }

  }else if(File.extension() == "brp" ||File.extension() == "BRP" ||File.extension() == "brep" ||File.extension() == "BREP" )
  {
    BRep_Builder aBuilder;

    // read brep-file
    BRepTools::Read(aShape,(const Standard_CString)File.filePath().c_str(),aBuilder);

  }else{
    throw Base::Exception("Unknown ending");
  }
  
  return aShape; 
}


