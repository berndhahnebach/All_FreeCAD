/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#endif

#include <Mod/Mesh/App/WildMagic4/Wm4Matrix3.h>
#include <Mod/Mesh/App/WildMagic4/Wm4Vector3.h>

#include "Triangulation.h"
#include "MeshKernel.h"
#include "triangle.h"


using namespace MeshCore;


float MeshPolygonTriangulation::Triangulate::Area(const std::vector<Base::Vector3f> &contour)
{
  int n = contour.size();

  float A=0.0f;

  for(int p=n-1,q=0; q<n; p=q++)
  {
    A+= contour[p].x*contour[q].y - contour[q].x*contour[p].y;
  }
  return A*0.5f;
}

/*
  InsideTriangle decides if a point P is Inside of the triangle
  defined by A, B, C.
*/
bool MeshPolygonTriangulation::Triangulate::InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
{
  float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  float cCROSSap, bCROSScp, aCROSSbp;

  ax = Cx - Bx;  ay = Cy - By;
  bx = Ax - Cx;  by = Ay - Cy;
  cx = Bx - Ax;  cy = By - Ay;
  apx= Px - Ax;  apy= Py - Ay;
  bpx= Px - Bx;  bpy= Py - By;
  cpx= Px - Cx;  cpy= Py - Cy;

  aCROSSbp = ax*bpy - ay*bpx;
  cCROSSap = cx*apy - cy*apx;
  bCROSScp = bx*cpy - by*cpx;

  return ((aCROSSbp >= FLOAT_EPS) && (bCROSScp >= FLOAT_EPS) && (cCROSSap >= FLOAT_EPS));
}

