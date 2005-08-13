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
# include <map>
# include <queue>
#endif

#include <Base/Sequencer.h>

#include "Helpers.h"
#include "MeshKernel.h"
#include "Iterator.h"

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
#ifdef Use_EdgeList
  _aclEdgeArray   = rclMesh._aclEdgeArray;
#endif
  _aclFacetArray  = rclMesh._aclFacetArray;
  _clBoundBox     = rclMesh._clBoundBox;
  _bValid         = rclMesh._bValid;
  return *this;
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

#ifdef Use_EdgeList
  // Kante(n) einfuegen
  AddEdge(clFacet, (unsigned long)(_aclFacetArray.size() - 1));
#endif

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

#ifdef Use_EdgeList
void MeshKernel::AddEdge (MeshFacet &rclFacet, unsigned long ulFacetIndex)
{
  MeshHelpEdge clFacetEdge;
  unsigned long i, j, k, ulCt;
  unsigned short usSide;

  ulCt = _aclEdgeArray.size();

  for (i = 0; i < 3; i++)
  {
    // Kante des neuen Facets
    rclFacet.GetEdge((unsigned short)(i), clFacetEdge);

    // suche gleiche Kante
    j = FindEdge(clFacetEdge);

    if (j != ULONG_MAX)
    {  // Kante gefunden, Nachbar-Indizies setzen
      usSide = (unsigned short)(_aclEdgeArray[j].Side());
      k = _aclEdgeArray[j].Index();  
      _aclFacetArray[k]._aulNeighbours[usSide] = ulFacetIndex;
    }
    else
    {  // keine Kanten gefunden, neue einfuegen
      _aclEdgeArray.Add(ulFacetIndex, i);
      // kein Nachbar-Index des neuen Facets
      rclFacet._aulNeighbours[i] = ULONG_MAX;
    }
  }
}
#endif

void MeshKernel::Clear (void)
{
  _aclPointArray.clear();
#ifdef Use_EdgeList
  _aclEdgeArray.clear();
#endif
  _aclFacetArray.clear();

  // auch Speicher freigeben
  MeshPointArray().swap(_aclPointArray);
#ifdef Use_EdgeList
  MeshEdgeArray().swap(_aclEdgeArray);
#endif
  MeshFacetArray().swap(_aclFacetArray);

  _clBoundBox.Flush();
}

MeshKernel& MeshKernel::operator = (std::vector<MeshGeomFacet> &rclVAry)
{
  // lineare Liste von Eckpunkten und Rueckwaerts-Indizierung zum Facet erzeugen
  MeshPointBuilder  clMap;
  clMap.resize(rclVAry.size() * 3);

  // Sequencer starten
  Base::Sequencer().start("create mesh structure...", rclVAry.size() * 8 + 1);

  unsigned long k = 0;
  unsigned long i = 0;
  for (std::vector<MeshGeomFacet>::iterator pI = rclVAry.begin(); pI != rclVAry.end(); pI++, i++)
  {
    Base::Sequencer().next();

    pI->AdjustCirculationDirection();  // Umlaufrichtung an Normale anpassen
    for (unsigned long j = 0; j < 3; j++)
      clMap[k++].Set(j, i, pI->_aclPoints[j]);
  }

  Assign(clMap);

  Base::Sequencer().stop();

  return *this;
}

