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
# include <gp_Pln.hxx>
# include <BRepMesh_Discret.hxx>
# include <BRepMesh_Triangle.hxx>
# include <BRepMesh_Edge.hxx>
# include <TopoDS_Shape.hxx>
#endif

#include <Base/Sequencer.h>

#include "MeshAlgorithm.h"
#include "Iterator.h"
#include "Algorithm.h"
#include "Grid.h"
//#include "Info.h"

using namespace Mesh;

bool MeshAlgorithm::NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, Vector3D &rclRes, 
                                       unsigned long &rulFacet) const
{
  Vector3D clProj, clRes;
  bool bSol = false;
  unsigned long ulInd = 0;

  // langsame Ausfuehrung ohne Grid
  MeshFacetIterator  clFIter(_rclMesh);
  for (clFIter.Init(); clFIter.More(); clFIter.Next())
  {
    if (MeshFacetFunc::Forminate(*clFIter, rclPt, rclDir, clRes) == true)
    {
      if (bSol == false) // erste Loesung
      {
        bSol   = true;
        clProj = clRes;
        ulInd  = clFIter.Position();
      }
      else
      {  // liegt Punkt naeher
        if ((clRes - rclPt).Length() < (clProj - rclPt).Length())
        {
          clProj = clRes;
          ulInd  = clFIter.Position();
        }
      }
    }
  }

  rclRes   = clProj;
  rulFacet = ulInd;
  return bSol;
}

bool MeshAlgorithm::NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, MeshFacetGrid &rclGrid, 
                                       Vector3D &rclRes, unsigned long &rulFacet) const
{
  std::vector<unsigned long> aulFacets;
  MeshGridIterator clGridIter(rclGrid);

  if (clGridIter.InitOnRay(rclPt, rclDir, aulFacets) == true)
  {
    if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet) == false)
    {
      aulFacets.clear();
      while (clGridIter.NextOnRay(aulFacets) == true)
      {
        if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet) == true)
          return true;
      }
    }
    else
      return true;
  } 

  return false;
}

bool MeshAlgorithm::NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, float fMaxSearchArea, 
                                       MeshFacetGrid &rclGrid, Vector3D &rclRes, unsigned long &rulFacet) const
{
  std::vector<unsigned long> aulFacets;
  MeshGridIterator  clGridIter(rclGrid);

  Vector3D clStartPoint = rclPt - fMaxSearchArea * rclDir;

  if (clGridIter.InitOnRay(clStartPoint, rclDir, fMaxSearchArea, aulFacets) == true)
  {
    if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet, 1.75f) == false)
    {
      aulFacets.clear();
      while (clGridIter.NextOnRay(aulFacets) == true)
      {
        if (RayNearestField(rclPt, rclDir, aulFacets, rclRes, rulFacet, 1.75f) == true)
          return true;
      }
    }
    else
      return true;
  } 

  return false;
}

/// @todo
bool MeshAlgorithm::FindFacetsOnRay (const Vector3D &rcBegin, const Vector3D &rcEnd, const MeshFacetGrid &rclGrid, 
                                     std::vector<unsigned long>&raulFacets) const
{
  std::vector<unsigned long> aulFacets;
  std::vector<unsigned long> tmp;
  MeshGridIterator  clGridIter(rclGrid);

  Vector3D cDir(rcEnd - rcBegin);
  float fLen = cDir.Length();
  cDir.Normalize();

  Vector3D res;
  if (clGridIter.InitOnRay(rcBegin, cDir, fLen, tmp) == true)
  {
    aulFacets.insert(aulFacets.end(), tmp.begin(), tmp.end());
    tmp.clear();
    while (clGridIter.NextOnRay(tmp) == true)
    {
      aulFacets.insert(aulFacets.end(), tmp.begin(), tmp.end());
    }
  }

  // now test all the found facets
  for (std::vector<unsigned long>::const_iterator pF = aulFacets.begin(); pF != aulFacets.end(); pF++)
  {
    if (MeshFacetFunc::IntersectWithLine(_rclMesh.GetFacet(*pF), rcBegin, cDir, res) == true)
    {
      // check if the found point is in between the line segment
      if ( (rcBegin - res) * (rcEnd - res) < 0 )
        raulFacets.push_back( *pF );
    }
  }


  return (raulFacets.size() > 0);
}

bool MeshAlgorithm::NearestFacetOnRay (const Vector3D &rclPt, const Vector3D &rclDir, const std::vector<unsigned long> &raulFacets, 
                                       Vector3D &rclRes, unsigned long &rulFacet) const
{
  Vector3D  clProj, clRes;
  bool bSol = false;
  unsigned long ulInd = 0;
 
  for (std::vector<unsigned long>::const_iterator pI = raulFacets.begin(); pI != raulFacets.end(); pI++)
  {
    MeshGeomFacet rclSFacet = _rclMesh.GetFacet(*pI);

    if (MeshFacetFunc::Forminate(rclSFacet, rclPt, rclDir, clRes) == true)
    {
      if (bSol == false) // erste Loesung
      {
        bSol   = true;
        clProj = clRes;
        ulInd  = *pI;
      }
      else
      {  // liegt Punkt naeher
        if ((clRes - rclPt).Length() < (clProj - rclPt).Length())
        {
          clProj = clRes;
          ulInd  = *pI;
        }
      }
    }
  }

  rclRes   = clProj;
  rulFacet = ulInd;
  return bSol;
}

