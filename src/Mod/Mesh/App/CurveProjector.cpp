/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
# ifdef FC_OS_LINUX
#	  include <unistd.h>
# endif
# include <gp_Pln.hxx>
#endif


#include "MeshAlgos.h"
#include "Mesh.h"
#include "MeshCurvature.h"
#include "CurveProjector.h"

#include "Core/MeshIO.h"
#include "Core/Stream.h"
#include "Core/MeshKernel.h"
#include "Core/Iterator.h"
#include "Core/Algorithm.h"

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Console.h>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_IntCS.hxx>

using namespace Mesh;




CurveProjector::CurveProjector(const TopoDS_Shape &aShape, const MeshWithProperty &pMesh)
: _Shape(aShape), _Mesh(pMesh)
{
}

void CurveProjector::writeIntersectionPointsToFile(const char *name)
{
  // export points
  FILE* file = fopen(name, "w");
  for (result_type::const_iterator it1 = mvEdgeSplitPoints.begin();it1!=mvEdgeSplitPoints.end();++it1)
    for (std::vector<FaceSplitEdge>::const_iterator it2 = it1->second.begin();it2!=it1->second.end();++it2)
    {
      fprintf(file, "%.4f %.4f %.4f\n", it2->p1.x, it2->p1.y, it2->p1.z);
    }

  fclose(file);
}


//**************************************************************************
//**************************************************************************
// Seperator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CurveProjectorShape::CurveProjectorShape(const TopoDS_Shape &aShape, const MeshWithProperty &pMesh)
: CurveProjector(aShape,pMesh)
{
  Do();
}

void CurveProjectorShape::Do(void)
{
  TopExp_Explorer Ex;
  TopoDS_Shape Edge;

  for (Ex.Init(_Shape, TopAbs_EDGE); Ex.More(); Ex.Next())
  {
	  const TopoDS_Edge& aEdge = TopoDS::Edge(Ex.Current());

    //std::vector<FaceSplitEdge> vSplitEdges;
    projectCurve(aEdge, mvEdgeSplitPoints[aEdge]);

  }

}


