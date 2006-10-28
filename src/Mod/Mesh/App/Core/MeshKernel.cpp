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
# include <algorithm>
# include <map>
# include <queue>
#endif

#include <Base/Sequencer.h>

#include "Algorithm.h"
#include "Helpers.h"
#include "MeshKernel.h"
#include "Iterator.h"
#include "Evaluation.h"
#include "Builder.h"

using namespace MeshCore;

MeshKernel::MeshKernel (void)
: _bValid(true)
{
  _clBoundBox.Flush();
}

MeshKernel::MeshKernel (const MeshKernel &rclMesh)
{
  *this = rclMesh;
}

MeshKernel& MeshKernel::operator = (const MeshKernel &rclMesh)
{
  _aclPointArray  = rclMesh._aclPointArray;
  _aclFacetArray  = rclMesh._aclFacetArray;
  _clBoundBox     = rclMesh._clBoundBox;
  _bValid         = rclMesh._bValid;
  return *this;
}

void MeshKernel::Assign(const MeshPointArray& rPoints, const MeshFacetArray& rFacets, bool checkNeighbourHood)
{
  _aclPointArray = rPoints;
  _aclFacetArray = rFacets;
  RecalcBoundBox();
  if (checkNeighbourHood)
    RebuildNeighbours();
}

void MeshKernel::Adopt(MeshPointArray& rPoints, MeshFacetArray& rFacets, bool checkNeighbourHood)
{
  _aclPointArray.swap(rPoints);
  _aclFacetArray.swap(rFacets);
  RecalcBoundBox();
  if (checkNeighbourHood)
    RebuildNeighbours();
}

MeshKernel& MeshKernel::operator += (const MeshGeomFacet &rclSFacet)
{
  unsigned long i;
  MeshFacet clFacet;

  // Eckpunkte einfuegen
  for (i = 0; i < 3; i++)
  {
    _clBoundBox &= rclSFacet._aclPoints[i];
    clFacet._aulPoints[i] = _aclPointArray.GetOrAddIndex(rclSFacet._aclPoints[i]);
  }

  // Umlaufrichtung der Normale anpassen
  AdjustNormal(clFacet, rclSFacet.GetNormal());

  unsigned long ulCt = _aclFacetArray.size();

  // Nachbarschaften setzen
  unsigned long ulP0 = clFacet._aulPoints[0];
  unsigned long ulP1 = clFacet._aulPoints[1];
  unsigned long ulP2 = clFacet._aulPoints[2];
  unsigned long ulCC = 0;
  for (TMeshFacetArray::iterator pF = _aclFacetArray.begin(); pF != _aclFacetArray.end(); pF++, ulCC++)
  {
    for (int i = 0; i < 2; i++)
    {
      unsigned long ulP = pF->_aulPoints[i];
      if ((ulP == ulP0) || (ulP == ulP1) || (ulP == ulP2))
      {
        unsigned long ulPInc = pF->_aulPoints[i+1];
        if (ulP == ulP0)
        {
          if (ulPInc == ulP1)
          {
            unsigned short usSide = clFacet.Side(ulP0, ulP1);
            clFacet._aulNeighbours[usSide] = ulCC;
            usSide = pF->Side(ulP, ulPInc);
            pF->_aulNeighbours[usSide] = ulCt;
          }
          else if (ulPInc == ulP2)
          {
            unsigned short usSide = clFacet.Side(ulP0, ulP2);
            clFacet._aulNeighbours[usSide] = ulCC;
            usSide = pF->Side(ulP, ulPInc);
            pF->_aulNeighbours[usSide] = ulCt;
          }
        }
        else if (ulP == ulP1)
        {
          if (ulPInc == ulP2)
          {
            unsigned short usSide = clFacet.Side(ulP1, ulP2);
            clFacet._aulNeighbours[usSide] = ulCC;
            usSide = pF->Side(ulP, ulPInc);
            pF->_aulNeighbours[usSide] = ulCt;
          }
        }
      }
    }    
  }

  // Facet ins Facet-Array einfuegen
  _aclFacetArray.push_back(clFacet);

  return *this;
}

MeshKernel& MeshKernel::operator += (const std::vector<MeshGeomFacet> &rclVAry)
{
  // kompleter Neu-Aufbau
  std::vector<MeshGeomFacet> clNew(rclVAry.size() + CountFacets());

  std::vector<MeshGeomFacet>::iterator pN = clNew.begin();

  MeshFacetIterator clF(*this);
  for (clF.Init(); clF.More(); clF.Next(), pN++)
    *pN = *clF;
  
  for (std::vector<MeshGeomFacet>::const_iterator pV = rclVAry.begin(); pV != rclVAry.end(); pV++, pN++)
    *pN = *pV;

  Base::Sequencer().setLocked( true );
  *this = clNew;
  Base::Sequencer().setLocked( false );

  return *this;
}

