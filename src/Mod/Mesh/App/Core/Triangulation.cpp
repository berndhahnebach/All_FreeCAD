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

#include "Triangulation.h"
#include "Approximation.h"
#include "MeshKernel.h"
#include "triangle.h"


using namespace MeshCore;


AbstractPolygonTriangulator::AbstractPolygonTriangulator()
{
    _discard = false;
}

AbstractPolygonTriangulator::~AbstractPolygonTriangulator()
{
}

void AbstractPolygonTriangulator::SetPolygon(const std::vector<Base::Vector3f>& raclPoints)
{
    this->_points = raclPoints;
    if (this->_points.size() > 0) {
        if (this->_points.front() == this->_points.back())
            this->_points.pop_back();
    }
}

std::vector<Base::Vector3f> AbstractPolygonTriangulator::GetPolygon() const
{
    return _points;
}

float AbstractPolygonTriangulator::GetLength() const
{
    float len = 0.0f;
    if (_points.size() > 2) {
        for (std::vector<Base::Vector3f>::const_iterator it = _points.begin(); it != _points.end();++it) {
            std::vector<Base::Vector3f>::const_iterator jt = it + 1;
            if (jt == _points.end()) jt = _points.begin();
            len += Base::Distance(*it, *jt);
        }
    }

    return len;
}

std::vector<Base::Vector3f> AbstractPolygonTriangulator::AddedPoints() const
{
    // Apply the inverse transformation to project back to world coordinates
    std::vector<Base::Vector3f> added;
    added.reserve(_newpoints.size());
    for (std::vector<Base::Vector3f>::const_iterator pt = _newpoints.begin(); pt != _newpoints.end(); ++pt)
        added.push_back(_inverse * *pt);
    return added;
}

Base::Matrix4D AbstractPolygonTriangulator::GetTransformToFitPlane() const
{
    PlaneFit planeFit;
    for (std::vector<Base::Vector3f>::const_iterator it = _points.begin(); it!=_points.end(); ++it)
        planeFit.AddPoint(*it);

    planeFit.Fit();

    Base::Vector3f bs = planeFit.GetBase();
    Base::Vector3f ex = planeFit.GetDirU();
    Base::Vector3f ey = planeFit.GetDirV();
    Base::Vector3f ez = planeFit.GetNormal();

    // build the matrix for the inverse transformation
    Base::Matrix4D rInverse;
    rInverse.setToUnity();
    rInverse[0][0] = ex.x; rInverse[0][1] = ey.x; rInverse[0][2] = ez.x; rInverse[0][3] = bs.x;
    rInverse[1][0] = ex.y; rInverse[1][1] = ey.y; rInverse[1][2] = ez.y; rInverse[1][3] = bs.y;
    rInverse[2][0] = ex.z; rInverse[2][1] = ey.z; rInverse[2][2] = ez.z; rInverse[2][3] = bs.z;

    return rInverse;
}

std::vector<Base::Vector3f> AbstractPolygonTriangulator::ProjectToFitPlane()
{
    std::vector<Base::Vector3f> proj = _points;
    _inverse = GetTransformToFitPlane();
    Base::Vector3f bs((float)_inverse[0][3], (float)_inverse[1][3], (float)_inverse[2][3]);
    Base::Vector3f ex((float)_inverse[0][0], (float)_inverse[1][0], (float)_inverse[2][0]);
    Base::Vector3f ey((float)_inverse[0][1], (float)_inverse[1][1], (float)_inverse[2][1]);
    for (std::vector<Base::Vector3f>::iterator jt = proj.begin(); jt!=proj.end(); ++jt)
        jt->TransformToCoordinateSystem(bs, ex, ey);
    return proj;
}

void AbstractPolygonTriangulator::ProjectOntoSurface(const std::vector<Base::Vector3f>& points)
{
    // For a good approximation we should have enough points, i.e. for 9 parameters
    // for the fit function we should have at least 50 points.
    unsigned int uMinPts = 50;

    PolynomialFit polyFit;
    Base::Vector3f bs((float)_inverse[0][3], (float)_inverse[1][3], (float)_inverse[2][3]);
    Base::Vector3f ex((float)_inverse[0][0], (float)_inverse[1][0], (float)_inverse[2][0]);
    Base::Vector3f ey((float)_inverse[0][1], (float)_inverse[1][1], (float)_inverse[2][1]);

    for (std::vector<Base::Vector3f>::const_iterator it = points.begin(); it != points.end(); ++it) {
        Base::Vector3f pt = *it;
        pt.TransformToCoordinateSystem(bs, ex, ey);
        polyFit.AddPoint(pt);
    }

    polyFit.Fit();

    if (polyFit.CountPoints() >= uMinPts) {
        for (std::vector<Base::Vector3f>::iterator pt = _newpoints.begin(); pt != _newpoints.end(); ++pt)
            pt->z = (float)polyFit.Value(pt->x, pt->y);
    }
}