void CurveProjectorShape::projectCurve( const TopoDS_Edge& aEdge,
                                        std::vector<FaceSplitEdge> &vSplitEdges)
{
  const MeshKernel &MeshK = *(_Mesh.getKernel());

  Standard_Real fFirst, fLast;
  Handle(Geom_Curve) hCurve = BRep_Tool::Curve( aEdge,fFirst,fLast );
  
  // getting start point
  gp_Pnt gpPt = hCurve->Value(fFirst);

  // projection of the first point 
  Vector3D cStartPoint = Vector3D(gpPt.X(),gpPt.Y(),gpPt.Z());
  Vector3D cResultPoint, cSplitPoint, cPlanePnt, cPlaneNormal;
  unsigned long uStartFacetIdx,uCurFacetIdx;
  unsigned long uLastFacetIdx=ULONG_MAX-1; // use another value as ULONG_MAX
  unsigned long auNeighboursIdx[3];
  bool GoOn;
  
  if( !projectPointToMesh(MeshK,cStartPoint,cResultPoint,uStartFacetIdx) )
    return;
  uCurFacetIdx = uStartFacetIdx;
  do{
    MeshGeomFacet cCurFacet= MeshK.GetFacet(uCurFacetIdx);
    MeshK.GetFacetNeighbours ( uCurFacetIdx, auNeighboursIdx[0], auNeighboursIdx[1], auNeighboursIdx[2]);
    Vector3D PointOnEdge[3];

    GoOn = false;
    int NbrOfHits = 0,HitIdx;
    
    for(int i=0; i<3; i++)
    {
      // ignore last visited facet
      if ( auNeighboursIdx[i] == uLastFacetIdx ) 
        continue;

      // get points of the edge i
      const Vector3D& cP0 = cCurFacet._aclPoints[i];
      const Vector3D& cP1 = cCurFacet._aclPoints[(i+1)%3];

      if ( auNeighboursIdx[i] != ULONG_MAX )
      {
        // calculate the normal by the edge vector and the middle between the two face normals
        MeshGeomFacet N = MeshK.GetFacet( auNeighboursIdx[i] );
        cPlaneNormal = ( N.GetNormal() + cCurFacet.GetNormal() ) % ( cP1 - cP0 );
        cPlanePnt    = cP0;
      }else{
        // with no neighbours the face normal is used
        cPlaneNormal = cCurFacet.GetNormal() % ( cP1 - cP0 );
        cPlanePnt    = cP0;
      }

      Handle(Geom_Plane) hPlane = new Geom_Plane(gp_Pln(gp_Pnt(cPlanePnt.x,cPlanePnt.y,cPlanePnt.z), 
                                                        gp_Dir(cPlaneNormal.x,cPlaneNormal.y,cPlaneNormal.z)));
 
      GeomAPI_IntCS Alg(hCurve,hPlane); 

      if ( Alg.IsDone() )
      {
        // deciding by the number of result points (intersections)
        if( Alg.NbPoints() == 1)
        {
          gp_Pnt P = Alg.Point(1);
          float l = ( (Vector3D(P.X(),P.Y(),P.Z())  - cP0 ) * (cP1 - cP0) ) / ( (cP1 - cP0) * ( cP1 - cP0) );
          // is the Point on the Edge of the facet?
          if(l<0.0 || l>1.0)
            PointOnEdge[i] = Vector3D(FLOAT_MAX,0,0);
          else{
            cSplitPoint = (1-l) * cP0 + l * cP1;
            PointOnEdge[i] = (1-l)*cP0 + l * cP1;
            NbrOfHits ++;
            HitIdx = i;
          }
        // no intersection
        }else if(Alg.NbPoints() == 0){
          PointOnEdge[i] = Vector3D(FLOAT_MAX,0,0);
        // more the one intersection (@ToDo)
        }else if(Alg.NbPoints() > 1){
          PointOnEdge[i] = Vector3D(FLOAT_MAX,0,0);
          Base::Console().Log("MeshAlgos::projectCurve(): More then one intersection in Facet %ld, Edge %d\n",uCurFacetIdx,i);
        }
      }
    }

    uLastFacetIdx = uCurFacetIdx;

    if(NbrOfHits == 1)
    {
      uCurFacetIdx = auNeighboursIdx[HitIdx];
      FaceSplitEdge splitEdge;
      splitEdge.ulFaceIndex = uCurFacetIdx;
      splitEdge.p1 = cResultPoint;
      splitEdge.p2 = cSplitPoint;
      vSplitEdges.push_back( splitEdge );
      cResultPoint = cSplitPoint;
      GoOn = true;
    }else{
      Base::Console().Log("MeshAlgos::projectCurve(): Posibel reentry in Facet %ld\n", uCurFacetIdx);
    }

    if( uCurFacetIdx == uStartFacetIdx )
      GoOn = false;

  }while(GoOn);

}

bool CurveProjectorShape::projectPointToMesh(const MeshKernel &MeshK,const Vector3D &Pnt,Vector3D &Rslt,unsigned long &FaceIndex)
{
  Vector3D TempResultPoint;
  float MinLength = FLOAT_MAX;
  bool bHit = false;

  // go through the whole Mesh
  MeshFacetIterator It(MeshK);
  for(It.Init();It.More();It.Next())
  {
    // try to project (with angle) to the face
    if(MeshFacetFunc::Foraminate (*It, Pnt, It->GetNormal(), TempResultPoint) )
    {
      // distance to the projected point
      float Dist = (Pnt-TempResultPoint).Length();
      if(Dist < MinLength)
      {
        // remember the point with the closest distance
        bHit = true;
        MinLength = Dist;
        Rslt = TempResultPoint;
        FaceIndex = It.Position();
      }
    }
  }
  return bHit;
}


