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
#include "Elements.h"

using namespace Mesh;

/*-------------------------------------------------------------------
DES: sucht den zugehoerigen Index des Punkts. Zwei Punkte sind gleich,
  wenn sich der Abstand den minimalen Wert (s. definitions.hxx) 
  unterschreitet
PAR:  rclPoint:  Punkt
RET:  unsigned long:    Index, ULONG_MAX wenn kein Index gefunden
--------------------------------------------------------------------*/
unsigned long MeshPointArray::Get (const MeshPoint &rclPoint)
{
  iterator clIter;

  clIter = std::find(begin(), end(), rclPoint);
  if (clIter != end())
    return clIter - begin();
  else
    return ULONG_MAX;  
}

/*-------------------------------------------------------------------
DES: gibt den Index des Punktes zurueck, bzw. fuegt einer neuer Punkt
  an und gibt dessen Index an. Zwei Punkte sind gleich wenn der Abstand
  den minimalen Abstand (s. definitions.hxx) unterschreitet
PAR:  rclPoint:  Punkt
RET:  unsigned long:    Index
--------------------------------------------------------------------*/
unsigned long MeshPointArray::GetOrAddIndex (const MeshPoint &rclPoint)
{
  unsigned long ulIndex;

  if ((ulIndex = Get(rclPoint)) == ULONG_MAX)
  {
    push_back(rclPoint);
    return (unsigned long)(size() - 1);
  }
  else
    return ulIndex;
}

/*-------------------------------------------------------------------
DES: Passt die Facet-Indizies eines entfernten Facet der Kanten an.
  D.h. alle nachfolgende Indizies werden um 1 dekrementiert.
PAR: ulIndex:  Facet-Index der entfernt wurde
RET: void
--------------------------------------------------------------------*/
void MeshEdgeArray::AdjustIndex (unsigned long ulIndex)
{
  _TIterator  pIter, pEnd;
  unsigned long      k;

  pIter = std::upper_bound(begin(), end(), MeshEdge(ulIndex, 0));
  pEnd = end();
  while (pIter < pEnd)
  {
    k = pIter->_ulIndex;
    pIter->_ulIndex = (((k >> 2) - 1) << 2) | (k & 3);
    pIter++;  
  }
}

/*-------------------------------------------------------------------
DES: loescht das Facet an der Pos. pIter aus dem Array. Die Nachbar-
  Indizies der anderen Facets werden entspr. angepasst.
PAR: pIter:  zu loeschendes Facet
RET: void
--------------------------------------------------------------------*/
void MeshFacetArray::Erase (_TIterator pIter)
{
  unsigned long i, *pulN;
  _TIterator  pPass, pEnd;
  unsigned long ulInd = pIter - begin();
  erase(pIter);
  pPass = begin();
  pEnd  = end();
  while (pPass < pEnd)
  {
    for (i = 0; i < 3; i++)
    {
      pulN = &pPass->_aulNeighbours[i];
      if ((*pulN > ulInd) && (*pulN != ULONG_MAX))
        (*pulN)--;
    }
    pPass++;
  }
}

/*-------------------------------------------------------------------
DES: prueft und vertauscht evt. die Punkt-Indizies der Facets
PAR: ulOrig: zu korrigierender Punkt-Index
     ulNew:  neuer Punkt-Index
RET: void
--------------------------------------------------------------------*/
void MeshFacetArray::TransposeIndicies (unsigned long ulOrig, unsigned long ulNew)
{
  _TIterator  pIter = begin(), pEnd = end();

  while (pIter < pEnd)
  {
    pIter->Transpose(ulOrig, ulNew);
    ++pIter;  
  }
}

/*-------------------------------------------------------------------
DES: dekrementiert alle Punkt-Indizies die groesser ulIndex sind
PAR: ulIndex:  Index
RET: void
--------------------------------------------------------------------*/
void MeshFacetArray::DecrementIndicies (unsigned long ulIndex)
{
  _TIterator  pIter = begin(), pEnd = end();

  while (pIter < pEnd)
  {
    pIter->Decrement(ulIndex);
    ++pIter;  
  }
}


