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
#endif


#include "MeshAlgos.h"
#include "Mesh.h"
#include "Core/MeshIO.h"
#include "Core/Stream.h"

#include <Base/Exception.h>

using namespace Mesh;


Mesh::MeshWithProperty* MeshAlgos::Load(const char *FileName)
{
  MeshWithProperty *Mesh = new MeshWithProperty();
  // ask for read permisson
	if ( access(FileName, 4) != 0 )
    throw Base::Exception("MeshAlgos::Load() not able to open File!\n");
 
  MeshSTL aReader(* (Mesh->getKernel()) );

  // read STL file
  FileStream str( FileName, std::ios::in);
  if ( !aReader.Load( str ) )
    throw Base::Exception("STL read failed (load file)");

  return Mesh; 
}