//**************************************************************************
//**************************************************************************
// Seperator for CurveProjectorSimple classe
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


CurveProjectorSimple::CurveProjectorSimple(const TopoDS_Shape &aShape, const MeshWithProperty &pMesh)
: CurveProjector(aShape,pMesh)
{
  Do();
}


void CurveProjectorSimple::Do(void)
{
  TopExp_Explorer Ex;
  TopoDS_Shape Edge;

  std::vector<Vector3D> vEdgePolygon;

  for (Ex.Init(_Shape, TopAbs_EDGE); Ex.More(); Ex.Next())
  {
	  const TopoDS_Edge& aEdge = TopoDS::Edge(Ex.Current());
    GetSampledCurves(aEdge,vEdgePolygon,30);

    //std::vector<FaceSplitEdge> vSplitEdges;
    projectCurve(aEdge,vEdgePolygon, mvEdgeSplitPoints[aEdge]);

  }

}


void CurveProjectorSimple::GetSampledCurves( const TopoDS_Edge& aEdge, std::vector<Vector3D>& rclPoints, unsigned long ulNbOfPoints)
{
  rclPoints.clear();

    Standard_Real fBegin, fEnd;

    Handle(Geom_Curve) hCurve = BRep_Tool::Curve(aEdge,fBegin,fEnd);
    float fLen   = float(fEnd - fBegin);

    for (unsigned long i = 0; i <= ulNbOfPoints; i++)
    {
      gp_Pnt gpPt = hCurve->Value(fBegin + (fLen * float(i)) / float(ulNbOfPoints));
      rclPoints.push_back(Vector3D(gpPt.X(),gpPt.Y(),gpPt.Z()));
    }
}


void CurveProjectorSimple::projectCurve( const TopoDS_Edge& aEdge,
                                   const std::vector<Vector3D> &rclPoints, 
                                   std::vector<FaceSplitEdge> &vSplitEdges)
{
  const MeshKernel &MeshK = *(_Mesh.getKernel());

  Standard_Real fFirst, fLast;
  Handle(Geom_Curve) hCurve = BRep_Tool::Curve( aEdge,fFirst,fLast );
  
  // getting start point
  gp_Pnt gpPt = hCurve->Value(fFirst);

  // projection of the first point 
  Vector3D cStartPoint = Vector3D(gpPt.X(),gpPt.Y(),gpPt.Z());
  Vector3D cResultPoint, cSplitPoint, cPlanePnt, cPlaneNormal;
  unsigned long uStartFacetIdx,uCurFacetIdx;
  unsigned long uLastFacetIdx=ULONG_MAX-1; // use another value as ULONG_MAX
  unsigned long auNeighboursIdx[3];
  bool GoOn;
  
  if( !projectPointToMesh(MeshK,cStartPoint,cResultPoint,uStartFacetIdx) )
    return;

  // cycling through the points on the edge
  for( std::vector<Vector3D>::const_iterator It = rclPoints.begin()+1;It!=rclPoints.end();++It)
  {



  }

}

bool CurveProjectorSimple::projectPointToMesh(const MeshKernel &MeshK,const Vector3D &Pnt,Vector3D &Rslt,unsigned long &FaceIndex)
{
  Vector3D TempResultPoint;
  float MinLength = FLOAT_MAX;
  bool bHit = false;

  // go through the whole Mesh
  MeshFacetIterator It(MeshK);
  for(It.Init();It.More();It.Next())
  {
    // try to project (with angle) to the face
    if(MeshFacetFunc::Foraminate (*It, Pnt, It->GetNormal(), TempResultPoint) )
    {
      // distance to the projected point
      float Dist = (Pnt-TempResultPoint).Length();
      if(Dist < MinLength)
      {
        // remember the point with the closest distance
        bHit = true;
        MinLength = Dist;
        Rslt = TempResultPoint;
        FaceIndex = It.Position();
      }
    }
  }
  return bHit;
}