bool MeshKernel::AddFacet(const MeshGeomFacet &rclSFacet)
{
  *this += rclSFacet;

  RebuildNeighbours();

  return true;
}

bool MeshKernel::AddFacet(const std::vector<MeshGeomFacet> &rclVAry)
{
  // operator += gehtnicht da dann das Mesh neu angelegt wird und somit alle props geloescht werden
  //
  for (std::vector<MeshGeomFacet>::const_iterator pN = rclVAry.begin();pN!=rclVAry.end();pN++)
    (*this) += (*pN);

  RebuildNeighbours();

  return true;
}

unsigned long MeshKernel::AddFacet(const std::vector<MeshFacet> &rclVAry)
{
  // Build map of edges to the referencing facets
  unsigned long k = 0;
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> > aclEdgeMap;
  for (MeshFacetArray::_TIterator pF = _aclFacetArray.begin(); pF != _aclFacetArray.end(); pF++, k++) {
    for (int i=0; i<3; i++) {
      unsigned long ulT0 = pF->_aulPoints[i];
      unsigned long ulT1 = pF->_aulPoints[(i+1)%3];
      unsigned long ulP0 = std::min<unsigned long>(ulT0, ulT1);
      unsigned long ulP1 = std::max<unsigned long>(ulT0, ulT1);
      aclEdgeMap[std::make_pair<unsigned long, unsigned long>(ulP0, ulP1)].push_front(k);
    }
  }

  // Do not insert directly to the data structure because we should get the correct size of new facets,
  // otherwise std::vector reallocates too much memory which couldn't be freed so easily
  std::vector<MeshFacet> tmp;
  tmp.reserve(rclVAry.size());

  // validate the correct topology of the facets to be inserted
#ifdef FC_DEBUG
  unsigned long countPoints = CountPoints();
#endif
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> > updateEdgeMap;
  for ( MeshFacetArray::_TConstIterator it = rclVAry.begin(); it != rclVAry.end(); ++it ) {
    bool canInsert = true;
    std::pair<unsigned long, unsigned long> edge[3];
    for ( int i=0; i<3; i++ ) {
#ifdef FC_DEBUG
      assert( it->_aulPoints[i] < countPoints );
#endif
      unsigned long ulT0 = it->_aulPoints[i];
      unsigned long ulT1 = it->_aulPoints[(i+1)%3];
      unsigned long ulP0 = std::min<unsigned long>(ulT0, ulT1);
      unsigned long ulP1 = std::max<unsigned long>(ulT0, ulT1);
      edge[i] = std::make_pair<unsigned long, unsigned long>(ulP0, ulP1);

      std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator pI = aclEdgeMap.find(edge[i]);
      if ( pI != aclEdgeMap.end() && pI->second.size() >= 2 )
      {
        // do not allow to insert the facet otherwise we get a non-manifold
        canInsert = false;
        break;
      }
    }

    if ( canInsert ) {
      // insert into the edge map
      aclEdgeMap[edge[0]].push_front(k);
      aclEdgeMap[edge[1]].push_front(k);
      aclEdgeMap[edge[2]].push_front(k);
      k++;

      // copy the edges which needs to update its referencing facets
      updateEdgeMap[edge[0]] = aclEdgeMap[edge[0]];
      updateEdgeMap[edge[1]] = aclEdgeMap[edge[1]];
      updateEdgeMap[edge[2]] = aclEdgeMap[edge[2]];
      
      tmp.push_back( *it );
    }
  }

  // now start inserting the facets to the data structure and set the correct neighbourhood as well
  _aclFacetArray.reserve( _aclFacetArray.size() + tmp.size() );
  for ( MeshFacetArray::_TConstIterator jt = tmp.begin(); jt != tmp.end(); ++jt )
    _aclFacetArray.push_back( *jt );

  // resolve neighbours
  for (std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator pI = updateEdgeMap.begin(); pI != updateEdgeMap.end(); pI++)
  {
    unsigned long ulP0 = pI->first.first;
    unsigned long ulP1 = pI->first.second;
    if (pI->second.size() == 1)  // border facet
    {
      unsigned long ulF0 = pI->second.front();
      unsigned short usSide =  _aclFacetArray[ulF0].Side(ulP0, ulP1);
      assert(usSide != USHRT_MAX);
      _aclFacetArray[ulF0]._aulNeighbours[usSide] = ULONG_MAX;
    }
    else if (pI->second.size() == 2)  // normal facet with neighbour
    {
      unsigned long ulF0 = pI->second.front();
      unsigned long ulF1 = pI->second.back();
      unsigned short usSide =  _aclFacetArray[ulF0].Side(ulP0, ulP1);
      assert(usSide != USHRT_MAX);
      _aclFacetArray[ulF0]._aulNeighbours[usSide] = ulF1;
      usSide =  _aclFacetArray[ulF1].Side(ulP0, ulP1);
      assert(usSide != USHRT_MAX);
      _aclFacetArray[ulF1]._aulNeighbours[usSide] = ulF0;
    }
  }

  return _aclFacetArray.size();
}

