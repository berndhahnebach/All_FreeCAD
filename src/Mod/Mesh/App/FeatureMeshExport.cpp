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

PROPERTY_SOURCE(Mesh::Export, Mesh::Feature)

Export::Export(void)
{
  ADD_PROPERTY(Source  ,(0));
  ADD_PROPERTY(FileName,(""));
  ADD_PROPERTY(Format  ,(""));

}

int Export::execute(void)
{
  Feature *pcFeat  = dynamic_cast<Feature*>(Source.getValue());
  if(!pcFeat || pcFeat->getStatus() != Valid)
  {
    setError("Cannot export invalid mesh feature '%s'", pcFeat->name.getValue());
    return 1;
  }

  // ask for write permission
  Base::FileInfo fi(FileName.getValue());
  Base::FileInfo di(fi.dirPath().c_str());
	if ( fi.exists() && fi.isWritable() == false || di.exists() == false || di.isWritable() == false )
  {
    setError("No write permission for file '%s'",FileName.getValue());
    return 1;
  }

  if ( fi.hasExtension("bms") )
  {
    std::ofstream cOut( FileName.getValue(), std::ios::out | std::ios::binary );
    pcFeat->getMesh().getKernel()->Write( cOut );
  }
  else if ( fi.hasExtension("stl") || fi.hasExtension("ast") )
  {
    MeshSTL aWriter(*(pcFeat->getMesh().getKernel()) );

    // write file
    bool ok = false;
    //FileStream str( FileName.getValue(), std::ios::out);
    std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );

    if ( std::string(Format.getValue()) == "ASCII STL" )
      ok = aWriter.SaveAscii( str );
    else // "Binary STL"
      ok = aWriter.SaveBinary( str );

    if ( !ok )
    {
      setError("Export of mesh to file '%s' failed",FileName.getValue());
      return 1;
    }
  }
  else
  {
    setError("File format '%s' not supported", fi.extension().c_str());
    return 1;
  }

  return 0;
}

