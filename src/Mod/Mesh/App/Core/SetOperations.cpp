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
# include <ios>
#endif

#include <fstream>
#include "SetOperations.h"
#include "Algorithm.h"
#include "Elements.h"
#include "Iterator.h"
#include "Grid.h"
#include "MeshIO.h"
#include "Visitor.h"
#include "Builder.h"
#include "triangle.h"
#include "Grid.h"
#include "Evaluation.h"
#include "Definitions.h"

#include <Base/Sequencer.h>
#include <Base/Builder3D.h>
#include <Base/Tools2D.h>

using namespace Base;
using namespace MeshCore;


SetOperations::SetOperations (MeshKernel &cutMesh1, MeshKernel &cutMesh2, MeshKernel &result, OperationType opType, float minDistanceToPoint)
: _cutMesh0(cutMesh1), 
  _cutMesh1(cutMesh2),
  _resultMesh(result),
  _operationType(opType),
  _minDistanceToPoint(minDistanceToPoint)
{
}

SetOperations::~SetOperations (void)
{
}

void SetOperations::Do ()
{
 _minDistanceToPoint = 0.0001f;
  float saveMinMeshDistance = MeshDefinitions::_fMinPointDistance;
  MeshDefinitions::SetMinPointDistance(_minDistanceToPoint);

//  Base::Sequencer().start("set operation", 5);

  // _builder.clear();

  //Base::Sequencer().next();
  std::set<unsigned long> facetsCuttingEdge0, facetsCuttingEdge1;
  Cut(facetsCuttingEdge0, facetsCuttingEdge1);
 
  unsigned long i;
  for (i = 0; i < _cutMesh0.CountFacets(); i++)
  {
    if (facetsCuttingEdge0.find(i) == facetsCuttingEdge0.end())
      _newMeshFacets[0].push_back(_cutMesh0.GetFacet(i));
  }

  for (i = 0; i < _cutMesh1.CountFacets(); i++)
  {
    if (facetsCuttingEdge1.find(i) == facetsCuttingEdge1.end())
      _newMeshFacets[1].push_back(_cutMesh1.GetFacet(i));
  }

  //Base::Sequencer().next();
  TriangulateMesh(_cutMesh0, 0);

  //Base::Sequencer().next();
  TriangulateMesh(_cutMesh1, 1);

  //_resultMesh = _newMeshFacets[0];
  //for (TMeshFacetArray::iterator iii = _resultMesh._aclFacetArray.begin(); iii != _resultMesh._aclFacetArray.end(); iii++)
  //{
  //  MeshGeomFacet fff = _resultMesh.GetFacet(*iii);
  //  for (int kkk = 0; kkk < 3; kkk++)
  //  {
  //    if (iii->_aulNeighbours[kkk] == ULONG_MAX)
  //    {
  //      _builder.addSingleLine(fff._aclPoints[kkk], fff._aclPoints[(kkk+1)%3], 2.0, 1.0, 0.0, 0.0, 0x000f);
  //    }
  //  }
  //}

  //_builder.saveToFile("c:/temp/vdbg.iv");
  //return;

  //for (std::set<EdgeInfo>::iterator it = _edges.begin(); it != _edges.end(); it++)
  //  _builder.addSingleLine(it->pt1, it->pt2, 3.0, 1.0, 1.0, 0.0);

  float mult0, mult1;
  switch (_operationType)
  {
    case Union:       mult0 =  1.0f; mult1 =  1.0f;  break;
    case Intersect:   mult0 = -1.0f; mult1 = -1.0f;  break;
    case Difference:  mult0 =  1.0f; mult1 = -1.0f;  break;
  }

  //Base::Sequencer().next();
  CollectFacets(0, mult0);
  //Base::Sequencer().next();
  CollectFacets(1, mult1);
  
  std::vector<MeshGeomFacet> facets;

  std::vector<MeshGeomFacet>::iterator itf;
  for (itf = _facetsOf[0].begin(); itf != _facetsOf[0].end(); itf++)
    facets.push_back(*itf);
  
  for (itf = _facetsOf[1].begin(); itf != _facetsOf[1].end(); itf++)
  {
    if (_operationType == Difference)
    { // toggle normal
      std::swap(itf->_aclPoints[0], itf->_aclPoints[1]);
      itf->CalcNormal();
    }
    facets.push_back(*itf);
  }

  _resultMesh = facets;

   //Base::Sequencer().stop();
  // _builder.saveToFile("c:/temp/vdbg.iv");

  MeshDefinitions::SetMinPointDistance(saveMinMeshDistance);
}