void MeshKernel::Merge( const MeshPointArray& rPoints, const MeshFacetArray& rFaces )
{
  // Create a map <point index -> face indices>. The size of the map is the number of points we have to append 
  std::map<unsigned long, std::vector<unsigned long> > point2Faces;

  // Copy the new faces immediately to the current array
  this->_aclFacetArray.reserve( this->_aclFacetArray.size() + rFaces.size() );
  MeshFacet face;
  for( MeshFacetArray::_TConstIterator it = rFaces.begin(); it != rFaces.end(); ++it )
  {
    face = *it;
    for ( int i=0; i<3; i++ ) {
      point2Faces[it->_aulPoints[i]].push_back( this->_aclFacetArray.size() );
      // We must reset the neighbourhood indices to ULONG_MAX as we set the correct indices at the end
      face._aulNeighbours[i] = ULONG_MAX;
    }

    this->_aclFacetArray.push_back( face );
  }

  // Reserve the additional memory to append the new points
  this->_aclPointArray.reserve(this->_aclPointArray.size() + point2Faces.size());
  
  // Now we can start inserting the points and adjust the point indices of the faces
  unsigned long cnt = this->_aclPointArray.size();
  for ( std::map<unsigned long, std::vector<unsigned long> >::iterator pM = point2Faces.begin(); pM != point2Faces.end(); ++pM )
  {
    unsigned long iPos = pM->first;

    // copy the mesh vertex
    this->_aclPointArray.push_back(rPoints[iPos]);

    // adjust the point indices of the referenced faces
    std::vector<unsigned long>& pt2Faces = pM->second;
    for ( std::vector<unsigned long>::iterator pF = pt2Faces.begin(); pF != pt2Faces.end(); ++pF )
    {
      for ( int i=0; i<3; i++ ) {
        if ( this->_aclFacetArray[*pF]._aulPoints[i] == iPos )
          this->_aclFacetArray[*pF]._aulPoints[i] = cnt;
      }
    }

    cnt++;
  }

  // Here we have adjusted the point indices, now we can also setup the neighbourhood
  for ( std::map<unsigned long, std::vector<unsigned long> >::iterator pM2 = point2Faces.begin(); pM2 != point2Faces.end(); ++pM2 )
  {
    std::vector<unsigned long>& pt2Faces = pM2->second;
    for ( std::vector<unsigned long>::iterator pF1 = pt2Faces.begin(); pF1 != pt2Faces.end(); ++pF1 )
    {
      for ( std::vector<unsigned long>::iterator pF2 = pt2Faces.begin(); pF2 != pt2Faces.end(); ++pF2 )
      {
        // compare all faces each other
        if ( *pF1 != *pF2 ){
          MeshCore::MeshFacet& rFace1 = this->_aclFacetArray[*pF1];
          MeshCore::MeshFacet& rFace2 = this->_aclFacetArray[*pF2];
          unsigned short side = rFace1.Side(rFace2);
          // both faces share a common edge
          if ( side != USHRT_MAX )
          {
            rFace1._aulNeighbours[side] = *pF2;
            side = rFace2.Side(rFace1);
            rFace2._aulNeighbours[side] = *pF1;
          }
        }
      }
    }
  }

  // finally recompute the bounding box
  this->RecalcBoundBox();
}

void MeshKernel::Clear (void)
{
  _aclPointArray.clear();
  _aclFacetArray.clear();

  // auch Speicher freigeben
  MeshPointArray().swap(_aclPointArray);
  MeshFacetArray().swap(_aclFacetArray);

  _clBoundBox.Flush();
}

MeshKernel& MeshKernel::operator = (std::vector<MeshGeomFacet> &rclVAry)
{
  MeshBuilder builder(*this);
  builder.Initialize(rclVAry.size());

  for (std::vector<MeshGeomFacet>::iterator it = rclVAry.begin(); it != rclVAry.end(); it++)
    builder.AddFacet(*it);

  builder.Finish();

	return *this;
}

