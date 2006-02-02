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
#include <Base/FileInfo.h>
#include <Base/Sequencer.h>
#include "FeatureMeshExport.h"

#include "Core/MeshIO.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::FeatureMeshExport, Mesh::MeshFeature)

FeatureMeshExport::FeatureMeshExport(void)
{
  ADD_PROPERTY(Source  ,(0));
  ADD_PROPERTY(FileName,(""));
  ADD_PROPERTY(Format  ,(""));

}

int FeatureMeshExport::execute(void)
{
  MeshFeature *pcFeat  = dynamic_cast<MeshFeature*>(Source.getValue());
  if(!pcFeat || pcFeat->getStatus() != Valid)
  {
    setError("not valid link \"Source\"");
    return 1;
  }

  // ask for write permission
  Base::FileInfo fi(FileName.getValue());
  Base::FileInfo di(fi.dirPath().c_str());
	if ( fi.exists() && fi.isWritable() == false || di.exists() == false || di.isWritable() == false )
  {
    setError("FeatureMeshExport::Execute() not able to open %s for write!\n",FileName.getValue());
    return 1;
  }

  if ( fi.extension() == "bms" )
  {
    std::ofstream cOut( FileName.getValue(), std::ios::out | std::ios::binary );
    pcFeat->getMesh().getKernel()->Write( cOut );
  }
  else
  {
    MeshSTL aWriter(*(pcFeat->getMesh().getKernel()) );

    // write file
    bool ok = false;
    FileStream str( FileName.getValue(), std::ios::out);

    if ( std::string(Format.getValue()) == "ASCII STL" )
      ok = aWriter.SaveAscii( str );
    else // "Binary STL"
      ok = aWriter.SaveBinary( str );

    if ( !ok )
    {
      setError("FeatureMeshExport::Execute() not able to export %s\n",FileName.getValue());
      return 1;
    }
  }

  return 0;
}