/*-------------------------------------------------------------------
DES: Ueberprueft, ob der Punkt dem Facet zugeordnet werden kann. Ein
  Punkt gehoert einem Facet wenn der Abstand zur Dreiecksflaeche kleiner
  fDistance ist und wenn sich der Punkt innerhalb des Dreiecks befindet.
PAR: rclPoint:  Punkt
     fDistance: max. Abstand zur Dreiecksflaeche
RET: bool: TRUE, wenn Punkt zum Dreieck gehoert
--------------------------------------------------------------------*/
bool MeshGeomFacet::IsPointOf (const Vector3D &rclPoint, float fDistance) const
{
  if (Distance(rclPoint) > fDistance)
    return FALSE;

  Vector3D clNorm(_clNormal), clProjPt(rclPoint), clEdge;
  Vector3D clP0(_aclPoints[0]), clP1(_aclPoints[1]), clP2(_aclPoints[2]);
  float     fLP, fLE;

  clNorm.Normalize();
  clProjPt.ProjToPlane(_aclPoints[0], clNorm);

    
  // Kante P0 --> P1
  clEdge = clP1 - clP0;
  fLP = clProjPt.DistanceToLine(clP0, clEdge); 
  if (fLP > 0.0f)
  {
    fLE = clP2.DistanceToLine(clP0, clEdge);
    if (fLP <= fLE)
    {
      if (clProjPt.DistanceToLine(clP2, clEdge) > fLE) 
        return FALSE;
    }
    else
      return FALSE;        
  }      

  // Kante P0 --> P2
  clEdge = clP2 - clP0;
  fLP = clProjPt.DistanceToLine(clP0, clEdge); 
  if (fLP > 0.0f)
  {
    fLE = clP1.DistanceToLine(clP0, clEdge);
    if (fLP <= fLE)
    {
      if (clProjPt.DistanceToLine(clP1, clEdge) > fLE) 
        return FALSE;
    }
    else
      return FALSE;        
  }      

  // Kante P1 --> P2
  clEdge = clP2 - clP1;
  fLP = clProjPt.DistanceToLine(clP1, clEdge); 
  if (fLP > 0.0f)
  {
    fLE = clP0.DistanceToLine(clP1, clEdge);
    if (fLP <= fLE)
    {
      if (clProjPt.DistanceToLine(clP0, clEdge) > fLE) 
        return FALSE;
    }
    else
      return FALSE;        
  }      

  return TRUE;
}

/*-------------------------------------------------------------------
DES: Ausdehnen des Facets
PAR: fDist: neuer Kanten-Abstand
RET: void:
--------------------------------------------------------------------*/
void MeshGeomFacet::Enlarge (float fDist)
{
  Vector3D  clM, clU, clV, clPNew[3];
  float      fA, fD;
  unsigned long     i, ulP1, ulP2, ulP3;

  for (i = 0; i < 3; i++)
  {
    ulP1  = i;
    ulP2  = (i + 1) % 3;
    ulP3  = (i + 2) % 3;
    clU   = _aclPoints[ulP2] - _aclPoints[ulP1];
    clV   = _aclPoints[ulP3] - _aclPoints[ulP1];
    clM   = -(clU + clV);
    fA    = clM.GetAngle(-clU);
    fD    = fDist / float(sin(fA));
    clM.Normalize();
    clM.Scale(fD, fD, fD);
    clPNew[ulP1] = _aclPoints[ulP1] + clM;
  }

  _aclPoints[0] = clPNew[0];
  _aclPoints[1] = clPNew[1];
  _aclPoints[2] = clPNew[2];
}



// globales Setzen eines Flags für alle Facets
void MeshFacetArray::SetFlag (MeshFacet::TFlagType tF)
{
  for (MeshFacetArray::_TIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

// Zurücksetzen eines Flags für alle Facets
void MeshFacetArray::ResetFlag (MeshFacet::TFlagType tF)
{
  for (MeshFacetArray::_TIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}

// setzen eines Flags für alle Kanten
void MeshEdgeArray::SetFlag (MeshEdge::TFlagType tF)
{
  for (MeshEdgeArray::_TIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

// rücksetzen eines Flags für alle Kanten
void MeshEdgeArray::ResetFlag (MeshEdge::TFlagType tF)
{
  for (MeshEdgeArray::_TIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}

// setzen eines Flags für alle Punkte
void MeshPointArray::SetFlag (MeshPoint::TFlagType tF)
{
  for (MeshPointArray::_TIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

// rücksetzen eines Flags für alle Punkte
void MeshPointArray::ResetFlag (MeshPoint::TFlagType tF)
{
  for (MeshPointArray::_TIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}


void MeshPointArray::SetProperty (unsigned long ulVal)
{
  for (_TIterator pP = begin(); pP != end(); pP++) pP->_ulProp = ulVal;
}

void MeshPointArray::ResetInvalid (void)
{
  for (_TIterator pP = begin(); pP != end(); pP++) pP->ResetInvalid();
}

void MeshFacetArray::SetProperty (unsigned long ulVal)
{
  for (_TIterator pF = begin(); pF != end(); pF++) pF->_ulProp = ulVal;
}

void MeshFacetArray::ResetInvalid (void)
{
  for (_TIterator pF = begin(); pF != end(); pF++) pF->ResetInvalid();
}

MeshPointArray& MeshPointArray::operator = (const MeshPointArray &rclPAry)
{
//  std::vector<MeshPoint>::operator=(rclPAry);
  TMeshPointArray::operator=(rclPAry);

  return *this;
}

MeshEdgeArray& MeshEdgeArray::operator = (const MeshEdgeArray &rclEAry)
{
  TMeshEdgeArray::operator=(rclEAry);
  return *this;
}

MeshFacetArray& MeshFacetArray::operator = (const MeshFacetArray &rclFAry)
{
  TMeshFacetArray::operator=(rclFAry);
  return *this;
}

