/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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

#include "Info.h"
#include "Iterator.h"

using namespace Mesh;


MeshInfo::MeshInfo (const MeshKernel &rclM)
: _rclMesh(rclM)
{
}

std::ostream& MeshInfo::GeneralInformation (std::ostream &rclStream) const
{
  unsigned long ulCtPt, ulCtEd, ulCtFc;

  ulCtPt = _rclMesh.CountPoints();
//  ulCtEd = _rclMesh.CountEdges();
  ulCtFc = _rclMesh.CountFacets();

  rclStream.precision(3);
  rclStream << "Mesh: ["
            << ulCtFc << " Faces, "
            << ulCtPt << " Points"
            << "]" << std::endl;

  return rclStream;
}

std::ostream& MeshInfo::DetailedInformation (std::ostream& rclStream) const
{
  unsigned long i, j;

  // print points
  rclStream << _rclMesh.CountPoints() << " Points:" << std::endl;
  MeshPointIterator pPIter(_rclMesh), pPEnd(_rclMesh);
  pPIter.Begin();
  pPEnd.End();
  i = 0;

  rclStream.precision(3);
  while (pPIter < pPEnd)
  {
    rclStream << "P " << (i++) << ": " << (*pPIter).x << ", " << (*pPIter).y << ", " << (*pPIter).z << std::endl;  
    ++pPIter;
  }  
/*
  // print edges
  printf("=== EDGES: %d ======================\n", _rclMesh.CountEdges());
  MeshEdgeIterator pEIter(_rclMesh), pEEnd(_rclMesh);
  pEIter.Begin();
  pEEnd.End();
  i = 0;
  while (pEIter < pEEnd)
  {
    printf("E:%4d: %8.3f  %8.3f  %8.3f | %8.3f  %8.3f  %8.3f | B:%c\n",
           i++, (*pEIter)._aclPoints[0].x, (*pEIter)._aclPoints[0].y,
           (*pEIter)._aclPoints[0].z, (*pEIter)._aclPoints[1].x,
           (*pEIter)._aclPoints[1].y, (*pEIter)._aclPoints[1].z,
           (*pEIter)._bBorder ? 'X' : '-');  
    ++pEIter;
  }
*/
  // print facets
  rclStream << _rclMesh.CountFacets() << " Faces:" << std::endl;
  MeshFacetIterator pFIter(_rclMesh), pFEnd(_rclMesh);
  pFIter.Begin();
  pFEnd.End();
  i = 0;
  while (pFIter < pFEnd)
  {
    rclStream << "F " << (i++) << ": N: " << (*pFIter).GetNormal().x << ", "
                                          << (*pFIter).GetNormal().y << ", "
                                          << (*pFIter).GetNormal().z << std::endl;
    for (j = 0; j < 3; j++)
    {
      rclStream << "   " << (*pFIter)._aclPoints[j].x << ", " 
                         << (*pFIter)._aclPoints[j].y << ", " 
                         << (*pFIter)._aclPoints[j].z << std::endl;  
    }
    ++pFIter;
  }

  return rclStream;
}

std::ostream& MeshInfo::InternalInformation (std::ostream& rclStream) const
{
  unsigned long i;

  // print points
  rclStream << _rclMesh.CountPoints() << " Points:" << std::endl;
  MeshPointIterator pPIter(_rclMesh), pPEnd(_rclMesh);
  pPIter.Begin();
  pPEnd.End();
  i = 0;

  rclStream.precision(3);
  rclStream.width(8);
  while (pPIter < pPEnd)
  {
    rclStream << "P " << (i++) << ": " << (*pPIter).x << ", " << (*pPIter).y << ", " << (*pPIter).z;  
    if (pPIter->IsValid() == true)
      rclStream << std::endl;
    else
      rclStream << " invalid" << std::endl;
    ++pPIter;
  }  
/*
  // print edges
  printf("=== EDGES: %d ======================\n", _rclMesh.CountEdges());
  CMeshEdgeArray::_TConstIterator  pEIter;
  pEIter = _rclMesh._aclEdgeArray.begin();
  i = 0;
  while (pEIter < _rclMesh._aclEdgeArray.end())
  {
    printf("E:%4d  I:%4d  S:%4d", i++, pEIter->Index(), pEIter->Side());
    if (pEIter->IsValid() == true)
      printf("\n");
    else
      printf(" invalid\n");
    pEIter++;
  }    
*/
  // print facets
  rclStream << _rclMesh.CountFacets() << " Faces:" << std::endl;
  MeshFacetArray::_TConstIterator pFIter;
  pFIter = _rclMesh._aclFacetArray.begin();
  i = 0;
  while (pFIter < _rclMesh._aclFacetArray.end())
  {
    rclStream << "F " << (i++) << ": P: " << pFIter->_aulPoints[0] << " "
                                          << pFIter->_aulPoints[1] << " "
                                          << pFIter->_aulPoints[2] << " "
                               << " N: "  << pFIter->_aulNeighbours[0] << " "
                                          << pFIter->_aulNeighbours[1] << " "
                                          << pFIter->_aulNeighbours[2] << " ";
    if (pFIter->IsValid() == true)
      rclStream << std::endl;
    else
      rclStream << " invalid" << std::endl;

    pFIter++;
  }

  return rclStream;
}