bool MeshKernel::DeleteFacet (const MeshFacetIterator &rclIter)
{
  unsigned long i, j, ulNFacet, ulInd;

  if (rclIter._clIter >= _aclFacetArray.end())
    return false;

  // Index des zu loeschenden Facet
  ulInd = rclIter._clIter - _aclFacetArray.begin(); 

  // Nachbar-Indizies der Nachbar-Facets auf dieses Facet als ungueltigt setzen
  // gleichzeitig Kanten loeschen oder umbiegen auf Nachbarfacet
  for (i = 0; i < 3; i++)
  {
    ulNFacet = rclIter._clIter->_aulNeighbours[i];  // Index: Nachbar-Facet
    if (ulNFacet != ULONG_MAX)
    {
      for (j = 0; j < 3; j++)
      {
        if (_aclFacetArray[ulNFacet]._aulNeighbours[j] == ulInd)
        {
          _aclFacetArray[ulNFacet]._aulNeighbours[j] = ULONG_MAX;
         break;
        }
      }
    }
  }

  // Eckpunkte gegenfalls loeschen
  for (i = 0; i < 3; i++)
  {
    if ((rclIter._clIter->_aulNeighbours[i] == ULONG_MAX) &&
        (rclIter._clIter->_aulNeighbours[(i+1)%3] == ULONG_MAX))
    {  // keine Nachbarn, evt. Punkt loeschen
      ErasePoint(rclIter._clIter->_aulPoints[(i+1)%3], ulInd);
    }
  }

  // Facet aus Array loeschen
  _aclFacetArray.Erase(_aclFacetArray.begin() + rclIter.Position());

  return true;
}

bool MeshKernel::DeleteFacet (unsigned long ulInd)
{
  if (ulInd >= _aclFacetArray.size() )
    return false;

  MeshFacetIterator clIter(*this);
  clIter.Set(ulInd);

  return DeleteFacet(clIter);
}

bool MeshKernel::DeletePoint (const MeshPointIterator &rclIter)
{
  MeshFacetIterator pFIter(*this), pFEnd(*this);
  std::vector<MeshFacetIterator>  clToDel; 
  MeshPoint clPt;
  unsigned long i;
 
  // Eckpunkte aller Facets ueberpruefen
  clPt = *rclIter;
  pFIter.Begin();
  pFEnd.End();

  while (pFIter < pFEnd)
  {
    for (i = 0; i < 3; i++)
    {
      if (clPt == _aclPointArray[pFIter._clIter->_aulPoints[i]])
        clToDel.push_back(pFIter);
    }
    ++pFIter;
  }

  // Iteratoren (Facets) nach Index RSsortieren
  std::sort(clToDel.begin(), clToDel.end());

  // jedes einzelne Facet loeschen
  for (i = clToDel.size(); i > 0; i--)
    DeleteFacet(clToDel[i-1]); 

  return true;
}

void MeshKernel::ErasePoint (unsigned long ulIndex, unsigned long ulFacetIndex, bool bOnlySetInvalid)
{
  unsigned long  i;
  std::vector<MeshFacet>::iterator pFIter, pFEnd, pFNot;

  pFIter = _aclFacetArray.begin();
  pFNot  = _aclFacetArray.begin() + ulFacetIndex;
  pFEnd  = _aclFacetArray.end();
 
  // pruefe alle Facets
  while (pFIter < pFNot)
  {
    for (i = 0; i < 3; i++)
    {
      if (pFIter->_aulPoints[i] == ulIndex)
        return; // Punkt noch zugeordnet ==> nicht loeschen
    }
    pFIter++;
  }

  pFIter++;
  while (pFIter < pFEnd)
  {
    for (i = 0; i < 3; i++)
    {
      if (pFIter->_aulPoints[i] == ulIndex)
        return; // Punkt noch zugeordnet ==> nicht loeschen
    }
    pFIter++;
  }


  if (bOnlySetInvalid == false)
  { // Punkt komplett loeschen
    _aclPointArray.erase(_aclPointArray.begin() + ulIndex);

    // Punkt-Indizies der Facets korrigieren
    pFIter = _aclFacetArray.begin();
    while (pFIter < pFEnd)
    {
      for (i = 0; i < 3; i++)
      {
        if (pFIter->_aulPoints[i] > ulIndex)
          pFIter->_aulPoints[i]--;
      }
      pFIter++;
    }
  }
  else // nur invalid setzen
    _aclPointArray[ulIndex].SetInvalid();
}