bool MeshAlgorithm::RayNearestField (const Vector3D &rclPt, const Vector3D &rclDir, const std::vector<unsigned long> &raulFacets, 
                                     Vector3D &rclRes, unsigned long &rulFacet, float fMaxAngle) const
{
  Vector3D  clProj, clRes;
  bool bSol = false;
  unsigned long ulInd = 0;

  for (std::vector<unsigned long>::const_iterator pF = raulFacets.begin(); pF != raulFacets.end(); pF++)
  {
    if (MeshFacetFunc::Forminate(_rclMesh.GetFacet(*pF), rclPt, rclDir, clRes/*, fMaxAngle*/) == true)
    {
      if (bSol == false) // erste Loesung
      {
        bSol   = true;
        clProj = clRes;
        ulInd  = *pF;
      }
      else
      {  // liegt Punkt naeher
        if ((clRes - rclPt).Length() < (clProj - rclPt).Length())
        {
          clProj = clRes;
          ulInd  = *pF;
        }
      }
    }
  }

  rclRes   = clProj;
  rulFacet = ulInd;

  return bSol;
}

void MeshAlgorithm::GetMeshBorders (std::list<std::vector<Vector3D> > &rclBorders)
{
  std::vector<unsigned long> aulAllFacets(_rclMesh.CountFacets());
  unsigned long k = 0;
  for (std::vector<unsigned long>::iterator pI = aulAllFacets.begin(); pI != aulAllFacets.end(); pI++)
    *pI = k++;

  GetFacetBorders(aulAllFacets, rclBorders);
/*
  MeshFacetArray   &rclFAry = _rclMesh._aclFacetArray;
  MeshEdgeArray    &rclEAry = _rclMesh._aclEdgeArray;
  MeshPointArray   &rclPAry = _rclMesh._aclPointArray;

  ResetFacetFlag(MeshFacet::VISIT);

  // Facet-Array nach Facets absuchen die noch nicht besucht sind und mind. 1 Border haben
  for (MeshEdgeArray::_TIterator pEIter = rclEAry.begin(); pEIter != rclEAry.end(); pEIter++)
  {
    if (pEIter->IsFlag(MeshEdge::VISIT) == false) 
    {
      if (_rclMesh.IsBorder(pEIter) == true)
      {
        std::vector<Vector3D> clBorder;
        MeshEdgeArray::_TIterator pENext(pEIter);
 
        unsigned long  ulInd  = pENext->Index();  // Facet-Index
        unsigned short usSide = pENext->Side();   // Kantennummer der Rand-Kante

        clBorder.push_back(rclPAry[rclFAry[ulInd]._aulPoints[usSide]]); // Anfangspunkt
        bool bAltCirculation = false;  // Umlaufrichtung gedreht

        while (pENext->IsFlag(MeshEdge::VISIT) == false)
        { // Rand-Kante gefunden ==> verfolgen
          pENext->SetFlag(MeshEdge::VISIT);

          if (bAltCirculation == false)
            clBorder.push_back(rclPAry[rclFAry[ulInd]._aulPoints[(usSide+1)%3]]);
          else
            clBorder.push_back(rclPAry[rclFAry[ulInd]._aulPoints[usSide]]);

          // Nachbar-Facets zum fortfuehren der Kanten suchen
          if (bAltCirculation == false)            
            usSide = (usSide+1) % 3;  
          else
            usSide = (usSide+2) % 3;  

          unsigned long  ulNB = 0;  // Index Nachbar-Facet
          unsigned short usSuccSide;
          while (ulNB != ULONG_MAX)  // solange Nachbar-Facets absuchen bis anschliessende Rand-Kante gefunden
          { 
   
            ulNB = rclFAry[ulInd]._aulNeighbours[usSide];  // Index Nachbar-Facet
            if (ulNB != ULONG_MAX)
            {
              unsigned short usPrevSide = rclFAry[ulNB].Side(ulInd);  // Kantennummer der gemeinsame Kanten mit Vorgaenge-Facet

              // Auf Umlaufrichtung testen
              if (rclFAry[ulInd]._aulPoints[usSide] == rclFAry[ulNB]._aulPoints[usPrevSide])
                bAltCirculation = !bAltCirculation;

              if (bAltCirculation == true)
                usSuccSide = (usPrevSide+2) % 3;  // Umlaufrichtung gleich
              else
                usSuccSide = (usPrevSide+1) % 3;  // Gegenlaeufig
            
              if (rclFAry[ulNB]._aulNeighbours[usSuccSide] != ULONG_MAX) // keine Rand-Kante
              {
                ulInd  = ulNB;
                usSide = usSuccSide;
              }
              else
              {
                usSide = usSuccSide;
                ulInd  = ulNB;        // aktueller Index auf das Nachbar-Facet
                ulNB   = ULONG_MAX;   // kein Nachbar, Abbruch der Schleife
              }
            }
          }
          // Kanten-Iterator auf neue Rand-Kante setzen
          pENext = _rclMesh.FindEdge(ulInd, usSide);
        }
        rclBorders.push_back(clBorder);
      }
    }
  }
*/
}


void MeshAlgorithm::SetFacetsFlag (const std::vector<unsigned long> &raulInds, MeshFacet::TFlagType tF)
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclFacetArray[*i].SetFlag(tF);
}

#ifdef Use_EdgeList
void MeshAlgorithm::SetEdgesFlag (const std::vector<unsigned long> &raulInds, MeshEdge::TFlagType tF)
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclEdgeArray[*i].SetFlag(tF);
}
#endif

void MeshAlgorithm::SetPointsFlag (const std::vector<unsigned long> &raulInds, MeshPoint::TFlagType tF)
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclPointArray[*i].SetFlag(tF);
}