bool AbstractPolygonTriangulator::TriangulatePolygon()
{
    bool ok = Triangulate();
    if (ok) Done();
    return ok;
}

std::vector<unsigned long> AbstractPolygonTriangulator::GetInfo() const
{
    return _info;
}

void AbstractPolygonTriangulator::Discard()
{
    if (!_discard) {
        _discard = true;
        _info.pop_back();
    }
}

void AbstractPolygonTriangulator::Done()
{
    _info.push_back(_points.size());
    _discard = false;
}

// -------------------------------------------------------------

EarClippingTriangulator::EarClippingTriangulator()
{
}

EarClippingTriangulator::~EarClippingTriangulator()
{
}

bool EarClippingTriangulator::Triangulate()
{
  _facets.clear();
  _triangles.clear();

  std::vector<Base::Vector3f> pts = ProjectToFitPlane();
  std::vector<unsigned long> result;

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

float EarClippingTriangulator::Triangulate::Area(const std::vector<Base::Vector3f> &contour)
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
bool EarClippingTriangulator::Triangulate::InsideTriangle(float Ax, float Ay, float Bx,
                                                          float By, float Cx, float Cy,
                                                          float Px, float Py)
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

bool EarClippingTriangulator::Triangulate::Snip(const std::vector<Base::Vector3f> &contour,
                                                int u,int v,int w,int n,int *V)
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

bool EarClippingTriangulator::Triangulate::_invert = false;

bool EarClippingTriangulator::Triangulate::Process(const std::vector<Base::Vector3f> &contour,
                                                   std::vector<unsigned long> &result)
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

// -------------------------------------------------------------

QuasiDelaunayTriangulator::QuasiDelaunayTriangulator()
{
}

QuasiDelaunayTriangulator::~QuasiDelaunayTriangulator()
{
}

bool QuasiDelaunayTriangulator::Triangulate()
{
  if (EarClippingTriangulator::Triangulate() == false)
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

// -------------------------------------------------------------

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

DelaunayTriangulator::DelaunayTriangulator()
{
}

DelaunayTriangulator::~DelaunayTriangulator()
{
}

bool DelaunayTriangulator::Triangulate()
{
    // before starting the triangulation we must make sure that all polygon 
    // points are different
    std::vector<Base::Vector3f> aPoints = _points;
    // sort the points ascending x,y coordinates
    std::sort(aPoints.begin(), aPoints.end(), Triangulation::Vertex2d_Less());
    // if there are two adjacent points whose distance is less then an epsilon
    if (std::adjacent_find(aPoints.begin(), aPoints.end(),
        Triangulation::Vertex2d_EqualTo()) < aPoints.end() )
        return false;

    _facets.clear();
    _triangles.clear();

    triangulateio* in = new triangulateio();
    memset(in, 0, sizeof(triangulateio));
    in->pointlist = new double[_points.size() * 2];
    in->numberofpoints = _points.size();

    // build up point list
    int i = 0;
    for (std::vector<Base::Vector3f>::iterator it = _points.begin(); it != _points.end(); it++) {
        in->pointlist[i++] = it->x;
        in->pointlist[i++] = it->y;
    }

    // set the triangulation parameter
    // z: start index from 0
    triangulateio* out = new triangulateio();
    memset(out, 0, sizeof(triangulateio));
    triangulate("z", in, out, NULL);

    // get the triangles
    MeshGeomFacet triangle;
    MeshFacet facet;
    bool succeeded = out->numberoftriangles > 0;
    for (i = 0; i < (out->numberoftriangles * 3); i += 3) {
        if ((out->trianglelist[i] == out->trianglelist[i+1]) || 
            (out->trianglelist[i] == out->trianglelist[i+2]) || 
            (out->trianglelist[i+1] == out->trianglelist[i+2])) {
            // two same triangle corner points (ignore)
            continue;
        }

        for (int j = 0; j < 3; j++) {
            int index = 2 * out->trianglelist[i+j];
            triangle._aclPoints[j].x = (float)out->pointlist[index];
            triangle._aclPoints[j].y = (float)out->pointlist[index+1];
            facet._aulPoints[j] = out->trianglelist[i+j];
        }

        _triangles.push_back(triangle);
        _facets.push_back(facet);
    }

    // free all memory
    delete in->pointlist;
    delete in;
    delete out->trianglelist;
    delete out->pointlist;
    delete out->pointmarkerlist;
    delete out;

    return succeeded;
}

// -------------------------------------------------------------

FlatTriangulator::FlatTriangulator()
{
}

FlatTriangulator::~FlatTriangulator()
{
}

bool FlatTriangulator::Triangulate()
{
    _newpoints.clear();
    // before starting the triangulation we must make sure that all polygon 
    // points are different
    std::vector<Base::Vector3f> aPoints = ProjectToFitPlane();
    std::vector<Base::Vector3f> tmp = aPoints;
    // sort the points ascending x,y coordinates
    std::sort(tmp.begin(), tmp.end(), Triangulation::Vertex2d_Less());
    // if there are two adjacent points whose distance is less then an epsilon
    if (std::adjacent_find(tmp.begin(), tmp.end(),
        Triangulation::Vertex2d_EqualTo()) < tmp.end() )
        return false;

    _facets.clear();
    _triangles.clear();

    triangulateio* in = new triangulateio();
    memset(in, 0, sizeof(triangulateio));
    in->pointlist = new double[aPoints.size() * 2];
    in->numberofpoints = aPoints.size();
    in->segmentlist = new int[aPoints.size() * 2];
    in->numberofsegments = aPoints.size();

    // build up point list
    int i = 0, j = 0, k = 0;
    int mod = aPoints.size();
    for (std::vector<Base::Vector3f>::iterator it = aPoints.begin(); it != aPoints.end(); it++) {
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

    triangulate("pYz", in, out, NULL);

    // get all added points by the algorithm
    for (int index = 2 * aPoints.size(); index < (out->numberofpoints * 2); ) {
        float x = (float)out->pointlist[index++];
        float y = (float)out->pointlist[index++];
        float z = 0.0f; // get the point on the plane
        Base::Vector3f vertex(x, y, z);
        _newpoints.push_back(vertex);
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
            triangle._aclPoints[j].x = (float)out->pointlist[index];
            triangle._aclPoints[j].y = (float)out->pointlist[index+1];
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

void FlatTriangulator::ProjectOntoSurface(const std::vector<Base::Vector3f>&)
{
}

// -------------------------------------------------------------

ConstraintDelaunayTriangulator::ConstraintDelaunayTriangulator(float area)
  : fMaxArea(area)
{
}

ConstraintDelaunayTriangulator::~ConstraintDelaunayTriangulator()
{
}

bool ConstraintDelaunayTriangulator::Triangulate()
{
    _newpoints.clear();
    // before starting the triangulation we must make sure that all polygon 
    // points are different
    std::vector<Base::Vector3f> aPoints = ProjectToFitPlane();
    std::vector<Base::Vector3f> tmp = aPoints;
    // sort the points ascending x,y coordinates
    std::sort(tmp.begin(), tmp.end(), Triangulation::Vertex2d_Less());
    // if there are two adjacent points whose distance is less then an epsilon
    if (std::adjacent_find(tmp.begin(), tmp.end(),
        Triangulation::Vertex2d_EqualTo()) < tmp.end() )
        return false;

    _facets.clear();
    _triangles.clear();

    triangulateio* in = new triangulateio();
    memset(in, 0, sizeof(triangulateio));
    in->pointlist = new double[aPoints.size() * 2];
    in->numberofpoints = aPoints.size();
    in->segmentlist = new int[aPoints.size() * 2];
    in->numberofsegments = aPoints.size();

    // build up point list
    int i = 0, j = 0, k = 0;
    int mod = aPoints.size();
    for (std::vector<Base::Vector3f>::iterator it = aPoints.begin(); it != aPoints.end(); it++) {
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
    char szBuf[51];
    float maxArea = this->fMaxArea;
    if (maxArea < 0.0f) {
        // determine automatically an approximate value
        // depedning on the length of the edges
        maxArea = GetLength() / ((float)aPoints.size());
        maxArea = (maxArea * maxArea)/2.0f;
    }

    snprintf(szBuf, 50, "pYzqa%.6f", maxArea);
    triangulate(szBuf, in, out, NULL);

    // get all added points by the algorithm
    for (int index = 2 * aPoints.size(); index < (out->numberofpoints * 2); ) {
        float x = (float)out->pointlist[index++];
        float y = (float)out->pointlist[index++];
        float z = 0.0f; // get the point on the plane
        Base::Vector3f vertex(x, y, z);
        _newpoints.push_back(vertex);
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
            triangle._aclPoints[j].x = (float)out->pointlist[index];
            triangle._aclPoints[j].y = (float)out->pointlist[index+1];
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