void MeshKernel::RemoveInvalids (bool bWithEdgeCorrect, bool bWithEdgeDelete)
{
  std::vector<unsigned long> aulDecrements;
  std::vector<unsigned long>::iterator       pDIter;
  unsigned long ulDec, i, k, ulSteps;
  MeshPointArray::_TIterator pPIter, pPEnd;
  MeshFacetArray::_TIterator pFIter, pFEnd;

  ulSteps = 0;

  // Array mit Dekrements generieren
  aulDecrements.resize(_aclPointArray.size());
  pDIter = aulDecrements.begin();
  ulDec  = 0;
  pPEnd  = _aclPointArray.end();
  for (pPIter = _aclPointArray.begin(); pPIter != pPEnd; pPIter++)
  {
    *pDIter++ = ulDec;
    if (pPIter->IsValid() == false)
      ulDec++;
  }

  // Punkt-Indizies der Facets korrigieren
  pFEnd  = _aclFacetArray.end();
  for (pFIter = _aclFacetArray.begin(); pFIter != pFEnd; pFIter++)
  {
    if (pFIter->IsValid() == true)
    {
      pFIter->_aulPoints[0] -= aulDecrements[pFIter->_aulPoints[0]];      
      pFIter->_aulPoints[1] -= aulDecrements[pFIter->_aulPoints[1]];
      pFIter->_aulPoints[2] -= aulDecrements[pFIter->_aulPoints[2]];
    }
  }
 
  // Punkte loeschen
  // Anzahl der noch gueltigen Punkte
  unsigned long ulNewPts = std::count_if(_aclPointArray.begin(), _aclPointArray.end(), std::mem_fun_ref(&MeshPoint::IsValid));
  // temp. Punktarray
  MeshPointArray  aclTempPt(ulNewPts);
  MeshPointArray::_TIterator pPTemp = aclTempPt.begin();
  pPEnd = _aclPointArray.end();
  for (pPIter = _aclPointArray.begin(); pPIter != pPEnd; pPIter++)
  {
    if (pPIter->IsValid() == true)
      *pPTemp++ = *pPIter;
  }
  _aclPointArray = aclTempPt;
  aclTempPt.clear();

  // Array mit Facet-Decrements generieren
  aulDecrements.resize(_aclFacetArray.size());
  pDIter = aulDecrements.begin();
  ulDec  = 0;
  pFEnd  = _aclFacetArray.end();
  for (pFIter = _aclFacetArray.begin(); pFIter != pFEnd; pFIter++, pDIter++)
  {
    *pDIter = ulDec;
    if (pFIter->IsValid() == false)
      ulDec++;
  }

  // Nachbar-Indizies der Facets korrigieren
  pFEnd = _aclFacetArray.end();
  for (pFIter = _aclFacetArray.begin(); pFIter != pFEnd; pFIter++)
  {
    if (pFIter->IsValid() == true)
    {
      for (i = 0; i < 3; i++)
      {
        k = pFIter->_aulNeighbours[i];
        if (k != ULONG_MAX)
        {
          if (_aclFacetArray[k].IsValid() == true)
            pFIter->_aulNeighbours[i] -= aulDecrements[k];
          else      
            pFIter->_aulNeighbours[i] = ULONG_MAX;
        }
      }
    }
  }


  // Facets loeschen
  // Anzahl der noch gueltigen Facets
  unsigned long ulDelFacets = std::count_if(_aclFacetArray.begin(), _aclFacetArray.end(), std::mem_fun_ref(&MeshFacet::IsValid));
  MeshFacetArray aclFArray(ulDelFacets);
  MeshFacetArray::_TIterator pFTemp = aclFArray.begin();  
  pFEnd  = _aclFacetArray.end();
  for (pFIter = _aclFacetArray.begin(); pFIter != pFEnd; pFIter++)
  {
    if (pFIter->IsValid() == true)
      *pFTemp++ = *pFIter;
  }
  _aclFacetArray = aclFArray;
}

std::vector<unsigned long> MeshKernel::HasFacets (const MeshPointIterator &rclIter) const
{
  unsigned long i, ulPtInd = rclIter.Position();
  std::vector<MeshFacet>::const_iterator  pFIter = _aclFacetArray.begin();
  std::vector<MeshFacet>::const_iterator  pFBegin = _aclFacetArray.begin();
  std::vector<MeshFacet>::const_iterator  pFEnd = _aclFacetArray.end();
  std::vector<unsigned long> aulBelongs;

  while (pFIter < pFEnd)
  {
    for (i = 0; i < 3; i++)
    {
      if (pFIter->_aulPoints[i] == ulPtInd)
      {
        aulBelongs.push_back(pFIter - pFBegin);
        break;
      }
    }
    pFIter++;
  }

  return aulBelongs;
}