void MeshAlgorithm::ResetFacetsFlag (const std::vector<unsigned long> &raulInds, MeshFacet::TFlagType tF)
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclFacetArray[*i].ResetFlag(tF);
}

#ifdef Use_EdgeList
void MeshAlgorithm::ResetEdgesFlag (const std::vector<unsigned long> &raulInds, MeshEdge::TFlagType tF)
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclEdgeArray[*i].ResetFlag(tF);
}
#endif

void MeshAlgorithm::ResetPointsFlag (const std::vector<unsigned long> &raulInds, MeshPoint::TFlagType tF)
{
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++)
    _rclMesh._aclPointArray[*i].ResetFlag(tF);
}

void MeshAlgorithm::SetFacetsProperty(const std::vector<unsigned long> &raulInds, const std::vector<unsigned long> &raulProps)
{
  if (raulInds.size() != raulProps.size()) return;

  std::vector<unsigned long>::const_iterator iP = raulProps.begin();
  for (std::vector<unsigned long>::const_iterator i = raulInds.begin(); i != raulInds.end(); i++, iP++)
    _rclMesh._aclFacetArray[*i]._ulProp = *iP;
}

void MeshAlgorithm::SetFacetFlag (MeshFacet::TFlagType tF)
{
  _rclMesh._aclFacetArray.SetFlag(tF);
}

#ifdef Use_EdgeList
void MeshAlgorithm::SetEdgeFlag (MeshEdge::TFlagType tF)
{
  _rclMesh._aclEdgeArray.SetFlag(tF);
}
#endif

void MeshAlgorithm::SetPointFlag (MeshPoint::TFlagType tF)
{
  _rclMesh._aclPointArray.SetFlag(tF);
}

void MeshAlgorithm::ResetFacetFlag (MeshFacet::TFlagType tF)
{
  _rclMesh._aclFacetArray.ResetFlag(tF);
}

#ifdef Use_EdgeList
void MeshAlgorithm::ResetEdgeFlag (MeshEdge::TFlagType tF)
{
  _rclMesh._aclEdgeArray.ResetFlag(tF);
}
#endif

void MeshAlgorithm::ResetPointFlag (MeshPoint::TFlagType tF)
{
  _rclMesh._aclPointArray.ResetFlag(tF);
}

unsigned long MeshAlgorithm::CountFacetFlag (MeshFacet::TFlagType tF) const
{
  return std::count_if(_rclMesh._aclFacetArray.begin(), _rclMesh._aclFacetArray.end(),
                    std::bind2nd(MeshIsFlag<MeshFacet>(), tF));
}

#ifdef Use_EdgeList
unsigned long MeshAlgorithm::CountEdgeFlag (MeshEdge::TFlagType tF) const
{
  return std::count_if(_rclMesh._aclEdgeArray.begin(), _rclMesh._aclEdgeArray.end(),
                    std::bind2nd(MeshIsFlag<MeshEdge>(), tF));
}
#endif

unsigned long MeshAlgorithm::CountPointFlag (MeshPoint::TFlagType tF) const
{
  return std::count_if(_rclMesh._aclPointArray.begin(), _rclMesh._aclPointArray.end(),
                    std::bind2nd(MeshIsFlag<MeshPoint>(), tF));
}

void MeshAlgorithm::CutFacets(const MeshFacetGrid& rclGrid, const ViewProjMethod* pclProj, 
                              const Polygon2D& rclPoly, bool bCutInner, std::vector<MeshGeomFacet> &raclFacets) const
{
  std::vector<unsigned long> aulFacets;

  CheckFacets(rclGrid, pclProj, rclPoly, bCutInner, aulFacets );

  for (std::vector<unsigned long>::iterator i = aulFacets.begin(); i != aulFacets.end(); i++)
    raclFacets.push_back(_rclMesh.GetFacet(*i));

  _rclMesh.DeleteFacets(aulFacets);
}

void MeshAlgorithm::CutFacets(const MeshFacetGrid& rclGrid, const ViewProjMethod* pclProj,
                              const Polygon2D& rclPoly, bool bInner, std::vector<unsigned long> &raclCutted) const
{
  CheckFacets(rclGrid, pclProj, rclPoly, bInner, raclCutted);
  _rclMesh.DeleteFacets(raclCutted);
/*  
  if (bInner)
  {
    CheckFacets(rclGrid, pclProj, rclPoly, bInner, raclCutted);
    _rclMesh.DeleteFacets(raclCutted);
  }
    // wenn Dreiecke ausserhalb schneiden, dann alles durchsuchen
  else
  {
    std::vector<unsigned long> aulPoints;
    MeshPointIterator clIter(_rclMesh, 0);
    Vector3D clPt2d, clPt3d;

    for (clIter.Init(); clIter.More(); clIter.Next())
    {
      clPt3d = *clIter;
      clPt2d = pclProj->operator ()(clPt3d);
      if (rclPoly.Contains(CVector2D(clPt2d.x, clPt2d.y)) == bInner)
      {
        aulPoints.push_back(clIter.Position());
      }
    }
    _rclMesh.DeletePoints(aulPoints);
  }
*/
}

