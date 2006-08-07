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

  std::ifstream str( FileName.getValue(), std::ios::in | std::ios::binary );
  
  MeshCore::MeshKernel *pcKernel=0;
  if ( fi.hasExtension("bms") )
  {
    try {
      pcKernel = new MeshCore::MeshKernel();
      pcKernel->Read( str );
      Mesh.setValue(pcKernel);
    } catch( const Base::MemoryException&) {
      setError("Invalid mesh file");
      delete pcKernel;
      return 1;
    }
  }
  else 
  {
    pcKernel = new MeshCore::MeshKernel();
    MeshInput aReader( *pcKernel );
    
    try {
      // read file
      bool ok = false;
      if ( fi.hasExtension("stl") || fi.hasExtension("ast") ) {
        ok = aReader.LoadSTL( str );
      } else if ( fi.hasExtension("iv") ) {
        ok = aReader.LoadInventor( str );
      } else if ( fi.hasExtension("nas") || fi.hasExtension("bdf") ) {
        ok = aReader.LoadNastran( str );
      } else {
        setError("File format '%s' not supported", fi.extension().c_str());
        delete pcKernel;
        return 1;
      }

      // Check whether load process succeeded
      if ( !ok ) {
        setError("Import of file '%s' failed",FileName.getValue());
        delete pcKernel;
        return 1;
      }

      // Mesh is okay
      Mesh.setValue(pcKernel);

    }catch ( Base::AbortException& e ){
      char szBuf[200];
      sprintf(szBuf, "Import of file '%s' aborted.", FileName.getValue());
      e.SetMessage( szBuf );
      delete pcKernel;
      throw e;
    }
  }

  return 0;
}