void MeshKernel::Write (std::ostream &rclOut) const 
{
  unsigned long uCtPts = CountPoints();
  unsigned long uCtFts = CountFacets();
  rclOut.write((const char*)&uCtPts, sizeof(unsigned long));
  rclOut.write((const char*)&uCtFts, sizeof(unsigned long));

  // the mesh kernel might be empty
  if ( uCtPts > 0 )
    rclOut.write((const char*)&(_aclPointArray[0]), uCtPts*sizeof(MeshPoint));
  if ( uCtFts > 0 )
    rclOut.write((const char*)&(_aclFacetArray[0]), uCtFts*sizeof(MeshFacet));
  rclOut.write((const char*)&_clBoundBox, sizeof(Base::BoundBox3f));
}

void MeshKernel::Read (std::istream &rclIn)
{
  Clear();

  unsigned long uCtPts=ULONG_MAX, uCtFts=ULONG_MAX;
  rclIn.read((char*)&uCtPts, sizeof(unsigned long));
  rclIn.read((char*)&uCtFts, sizeof(unsigned long));

  // the stored mesh kernel might be empty
  if ( uCtPts > 0 ) {
    _aclPointArray.resize(uCtPts);
    rclIn.read((char*)&(_aclPointArray[0]), uCtPts*sizeof(MeshPoint));
  }
  if ( uCtFts > 0 ) {
    _aclFacetArray.resize(uCtFts);
    rclIn.read((char*)&(_aclFacetArray[0]), uCtFts*sizeof(MeshFacet));
  }
  rclIn.read((char*)&_clBoundBox, sizeof(Base::BoundBox3f));
}

void MeshKernel::operator *= (const Base::Matrix4D &rclMat)
{
  MeshPointArray::_TIterator  clPIter = _aclPointArray.begin(), clPEIter = _aclPointArray.end();
  Base::Matrix4D clMatrix(rclMat);

  _clBoundBox.Flush();
  while (clPIter < clPEIter)
  {
    *clPIter *= clMatrix;
    _clBoundBox &= *clPIter;
    clPIter++;
  }
}

void MeshKernel::Transform (const Base::Matrix4D &rclMat)
{
  (*this) *= rclMat;
}

void MeshKernel::DeleteFacets (const std::vector<unsigned long> &raulFacets)
{
  _aclPointArray.SetProperty(0);

  // Anzahl der ref. Facet pro Punkt
  for (MeshFacetArray::_TConstIterator pF = _aclFacetArray.begin(); pF != _aclFacetArray.end(); pF++)
  {
    _aclPointArray[pF->_aulPoints[0]]._ulProp++;
    _aclPointArray[pF->_aulPoints[1]]._ulProp++;
    _aclPointArray[pF->_aulPoints[2]]._ulProp++;
  }

  // Facet ungueltig markieren und Punkt-Ref. abstimmen
  _aclFacetArray.ResetInvalid();
  for (std::vector<unsigned long>::const_iterator pI = raulFacets.begin(); pI != raulFacets.end(); pI++)
  {
    MeshFacet &rclFacet = _aclFacetArray[*pI];
    rclFacet.SetInvalid();
    _aclPointArray[rclFacet._aulPoints[0]]._ulProp--;
    _aclPointArray[rclFacet._aulPoints[1]]._ulProp--;
    _aclPointArray[rclFacet._aulPoints[2]]._ulProp--;
  }

  // Alle Punkte die kein Facet mehr ref. ungueltig setzen
  _aclPointArray.ResetInvalid();
  for (MeshPointArray::_TIterator pP = _aclPointArray.begin(); pP != _aclPointArray.end(); pP++)
  {
    if (pP->_ulProp == 0)
      pP->SetInvalid();
  }

  RemoveInvalids(true, true);
  RecalcBoundBox();
}