void MeshAlgorithm::CheckFacets(const MeshFacetGrid& rclGrid, const ViewProjMethod* pclProj, const Polygon2D& rclPoly, 
                                bool bInner, std::vector<unsigned long> &raulFacets) const
{
    std::vector<unsigned long>::iterator it;
    MeshFacetIterator clIter(_rclMesh, 0);
    Vector3D clPt2d;
    Vector3D clGravityOfFacet;
    bool bNoPointInside;

    // Falls true, verwende Grid auf Mesh, um Suche zu beschleunigen
    if (bInner)
    {
        BoundBox3D clBBox3d;
        BoundBox2D clViewBBox, clPolyBBox;
        std::vector<unsigned long> aulAllElements;

        //B-Box des Polygons
        clPolyBBox = rclPoly.CalcBoundBox();
        // Iterator fuer die zu durchsuchenden B-Boxen des Grids
        MeshGridIterator clGridIter(rclGrid);
        // alle B-Boxen durchlaufen und die Facets speichern
        for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
        {
            clBBox3d = clGridIter.GetBoundBox();
            clViewBBox = clBBox3d.ProjectBox(pclProj);
            if (clViewBBox || clPolyBBox)
            {
                // alle Elemente in AllElements sammeln
                clGridIter.GetElements(aulAllElements);
            }
        }
        // doppelte Elemente wieder entfernen
        std::sort(aulAllElements.begin(), aulAllElements.end());
        aulAllElements.erase(std::unique(aulAllElements.begin(), aulAllElements.end()), aulAllElements.end());

        Base::Sequencer().start( "Check facets", aulAllElements.size() );

        for (it = aulAllElements.begin(); it != aulAllElements.end(); it++)
        {
            bNoPointInside = true;
            clGravityOfFacet.Set(0.0f, 0.0f, 0.0f);
            MeshGeomFacet rclFacet = _rclMesh.GetFacet(*it);
            for (int j=0; j<3; j++)
            {
                clPt2d = pclProj->operator()(rclFacet._aclPoints[j]);
                clGravityOfFacet += clPt2d;
                if (rclPoly.Contains(Vector2D(clPt2d.x, clPt2d.y)) == bInner)
                {
                    raulFacets.push_back(*it);
                    bNoPointInside = false;
                    break;
                }   
            }
            
            // if no facet point is inside the polygon then check also the gravity
            if (bNoPointInside == true) 
            {
              clGravityOfFacet *= 1.0f/3.0f;

              if (rclPoly.Contains(Vector2D(clGravityOfFacet.x, clGravityOfFacet.y)) == bInner)
                 raulFacets.push_back(*it);
            }

            Base::Sequencer().next();
        }
        Base::Sequencer().stop();
    }
    // Dreiecke ausserhalb schneiden, dann alles durchsuchen
    else
    {
      Base::Sequencer().start("Check facets", _rclMesh.CountFacets());
      for (clIter.Init(); clIter.More(); clIter.Next())
      {
          for (int j=0; j<3; j++)
          {
              clPt2d = pclProj->operator()(clIter->_aclPoints[j]);
              if (rclPoly.Contains(Vector2D(clPt2d.x, clPt2d.y)) == bInner)
              {
                  raulFacets.push_back(clIter.Position());
                  break;
              }
          }
          Base::Sequencer().next();
      }
      Base::Sequencer().stop();
    }
}

void MeshAlgorithm::GetFacetBorders (const std::vector<unsigned long> &raulInd, std::list<std::vector<Vector3D> > &rclBorders)
{
  MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  // alle Facets markieren die in der Indizie-Liste vorkommen
  ResetFacetFlag(MeshFacet::VISIT);
  for (std::vector<unsigned long>::const_iterator pIter = raulInd.begin(); pIter != raulInd.end(); pIter++)
    rclFAry[*pIter].SetFlag(MeshFacet::VISIT);

  std::list<std::pair<unsigned long, unsigned long> >  aclEdges;
  // alle Randkanten suchen und ablegen (unsortiert)
  for (std::vector<unsigned long>::const_iterator pIter2 = raulInd.begin(); pIter2 != raulInd.end(); pIter2++)
  {
    MeshFacet  &rclFacet = rclFAry[*pIter2];
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulNB = rclFacet._aulNeighbours[i];
      if (ulNB != ULONG_MAX)
      {
        if (rclFAry[ulNB].IsFlag(MeshFacet::VISIT) == true)
          continue;
      }

      aclEdges.push_back(rclFacet.GetEdge(i));
    }
  }

  if (aclEdges.size() == 0)
    return; // no borders found (=> solid)

  // Kanten aus der unsortieren Kantenliste suchen
  MeshPointArray &rclPAry = _rclMesh._aclPointArray;
  unsigned long              ulFirst, ulLast;
  std::list<Vector3D>   clBorder;
  ulFirst = aclEdges.begin()->first;
  ulLast  = aclEdges.begin()->second;

  aclEdges.erase(aclEdges.begin());
  clBorder.push_back(rclPAry[ulFirst]);
  clBorder.push_back(rclPAry[ulLast]);

  while (aclEdges.size() > 0)
  {
    // naechste anliegende Kante suchen
    std::list<std::pair<unsigned long, unsigned long> >::iterator pEI;
    for (pEI = aclEdges.begin(); pEI != aclEdges.end(); pEI++)
    {
      if (pEI->first == ulLast)
      {
        ulLast = pEI->second;
        clBorder.push_back(rclPAry[ulLast]);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->second == ulLast)
      {
        ulLast = pEI->first;
        clBorder.push_back(rclPAry[ulLast]);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->first == ulFirst)
      {
        ulFirst = pEI->second;
        clBorder.push_front(rclPAry[ulFirst]);
        aclEdges.erase(pEI);
        break;
      }
      else if (pEI->second == ulFirst)
      {
        ulFirst = pEI->first;
        clBorder.push_front(rclPAry[ulFirst]);
        aclEdges.erase(pEI);
        break;
      }
    }
    if ((pEI == aclEdges.end()) || (ulLast == ulFirst))
    {  // keine weitere Kante gefunden bzw. Polylinie geschlossen
      std::vector<Vector3D> clTmp(clBorder.begin(), clBorder.end());
      rclBorders.push_back(std::vector<Vector3D>(clBorder.begin(), clBorder.end()));
      clBorder.clear();

      if (aclEdges.size() > 0)
      {  // neue Border anfangen
        ulFirst = aclEdges.begin()->first;
        ulLast  = aclEdges.begin()->second;
        aclEdges.erase(aclEdges.begin());
        clBorder.push_back(rclPAry[ulFirst]);
        clBorder.push_back(rclPAry[ulLast]);
      }
    }
  }
}