void SetOperations::Cut (std::set<unsigned long>& facetsCuttingEdge0, std::set<unsigned long>& facetsCuttingEdge1)
{
  MeshFacetGrid grid1(_cutMesh0, 20);
  MeshFacetGrid grid2(_cutMesh1, 20);
  BoundBox3D bbMesh2 = _cutMesh1.GetBoundBox();

  unsigned long ctGx1, ctGy1, ctGz1;
  grid1.GetCtGrids(ctGx1, ctGy1, ctGz1);

  unsigned long gx1;
  for (gx1 = 0; gx1 < ctGx1; gx1++)  
  {
    unsigned long gy1;
    for (gy1 = 0; gy1 < ctGy1; gy1++)
    {
      unsigned long gz1;
      for (gz1 = 0; gz1 < ctGz1; gz1++)
      {
        if (grid1.GetCtElements(gx1, gy1, gz1) > 0)
        {
          std::vector<unsigned long> vecFacets2;
          grid2.InSide(grid1.GetBoundBox(gx1, gy1, gz1), vecFacets2);
      
          if (vecFacets2.size() > 0)
          {
            std::set<unsigned long> vecFacets1;
            grid1.GetElements(gx1, gy1, gz1, vecFacets1);
            
            std::set<unsigned long>::iterator it1;
            for (it1 = vecFacets1.begin(); it1 != vecFacets1.end(); it1++)
            {
              unsigned long fidx1 = *it1;
              MeshGeomFacet f1 = _cutMesh0.GetFacet(*it1);
              
              std::vector<unsigned long>::iterator it2;
              for (it2 = vecFacets2.begin(); it2 != vecFacets2.end(); it2++)
              {
                unsigned long fidx2 = *it2;
                MeshGeomFacet f2 = _cutMesh1.GetFacet(fidx2);

                // check if bounding boxes cuts
                if (f1.GetBoundBox() && f2.GetBoundBox())
                {
                  MeshPoint p0, p1;
                  if (f1.IntersectWithFacet(f2, p0, p1))
                  {            
                    facetsCuttingEdge0.insert(fidx1);
                    facetsCuttingEdge1.insert(fidx2);

                    // optimize cut line if distance to nearest point is too small
                    float minDist1 = _minDistanceToPoint, minDist2 = _minDistanceToPoint;
                    MeshPoint np0 = p0, np1 = p1;
                    int i;
                    for (i = 0; i < 3; i++)
                    {
                      float d1 = (f1._aclPoints[i] - p0).Length();
                      float d2 = (f1._aclPoints[i] - p1).Length();
                      if (d1 < minDist1)
                      {
                        minDist1 = d1;
                        np0 = f1._aclPoints[i];
                      }
                      if (d2 < minDist2)
                      {
                        minDist2 = d2;
                        p1 = f1._aclPoints[i];
                      }
                    } // for (int i = 0; i < 3; i++)

                    // optimize cut line if distance to nearest point is too small
                    for (i = 0; i < 3; i++)
                    {
                      float d1 = (f2._aclPoints[i] - p0).Length();
                      float d2 = (f2._aclPoints[i] - p1).Length();
                      if (d1 < minDist1)
                      {
                        minDist1 = d1;
                        np0 = f2._aclPoints[i];
                      }
                      if (d2 < minDist2)
                      {
                        minDist2 = d2;
                        np1 = f2._aclPoints[i];
                      }
                    } // for (int i = 0; i < 3; i++)

                    MeshPoint mp0 = np0;
                    MeshPoint mp1 = np1;

                    if (mp0 != mp1)
                    {
                      _edges.insert(EdgeInfo(mp0, mp1));
                      _facet2points[0][fidx1].push_back(mp0);
                      _facet2points[0][fidx1].push_back(mp1);
                      _facet2points[1][fidx2].push_back(mp0);
                      _facet2points[1][fidx2].push_back(mp1);
                    }
                    else
                    {
                      _facet2points[0][fidx1].push_back(mp0);
                      _facet2points[1][fidx2].push_back(mp0);
                    }

                  } // if (f1.IntersectWithFacet(f2, p0, p1))
                } // if (f1.GetBoundBox() && f2.GetBoundBox())
              } // for (it2 = vecFacets2.begin(); it2 != vecFacets2.end(); it2++)
            } // for (it1 = vecFacets1.begin(); it1 != vecFacets1.end(); it1++)
          } // if (vecFacets2.size() > 0)
        } // if (grid1.GetCtElements(gx1, gy1, gz1) > 0)
      } // for (gz1 = 0; gz1 < ctGz1; gz1++)
    } // for (gy1 = 0; gy1 < ctGy1; gy1++)
  } // for (gx1 = 0; gx1 < ctGx1; gx1++)  
}