void MeshKernel::DeletePoints (const std::vector<unsigned long> &raulPoints)
{
  _aclPointArray.ResetInvalid();
  for (std::vector<unsigned long>::const_iterator pI = raulPoints.begin(); pI != raulPoints.end(); pI++)
    _aclPointArray[*pI].SetInvalid();

  // Facets loeschen, wenn mind. 1 Eckpunkt Invalid ist
  _aclPointArray.SetProperty(0);
  for (MeshFacetArray::_TIterator pF = _aclFacetArray.begin(); pF != _aclFacetArray.end(); pF++)
  {
    MeshPoint &rclP0 = _aclPointArray[pF->_aulPoints[0]];
    MeshPoint &rclP1 = _aclPointArray[pF->_aulPoints[1]];
    MeshPoint &rclP2 = _aclPointArray[pF->_aulPoints[2]];

    if ((rclP0.IsValid() == false) || (rclP1.IsValid() == false) || (rclP2.IsValid() == false))
    {
      pF->SetInvalid();
    }
    else
    {
      pF->ResetInvalid();
      rclP0._ulProp++;
      rclP1._ulProp++;
      rclP2._ulProp++;
    }
  }
 
  // alle einzele Punkte (ohne Ref. zu Facet) loeschen
  for (MeshPointArray::_TIterator pP = _aclPointArray.begin(); pP != _aclPointArray.end(); pP++)
  {
    if (pP->_ulProp == 0)
      pP->SetInvalid();
  }

  RemoveInvalids(true, true);
  RecalcBoundBox();
}

void MeshKernel::CutFacets(const MeshFacetGrid& rclGrid, const Base::ViewProjMethod* pclProj, 
                           const Base::Polygon2D& rclPoly, bool bCutInner, std::vector<MeshGeomFacet> &raclFacets) 
{
  std::vector<unsigned long> aulFacets;

  MeshAlgorithm(*this).CheckFacets(rclGrid, pclProj, rclPoly, bCutInner, aulFacets );

  for (std::vector<unsigned long>::iterator i = aulFacets.begin(); i != aulFacets.end(); i++)
    raclFacets.push_back(GetFacet(*i));

  DeleteFacets(aulFacets);
}

void MeshKernel::CutFacets(const MeshFacetGrid& rclGrid, const Base::ViewProjMethod* pclProj,
                           const Base::Polygon2D& rclPoly, bool bInner, std::vector<unsigned long> &raclCutted)
{
  MeshAlgorithm(*this).CheckFacets(rclGrid, pclProj, rclPoly, bInner, raclCutted);
  DeleteFacets(raclCutted);
}

void MeshKernel::RecalcBoundBox (void)
{
  _clBoundBox.Flush();
  for (MeshPointArray::_TConstIterator pI = _aclPointArray.begin(); pI != _aclPointArray.end(); pI++)
    _clBoundBox &= *pI;
}

std::vector<Base::Vector3f> MeshKernel::CalcVertexNormals() const
{
  std::vector<Base::Vector3f> normals;

  normals.resize(CountPoints());

  // data structure to hold all faces belongs to one vertex
  unsigned long p1,p2,p3;

  // collecting all Facetes indices belonging to a vertex index
  unsigned int ct = CountFacets();
  for (unsigned int pFIter = 0;pFIter < ct; pFIter++)
  {
    GetFacetPoints(pFIter,p1,p2,p3);
    
    Base::Vector3f Norm = (GetPoint(p2)-GetPoint(p1) ) % (GetPoint(p3)-GetPoint(p1));

    normals[p1] += Norm;
    normals[p2] += Norm;
    normals[p3] += Norm;
  }

  return normals;
}

void MeshKernel::RebuildNeighbours (void)
{
  std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >   aclEdgeMap; // Map<Kante, Liste von Facets>

  // Kantenmap aufbauen
  unsigned long k = 0;
  for (MeshFacetArray::_TIterator pF = _aclFacetArray.begin(); pF != _aclFacetArray.end(); pF++, k++)
  {
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulT0 = pF->_aulPoints[i];
      unsigned long ulT1 = pF->_aulPoints[(i+1)%3];
      unsigned long ulP0 = std::min<unsigned long>(ulT0, ulT1);
      unsigned long ulP1 = std::max<unsigned long>(ulT0, ulT1);
      aclEdgeMap[std::make_pair(ulP0, ulP1)].push_front(k);
    }
  }

  // Nachbarn aufloesen
  for (std::map<std::pair<unsigned long, unsigned long>, std::list<unsigned long> >::iterator pI = aclEdgeMap.begin(); pI != aclEdgeMap.end(); pI++)
  {
    unsigned long ulP0 = pI->first.first;
    unsigned long ulP1 = pI->first.second;
    if (pI->second.size() == 1)  // kein Nachbar ==> Randfacet
    {
      unsigned long ulF0 = pI->second.front();
      unsigned short usSide =  _aclFacetArray[ulF0].Side(ulP0, ulP1);
      assert(usSide != USHRT_MAX);
      _aclFacetArray[ulF0]._aulNeighbours[usSide] = ULONG_MAX;
    }
    else if (pI->second.size() == 2)  // normales Facet mit Nachbar
    {
      unsigned long ulF0 = pI->second.front();
      unsigned long ulF1 = pI->second.back();
      unsigned short usSide =  _aclFacetArray[ulF0].Side(ulP0, ulP1);
      assert(usSide != USHRT_MAX);
      _aclFacetArray[ulF0]._aulNeighbours[usSide] = ulF1;
      usSide =  _aclFacetArray[ulF1].Side(ulP0, ulP1);
      assert(usSide != USHRT_MAX);
      _aclFacetArray[ulF1]._aulNeighbours[usSide] = ulF0;
    }
//    else
//      assert(false);
  }
}