bool MeshAlgorithm::MeshTopoShape(TopoDS_Shape aShape, float fAccuracy, float fAngle) const
{
  Standard_Integer e1,e2,e3,i1,i2,i3;
  Standard_Boolean b1,b2,b3;
  std::vector<MeshGeomFacet> vFacets;
  MeshGeomFacet cSimpleFacet;

  try{
    BRepMesh_Discret mesh(fAccuracy,aShape,fAngle);
    //BRepMesh_Discret mesh(fAccuracy,aShape,fAngle);
    int NbTri = mesh.NbTriangles() ;
    for ( int nbt = 1, i = 1 ;nbt <= NbTri;nbt++, i += 3 ) {
      BRepMesh_Triangle tri = mesh.Triangle(nbt);
      tri.Edges(e1,e2,e3,b1,b2,b3);
        if (b1) {
          i1 = mesh.Edge(e1).FirstNode() ;
          i2 = mesh.Edge(e1).LastNode() ;
        }
        else {
          i1 = mesh.Edge(e1).LastNode() ;
          i2 = mesh.Edge(e1).FirstNode() ;
        }

        if (b2) {
          i3 = mesh.Edge(e2).LastNode();
        }
        else {
          i3 = mesh.Edge(e2).FirstNode() ;
        }
        cSimpleFacet._aclPoints[0].x = float(mesh.Pnt( i1 ).X());
        cSimpleFacet._aclPoints[0].y = float(mesh.Pnt( i1 ).Y());
        cSimpleFacet._aclPoints[0].z = float(mesh.Pnt( i1 ).Z());
        cSimpleFacet._aclPoints[1].x = float(mesh.Pnt( i2 ).X());
        cSimpleFacet._aclPoints[1].y = float(mesh.Pnt( i2 ).Y());
        cSimpleFacet._aclPoints[1].z = float(mesh.Pnt( i2 ).Z());
        cSimpleFacet._aclPoints[2].x = float(mesh.Pnt( i3 ).X());
        cSimpleFacet._aclPoints[2].y = float(mesh.Pnt( i3 ).Y());
        cSimpleFacet._aclPoints[2].z = float(mesh.Pnt( i3 ).Z());

        vFacets.push_back(cSimpleFacet);
   }

    _rclMesh.Clear();
    _rclMesh = vFacets;

  } catch(...){
      return false;
  }

  return true;
}

float MeshAlgorithm::Surface (void) const
{
  float              fTotal = 0.0f;
  MeshFacetIterator clFIter(_rclMesh);

  for (clFIter.Init(); clFIter.More(); clFIter.Next())
    fTotal +=  MeshFacetFunc::Area(*clFIter);
  
  return fTotal;
}

void MeshAlgorithm::SubSampleByDist (float fDist, std::vector<Vector3D> &rclPoints) const
{ 
  rclPoints.clear();
  MeshFacetIterator clFIter(_rclMesh);
  for (clFIter.Init(); clFIter.More(); clFIter.Next())
  {
    unsigned long k = rclPoints.size();
    MeshFacetFunc::SubSample(*clFIter, fDist, rclPoints);
    if (rclPoints.size() == k)
      rclPoints.push_back(clFIter->GetGravityPoint()); // min. add middle point
  }
}

void MeshAlgorithm::SubSampleAllPoints (std::vector<Vector3D> &rclPoints) const
{ 
  rclPoints.clear();

  // Add all Points
  //
  MeshPointIterator clPIter(_rclMesh);
  for (clPIter.Init(); clPIter.More(); clPIter.Next())
  {
      rclPoints.push_back(*clPIter);
  }
}

void MeshAlgorithm::SubSampleByCount (unsigned long ulCtPoints, std::vector<Vector3D> &rclPoints) const
{
  float fDist = float(sqrt(Surface() / float(ulCtPoints)));
  SubSampleByDist(fDist, rclPoints);
}

void MeshAlgorithm::SearchFacetsFromPolyline (const std::vector<Vector3D> &rclPolyline, float fRadius,
                                              const MeshFacetGrid& rclGrid, std::vector<unsigned long> &rclResultFacetsIndices) const
{
  rclResultFacetsIndices.clear();

  std::set<unsigned long>  aclFacets;
  for (std::vector<Vector3D>::const_iterator pV = rclPolyline.begin(); pV < (rclPolyline.end() - 1); pV++)
  {
    const Vector3D &rclP0 = *pV, &rclP1 = *(pV + 1);
   
    // BB eines Polyline-Segments
    BoundBox3D clSegmBB(rclP0.x, rclP0.y, rclP0.z, rclP0.x, rclP0.y, rclP0.z);
    clSegmBB &= rclP1;
    clSegmBB.Enlarge(fRadius);  // BB um Suchradius vergroessern

    std::vector<unsigned long> aclBBFacets;
    unsigned long k = rclGrid.InSide(clSegmBB, aclBBFacets, false);
    for (unsigned long i = 0; i < k; i++)
    {
      if (MeshFacetFunc::DistanceToLineSegment(_rclMesh.GetFacet(aclBBFacets[i]), rclP0, rclP1) < fRadius)
        aclFacets.insert(aclBBFacets[i]);
    }
  }

  rclResultFacetsIndices.insert(rclResultFacetsIndices.begin(), aclFacets.begin(), aclFacets.end());
}

