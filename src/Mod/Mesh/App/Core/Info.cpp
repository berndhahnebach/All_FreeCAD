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

#ifndef _PreComp_
# include <iomanip>
# include <ios>
# include <map>
# include <set>
#endif

#include "Info.h"
#include "Algorithm.h"
#include "Iterator.h"

using namespace MeshCore;

MeshInfo::MeshInfo (MeshKernel &rclM)
: _rclMesh(rclM)
{
}

std::ostream& MeshInfo::GeneralInformation (std::ostream &rclStream) const
{
  unsigned long ulCtPt, ulCtEd, ulCtFc;

  ulCtPt = _rclMesh.CountPoints();
#ifdef Use_EdgeList
  ulCtEd = _rclMesh.CountEdges();
#else
  std::set<std::pair<unsigned long, unsigned long> > lEdges;
  MeshFacetArray::_TConstIterator pFIter;
  pFIter = _rclMesh._aclFacetArray.begin();
  unsigned long i = 0;
  while (pFIter < _rclMesh._aclFacetArray.end())
  {
    const MeshFacet& rFacet = *pFIter;
    for ( int j=0; j<3; j++ )
    {
      unsigned long ulPt0 = std::min<unsigned long>(rFacet._aulPoints[j],  rFacet._aulPoints[(j+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(rFacet._aulPoints[j],  rFacet._aulPoints[(j+1)%3]);
      std::pair<unsigned long, unsigned long> cEdge(ulPt0, ulPt1); 
      lEdges.insert( cEdge );
    }

    pFIter++;
    i++;
  }

  ulCtEd = lEdges.size();
#endif
  ulCtFc = _rclMesh.CountFacets();

  rclStream << "Mesh: ["
            << ulCtFc << " Faces, "
            << ulCtEd << " Edges, "
            << ulCtPt << " Points"
            << "]" << std::endl;

  return rclStream;
}

std::ostream& MeshInfo::DetailedPointInfo (std::ostream& rclStream) const
{
  // print points
  unsigned long i;
  rclStream << _rclMesh.CountPoints() << " Points:" << std::endl;
  MeshPointIterator pPIter(_rclMesh), pPEnd(_rclMesh);
  pPIter.Begin();
  pPEnd.End();
  i = 0;

  rclStream.precision(3);
  rclStream.setf(std::ios::fixed | std::ios::showpoint | std::ios::showpos);
  while (pPIter < pPEnd)
  {
    rclStream << "P " << std::setw(4) << (i++)       << ": ("
                      << std::setw(8) << (*pPIter).x << ", "
                      << std::setw(8) << (*pPIter).y << ", "
                      << std::setw(8) << (*pPIter).z << ")" << std::endl;
    ++pPIter;
  }

  return rclStream;
}

std::ostream& MeshInfo::DetailedEdgeInfo (std::ostream& rclStream) const
{
  // print edges
  unsigned long i;
#ifdef Use_EdgeList
//  printf("=== EDGES: %d ======================\n", _rclMesh.CountEdges());
  MeshEdgeIterator pEIter(_rclMesh), pEEnd(_rclMesh);
  pEIter.Begin();
  pEEnd.End();
  i = 0;

  rclStream.precision(3);
  rclStream.setf(std::ios::fixed | std::ios::showpoint | std::ios::showpos);
  while (pEIter < pEEnd)
  {
    const Vector3D& rP0 = pEIter->_aclPoints[0];
    const Vector3D& rP1 = pEIter->_aclPoints[0];
    rclStream << "E "    << std::setw(4) << (i++) << ": "
              << "  P (" << std::setw(8) << rP0.x << ", "
                         << std::setw(8) << rP0.y << ", "
                         << std::setw(8) << rP0.z << "); "
              << "  P (" << std::setw(8) << rP1.x << ", "
                         << std::setw(8) << rP1.y << ", "
                         << std::setw(8) << rP1.z << "),  B: " << (pEIter->_bBorder ? "y" : "n") << std::endl;
    ++pEIter;
  }
#else
  // get edges from facets
  std::map<std::pair<unsigned long, unsigned long>, int > lEdges;
  MeshFacetArray::_TConstIterator pFIter;
  pFIter = _rclMesh._aclFacetArray.begin();
  i = 0;
  while (pFIter < _rclMesh._aclFacetArray.end())
  {
    const MeshFacet& rFacet = *pFIter;
    for ( int j=0; j<3; j++ )
    {
      unsigned long ulPt0 = std::min<unsigned long>(rFacet._aulPoints[j],  rFacet._aulPoints[(j+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(rFacet._aulPoints[j],  rFacet._aulPoints[(j+1)%3]);
      std::pair<unsigned long, unsigned long> cEdge(ulPt0, ulPt1);
      lEdges[ cEdge ]++;
    }

    pFIter++;
    i++;
  }

  // print edges
  rclStream << lEdges.size() << " Edges:" << std::endl;
  std::map<std::pair<unsigned long, unsigned long>, int >::const_iterator  pEIter;
  pEIter = lEdges.begin();
  i = 0;

  rclStream.precision(3);
  rclStream.setf(std::ios::fixed | std::ios::showpoint | std::ios::showpos);
  while (pEIter != lEdges.end())
  {
    int ct = pEIter->second;
    const Vector3D& rP0 = _rclMesh._aclPointArray[ pEIter->first.first ];
    const Vector3D& rP1 = _rclMesh._aclPointArray[ pEIter->first.second ];

    rclStream << "E "    << std::setw(4) << (i++) << ": "
              << "  P (" << std::setw(8) << rP0.x << ", "
                         << std::setw(8) << rP0.y << ", "
                         << std::setw(8) << rP0.z << "); "
              << "  P (" << std::setw(8) << rP1.x << ", "
                         << std::setw(8) << rP1.y << ", "
                         << std::setw(8) << rP1.z << "),  B: " << (ct == 2 ? "n" : "y") << std::endl;
    pEIter++;
  }
#endif

  return rclStream;
}

std::ostream& MeshInfo::DetailedFacetInfo (std::ostream& rclStream) const
{
  // print facets
  unsigned long i, j;
  rclStream << _rclMesh.CountFacets() << " Faces:" << std::endl;
  MeshFacetIterator pFIter(_rclMesh), pFEnd(_rclMesh);
  pFIter.Begin();
  pFEnd.End();
  i = 0;

  rclStream.precision(3);
  rclStream.setf(std::ios::fixed | std::ios::showpoint | std::ios::showpos);
  while (pFIter < pFEnd)
  {
    rclStream << "F "    << std::setw(4) << (i++) << ":" << std::endl;
    rclStream << "  N (" << std::setw(8) << (*pFIter).GetNormal().x << ", "
                         << std::setw(8) << (*pFIter).GetNormal().y << ", "
                         << std::setw(8) << (*pFIter).GetNormal().z << ")" << std::endl;
    for (j = 0; j < 3; j++)
    {
      rclStream << "  P (" << std::setw(8) << (*pFIter)._aclPoints[j].x << ", "
                           << std::setw(8) << (*pFIter)._aclPoints[j].y << ", "
                           << std::setw(8) << (*pFIter)._aclPoints[j].z << ")" << std::endl;
    }
    ++pFIter;
  }

  return rclStream;
}

std::ostream& MeshInfo::DetailedInformation (std::ostream& rclStream) const
{
  DetailedPointInfo( rclStream );
  DetailedEdgeInfo ( rclStream );
  DetailedFacetInfo( rclStream );

  return rclStream;
}

std::ostream& MeshInfo::InternalPointInfo (std::ostream& rclStream) const
{
  // print points
  unsigned long i;
  rclStream << _rclMesh.CountPoints() << " Points:" << std::endl;
  MeshPointIterator pPIter(_rclMesh), pPEnd(_rclMesh);
  pPIter.Begin();
  pPEnd.End();
  i = 0;

  rclStream.precision(3);
  rclStream.setf(std::ios::fixed | std::ios::showpoint | std::ios::showpos);
  while (pPIter < pPEnd)
  {
    rclStream << "P " << std::setw(4) << (i++)       << ": ("
                      << std::setw(8) << (*pPIter).x << ", "
                      << std::setw(8) << (*pPIter).y << ", "
                      << std::setw(8) << (*pPIter).z << ")";
    if (pPIter->IsValid() == true)
      rclStream << std::endl;
    else
      rclStream << " invalid" << std::endl;
    ++pPIter;
  }

  return rclStream;
}

std::ostream& MeshInfo::InternalEdgeInfo (std::ostream& rclStream) const
{
  unsigned long i;

#ifdef Use_EdgeList
  rclStream << _rclMesh.CountEdges() << " Edge:" << std::endl;
  MeshEdgeArray::_TConstIterator  pEIter;
  pEIter = _rclMesh._aclEdgeArray.begin();
  i = 0;
  while (pEIter < _rclMesh._aclEdgeArray.end())
  {
    rclStream << "E " << std::setw(4) << (i++) << " F: " << std::setw(4) << pEIter->Index()
                                               << " S: " << std::setw(4) << pEIter->Side();
    if (pEIter->IsValid() == true)
      rclStream << std::endl;
    else
      rclStream << " invalid" << std::endl;
    pEIter++;
  }

#else
  // get edges from facets
  std::map<std::pair<unsigned long, unsigned long>, std::vector<MeshEdge> > lEdges;
  MeshFacetArray::_TConstIterator pFIter;
  pFIter = _rclMesh._aclFacetArray.begin();
  i = 0;
  while (pFIter < _rclMesh._aclFacetArray.end())
  {
    const MeshFacet& rFacet = *pFIter;
    for ( int j=0; j<3; j++ )
    {
      MeshEdge edge;
      edge.Set(i, j);

      unsigned long ulPt0 = std::min<unsigned long>(rFacet._aulPoints[j],  rFacet._aulPoints[(j+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(rFacet._aulPoints[j],  rFacet._aulPoints[(j+1)%3]);

      std::pair<unsigned long, unsigned long> cEdge(ulPt0, ulPt1);
      lEdges[ cEdge ].push_back( edge );
    }

    pFIter++;
    i++;
  }

  // print edges
  rclStream << lEdges.size() << " Edges:" << std::endl;
  std::map<std::pair<unsigned long, unsigned long>, std::vector<MeshEdge> >::const_iterator  pEIter;
  pEIter = lEdges.begin();
  i = 0;
  while (pEIter != lEdges.end())
  {
    const std::vector<MeshEdge>& ed = pEIter->second;

    rclStream << "E " << std::setw(4) << (i++) << ": P ("
                      << std::setw(4) << pEIter->first.first  << ", "
                      << std::setw(4) << pEIter->first.second << ")  ";
    for ( std::vector<MeshEdge>::const_iterator it = ed.begin(); it!=ed.end(); ++it )
    {
      rclStream << "F/S (" << std::setw(4) << it->Index() << ", "
                           << std::setw(1) << it->Side() << "), ";
    }
    if ( ed.size() == 2 )
      rclStream << std::endl;
    else if ( ed.size() == 1 )
      rclStream << " open" << std::endl;
    else
      rclStream << " invalid" << std::endl;
    pEIter++;
  }
#endif
  return rclStream;
}

std::ostream& MeshInfo::InternalFacetInfo (std::ostream& rclStream) const
{
  // print facets
  unsigned long i;
  rclStream << _rclMesh.CountFacets() << " Faces:" << std::endl;
  MeshFacetArray::_TConstIterator pFIter;
  pFIter = _rclMesh._aclFacetArray.begin();
  i = 0;
  while (pFIter < _rclMesh._aclFacetArray.end())
  {
    rclStream << "F " << std::setw(4) << (i++) << ": P ("
                      << std::setw(4) << pFIter->_aulPoints[0] << ", "
                      << std::setw(4) << pFIter->_aulPoints[1] << ", "
                      << std::setw(4) << pFIter->_aulPoints[2] << ")  "
             << "N (" << std::setw(4) << pFIter->_aulNeighbours[0] << ", "
                      << std::setw(4) << pFIter->_aulNeighbours[1] << ", "
                      << std::setw(4) << pFIter->_aulNeighbours[2] << ") ";

    if (pFIter->IsValid() == true)
      rclStream << std::endl;
    else
      rclStream << " invalid" << std::endl;

    pFIter++;
  }

  return rclStream;
}

std::ostream& MeshInfo::InternalInformation (std::ostream& rclStream) const
{
  InternalPointInfo( rclStream );
  InternalEdgeInfo ( rclStream );
  InternalFacetInfo( rclStream );

  return rclStream;
}

