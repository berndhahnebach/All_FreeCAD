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
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Sequencer.h>
#include "FeatureMeshImport.h"

#include "Core/MeshIO.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Import, Mesh::Feature)


Import::Import(void)
{
  ADD_PROPERTY(FileName,(""));
}

int Import::execute(void)
{
  // ask for read permission
  Base::FileInfo fi(FileName.getValue());
	if ( !fi.exists() || !fi.isFile() || !fi.isReadable() )
  {
    setError("No read permission for file '%s'",FileName.getValue());
    return 1;
  }

  if ( fi.hasExtension("bms") )
  {
    try {
      std::ifstream cIn( FileName.getValue(), std::ios::in | std::ios::binary );
      _cMesh.getKernel()->Read( cIn );
    } catch( const Base::MemoryException&) {
      setError("Invalid mesh file");
      return 1;
    }
  }
  else if ( fi.hasExtension("stl") || fi.hasExtension("ast") )
  {
    MeshSTL aReader(*(_cMesh.getKernel()) );

    // read file
    //FileStream str( FileName.getValue(), std::ios::in);
    std::ifstream str( FileName.getValue(), std::ios::in | std::ios::binary );

    // catches the abort exception to set a more detailed description
    try{
      if ( !aReader.Load( str ) )
      {
        setError("Import of mesh to file '%s' failed",FileName.getValue());
        return 1;
      }
    }catch ( Base::AbortException& e ){
      char szBuf[200];
      sprintf(szBuf, "Loading of STL file '%s' aborted.", FileName.getValue());
      e.SetMessage( szBuf );
      throw e;
    }
  }
  else
  {
    setError("File format '%s' not supported", fi.extension().c_str());
    return 1;
  }

  return 0;
}