void MeshAlgorithm::CutBorderFacets (std::vector<unsigned long> &raclFacetIndices, unsigned short usLevel)
{
  std::vector<unsigned long> aclToDelete;

  CheckBorderFacets(raclFacetIndices, aclToDelete, usLevel);

  // alle gefunden "Rand"-Facetsindizes" aus dem Array loeschen
  std::vector<unsigned long>  aclResult;
  std::set<unsigned long>     aclTmp(aclToDelete.begin(), aclToDelete.end());

  for (std::vector<unsigned long>::iterator pI = raclFacetIndices.begin(); pI != raclFacetIndices.end(); pI++)
  {
    if (aclTmp.find(*pI) == aclTmp.end())
      aclResult.push_back(*pI);
  }

  raclFacetIndices = aclResult;
}

void MeshAlgorithm::CheckBorderFacets (const std::vector<unsigned long> &raclFacetIndices, std::vector<unsigned long> &raclResultIndices, unsigned short usLevel)
{
  ResetFacetFlag(MeshFacet::TMP0);
  SetFacetsFlag(raclFacetIndices, MeshFacet::TMP0);

  MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  for (unsigned short usL = 0; usL < usLevel; usL++)
  {
    for (std::vector<unsigned long>::const_iterator pF = raclFacetIndices.begin(); pF != raclFacetIndices.end(); pF++)
    {
      for (int i = 0; i < 3; i++)
      {
        unsigned long ulNB = rclFAry[*pF]._aulNeighbours[i];
        if (ulNB == ULONG_MAX)
        {
          raclResultIndices.push_back(*pF);
          rclFAry[*pF].ResetFlag(MeshFacet::TMP0);
          continue;
        }
        if (rclFAry[ulNB].IsFlag(MeshFacet::TMP0) == false)
        {
          raclResultIndices.push_back(*pF);
          rclFAry[*pF].ResetFlag(MeshFacet::TMP0);
          continue;
        }
      }
    }
  }
}

void MeshAlgorithm::GetBorderPoints (const std::vector<unsigned long> &raclFacetIndices, std::set<unsigned long> &raclResultPointsIndices)
{
  ResetFacetFlag(MeshFacet::TMP0);
  SetFacetsFlag(raclFacetIndices, MeshFacet::TMP0);

  MeshFacetArray &rclFAry = _rclMesh._aclFacetArray;

  for (std::vector<unsigned long>::const_iterator pF = raclFacetIndices.begin(); pF != raclFacetIndices.end(); pF++)
  {
    for (int i = 0; i < 3; i++)
    {
      MeshFacet &rclFacet = rclFAry[*pF];
      unsigned long      ulNB     = rclFacet._aulNeighbours[i];
      if (ulNB == ULONG_MAX)
      {
        raclResultPointsIndices.insert(rclFacet._aulPoints[i]);
        raclResultPointsIndices.insert(rclFacet._aulPoints[(i+1)%3]);
        continue;
      }
      if (rclFAry[ulNB].IsFlag(MeshFacet::TMP0) == false)
      {
        raclResultPointsIndices.insert(rclFacet._aulPoints[i]);
        raclResultPointsIndices.insert(rclFacet._aulPoints[(i+1)%3]);
        continue;
      }
    }
  }
}

bool MeshAlgorithm::NearestPointFromPoint (const Vector3D &rclPt, unsigned long &rclResFacetIndex, Vector3D &rclResPoint) const
{
  if (_rclMesh.CountFacets() == 0)
    return false;

  // calc each facet
  float fMinDist = FLOAT_MAX;
  unsigned long ulInd   = ULONG_MAX;
  MeshFacetIterator pF(_rclMesh);
  for (pF.Init(); pF.More(); pF.Next())
  {
    float fDist = MeshFacetFunc::DistanceToPoint(*pF, rclPt);
    if (fDist < fMinDist)
    {
      fMinDist = fDist;
      ulInd    = pF.Position();
    }
  }

  MeshGeomFacet rclSFacet = _rclMesh.GetFacet(ulInd);
  MeshFacetFunc::DistanceToPoint(rclSFacet, rclPt, rclResPoint);
  rclResFacetIndex = ulInd;

  return true;
}

bool MeshAlgorithm::NearestPointFromPoint (const Vector3D &rclPt, const MeshFacetGrid& rclGrid, unsigned long &rclResFacetIndex, Vector3D &rclResPoint) const
{
  unsigned long ulInd = rclGrid.SearchNearestFromPoint(rclPt);

  if (ulInd == ULONG_MAX)
  {
    return false;
  }

  MeshGeomFacet rclSFacet = _rclMesh.GetFacet(ulInd);
  MeshFacetFunc::DistanceToPoint(rclSFacet, rclPt, rclResPoint);
  rclResFacetIndex = ulInd;

  return true;
}

bool MeshAlgorithm::NearestPointFromPoint (const Vector3D &rclPt, const MeshFacetGrid& rclGrid, float fMaxSearchArea,
                                           unsigned long &rclResFacetIndex, Vector3D &rclResPoint) const
{
  unsigned long ulInd = rclGrid.SearchNearestFromPoint(rclPt, fMaxSearchArea);

  if (ulInd == ULONG_MAX)
    return false;  // no facets inside BoundingBox

  MeshGeomFacet rclSFacet = _rclMesh.GetFacet(ulInd);
  MeshFacetFunc::DistanceToPoint(rclSFacet, rclPt, rclResPoint);
  rclResFacetIndex = ulInd;

  return true;
}

