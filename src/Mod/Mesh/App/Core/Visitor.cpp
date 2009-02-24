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

#include "MeshKernel.h"
#include "Visitor.h"
#include "Algorithm.h"

using namespace MeshCore;


unsigned long MeshKernel::VisitNeighbourFacets (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet) const
{
  unsigned long ulVisited = 0, j, ulLevel = 0;
  unsigned long ulCount = _aclFacetArray.size();
  std::vector<unsigned long> clCurrentLevel, clNextLevel;
  std::vector<unsigned long>::iterator  clCurrIter;  
  MeshFacetArray::_TConstIterator clCurrFacet, clNBFacet;

  // Startpunke aufnehmen
  clCurrentLevel.push_back(ulStartFacet);
  _aclFacetArray[ulStartFacet].SetFlag(MeshFacet::VISIT);

  // solange noch freie Nachbarn
  while (clCurrentLevel.size() > 0)
  {
    // besuche alle Nachbarn des aktuellen Level
    for (clCurrIter = clCurrentLevel.begin(); clCurrIter < clCurrentLevel.end(); clCurrIter++)
    {
      clCurrFacet = _aclFacetArray.begin() + *clCurrIter;

      // alle Nachbarn des aktuellen Dreiecks besuchen wenn nicht schon besucht wurde
      for (unsigned short i = 0; i < 3; i++)
      {
        j = clCurrFacet->_aulNeighbours[i]; // Index Nachbar-Facet
        if (j == ULONG_MAX) 
          continue;      // kein Nachbarn-Facet vorhanden

        if (j >= ulCount) 
          continue;      // Fehler in der Datenstruktur

        clNBFacet = _aclFacetArray.begin() + j;

        if (!rclFVisitor.AllowVisit(*clNBFacet, *clCurrFacet, j, ulLevel, i))
          continue;

        if (clNBFacet->IsFlag(MeshFacet::VISIT) == true)
          continue; // Nachbar-Facet schon besucht
        else
        { // besuche und markiere
          ulVisited++;
          clNextLevel.push_back(j);
          clNBFacet->SetFlag(MeshFacet::VISIT);

          if (rclFVisitor.Visit(*clNBFacet, *clCurrFacet, j, ulLevel) == false)
            return ulVisited;
        }
      }
    }
    clCurrentLevel = clNextLevel;
    clNextLevel.clear();
    ulLevel++;
  }  

  return ulVisited;
}

unsigned long MeshKernel::VisitNeighbourFacetsOverCorners (MeshFacetVisitor &rclFVisitor, unsigned long ulStartFacet) const
{
  unsigned long ulVisited = 0, ulLevel = 0;
  MeshRefPointToFacets clRPF(*this);
  const MeshFacetArray& raclFAry = _aclFacetArray;
  MeshFacetArray::_TConstIterator pFBegin = raclFAry.begin();
  std::vector<unsigned long> aclCurrentLevel, aclNextLevel;

  aclCurrentLevel.push_back(ulStartFacet);
  raclFAry[ulStartFacet].SetFlag(MeshFacet::VISIT);

  while (aclCurrentLevel.size() > 0)
  {
    // besuche alle Nachbarn des aktuellen Level
    for (std::vector<unsigned long>::iterator pCurrFacet = aclCurrentLevel.begin(); pCurrFacet < aclCurrentLevel.end(); pCurrFacet++)
    {
      for (int i = 0; i < 3; i++)
      {
        const MeshFacet &rclFacet = raclFAry[*pCurrFacet];
        std::set<MeshFacetArray::_TConstIterator> raclNB = clRPF[rclFacet._aulPoints[i]];
        for (std::set<MeshFacetArray::_TConstIterator>::iterator pINb = raclNB.begin(); pINb != raclNB.end(); pINb++)
        {
          if ((*pINb)->IsFlag(MeshFacet::VISIT) == false)  // nur besuchen wenn VISIT Flag nicht gesetzt
          {          
            ulVisited++;
            unsigned long ulFInd = *pINb - pFBegin;
            aclNextLevel.push_back(ulFInd);
            (*pINb)->SetFlag(MeshFacet::VISIT);
            if (rclFVisitor.Visit(*(*pINb), raclFAry[*pCurrFacet], ulFInd, ulLevel) == false)
              return ulVisited;
          }
        }
      }
    }
    aclCurrentLevel = aclNextLevel;
    aclNextLevel.clear();
    ulLevel++;
  }

  return ulVisited;
}

unsigned long MeshKernel::VisitNeighbourPoints (MeshPointVisitor &rclPVisitor, unsigned long ulStartPoint) const
{
  unsigned long ulVisited = 0, ulLevel = 0;
  std::vector<unsigned long> aclCurrentLevel, aclNextLevel;
  std::vector<unsigned long>::iterator  clCurrIter;  
  MeshPointArray::_TConstIterator pPBegin = _aclPointArray.begin();
  MeshRefPointToPoints clNPs(*this);

  aclCurrentLevel.push_back(ulStartPoint);
  (pPBegin + ulStartPoint)->SetFlag(MeshPoint::VISIT);

  while (aclCurrentLevel.size() > 0)
  {
    // besuche alle Nachbarn des aktuellen Level
    for (clCurrIter = aclCurrentLevel.begin(); clCurrIter < aclCurrentLevel.end(); ++clCurrIter)
    {
      std::set<MeshPointArray::_TConstIterator> raclNB = clNPs[*clCurrIter];
      for (std::set<MeshPointArray::_TConstIterator>::iterator pINb = raclNB.begin(); pINb != raclNB.end(); ++pINb)
      {
        if ((*pINb)->IsFlag(MeshPoint::VISIT) == false)  // nur besuchen wenn VISIT Flag nicht gesetzt
        {          
          ulVisited++;
          unsigned long ulPInd = *pINb - pPBegin;
          aclNextLevel.push_back(ulPInd);
          (*pINb)->SetFlag(MeshPoint::VISIT);
          if (rclPVisitor.Visit(*(*pINb), *(pPBegin + (*clCurrIter)), ulPInd, ulLevel) == false)
            return ulVisited;
        }
      }
    }
    aclCurrentLevel = aclNextLevel;
    aclNextLevel.clear();
    ulLevel++;
  }

  return ulVisited;
}

// -------------------------------------------------------------------------

MeshSearchNeighbourFacetsVisitor::MeshSearchNeighbourFacetsVisitor (const MeshKernel &rclMesh, float fRadius, unsigned long ulStartFacetIdx)
: _rclMeshBase(rclMesh),
  _clCenter(rclMesh.GetFacet(ulStartFacetIdx).GetGravityPoint()),
  _fRadius(fRadius),
  _ulCurrentLevel(0),
  _bFacetsFoundInCurrentLevel(false)
{
}

std::vector<unsigned long> MeshSearchNeighbourFacetsVisitor::GetAndReset (void)
{
  MeshAlgorithm(_rclMeshBase).ResetFacetsFlag(_vecFacets, MeshFacet::VISIT);
  return _vecFacets;
}
