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
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_IntCS.hxx>

using namespace Mesh;




void CurveProjector::cutByShape(const TopoDS_Shape &aShape, MeshWithProperty* pMesh)
{
  TopExp_Explorer Ex;
  TopoDS_Shape Edge;
  std::vector<Vector3D> vEdgePolygon;

  for (Ex.Init(aShape, TopAbs_EDGE); Ex.More(); Ex.Next())
  {
	  const TopoDS_Edge& aEdge = TopoDS::Edge(Ex.Current());
    GetSampledCurves(aEdge,vEdgePolygon,30);

    std::vector<FaceSplitEdge> vSplitEdges;
    projectCurve(pMesh,aEdge,vEdgePolygon, vSplitEdges);

//    cutByCurve(pMesh,vSplitEdges);

  }

}


void CurveProjector::GetSampledCurves( const TopoDS_Edge& aEdge, std::vector<Vector3D>& rclPoints, unsigned long ulNbOfPoints)
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


void CurveProjector::projectCurve( MeshWithProperty* pMesh,
                              const TopoDS_Edge& aEdge,
                              const std::vector<Vector3D> &rclPoints, 
                              std::vector<FaceSplitEdge> &vSplitEdges)
{
  MeshKernel &MeshK = *(pMesh->getKernel());
  Standard_Real                          fFirst, fLast;
  Handle(Geom_Curve) hCurve = BRep_Tool::Curve( aEdge,fFirst,fLast );
  
  // geting starting point
  gp_Pnt gpPt = hCurve->Value(fFirst);

  // projection of the first point 
  Vector3D StartingPoint = Vector3D(gpPt.X(),gpPt.Y(),gpPt.Z());
  Vector3D ResultPoint, PlanePnt,PlaneNormal;
  unsigned long StartFacetIdx,ActFacetIdx,LastActFacetIdx=ULONG_MAX-1;
  unsigned long NighboursIdx[3];
  bool GoOn;
  
  if(! projectPointToMesh(MeshK,StartingPoint,ResultPoint,StartFacetIdx) )
    return;
  ActFacetIdx = StartFacetIdx;
  do{
    MeshGeomFacet ActFacet= MeshK.GetFacet(ActFacetIdx);
    MeshK.GetFacetNeighbours ( ActFacetIdx, NighboursIdx[0], NighboursIdx[1], NighboursIdx[2]);
    Vector3D PointOnEdge[3];

    GoOn = false;
    int NbrOfHits = 0,HitIdx;
    
    for(int i=0; i<3; i++)
    {
      // ignore last visited Facet
      if(NighboursIdx[i] == LastActFacetIdx ) 
        continue;

      if(NighboursIdx[i] != ULONG_MAX)
      {
        MeshGeomFacet N = MeshK.GetFacet(NighboursIdx[i]);
        // calculate the normal by the edge vector and the middle between the two face normales
        PlaneNormal = (N.GetNormal() + ActFacet.GetNormal()) % (ActFacet._aclPoints[(i+1)%3] - ActFacet._aclPoints[i]);
        PlanePnt    = ActFacet._aclPoints[i];
      }else{
        // with no neighbours the plane face normal is used
        PlaneNormal = ActFacet.GetNormal() % (ActFacet._aclPoints[(i+1)%3] - ActFacet._aclPoints[i]);
        PlanePnt    = ActFacet._aclPoints[i];
      }

      Handle(Geom_Plane) hPlane = new Geom_Plane(gp_Pln(gp_Pnt(PlanePnt.x,PlanePnt.y,PlanePnt.z), 
                                                       gp_Dir(PlaneNormal.x,PlaneNormal.y,PlaneNormal.z)));

      GeomAPI_IntCS Alg(hCurve,hPlane); 

      if(Alg.IsDone())
      {
        // deciding by the number of result points (intersections)
        if( Alg.NbPoints() == 1)
        {
          gp_Pnt P = Alg.Point(1);
          float l = ((Vector3D(P.X(),P.Y(),P.Z())  - ActFacet._aclPoints[i]) * (ActFacet._aclPoints[(i+1)%3] - ActFacet._aclPoints[i])) / 
                    ((ActFacet._aclPoints[(i+1)%3] - ActFacet._aclPoints[i]) * (ActFacet._aclPoints[(i+1)%3] - ActFacet._aclPoints[i]));
          // is the Point on the Edge of the facet?
          if(l<0.0 || l>1.0)
            PointOnEdge[i] = Vector3D(FLOAT_MAX,0,0);
          else{
            PointOnEdge[i] = (1-l)*ActFacet._aclPoints[i] + l * ActFacet._aclPoints[(i+1)%3];
            NbrOfHits ++;
            HitIdx = i;
          }
        // no intersection
        }else if(Alg.NbPoints() == 0){
          PointOnEdge[i] = Vector3D(FLOAT_MAX,0,0);
        // more the one intersection (@ToDo)
        }else if(Alg.NbPoints() > 1){
          PointOnEdge[i] = Vector3D(FLOAT_MAX,0,0);
          Base::Console().Log("MeshAlgos::projectCurve(): More then one intersection in Facet %ld, Edge %d\n",ActFacetIdx,i);
        }

      }

	
    }

    LastActFacetIdx = ActFacetIdx;
 
    if(NbrOfHits = 1)
    {
      ActFacetIdx = NighboursIdx[HitIdx];
      GoOn = true;
    }else{
        Base::Console().Log("MeshAlgos::projectCurve(): Posibel reentry in Facet %ld\n",ActFacetIdx);
    }

    if(ActFacetIdx == StartFacetIdx)
      GoOn = false;

  }while(GoOn);

  // cycling through the points on the edge
  for( std::vector<Vector3D>::const_iterator It = rclPoints.begin()+1;It!=rclPoints.end();++It)
  {



  }

}

bool CurveProjector::projectPointToMesh(MeshKernel &MeshK,const Vector3D &Pnt,Vector3D &Rslt,unsigned long &FaceIndex)
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