bool MeshAlgorithm::CutWithPlane (const gp_Pln &rclPlane, const MeshFacetGrid &rclGrid, 
                                  std::list<std::vector<Vector3D> > &rclResult, float fMinEps)
{
  Vector3D  clBase, clNormal; // Schnittebene

  gp_Pnt clPos  = rclPlane.Axis().Location();
  gp_Dir clDir  = rclPlane.Axis().Direction();
  clBase.Set(clPos.X(), clPos.Y(), clPos.Z());
  clNormal.Set(clDir.X(), clDir.Y(), clDir.Z());

  return CutWithPlane (clBase, clNormal, rclGrid, rclResult, fMinEps, false);
}

bool MeshAlgorithm::CutWithPlane (const Vector3D &clBase, const Vector3D &clNormal, const MeshFacetGrid &rclGrid, 
                                  std::list<std::vector<Vector3D> > &rclResult, float fMinEps, bool bConnectPolygons)
{
  std::vector<unsigned long>  aulFacets;  

  // Grid durschsuchen
  MeshGridIterator clGridIter(rclGrid);
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {  
    // wenn Gridvoxel Ebene schneidet: alle Facets aufnehmen zum Schneiden
    if (clGridIter.GetBoundBox().IsCutPlane(clBase, clNormal) == true)
      clGridIter.GetElements(aulFacets);
  }

  // mehrfach vorhanden Dreiecke entfernen
  std::sort(aulFacets.begin(), aulFacets.end());
  aulFacets.erase(std::unique(aulFacets.begin(), aulFacets.end()), aulFacets.end());  

  // alle Facets mit Ebene schneiden
  std::list<std::pair<Vector3D, Vector3D> > clTempPoly;  // Feld mit Schnittlinien (unsortiert, nicht verkettet)

  for (std::vector<unsigned long>::iterator pF = aulFacets.begin(); pF != aulFacets.end(); pF++)
  {
    Vector3D  clE1, clE2;
    const MeshGeomFacet clF(_rclMesh.GetFacet(*pF));

    // Facet schneiden und Schnittstrecke ablegen
    if (MeshFacetFunc::IntersectWithPlane(clF, clBase, clNormal, clE1, clE2) == true)
      clTempPoly.push_back(std::pair<Vector3D, Vector3D>(clE1, clE2));
  }

  if(bConnectPolygons)
  {
      //std::list<std::pair<Vector3D, Vector3D> > rclTempLines;
      std::list<std::pair<Vector3D, Vector3D> > rclResultLines(clTempPoly.begin(),clTempPoly.end());
      std::list<std::vector<Vector3D> > tempList;
      ConnectLines(clTempPoly, tempList, fMinEps);
      ConnectPolygons(tempList, clTempPoly);

      for(std::list<std::pair<Vector3D, Vector3D> >::iterator iter = clTempPoly.begin(); iter != clTempPoly.end(); iter++)
      {
        rclResultLines.push_front(*iter);
      }

      return ConnectLines(rclResultLines, rclResult, fMinEps);
  }

  return ConnectLines(clTempPoly, rclResult, fMinEps);
}

bool MeshAlgorithm::ConnectLines (std::list<std::pair<Vector3D, Vector3D> > &rclLines, 
                                  std::list<std::vector<Vector3D> > &rclPolylines, float fMinEps) const
{
  typedef std::list<std::pair<Vector3D, Vector3D> >::iterator  TCIter;

  // square search radius
  // const float fMinEps = 1.0e-2f; // := 10 mirometer distance
  fMinEps = fMinEps * fMinEps;  

  // remove all lines which distance is smaller than epsilon
  std::list<TCIter> _clToDelete;
  float fToDelDist = fMinEps / 10.0f;
  for (TCIter pF = rclLines.begin(); pF != rclLines.end(); pF++)
  {
    if (DistanceP2(pF->first, pF->second) < fToDelDist)
      _clToDelete.push_back(pF);
  }
  for (std::list<TCIter>::iterator pI = _clToDelete.begin(); pI != _clToDelete.end(); pI++)
    rclLines.erase(*pI);


  while (rclLines.size() > 0)
  {
    TCIter pF;
    
    // new polyline
    std::list<Vector3D> clPoly;

    // add first line and delete from the list
    Vector3D clFront = rclLines.begin()->first;  // current start point of the polyline
    Vector3D clEnd   = rclLines.begin()->second; // current end point of the polyline
    clPoly.push_back(clFront);
    clPoly.push_back(clEnd);
    rclLines.erase(rclLines.begin());

    // search for the next line on the begin/end of the polyline and add it
    TCIter pFront, pEnd;
    do
    {
      float  fFrontMin = fMinEps, fEndMin = fMinEps;
      bool   bFrontFirst, bEndFirst;
      pFront = NULL;
      pEnd   = NULL;
      for (pF = rclLines.begin(); pF != rclLines.end(); pF++)
      {
        if (DistanceP2(clFront, pF->first) < fFrontMin) 
        {
          fFrontMin   = DistanceP2(clFront, pF->first);
          pFront      = pF;
          bFrontFirst = true;
        }
        else if (DistanceP2(clEnd, pF->first) < fEndMin)
        {
          fEndMin     = DistanceP2(clEnd, pF->first);
          pEnd        = pF;
          bEndFirst   = true;
        }
        else if (DistanceP2(clFront, pF->second) < fFrontMin)
        {
          fFrontMin   = DistanceP2(clFront, pF->second);
          pFront      = pF;
          bFrontFirst = false;
        }
        else if (DistanceP2(clEnd, pF->second) < fEndMin)
        {
          fEndMin     = DistanceP2(clEnd, pF->second);
          pEnd        = pF;
          bEndFirst   = false;
        }        
      }

      if (pFront != NULL)
      {
        if (bFrontFirst == true)
        {
          clPoly.push_front(pFront->second);
          clFront = pFront->second;
        }
        else
        {
          clPoly.push_front(pFront->first);
          clFront = pFront->first;
        }
        rclLines.erase(pFront);
      }

      if (pEnd != NULL)
      {
        if (bEndFirst == true)
        {
          clPoly.push_back(pEnd->second);
          clEnd = pEnd->second;
        }
        else
        {
          clPoly.push_back(pEnd->first);
          clEnd = pEnd->first;
        }
        rclLines.erase(pEnd);
      }
    }
    while ((pFront != NULL) || (pEnd != NULL));

    rclPolylines.push_back(std::vector<Vector3D>(clPoly.begin(), clPoly.end()));
  }  
  
  // remove all polylines with too few length
  typedef std::list<std::vector<Vector3D> >::iterator TPIter;
  std::list<TPIter> _clPolyToDelete;
  for (TPIter pJ = rclPolylines.begin(); pJ != rclPolylines.end(); pJ++)
  {
    if (pJ->size() == 2) // only one line segment
    {
      if (DistanceP2(*pJ->begin(), *(pJ->begin() + 1)) <= fMinEps)
        _clPolyToDelete.push_back(pJ);
    }
  }
  for (std::list<TPIter>::iterator pK = _clPolyToDelete.begin(); pK != _clPolyToDelete.end(); pK++)
    rclPolylines.erase(*pK);

  return true;
}