void MeshKernel::Assign (MeshPointBuilder &rclMap)
{
  unsigned long ulCtFacets, ulCtEdges, ulCtPoints, ulSteps; 

  Clear();  // Alle Arrays zuruecksetzen

  ulCtFacets = rclMap.size() / 3;  // Anzahl Facets
  ulCtPoints = ulCtFacets / 2;      // Anzahl Punkt geschaetzt
  ulCtEdges  = ulCtFacets * 3 / 2;  // Anzahl Edges geschaetzt

  ulSteps = ulCtFacets;

  // lineare Liste von Eckpunkten und Rueckwaerts-Indizierung zum Facet erzeugen
  { ////////////////////////////////////////////////////////////////////////////
    // copy normals
    std::sort(rclMap.begin(), rclMap.end());

    _aclFacetArray.resize(ulCtFacets);  // Facet-Array allozieren

    // Punktliste aufbauen, mehrfache Punkte entfernen
    unsigned long ulPtIndex = 0;

    _aclPointArray.reserve((unsigned long)(float(ulCtPoints) * 1.05f));  // Schaetzwert + 5%

    for (std::vector<MeshHelpPoint>::iterator pM = rclMap.begin(); pM != rclMap.end();)
    {
      Base::Sequencer().next();
      MeshPoint &rclPt = pM->_clPt;
      _aclPointArray.push_back(rclPt);
      _clBoundBox &= rclPt;
      _aclFacetArray[pM->Index()]._aulPoints[pM->Corner()] = ulPtIndex;

      // gleiche aufeinanderfoldgende Punkte ueberlesen und Index der Facets setzen
      std::vector<MeshHelpPoint>::iterator pM2;
      for ( pM2 = pM+1; (pM2 < rclMap.end()) && (pM2->_clPt == rclPt); pM2++ )
        _aclFacetArray[pM2->Index()]._aulPoints[pM2->Corner()] = ulPtIndex;

      pM = pM2;
      ulPtIndex++;
    }

    ulSteps += rclMap.size();

    // Help-Map loeschen
    rclMap.clear();
    MeshPointBuilder().swap(rclMap);  // kleiner Trick um Speicher freizugeben
  }

  // sortierte Kantenliste generieren
  {  ////////////////////////////////////////////////////////////////////////////////////
    MeshEdgeBuilder  clEdger;

    unsigned long ulCt = _aclFacetArray.size();
    clEdger.resize(ulCt * 3);
    unsigned long i = 0;
    std::vector<MeshHelpBuilderEdge>::iterator pE = clEdger.begin();
    for (std::vector<MeshFacet>::iterator pF = _aclFacetArray.begin(); pF != _aclFacetArray.end(); pF++, i++)
    {
      Base::Sequencer().next();

      (pE++)->Set(pF->_aulPoints[0], pF->_aulPoints[1], 0, i);        
      (pE++)->Set(pF->_aulPoints[1], pF->_aulPoints[2], 1, i);        
      (pE++)->Set(pF->_aulPoints[2], pF->_aulPoints[0], 2, i);
    }

    std::sort(clEdger.begin(), clEdger.end());
  

#ifdef Use_EdgeList
    _aclEdgeArray.reserve((unsigned long)(float(ulCtEdges) * 1.05f));  // Schaetztwert + 5%
#endif

    bool bET = true;
    MeshEdge clEdge;

    // Kantenliste aufbauen, doppelte Kante vermeiden, Nachbarschaften setzen
    ulCt = clEdger.size();
    for (pE = clEdger.begin(); pE != clEdger.end();)
    {
      Base::Sequencer().next();

      unsigned long ulInd  = pE->Index();
      unsigned long ulSide = pE->Side();
      clEdge.Set(ulInd, ulSide);

#ifdef Use_EdgeList
      _aclEdgeArray.push_back(clEdge);
#endif

      std::vector<MeshHelpBuilderEdge>::iterator pE2;
      for ( pE2 = pE+1; (pE2 != clEdger.end()) && (*pE2 == *pE); pE2++ )
      {
        unsigned long k = pE2->Index();
        unsigned long l = pE2->Side();
        _aclFacetArray[ulInd]._aulNeighbours[ulSide] = k;
        _aclFacetArray[k]._aulNeighbours[l] = ulInd;
      } 
    
      bET = bET && ((pE2-pE) < 3);  // Erkennnung: wenn mehr als 2 Facets eine Kante ref.

      pE = pE2;
    }

    _bValid = bET;  
  }

#ifdef Use_EdgeList
  std::sort(_aclEdgeArray.begin(), _aclEdgeArray.end());  // Kantenliste sortieren
#endif

  ulCtFacets = _aclFacetArray.size();
  ulCtPoints = _aclPointArray.size();
#ifdef Use_EdgeList
  ulCtEdges  = _aclEdgeArray.size();
#endif

  _aclFacetArray.resize(ulCtFacets);
  _aclPointArray.resize(ulCtPoints);
#ifdef Use_EdgeList
  _aclEdgeArray.resize(ulCtEdges);
#endif

}

