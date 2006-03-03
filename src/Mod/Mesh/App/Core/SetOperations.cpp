/***************************************************************************
 *   Copyright (c) Berthold Grupp          2005                            *
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


//#include "BRepBuilderAPI_MakePolygon.hxx"
//#include <BRep_Builder.hxx>
//#include <BRepTools.hxx>
//#include <TopTools_HSequenceOfShape.hxx>


#ifndef _PreComp_
//# include <gp_Pln.hxx>
//# include <BRepMesh_Discret.hxx>
//# include <BRepMesh_Triangle.hxx>
//# include <BRepMesh_Edge.hxx>
//# include <TopoDS_Shape.hxx>
# include <Wm3Matrix3.h>
# include <Wm3Vector3.h>
# include <Wm3Delaunay2.h>
# include <Wm3Vector2.h>
# include <Wm3Query.h>
# include <io.h>
#endif

#include <fstream>
#include "SetOperations.h"
#include "Algorithm.h"
#include "Elements.h"
#include "Iterator.h"
#include "Grid.h"
#include "MeshIO.h"
#include "Visitor.h"

#include "triangle.h"

#include <Base/Sequencer.h>
#include <Base/Builder3D.h>

using namespace MeshCore;
using Base::BoundBox3D;
using Base::BoundBox2D;
using Base::Polygon2D;


SetOperations::SetOperations (MeshKernel &cutMesh1, MeshKernel &cutMesh2, MeshKernel &result, OperationType opType, float minDistanceToPoint)
: _cutMesh1(cutMesh1), 
  _cutMesh2(cutMesh2),
  _resultMesh(result),
  _operationType(opType),
  _minDistanceToPoint(minDistanceToPoint)
{
}

void SetOperations::Do ()
{
  _builder.startPoints(5.0, 1.0, 1.0, 1.0);

  //_builder.addPoint(1.0, 1.0, 1.0);
  //_builder.addPoint(2.0, 2.0, 2.0);
  //_builder.addPoint(3.0, 3.0, 3.0);


  Cut();
  TriangulateMesh(_cutMesh1, _facet2points1, _facets1);
  TriangulateMesh(_cutMesh2, _facet2points2, _facets2);

  //_resultMesh = _facets1;
  //return;

  for (std::map<unsigned long, std::list<MeshPoint> >::iterator it = _facet2points1.begin(); it != _facet2points1.end(); it++)
  {
    _cutFacets1.push_back(it->first);
    for (std::list<MeshPoint>::iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
      _points2facets12[*it1].first.push_back(it->first); 
  }

  for (std::map<unsigned long, std::list<MeshPoint> >::iterator it2 = _facet2points2.begin(); it2 != _facet2points2.end(); it2++)
  {
    _cutFacets2.push_back(it2->first);
    for (std::list<MeshPoint>::iterator it1 = it2->second.begin(); it1 != it2->second.end(); it1++)
      _points2facets12[*it1].second.push_back(it2->first); 
  }

  CollectFacets(_cutMesh1, _cutMesh2, _facets1, _cutFacets1, _facetsOf1, true);
  CollectFacets(_cutMesh2, _cutMesh1, _facets2, _cutFacets2, _facetsOf2, false);

  vector<MeshGeomFacet> facets;
  for (std::vector<MeshGeomFacet>::iterator it3 = _facetsOf1.begin(); it3 != _facetsOf1.end(); it3++)
    facets.push_back(*it3);
  //for (std::vector<MeshGeomFacet>::iterator it = _facetsOf2.begin(); it != _facetsOf2.end(); it++)
  //  facets.push_back(*it);

  _resultMesh = facets;

  _builder.endPoints();
  _builder.saveToLog();
  _builder.saveToFile("c:/temp/builder.log");

}


void SetOperations::Cut ()
{
  MeshFacetIterator  it1(_cutMesh1);
  MeshFacetIterator  it2(_cutMesh2);

  std::list<std::pair<MeshPoint, MeshPoint> > tmpPoly;  // list of cut lines (not sorted)
  

  for (it1.Init(); it1.More(); it1.Next())
  {
    for (it2.Init(); it2.More(); it2.Next())
    {
      // check if bounding boxes cuts
      if (it1->GetBoundBox() && it2->GetBoundBox())
      {
        MeshPoint p0, p1;
        if (it1->IntersectWithFacet(*it2, p0, p1))
        {
          // optimize cut line if distance to nearest point is too small
          float minDist1 = FLOAT_MAX, minDist2 = FLOAT_MAX;
          int k1 = -1, k2 = -1;
          for (int i = 0; i < 3; i++)
          {
            float d1 = (it1->_aclPoints[i] - p0).Length();
            float d2 = (it1->_aclPoints[i] - p1).Length();
            if (d1 < minDist1)
            {
              minDist1 = d1;
              k1 = i;
            }
            if (d2 < minDist2)
            {
              minDist2 = d2;
              k2 = i;
            }
          }
          if (minDist1 < _minDistanceToPoint)
          { // move point p0 to point k1
            p0 = it1->_aclPoints[k1];
          }

          if (minDist2 < _minDistanceToPoint)
          { // move point p0 to point k1
            p1 = it1->_aclPoints[k2];
          }

          MeshPoint mp0 = p0;
          MeshPoint mp1 = p1;

          unsigned long fidx = it1.Position();
          _facet2points1[fidx].push_back(mp0);
          _facet2points1[fidx].push_back(mp1);

          fidx = it2.Position();
          _facet2points2[fidx].push_back(mp0);
          _facet2points2[fidx].push_back(mp1);
      
          tmpPoly.push_back(std::pair<MeshPoint, MeshPoint>(mp0, mp1));
        }
      }
    }
  }
  
 // //===BEGIN TEST======================================================
 // std::list<std::vector<MeshPoint> > res;
 // ConnectLines(tmpPoly, res, 1.0e-2);

 // BRepBuilderAPI_MakePolygon mp;

 // for (std::list<std::vector<MeshPoint> >::iterator it = res.begin(); it != res.end(); it++)
 // {
 //   std::vector<MeshPoint> vec;
 //   for (std::vector<MeshPoint>::iterator it1 = it->begin(); it1 != it->end(); it1++)
 //   {
 //     
 //     gp_Pnt p(it1->x, it1->y, it1->z);
 //     mp.Add(p);
 //     vec.push_back(Vector3D(it1->x, it1->y, it1->z));
 //   }
 // }

 // mp.Build();
 // TopoDS_Shape shape = mp.Shape();
 // Standard_Boolean result = BRepTools::Write(shape, (const Standard_CString)"c:/temp/testwire.brep"); 

 // return;
 ////===END TEST======================================================
}

void SetOperations::TriangulateMesh (MeshKernel &cutMesh, std::map<unsigned long, std::list<MeshPoint> > &mapPoints, std::vector<MeshGeomFacet> &facets)
{
    // Triangulate Mesh 1
  for (std::map<unsigned long, std::list<MeshPoint> >::iterator it1 = mapPoints.begin(); it1 != mapPoints.end(); it1++)
  {
    std::set<MeshPoint> pointsSet;

    unsigned long fidx = it1->first;
    MeshGeomFacet f = cutMesh.GetFacet(fidx);
    for (int i = 0; i < 3; i++)
    {
      pointsSet.insert(f._aclPoints[i]);
    }
    
    for (std::list<MeshPoint>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
    {
      pointsSet.insert(*it2);
    }

    std::vector<Vector3D> points;
    for (std::set<MeshPoint>::iterator pp = pointsSet.begin(); pp != pointsSet.end(); pp++)
      points.push_back(*pp);

    Vector3D normal = f.GetNormal();
    Vector3D base = points[0];

    triangulateio* in = new triangulateio();
    memset(in, 0, sizeof(triangulateio));
    in->pointlist = new float[points.size() * 2];
    in->numberofpoints = points.size();

    // project points to 2D plane
    //std::vector<Wm3::Vector2<float> > points2D;
    int k = 0;
    for (std::vector<Vector3D>::iterator it = points.begin(); it != points.end(); it++)
    {
      Vector3D pv = it->ProjToPlane(base, normal);
      //points2D.push_back(Wm3::Vector2<float>(pv.x, pv.y));      
      in->pointlist[k++] = pv.x;
      in->pointlist[k++] = pv.y;
    }

    triangulateio* out = new triangulateio();
    memset(out, 0, sizeof(triangulateio));

    triangulate("z", in, out, NULL);

    for (int j = 0; j < (out->numberoftriangles * 3); j += 3)
    {
      //if ((out->trianglelist[i] + out->trianglelist[i+1]+ out->trianglelist[i+2]) == 3)
      //{ // triangle itself
      //  continue;
      //}

      if ((out->trianglelist[j] == out->trianglelist[j+1]) || (out->trianglelist[j] == out->trianglelist[j+2]) || (out->trianglelist[j+1] == out->trianglelist[j+2]))
      { // two same triangle corner points
        continue;
      }
  
      MeshGeomFacet facet(points[out->trianglelist[j]], points[out->trianglelist[j+1]], points[out->trianglelist[j+2]]);

      // !!!
      if (facet.Area() < 0.00001f)
      { // too small facet
        continue;
      }

      facet.CalcNormal();
      if ((facet.GetNormal() * f.GetNormal()) < 0.0f)
      { // adjust normal
         std::swap(facet._aclPoints[0], facet._aclPoints[1]);
         facet.CalcNormal();
      }
      facets.push_back(facet);
    }

    // free all memory
    delete in->pointlist;
    delete in; //!!! free inner arrays
    delete out->trianglelist;
    delete out;

    //Wm3::Delaunay2<float> delaunay(points2D.size(), &points2D[0], 0.000001, false, Wm3::Query::QT_REAL);

    //int i = 0;
    //int aiIndex[3];
    //
    //while (delaunay.GetIndexSet(i, aiIndex))
    //{
    //  MeshGeomFacet facet;
    //  for (int j = 0; j < 3; j++)
    //    facet._aclPoints[j] = points[aiIndex[j]];

    //  if (facet.Area() > 0.00001f)
    //  {
    //    facet.CalcNormal();
    //    if ((facet.GetNormal() * f.GetNormal()) < 0.0f)
    //    { // adjust normal
    //      std::swap(facet._aclPoints[0], facet._aclPoints[1]);
    //      facet.CalcNormal();
    //    }

    //    facets.push_back(facet);
    //  }
    //  i++;
    //}
  }
}


void SetOperations::CollectFacets (MeshKernel &meshForRegionGrowing, MeshKernel &meshOther, std::vector<MeshGeomFacet> &facetsFromCutting, vector<unsigned long> facetsFromCuttingIndex, vector<MeshGeomFacet> &facetsCollected, bool first)
{
  MeshAlgorithm algo(meshForRegionGrowing);
  algo.ResetFacetFlag(MeshFacet::VISIT);
  algo.SetFacetsFlag(facetsFromCuttingIndex, MeshFacet::VISIT);

  MeshRefPointToFacets rpf(meshForRegionGrowing);

  // find the first facets two of his points lying in direction of the cutting mesh
  for (std::map<MeshPoint, std::pair<list<unsigned long>, list<unsigned long> > >::iterator it = _points2facets12.begin(); it != _points2facets12.end(); it++)
  {
    MeshGeomFacet f2;
    std::list<unsigned long>::iterator begin, end;
    if (first)
    {
      f2 = meshOther.GetFacet(*it->second.second.begin()); // first facet of mesh 2
      begin = it->second.first.begin();
      end = it->second.first.end();
    }
    else
    {
      f2 = meshOther.GetFacet(*it->second.first.begin()); // first facet of mesh 1
      begin = it->second.second.begin();
      end = it->second.second.end();
    }

    Vector3D normal2 = f2.GetNormal();

    int kkk = 0;

    for (std::list<unsigned long>::iterator it1 = begin; it1 != end; it1++)
    { // test all facets of mesh 1 not visited and at least have the right orientation
      MeshFacet mf1 = meshForRegionGrowing._aclFacetArray[*it1];
      MeshGeomFacet f1 = meshForRegionGrowing.GetFacet(*it1);
      Vector3D normal1 = f1.GetNormal();

      for (int i = 0; i < 3; i++)
      {
        Vector3D pt = f1._aclPoints[i];
        Vector3D dir = pt - it->first;
        dir.Normalize();


        if (((dir % normal1) * (normal1 % normal2)) > 0.0f)    
        { // ok, point found, get a neighbour-facet not visited
          //builder.addSinglePoint(pt, 5.0, 1.0, 1.0, 1.0);
        //  if (kkk < 1)
            _builder.addPoint(pt);
          char txt[20];
          itoa(kkk++, txt, 10);
          //builder.addText(pt, txt, 1.0, 1.0, 1.0);


          // collect all created facets (delaunay) which one corner point attached this point
          for (std::vector<MeshGeomFacet>::iterator it2 = facetsFromCutting.begin(); it2 != facetsFromCutting.end(); it2++)
          {
            if (!it2->IsFlag(MeshFacet::MARKED)) // only add at once
            {
              for (int j = 0; j < 3; j++)
              {
                if (it2->_aclPoints[j] == pt)
                {
                  facetsCollected.push_back(*it2);
                  it2->SetFlag(MeshFacet::MARKED);
                  break;
                }
              }
            }
          }

          //std::set<MeshFacetArray::_TIterator> &nbs = rpf[mf1._aulPoints[i]]; 
          //for (std::set<MeshFacetArray::_TIterator>::iterator pf = nbs.begin(); pf != nbs.end(); pf++)
          //{
          //  if (!(*pf)->IsFlag(MeshFacet::VISIT)) 
          //  { // "free" neighbour-facet found, do a region growing
          //    // facetsCollected.push_back(meshForRegionGrowing.GetFacet(*(*pf))); // seeds
          //    vector<unsigned long> visitedFacets;
          //    MeshTopFacetVisitor visitor(visitedFacets);
          //    unsigned long startFacet = *pf - meshForRegionGrowing._aclFacetArray.begin();
          //    meshForRegionGrowing.VisitNeighbourFacets(visitor, startFacet);
          //    for (vector<unsigned long>::iterator vf = visitedFacets.begin(); vf != visitedFacets.end(); vf++)
          //      facetsCollected.push_back(meshForRegionGrowing.GetFacet(*vf));
          //    facetsCollected.push_back(meshForRegionGrowing.GetFacet(startFacet));
          //  }
          //}
        }
      }
    }
  }
}