bool MeshAlgorithm::ConnectPolygons(std::list<std::vector<Vector3D> > &clPolyList, 
                                    std::list<std::pair<Vector3D, Vector3D> > &rclLines) const
{

  for(std::list< std::vector<Vector3D> >::iterator OutIter = clPolyList.begin(); OutIter != clPolyList.end() ; OutIter++)
  {
    std::pair<Vector3D,Vector3D> currentSort;
    float fDist = Distance(OutIter->front(),OutIter->back());
    currentSort.first = OutIter->front();
    currentSort.second = OutIter->back();

    for(std::list< std::vector<Vector3D> >::iterator InnerIter = clPolyList.begin(); InnerIter != clPolyList.end(); InnerIter++)
    {
      if(OutIter == InnerIter) continue;

      if(Distance(OutIter->front(),InnerIter->front()) < fDist)
      {
        currentSort.second = InnerIter->front();
        fDist = Distance(OutIter->front(),InnerIter->front());
      }
      if(Distance(OutIter->front(),InnerIter->back()) < fDist)
      {
        currentSort.second = InnerIter->back();
        fDist = Distance(OutIter->front(),InnerIter->back());
      }
    }

    rclLines.push_front(currentSort);

  }

  return true;
}

void MeshAlgorithm::GetFacetsFromPlane (const MeshFacetGrid &rclGrid, const gp_Pln clPlane, const Vector3D &rclLeft, 
                                        const Vector3D &rclRight, std::vector<unsigned long> &rclRes) const
{
  std::vector<unsigned long> aulFacets;
 
  gp_Pnt clPos  = clPlane.Position().Location();
  gp_Dir clDir  = clPlane.Position().Direction();

  Vector3D clBase(float(clPos.X()), float(clPos.Y()), float(clPos.Z()));
  Vector3D clNormal(float(clDir.X()), float(clDir.Y()), float(clDir.Z()));
  clNormal.Normalize();

  Vector3D clPtNormal(rclLeft - rclRight);
  clPtNormal.Normalize();

  // search grid 
  MeshGridIterator clGridIter(rclGrid);
  for (clGridIter.Init(); clGridIter.More(); clGridIter.Next())
  {  
    // add facets from grid if the plane if cut the grid-voxel
    if (clGridIter.GetBoundBox().IsCutPlane(clBase, clNormal) == true)
      clGridIter.GetElements(aulFacets);
  }

  // testing facet against planes
  for (std::vector<unsigned long>::iterator pI = aulFacets.begin(); pI != aulFacets.end(); pI++)
  {
    MeshGeomFacet clSFacet = _rclMesh.GetFacet(*pI);
    if (MeshFacetFunc::IntersectWithPlane(clSFacet, clBase, clNormal) == true)
    {
      bool bInner = false;
      for (int i = 0; (i < 3) && (bInner == false); i++)
      {
        Vector3D clPt = clSFacet._aclPoints[i];
        if ((clPt.DistanceToPlane(rclLeft, clPtNormal) <= 0.0f) && (clPt.DistanceToPlane(rclRight, clPtNormal) >= 0.0f))
          bInner = true;
      }

      if (bInner == true)
        rclRes.push_back(*pI);
    }
  }
}

void MeshAlgorithm::PointsFromFacetsIndices (const std::vector<unsigned long> &rvecIndices, std::vector<Vector3D> &rvecPoints) const
{
  std::set<unsigned long> setPoints;

  for (std::vector<unsigned long>::const_iterator itI = rvecIndices.begin(); itI != rvecIndices.end(); itI++)
  {
    for (int i = 0; i < 3; i++)
      setPoints.insert(_rclFAry[*itI]._aulPoints[i]);
  }

  rvecPoints.clear();
  for (std::set<unsigned long>::iterator itP = setPoints.begin(); itP != setPoints.end(); itP++)
    rvecPoints.push_back(_rclPAry[*itP]);
}