bool MeshKernel::DeleteFacet (const MeshFacetIterator &rclIter)
{
  unsigned long i, j, ulNFacet, ulInd, ulEdge;

  if (rclIter._clIter >= _aclFacetArray.end())
    return false;

  // Index des zu loeschenden Facet
  ulInd = rclIter._clIter - _aclFacetArray.begin(); 

  // Nachbar-Indizies der Nachbar-Facets auf dieses Facet als ungueltigt setzen
  // gleichzeitig Kanten loeschen oder umbiegen auf Nachbarfacet
  for (i = 0; i < 3; i++)
  {
#ifdef Use_EdgeList
    ulEdge   = _aclEdgeArray.Find(ulInd, i);        // Index: zugehoerige Kante
#endif
    ulNFacet = rclIter._clIter->_aulNeighbours[i];  // Index: Nachbar-Facet
    if (ulNFacet != ULONG_MAX)
    {
      for (j = 0; j < 3; j++)
      {
        if (_aclFacetArray[ulNFacet]._aulNeighbours[j] == ulInd)
        {
          _aclFacetArray[ulNFacet]._aulNeighbours[j] = ULONG_MAX;
#        ifdef Use_EdgeList
           if (ulEdge != ULONG_MAX)
           {  // Kante auf Nachbar-Facet umbiegen
             _aclEdgeArray.erase(_aclEdgeArray.begin() + ulEdge);
             _aclEdgeArray.Add(ulNFacet, j);      
           }
#        endif
         break;
        }
      }
    }
#ifdef Use_EdgeList
    else if (ulEdge != ULONG_MAX)  // Kante entgueltig loeschen da keine Nachbar-Facets
      _aclEdgeArray.erase(_aclEdgeArray.begin() + ulEdge);
#endif
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

#ifdef Use_EdgeList
  // Indizies des Kanten-Arrays zu Facets abstimmen
  _aclEdgeArray.AdjustIndex(ulInd);
#endif

  return true;
}

#ifdef Use_EdgeList
bool MeshKernel::DeleteEdge (const MeshEdgeIterator &rclIter)
{
  MeshFacetIterator  clFIter1(*this), clFIter2(*this);
  MeshFacet clFacet;
  unsigned long ulIndex, ulSide;   

  ulIndex = rclIter._clIter->Index();
  ulSide  = rclIter._clIter->Side();
  clFacet = _aclFacetArray[ulIndex];
  if (clFacet._aulNeighbours[ulSide] == ULONG_MAX)
  {  // kein Nachbar-Facet
    clFIter1.Set(ulIndex);
    DeleteFacet(clFIter1);
  }
  else
  {  // mit Nachbar-Facet
    clFIter1.Set(ulIndex);
    ulIndex = clFIter1._clIter->_aulNeighbours[ulSide];
    clFIter2.Set(ulIndex);
    if (clFIter1 < clFIter2)  // Facet mit hoeheren Index zuerst loeschen
    {
      DeleteFacet(clFIter2);
      DeleteFacet(clFIter1);
    }
    else
    {
      DeleteFacet(clFIter1);
      DeleteFacet(clFIter2);
    }
  }
 
  return true;
}
#endif

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

#ifdef Use_EdgeList
void MeshKernel::CheckAndCorrectEdge (unsigned long ulFacetIndex)
{
  unsigned long i, ulInd, ulSide;
  MeshFacet *pclFacet;

  for (i = 0; i < 3; i++)
  {
    ulInd = _aclEdgeArray.Find(ulFacetIndex, i);
    if (ulInd != ULONG_MAX)
    {
      pclFacet = &_aclFacetArray[ulFacetIndex];
      _aclEdgeArray.erase(_aclEdgeArray.begin() + ulInd);
      if (pclFacet->_aulNeighbours[i] != ULONG_MAX)
      {
        ulSide = _aclFacetArray[pclFacet->_aulNeighbours[i]].Side(ulFacetIndex);
        _aclEdgeArray.Add(pclFacet->_aulNeighbours[i], ulSide);
      }
    }
  }
}
#endif

void MeshKernel::RemoveInvalids (bool bWithEdgeCorrect, bool bWithEdgeDelete)
{
  std::vector<unsigned long> aulDecrements;
  std::vector<unsigned long>::iterator       pDIter;
  unsigned long ulDec, i, j, k, ulSteps;
  MeshPointArray::_TIterator pPIter, pPEnd;
  MeshEdgeArray::_TIterator  pEIter, pEEnd;
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

#ifdef Use_EdgeList
  // Kanten loeschen
  pEEnd  = _aclEdgeArray.end();
  unsigned long ulValidEdges = 0;
  if (bWithEdgeDelete == true)
  {
    for (pEIter = _aclEdgeArray.begin(); pEIter != pEEnd; pEIter++)
    {
      i = pEIter->Index();
      k = pEIter->Side();  
      if (_aclFacetArray[i].IsValid() == false)  // Facet valid ?
      { // nein, Nachbar-Facet valid ?
        j = _aclFacetArray[i]._aulNeighbours[k];
        if (j != ULONG_MAX) // hat Nachbar
        {
          if (_aclFacetArray[j].IsValid() == true) // Nachbar-Facet geloescht ?
          {  // Kante auf Nachbar-Facet indizieren
            k = _aclFacetArray[j].Side(i);
            pEIter->Set(j, k);
            ulValidEdges++;
          }
          else 
            pEIter->SetInvalid();
        }
        else
          pEIter->SetInvalid();
      }
      else
        ulValidEdges++;
    }    

    // Kanten loeschen
    MeshEdgeArray  aclEdgeTemp(ulValidEdges);  
    MeshEdgeArray::_TIterator pETemp = aclEdgeTemp.begin();
    pEEnd  = _aclEdgeArray.end();
    for (pEIter = _aclEdgeArray.begin(); pEIter != pEEnd; pEIter++)
    {
      if (pEIter->IsValid() == true)
        *pETemp++ = *pEIter;
    }
    _aclEdgeArray = aclEdgeTemp;
    aclEdgeTemp.clear();
  }
#endif

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

#ifdef Use_EdgeList
  // Indizies der Kanten-Liste korrigieren
  if (bWithEdgeCorrect == true)
  {
    pEEnd  = _aclEdgeArray.end();
    for (pEIter = _aclEdgeArray.begin(); pEIter != pEEnd; pEIter++)
    {
      i = pEIter->Index();
      k = pEIter->_ulIndex;  
      pEIter->_ulIndex = ((i - aulDecrements[i]) << 2) | (k & 3);
    }
    std::sort(_aclEdgeArray.begin(), _aclEdgeArray.end());
  }
#endif

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

void MeshKernel::SaveStream (DataStream &rclOut)
{
  MeshPointArray::_TIterator  clPIter = _aclPointArray.begin(), clPEIter = _aclPointArray.end();
#ifdef Use_EdgeList
  MeshEdgeArray::_TIterator   clEIter = _aclEdgeArray.begin(),  clEEIter = _aclEdgeArray.end();
#endif
  MeshFacetArray::_TIterator  clFIter = _aclFacetArray.begin(), clFEIter = _aclFacetArray.end();

  rclOut << CountPoints() 
#ifdef Use_EdgeList
         << CountEdges() 
#endif
         << CountFacets();
  while (clPIter < clPEIter) rclOut << *(clPIter++);
#ifdef Use_EdgeList
  while (clEIter < clEEIter) rclOut << *(clEIter++);
#endif
  while (clFIter < clFEIter) rclOut << *(clFIter++);

  rclOut << _clBoundBox.MinX << _clBoundBox.MinY << _clBoundBox.MinZ <<
            _clBoundBox.MaxX << _clBoundBox.MaxY << _clBoundBox.MaxZ;
}

void MeshKernel::RestoreStream (DataStream &rclIn)
{
  unsigned long  ulCtPt, ulCtEd, ulCtFc;

  Clear();
  rclIn >> ulCtPt >> ulCtEd >> ulCtFc; 
  _aclPointArray.resize(ulCtPt);  
#ifdef Use_EdgeList
  _aclEdgeArray.resize(ulCtEd);
#endif
  _aclFacetArray.resize(ulCtFc);

  MeshPointArray::_TIterator  clPIter = _aclPointArray.begin(), clPEIter = _aclPointArray.end();
#ifdef Use_EdgeList
  MeshEdgeArray::_TIterator   clEIter = _aclEdgeArray.begin(),  clEEIter = _aclEdgeArray.end();
#endif
  MeshFacetArray::_TIterator  clFIter = _aclFacetArray.begin(), clFEIter = _aclFacetArray.end();

  while (clPIter < clPEIter) rclIn >> *(clPIter++); 
#ifdef Use_EdgeList
  while (clEIter < clEEIter) rclIn >> *(clEIter++);
#endif
  while (clFIter < clFEIter) rclIn >> *(clFIter++);

  rclIn >> _clBoundBox.MinX >> _clBoundBox.MinY >> _clBoundBox.MinZ >>
           _clBoundBox.MaxX >> _clBoundBox.MaxY >> _clBoundBox.MaxZ;
}

void MeshKernel::operator *= (const Matrix4D &rclMat)
{
  MeshPointArray::_TIterator  clPIter = _aclPointArray.begin(), clPEIter = _aclPointArray.end();
  Matrix4D                    clMatrix(rclMat);
  
  _clBoundBox.Flush();
  while (clPIter < clPEIter)
  {
    *clPIter *= clMatrix;
    _clBoundBox &= *clPIter;
    clPIter++;
  }
}

#ifdef Use_EdgeList
void MeshKernel::RebuildEdgeArray (void)
{
  unsigned long ulCt, i, j, k;
  MeshEdgeBuilder clEdger;

  // ueberkomplette Kantenliste generieren
  ulCt = _aclFacetArray.size();
  clEdger.resize(ulCt * 3);
  k = 0;
  for (i = 0; i < ulCt; i++)
  {
    for (j = 0; j < 3; j++)
    {
      clEdger[k++].Set(_aclFacetArray[i]._aulPoints[j],
                       _aclFacetArray[i]._aulPoints[(j+1)%3], j, i);
    } 
  }

  // Kantenliste sortieren und doppelte Kante verwerfen
  std::sort(clEdger.begin(), clEdger.end());
  clEdger.erase(std::unique(clEdger.begin(), clEdger.end()), clEdger.end());

  // Kanten uebertragen
  _aclEdgeArray.clear();
  for (MeshEdgeBuilder::_TIterator pI = clEdger.begin(); pI != clEdger.end(); pI++)
    _aclEdgeArray.push_back(MeshEdge(pI->Index(), pI->Side()));
  std::sort(_aclEdgeArray.begin(), _aclEdgeArray.end());
}
#endif

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

#ifdef Use_EdgeList
MeshEdgeArray::_TConstIterator MeshKernel::FindEdge (unsigned long ulFacet, unsigned short usSide) const 
{
  unsigned long ulEdge = (ulFacet << 2) + (unsigned long)(usSide);

  MeshEdgeArray::_TConstIterator pBegin, pEnd, pPIter;
  pBegin = _aclEdgeArray.begin();
  pEnd   = _aclEdgeArray.end();

  pPIter = std::lower_bound<MeshEdgeArray::_TConstIterator,MeshEdge> (pBegin, pEnd, MeshEdge(ulEdge));

  if (pPIter == _aclEdgeArray.end())
  {
    unsigned long k = _aclFacetArray[ulFacet]._aulNeighbours[usSide];
    if (k != ULONG_MAX)
    {
      ulEdge = (k << 2) + (unsigned long)(_aclFacetArray[k].Side(ulFacet));
      return std::lower_bound<MeshEdgeArray::_TConstIterator,MeshEdge>(pBegin, pEnd, MeshEdge(ulEdge));
    }
  }
  else
    return pPIter;

  return _aclEdgeArray.end();
}
#endif

void MeshKernel::RecalcBoundBox (void)
{
  _clBoundBox.Flush();
  for (MeshPointArray::_TConstIterator pI = _aclPointArray.begin(); pI != _aclPointArray.end(); pI++)
    _clBoundBox &= *pI;
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