bool MeshPolygonTriangulation::Triangulate::Snip(const std::vector<Base::Vector3f> &contour,int u,int v,int w,int n,int *V)
{
  int p;
  float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

  Ax = contour[V[u]].x;
  Ay = contour[V[u]].y;

  Bx = contour[V[v]].x;
  By = contour[V[v]].y;

  Cx = contour[V[w]].x;
  Cy = contour[V[w]].y;

  if ( FLOAT_EPS > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

  for (p=0;p<n;p++)
  {
    if( (p == u) || (p == v) || (p == w) ) continue;
    Px = contour[V[p]].x;
    Py = contour[V[p]].y;
    if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
  }

  return true;
}

bool MeshPolygonTriangulation::Triangulate::_invert = false;

bool MeshPolygonTriangulation::Triangulate::Process(const std::vector<Base::Vector3f> &contour, std::vector<unsigned long> &result)
{
  /* allocate and initialize list of Vertices in polygon */

  int n = contour.size();
  if ( n < 3 ) return false;

  int *V = new int[n];

  /* we want a counter-clockwise polygon in V */

  if ( 0.0f < Area(contour) )
  {
    for (int v=0; v<n; v++) V[v] = v;
    _invert = true;
  }
//    for(int v=0; v<n; v++) V[v] = (n-1)-v;
  else
  {
    for(int v=0; v<n; v++) V[v] = (n-1)-v;
    _invert = false;
  }

  int nv = n;

  /*  remove nv-2 Vertices, creating 1 triangle every time */
  int count = 2*nv;   /* error detection */

  for(int m=0, v=nv-1; nv>2; )
  {
    /* if we loop, it is probably a non-simple polygon */
    if (0 >= (count--))
    {
      //** Triangulate: ERROR - probable bad polygon!
      return false;
    }

    /* three consecutive vertices in current polygon, <u,v,w> */
    int u = v  ; if (nv <= u) u = 0;     /* previous */
    v = u+1; if (nv <= v) v = 0;     /* new v    */
    int w = v+1; if (nv <= w) w = 0;     /* next     */

    if ( Snip(contour,u,v,w,nv,V) )
    {
      int a,b,c,s,t;

      /* true names of the vertices */
      a = V[u]; b = V[v]; c = V[w];

      /* output Triangle */
      result.push_back( a );
      result.push_back( b );
      result.push_back( c );

      m++;

      /* remove v from remaining polygon */
      for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

      /* resest error detection counter */
      count = 2*nv;
    }
  }

  delete V;

  return true;
}

//----------------------------------------------------------------------------

MeshPolygonTriangulation::MeshPolygonTriangulation()
{
}

MeshPolygonTriangulation::MeshPolygonTriangulation(const std::vector<Base::Vector3f>& raclPoints)
  : _points(raclPoints)
{
}

MeshPolygonTriangulation::~MeshPolygonTriangulation()
{
}

bool MeshPolygonTriangulation::Compute()
{
  _facets.clear();
  _triangles.clear();

  std::vector<Base::Vector3f> pts;
  std::vector<unsigned long> result;
  for ( std::vector<Base::Vector3f>::iterator it = _points.begin(); it != _points.end(); ++it)
  {
    pts.push_back(*it);
  }

  //  Invoke the triangulator to triangulate this polygon.
  Triangulate::Process(pts,result);

  // print out the results.
  unsigned long tcount = result.size()/3;

  bool ok = tcount+2 == _points.size();
  if  ( tcount > _points.size() )
    return false; // no valid triangulation

  MeshGeomFacet clFacet;
  MeshFacet clTopFacet;
  for (unsigned long i=0; i<tcount; i++)
  {
    if ( Triangulate::_invert )
    {
      clFacet._aclPoints[0] = _points[result[i*3+0]];
      clFacet._aclPoints[2] = _points[result[i*3+1]];
      clFacet._aclPoints[1] = _points[result[i*3+2]];
      clTopFacet._aulPoints[0] = result[i*3+0];
      clTopFacet._aulPoints[2] = result[i*3+1];
      clTopFacet._aulPoints[1] = result[i*3+2];
    }
    else
    {
      clFacet._aclPoints[0] = _points[result[i*3+0]];
      clFacet._aclPoints[1] = _points[result[i*3+1]];
      clFacet._aclPoints[2] = _points[result[i*3+2]];
      clTopFacet._aulPoints[0] = result[i*3+0];
      clTopFacet._aulPoints[1] = result[i*3+1];
      clTopFacet._aulPoints[2] = result[i*3+2];
    }

    _triangles.push_back(clFacet);
    _facets.push_back(clTopFacet);
  }

  return ok;
}

bool MeshPolygonTriangulation::ComputeQuasiDelaunay()
{
  if ( Compute() == false )
    return false; // no valid triangulation

  // For each internal edge get the adjacent facets. When doing an edge swap we must update
  // this structure.
  std::map<std::pair<unsigned long, unsigned long>, std::vector<unsigned long> > aEdge2Face;
  for (std::vector<MeshFacet>::iterator pI = _facets.begin(); pI != _facets.end(); pI++)
  {
    for (int i = 0; i < 3; i++)
    {
      unsigned long ulPt0 = std::min<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(pI->_aulPoints[i],  pI->_aulPoints[(i+1)%3]);
      // ignore borderlines of the polygon
      if ( (ulPt1-ulPt0)%(_points.size()-1) > 1 )
        aEdge2Face[std::pair<unsigned long, unsigned long>(ulPt0, ulPt1)].push_back(pI - _facets.begin());
    }
  }

  // fill up this list with all internal edges and perform swap edges until this list is empty
  std::list<std::pair<unsigned long, unsigned long> > aEdgeList;
  std::map<std::pair<unsigned long, unsigned long>, std::vector<unsigned long> >::iterator pE;
  for (pE = aEdge2Face.begin(); pE != aEdge2Face.end(); ++pE)
    aEdgeList.push_back(pE->first);

  // to be sure to avoid an endless loop
  unsigned long uMaxIter = 5 * aEdge2Face.size();

  // Perform a swap edge where needed
  while ( !aEdgeList.empty() && uMaxIter > 0 ) {
    // get the first edge and remove it from the list
    std::pair<unsigned long, unsigned long> aEdge = aEdgeList.front();
    aEdgeList.pop_front();
    uMaxIter--;

    // get the adjacent facets to this edge
    pE = aEdge2Face.find( aEdge );

    // this edge has been removed some iterations before
    if( pE == aEdge2Face.end() )
      continue;

    MeshFacet& rF1 = _facets[pE->second[0]];
    MeshFacet& rF2 = _facets[pE->second[1]];
    unsigned short side1 = rF1.Side(aEdge.first, aEdge.second);

    Base::Vector3f cP1 = _points[rF1._aulPoints[side1]];
    Base::Vector3f cP2 = _points[rF1._aulPoints[(side1+1)%3]];
    Base::Vector3f cP3 = _points[rF1._aulPoints[(side1+2)%3]];

    unsigned short side2 = rF2.Side(aEdge.first, aEdge.second);
    Base::Vector3f cP4 = _points[rF2._aulPoints[(side2+2)%3]];

    MeshGeomFacet cT1(cP1, cP2, cP3); float fMax1 = cT1.MaximumAngle();
    MeshGeomFacet cT2(cP2, cP1, cP4); float fMax2 = cT2.MaximumAngle();
    MeshGeomFacet cT3(cP4, cP3, cP1); float fMax3 = cT3.MaximumAngle();
    MeshGeomFacet cT4(cP3, cP4, cP2); float fMax4 = cT4.MaximumAngle();

    float fMax12 = std::max<float>(fMax1, fMax2);
    float fMax34 = std::max<float>(fMax3, fMax4);

    // We must make sure that the two adjacent triangles builds a convex polygon, otherwise 
    // the swap edge operation is illegal
    Base::Vector3f cU = cP2-cP1;
    Base::Vector3f cV = cP4-cP3;
    // build a helper plane through cP1 that must separate cP3 and cP4
    Base::Vector3f cN1 = (cU % cV) % cU;
    if ( ((cP3-cP1)*cN1)*((cP4-cP1)*cN1) >= 0.0f )
      continue; // not convex
    // build a helper plane through cP3 that must separate cP1 and cP2
    Base::Vector3f cN2 = (cU % cV) % cV;
    if ( ((cP1-cP3)*cN2)*((cP2-cP3)*cN2) >= 0.0f )
      continue; // not convex

    // ok, here we should perform a swap edge to minimize the maximum angle
    if ( fMax12 > fMax34 ) {
      rF1._aulPoints[(side1+1)%3] = rF2._aulPoints[(side2+2)%3];
      rF2._aulPoints[(side2+1)%3] = rF1._aulPoints[(side1+2)%3];

      // adjust the edge list
      for ( int i=0; i<3; i++ ) {
        std::map<std::pair<unsigned long, unsigned long>, std::vector<unsigned long> >::iterator it;
        // first facet
        unsigned long ulPt0 = std::min<unsigned long>(rF1._aulPoints[i],  rF1._aulPoints[(i+1)%3]);
        unsigned long ulPt1 = std::max<unsigned long>(rF1._aulPoints[i],  rF1._aulPoints[(i+1)%3]);
        it = aEdge2Face.find( std::make_pair(ulPt0, ulPt1) );
        if ( it != aEdge2Face.end() ) {
          if ( it->second[0] == pE->second[1] )
            it->second[0] = pE->second[0];
          else if ( it->second[1] == pE->second[1] )
            it->second[1] = pE->second[0];
          aEdgeList.push_back( it->first );
        }

        // second facet
        ulPt0 = std::min<unsigned long>(rF2._aulPoints[i],  rF2._aulPoints[(i+1)%3]);
        ulPt1 = std::max<unsigned long>(rF2._aulPoints[i],  rF2._aulPoints[(i+1)%3]);
        it = aEdge2Face.find( std::make_pair(ulPt0, ulPt1) );
        if ( it != aEdge2Face.end() ) {
          if ( it->second[0] == pE->second[0] )
            it->second[0] = pE->second[1];
          else if ( it->second[1] == pE->second[0] )
            it->second[1] = pE->second[1];
          aEdgeList.push_back( it->first );
        }
      }

      // Now we must remove the edge and replace it through the new edge
      unsigned long ulPt0 = std::min<unsigned long>(rF1._aulPoints[(side1+1)%3], rF2._aulPoints[(side2+1)%3]);
      unsigned long ulPt1 = std::max<unsigned long>(rF1._aulPoints[(side1+1)%3], rF2._aulPoints[(side2+1)%3]);
      std::pair<unsigned long, unsigned long> aNewEdge = std::make_pair(ulPt0, ulPt1);
      aEdge2Face[aNewEdge] = pE->second;
      aEdge2Face.erase(pE);
    }
  }

  return true;
}

namespace MeshCore {
namespace Triangulation {
struct Vertex2d_Less  : public std::binary_function<const Base::Vector3f&, const Base::Vector3f&, bool>
{
    bool operator()(const Base::Vector3f& p, const Base::Vector3f& q) const
    {
        if (fabs(p.x - q.x) < MeshDefinitions::_fMinPointDistanceD1) {
        if (fabs(p.y - q.y) < MeshDefinitions::_fMinPointDistanceD1) {
        return false; } else return p.y < q.y;
        } else return p.x < q.x; return true;
    }
};
struct Vertex2d_EqualTo  : public std::binary_function<const Base::Vector3f&, const Base::Vector3f&, bool>
{
    bool operator()(const Base::Vector3f& p, const Base::Vector3f& q) const
    {
        if (fabs(p.x - q.x) < MeshDefinitions::_fMinPointDistanceD1
        &&  fabs(p.y - q.y) < MeshDefinitions::_fMinPointDistanceD1)
        return true; return false;
    }
};
}
}

bool MeshPolygonTriangulation::ComputeConstrainedDelaunay(float fMaxArea, std::vector<Base::Vector3f>& newPoints)
{
    // before starting the triangulation we must make sure that all polygon points are different
    //
    std::vector<Base::Vector3f> aPoints = _points;
    // sort the points ascending x,y coordinates
    std::sort(aPoints.begin(), aPoints.end(), Triangulation::Vertex2d_Less());
    // if there are two adjacent points whose distance is less then an epsilon
    if (std::adjacent_find(aPoints.begin(), aPoints.end(), Triangulation::Vertex2d_EqualTo()) < aPoints.end() )
        return false;

    _facets.clear();
    _triangles.clear();

    triangulateio* in = new triangulateio();
    memset(in, 0, sizeof(triangulateio));
    in->pointlist = new float[_points.size() * 2];
    in->numberofpoints = _points.size();
    in->segmentlist = new int[_points.size() * 2];
    in->numberofsegments = _points.size();

    // build up point list
    int i = 0, j = 0, k = 0;
    int mod = _points.size();
    for (std::vector<Base::Vector3f>::iterator it = _points.begin(); it != _points.end(); it++) {
        in->pointlist[i++] = it->x;
        in->pointlist[i++] = it->y;
        in->segmentlist[j++] = k;
        in->segmentlist[j++] = (k+1)%mod;
        k++;
    }

    // set the triangulation parameter
    // p: define a polygon to triangulate
    // Y: do not insert new points to the boundary
    // z: start index from 0
    // q: create quality triangles
    // a: max. area of triangles
    triangulateio* out = new triangulateio();
    memset(out, 0, sizeof(triangulateio));
    char szBuf[50];
    snprintf(szBuf, 50, "pYzqa%.6f", fMaxArea);
    triangulate(szBuf, in, out, NULL);

    // get all added points by the algorithm
    for (int index = 2 * _points.size(); index < (out->numberofpoints * 2); ) {
        float x = out->pointlist[index++];
        float y = out->pointlist[index++];
        Base::Vector3f vertex(x, y, 0.0f);
        newPoints.push_back(vertex);
    }

    // get the triangles
    MeshGeomFacet triangle;
    MeshFacet facet;
    bool succeeded = out->numberoftriangles > 0;
    for (i = 0; i < (out->numberoftriangles * 3); i += 3) {
        if ((out->trianglelist[i] == out->trianglelist[i+1]) || 
            (out->trianglelist[i] == out->trianglelist[i+2]) || 
            (out->trianglelist[i+1] == out->trianglelist[i+2])) {
            // two same triangle corner points
            succeeded = false;
            break;
        }
        
        for (int j = 0; j < 3; j++) {
            int index = 2 * out->trianglelist[i+j];
            triangle._aclPoints[j].x = out->pointlist[index];
            triangle._aclPoints[j].y = out->pointlist[index+1];
            facet._aulPoints[j] = out->trianglelist[i+j];
        }
        
        _triangles.push_back(triangle);
        _facets.push_back(facet);
    }

    // free all memory
    delete in->pointlist;
    delete in->segmentlist;
    delete in;
    delete out->trianglelist;
    delete out->pointlist;
    delete out->pointmarkerlist;
    delete out;

    return succeeded;
}

void MeshPolygonTriangulation::SetPolygon(const std::vector<Base::Vector3f>& raclPoints)
{
    _points = raclPoints;
    if (_points.size() > 0) {
        if (_points.front() == _points.back())
            _points.pop_back();
    }
}

std::vector<Base::Vector3f> MeshPolygonTriangulation::GetPolygon() const
{
    return _points;
}

Base::Vector3f MeshPolygonTriangulation::TransformToFitPlane(Base::Matrix4D& rInverse)
{
    float sxx,sxy,sxz,syy,syz,szz,mx,my,mz;
    sxx=sxy=sxz=syy=syz=szz=mx=my=mz=0.0f;

    for ( std::vector<Base::Vector3f>::iterator it = _points.begin(); it!=_points.end(); ++it) {
        sxx += it->x * it->x; sxy += it->x * it->y;
        sxz += it->x * it->z; syy += it->y * it->y;
        syz += it->y * it->z; szz += it->z * it->z;
        mx += it->x; my += it->y; mz += it->z;
    }

    unsigned int nSize = _points.size();
    sxx = sxx - mx*mx/((float)nSize);
    sxy = sxy - mx*my/((float)nSize);
    sxz = sxz - mx*mz/((float)nSize);
    syy = syy - my*my/((float)nSize);
    syz = syz - my*mz/((float)nSize);
    szz = szz - mz*mz/((float)nSize);

    // Kovarianzmatrix
    Wm4::Matrix3<float> akMat(sxx,sxy,sxz,sxy,syy,syz,sxz,syz,szz);
    Wm4::Matrix3<float> rkRot, rkDiag;
    akMat.EigenDecomposition(rkRot, rkDiag);

    Wm4::Vector3<float> U = rkRot.GetColumn(1);
    Wm4::Vector3<float> V = rkRot.GetColumn(2);
    Wm4::Vector3<float> W = rkRot.GetColumn(0);
    Wm4::Vector3<float> B = Wm4::Vector3<float>(mx/(float)nSize, my/(float)nSize, mz/(float)nSize);

    Base::Vector3f bs(B.X(), B.Y(), B.Z());
    Base::Vector3f ex(U.X(), U.Y(), U.Z());
    Base::Vector3f ey(V.X(), V.Y(), V.Z());

    for ( std::vector<Base::Vector3f>::iterator jt = _points.begin(); jt!=_points.end(); ++jt )
        jt->TransformToCoordinateSystem(bs, ex, ey);

    // build the matrix for the inverse transformation
    rInverse.unity();
    rInverse[0][0] = U.X(); rInverse[0][1] = V.X(); rInverse[0][2] = W.X(); rInverse[0][3] = bs.x;
    rInverse[1][0] = U.Y(); rInverse[1][1] = V.Y(); rInverse[1][2] = W.Y(); rInverse[1][3] = bs.y;
    rInverse[2][0] = U.Z(); rInverse[2][1] = V.Z(); rInverse[2][2] = W.Z(); rInverse[2][3] = bs.z;

    return Base::Vector3f(W.X(), W.Y(), W.Z());
}
