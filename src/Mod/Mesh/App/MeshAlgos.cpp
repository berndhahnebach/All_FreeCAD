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
#include <Base/FileInfo.h>


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

void MeshAlgos::writeBin(MeshWithProperty* Mesh,const char *FileName)
{
   Base::FileInfo File(FileName);
  
  // checking on the file
  if(File.exists() && !File.isWritable())
        throw Base::Exception("File not writable");

  MeshSTL aReader(*(Mesh->getKernel()) );

  // read STL file
  FileStream str( File.filePath().c_str(), std::ios::out);

  if ( !aReader.SaveBinary( str ) )
    throw Base::Exception("STL write failed to write");
}

void MeshAlgos::writeAscii(MeshWithProperty* Mesh,const char *FileName)
{
   Base::FileInfo File(FileName);
  
  // checking on the file
  if(File.exists() && !File.isWritable())
        throw Base::Exception("File not writable");

  MeshSTL aReader(*(Mesh->getKernel()) );

  // read STL file
  FileStream str( File.filePath().c_str(), std::ios::out);

  if ( !aReader.SaveAscii( str ) )
    throw Base::Exception("STL write failed to write");
}


void MeshAlgos::CalcVertexNormales(MeshWithProperty* Mesh)
{
  MeshPropertyNormal *prop = dynamic_cast<MeshPropertyNormal*> (Mesh->Get("VertexNormales") );

  if(prop && prop->isValid())
    return;

  // remove invalid Normales
  if(prop) Mesh->Remove("VertexNormales");

  const MeshKernel &MeshK = *(Mesh->getKernel());

  // create a property with the right size
  prop = new MeshPropertyNormal(MeshK.CountPoints());

  // data structure to hold all faces belongs to one vertex
  std::vector<std::set<unsigned int> > faceMap(MeshK.CountPoints());
  unsigned long p1,p2,p3;

  // colecting all Facetes indexes blonging to a vertex index
  for (unsigned int pFIter = 0;pFIter < MeshK.CountFacets(); pFIter++)
  {
    Mesh->getKernel()->GetFacetPoints(pFIter,p1,p2,p3);
    
    Vector3D Norm = (MeshK.GetPoint(p2)-MeshK.GetPoint(p1) ) % (MeshK.GetPoint(p3)-MeshK.GetPoint(p1));

    prop->Normales[p1] += Norm;
    prop->Normales[p2] += Norm;
    prop->Normales[p3] += Norm;
    
/*    faceMap[p1].insert(pFIter);
    faceMap[p2].insert(pFIter);
    faceMap[p3].insert(pFIter);*/
  }

/*  // calculating the normale weighted by size and write it to the property
  for( unsigned int ItVertex = 0;ItVertex < Mesh->getKernel()->CountFacets(); ItVertex++)
  {
    prop->Normales[ItVertex] = Vector3D(0.0,0.0,0.0);
  }*/

  Mesh->Add(prop,"VertexNormales");  
}
