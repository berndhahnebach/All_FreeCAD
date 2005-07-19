/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <TFunction_Logbook.hxx>
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeatureMeshImport.h"

#include "Core/MeshIO.h"
#include "Core/Stream.h"


using namespace Mesh;

void FeatureMeshImport::InitLabel(const TDF_Label &rcLabel)
{
  Base::Console().Log("FeatureMeshImport::InitLabel()\n");
  AddProperty("String","FileName","");
}

Standard_Integer FeatureMeshImport::Execute(TFunction_Logbook& log)
{
  Base::Console().Log("FeatureMeshImport::Execute()\n");

  try{

    const char* FileName = GetStringProperty("FileName");

    // ask for read permisson
		if ( access(FileName, 4) != 0 )
    {
      Base::Console().Log("FeatureMeshImport::Execute() not able to open %s!\n",FileName);
      return 1;
    }

    MeshSTL aReader(*(_cMesh.getKernel()) );

    // read file
    FileStream str( FileName, std::ios::in);
    if ( !aReader.Load( str ) )
      throw Base::Exception("Import failed (load file)");
  }
  catch(Base::AbortException& e){
    return 0;
  }
  catch(...){
    Base::Console().Error("FeatureMeshImport::Execute() failed!");
    return 2;
  }

  return 0;
}

void FeatureMeshImport::Validate(TFunction_Logbook& log)
{
  Base::Console().Log("FeatureMeshImport::Validate()\n");
}