// Evaluation
float MeshKernel::GetSurface() const
{
  float fSurface = 0.0;
  MeshFacetIterator cIter(*this);
  for (cIter.Init(); cIter.More(); cIter.Next())
  {
    fSurface += cIter->Area();
  }

  return fSurface;
}

float MeshKernel::GetSurface( const std::vector<unsigned long>& aSegment ) const
{
  float fSurface = 0.0;
  MeshFacetIterator cIter(*this);

  for ( std::vector<unsigned long>::const_iterator it = aSegment.begin(); it != aSegment.end(); ++it )
  {
    cIter.Set(*it);
    fSurface += cIter->Area();
  }

  return fSurface;
}

float MeshKernel::GetVolume() const
{
  MeshEvalSolid cSolid(*this);
  if ( !cSolid.Evaluate() )
    return 0.0f; // no solid

  float fVolume = 0.0;
  MeshFacetIterator cIter(*this);
  Base::Vector3f p1,p2,p3;
  for (cIter.Init(); cIter.More(); cIter.Next())
  {
    const MeshGeomFacet& rclF = *cIter;
    p1 = rclF._aclPoints[0];
    p2 = rclF._aclPoints[1];
    p3 = rclF._aclPoints[2];

    fVolume += (-p3.x*p2.y*p1.z + p2.x*p3.y*p1.z + p3.x*p1.y*p2.z - p1.x*p3.y*p2.z - p2.x*p1.y*p3.z + p1.x*p2.y*p3.z);
  }

  fVolume /= 6.0;
  fVolume = (float)fabs(fVolume);

  return fVolume;
}

bool MeshKernel::HasOpenEdges() const
{
  MeshEvalSolid eval(*this);
  return !eval.Evaluate();
}

bool MeshKernel::HasNonManifolds() const
{
  MeshEvalTopology eval(*this);
  return !eval.Evaluate();
}

bool MeshKernel::HasSelfIntersections() const
{
  MeshEvalSelfIntersection eval(*this);
  return !eval.Evaluate();
}

// Iterators
MeshFacetIterator MeshKernel::FacetIterator() const
{
  MeshFacetIterator it(*this);
  it.Begin(); return it;
}

MeshPointIterator MeshKernel::PointIterator() const
{
  MeshPointIterator it(*this);
  it.Begin(); return it;
}

void MeshKernel::GetEdges (std::vector<MeshGeomEdge>& edges) const
{
  std::set<MeshBuilder::Edge> tmp;

  for (MeshFacetArray::_TConstIterator it = _aclFacetArray.begin(); it != _aclFacetArray.end(); it++)
  {
    for (int i = 0; i < 3; i++)
    {
      tmp.insert(MeshBuilder::Edge(it->_aulPoints[i], it->_aulPoints[(i+1)%3], it->_aulNeighbours[i]));
    }
  }

  edges.reserve(tmp.size());
  for (std::set<MeshBuilder::Edge>::iterator it2 = tmp.begin(); it2 != tmp.end(); it2++)
  {
    MeshGeomEdge edge;
    edge._aclPoints[0] = this->_aclPointArray[it2->pt1];
    edge._aclPoints[1] = this->_aclPointArray[it2->pt2];
    edge._bBorder = it2->facetIdx == ULONG_MAX;

    edges.push_back(edge);
  }
}

unsigned long MeshKernel::CountEdges (void) const
{
  unsigned long openEdges = 0, closedEdges = 0;

  for (MeshFacetArray::_TConstIterator it = _aclFacetArray.begin(); it != _aclFacetArray.end(); it++)
  {
    for (int i = 0; i < 3; i++)
    {
      if (it->_aulNeighbours[i] == ULONG_MAX)
        openEdges++;
      else
        closedEdges++;
    }
  }

  return (openEdges + (closedEdges / 2));
}

