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

using namespace MeshCore;

unsigned long MeshPointArray::Get (const MeshPoint &rclPoint)
{
  iterator clIter;

  clIter = std::find(begin(), end(), rclPoint);
  if (clIter != end())
    return clIter - begin();
  else
    return ULONG_MAX;  
}

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

void MeshPointArray::SetFlag (MeshPoint::TFlagType tF)
{
  for (MeshPointArray::_TIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

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

MeshPointArray& MeshPointArray::operator = (const MeshPointArray &rclPAry)
{
//  std::vector<MeshPoint>::operator=(rclPAry);
  TMeshPointArray::operator=(rclPAry);

  return *this;
}

// -----------------------------------------------------------------

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

void MeshEdgeArray::SetFlag (MeshEdge::TFlagType tF)
{
  for (MeshEdgeArray::_TIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

void MeshEdgeArray::ResetFlag (MeshEdge::TFlagType tF)
{
  for (MeshEdgeArray::_TIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}

MeshEdgeArray& MeshEdgeArray::operator = (const MeshEdgeArray &rclEAry)
{
  TMeshEdgeArray::operator=(rclEAry);
  return *this;
}

// -----------------------------------------------------------------

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

void MeshFacetArray::TransposeIndicies (unsigned long ulOrig, unsigned long ulNew)
{
  _TIterator  pIter = begin(), pEnd = end();

  while (pIter < pEnd)
  {
    pIter->Transpose(ulOrig, ulNew);
    ++pIter;  
  }
}

void MeshFacetArray::DecrementIndicies (unsigned long ulIndex)
{
  _TIterator  pIter = begin(), pEnd = end();

  while (pIter < pEnd)
  {
    pIter->Decrement(ulIndex);
    ++pIter;  
  }
}

void MeshFacetArray::SetFlag (MeshFacet::TFlagType tF)
{
  for (MeshFacetArray::_TIterator i = begin(); i < end(); i++) i->SetFlag(tF);
}

void MeshFacetArray::ResetFlag (MeshFacet::TFlagType tF)
{
  for (MeshFacetArray::_TIterator i = begin(); i < end(); i++) i->ResetFlag(tF);
}

void MeshFacetArray::SetProperty (unsigned long ulVal)
{
  for (_TIterator pF = begin(); pF != end(); pF++) pF->_ulProp = ulVal;
}

void MeshFacetArray::ResetInvalid (void)
{
  for (_TIterator pF = begin(); pF != end(); pF++) pF->ResetInvalid();
}

MeshFacetArray& MeshFacetArray::operator = (const MeshFacetArray &rclFAry)
{
  TMeshFacetArray::operator=(rclFAry);
  return *this;
}

// -----------------------------------------------------------------

MeshGeomFacet::MeshGeomFacet (void) 
  :_bNormalCalculated(false) 
{ 

}


MeshGeomFacet::MeshGeomFacet (const Vector3D &v1,const Vector3D &v2,const Vector3D &v3)
  : _bNormalCalculated(false) 
{
  _aclPoints[0] = v1;
  _aclPoints[1] = v2;
  _aclPoints[2] = v3;
}



bool MeshGeomFacet::IsPointOf (const Vector3D &rclPoint, float fDistance) const
{
  if (Distance(rclPoint) > fDistance)
    return false;

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
        return false;
    }
    else
      return false;
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
        return false;
    }
    else
      return false;
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
        return false;
    }
    else
      return false;
  }      

  return true;
}

void MeshGeomFacet::Enlarge (float fDist)
{
  Vector3D  clM, clU, clV, clPNew[3];
  float      fA, fD;
  unsigned long i, ulP1, ulP2, ulP3;

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