void SetOperations::TriangulateMesh (MeshKernel &cutMesh, int side)
{

  // Triangulate Mesh 
  std::map<unsigned long, std::list<MeshPoint> >::iterator it1;
  for (it1 = _facet2points[side].begin(); it1 != _facet2points[side].end(); it1++)
  {
    std::vector<Vector3D> points;
    std::set<MeshPoint>   pointsSet;

    unsigned long fidx = it1->first;
    MeshGeomFacet f = cutMesh.GetFacet(fidx);

     // facet corner points
    MeshFacet& mf = cutMesh._aclFacetArray[fidx];
    int i;
    for (i = 0; i < 3; i++)
    {
      pointsSet.insert(f._aclPoints[i]);
      points.push_back(f._aclPoints[i]);
    }
    
    // triangulated facets
    std::list<MeshPoint>::iterator it2;
    for (it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
    {
      if (pointsSet.find(*it2) == pointsSet.end())
      {
        pointsSet.insert(*it2);
        points.push_back(*it2);
      }

    }

    Vector3D normal = f.GetNormal();
    Vector3D base = points[0];
    Vector3D dirX = points[1] - points[0];
    dirX.Normalize();
    Vector3D dirY = dirX % normal;

    triangulateio* in = new triangulateio();
    memset(in, 0, sizeof(triangulateio));
    in->pointlist = new float[points.size() * 2];
    in->numberofpoints = points.size();

    // project points to 2D plane
    i = 0;
    std::vector<Vector3D>::iterator it;
    for (it = points.begin(); it != points.end(); it++)
    {
      Vector3D pv = *it;
      pv.TransformToCoordinateSystem(base, dirX, dirY);
      in->pointlist[i++] = pv.x;
      in->pointlist[i++] = pv.y;
    }

    triangulateio* out = new triangulateio();
    memset(out, 0, sizeof(triangulateio));

    triangulate("z", in, out, NULL);

    for (i = 0; i < (out->numberoftriangles * 3); i += 3)
    {
 
      if ((out->trianglelist[i] == out->trianglelist[i+1]) || (out->trianglelist[i] == out->trianglelist[i+2]) || (out->trianglelist[i+1] == out->trianglelist[i+2]))
      { // two same triangle corner points
        continue;
      }
  
      MeshGeomFacet facet(points[out->trianglelist[i]], points[out->trianglelist[i+1]], points[out->trianglelist[i+2]]);

      if (facet.Area() < 0.0001f)
      { // too small facet
        continue;
      }

      facet.CalcNormal();
      if ((facet.GetNormal() * f.GetNormal()) < 0.0f)
      { // adjust normal
         std::swap(facet._aclPoints[0], facet._aclPoints[1]);
         facet.CalcNormal();
      }

      int j;
      for (j = 0; j < 3; j++)
      {
        std::set<EdgeInfo>::iterator eit = _edges.find(EdgeInfo(facet._aclPoints[j], facet._aclPoints[(j+1)%3]));
        if (eit != _edges.end())
        {
          if (eit->fcounter[side] < 2)
          {
            //FIXME: The three lines below lead to an compiler error meaning that a constant object cannot be modified.
            //       I think using an STL map with edge as key and info to edge as value would fix the problem. (Werner)
//            eit->facet[side] = fidx;
//            eit->facets[side][eit->fcounter[side]] = facet;
//            eit->fcounter[side]++;
            facet.SetFlag(MeshFacet::MARKED); // set all facets connected to an edge: MARKED

          }
        }
      }

      
      _newMeshFacets[side].push_back(facet);

    }
    // free all memory
    delete in->pointlist;
    delete in;
    delete out->trianglelist;
    delete out->pointlist;
    delete out->pointmarkerlist;
    delete out;
  }
}

 
void SetOperations::CollectFacets (int side, float mult)
{
  float distSave = MeshDefinitions::_fMinPointDistance;
  //MeshDefinitions::SetMinPointDistance(1.0e-4f);

  MeshKernel mesh;
  MeshBuilder mb(mesh);
  mb.Initialize(_newMeshFacets[side].size());
  std::vector<MeshGeomFacet>::iterator it;
  for (it = _newMeshFacets[side].begin(); it != _newMeshFacets[side].end(); it++)
  {
    mb.AddFacet(*it, true);
  }
  mb.Finish();

  MeshAlgorithm algo(mesh);
  algo.ResetFacetFlag((MeshFacet::TFlagType)(MeshFacet::VISIT | MeshFacet::TMP0));

   bool hasFacetsNotVisited = true; // until facets not visited
  // search for facet not visited
  TMeshFacetArray::iterator itf;
  for (itf = mesh._aclFacetArray.begin(); itf != mesh._aclFacetArray.end(); itf++)
  {
    if (!itf->IsFlag(MeshFacet::VISIT))
    { // Facet found, visit neighbours
      std::vector<unsigned long> facets;
      facets.push_back(itf - mesh._aclFacetArray.begin()); // add seed facet
      CollectFacetVisitor visitor(mesh, facets, _edges, side, mult, _builder); 
      mesh.VisitNeighbourFacets(visitor, itf - mesh._aclFacetArray.begin());
      
      if (visitor._addFacets == 0)
      { // mark all facets to add it to the result
        algo.SetFacetsFlag(facets, MeshFacet::TMP0);
      }
    }
  }

  // add all facets to the result vector
  for (itf = mesh._aclFacetArray.begin(); itf != mesh._aclFacetArray.end(); itf++)
  {
    if (itf->IsFlag(MeshFacet::TMP0))
    {
      _facetsOf[side].push_back(mesh.GetFacet(*itf));
    }
  }

  MeshDefinitions::SetMinPointDistance(distSave);
}

SetOperations::CollectFacetVisitor::CollectFacetVisitor (MeshKernel& mesh, std::vector<unsigned long>& facets, std::set<EdgeInfo>& edges, int side, float mult , Base::Builder3D& builder )
: _mesh(mesh),
  _facets(facets),
  _edges(edges),
  _side(side),
  _mult(mult),
  _addFacets(-1)
  ,_builder(builder)
{
}

bool SetOperations::CollectFacetVisitor::Visit (MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long ulLevel)
{
  _facets.push_back(ulFInd);
  return true;
}

bool SetOperations::CollectFacetVisitor::AllowVisit (MeshFacet& rclFacet, MeshFacet& rclFrom, unsigned long ulFInd, unsigned long ulLevel, unsigned short neighbourIndex)
{
  if (rclFacet.IsFlag(MeshFacet::MARKED) && rclFrom.IsFlag(MeshFacet::MARKED))
  { // facet connected to an edge
    unsigned long pt0 = rclFrom._aulPoints[neighbourIndex], pt1 = rclFrom._aulPoints[(neighbourIndex+1)%3];
    EdgeInfo edge(_mesh.GetPoint(pt0), _mesh.GetPoint(pt1));

    std::set<EdgeInfo>::iterator it = _edges.find(edge);

    if (it != _edges.end())
    {
      if (_addFacets == -1)
      { // detemine if the facets shoud add or not only once
        MeshGeomFacet facet = _mesh.GetFacet(rclFacet); // triangulated facet
        MeshGeomFacet facetOther = it->facets[1-_side][0]; // triangulated facet from same edge and other mesh
        Vector3D normalOther = facetOther.GetNormal();

        Vector3D edgeDir = it->pt1 - it->pt2;
        Vector3D ocDir = (edgeDir % (facet.GetGravityPoint() - it->pt1)) % edgeDir;
        ocDir.Normalize();
             
       //_builder.addSingleTriangle(facet._aclPoints[0], facet._aclPoints[1], facet._aclPoints[2], 3.0, 0.9, 0.0, 0.1);
       //_builder.addSingleTriangle(facetOther._aclPoints[0], facetOther._aclPoints[1], facetOther._aclPoints[2], 3.0, 0.1, 0.0, 0.9);

        bool match = ((ocDir * normalOther) * _mult) < 0.0f;
        if (match)
          _addFacets = 0;
        else
          _addFacets = 1;
      }

      return false;
    }    
  }

  return true;
}

