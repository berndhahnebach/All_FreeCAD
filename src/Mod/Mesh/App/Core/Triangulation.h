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


#ifndef MESH_TRIANGULATION_H
#define MESH_TRIANGULATION_H

#include "Elements.h"
#include <Base/Vector3D.h>

namespace MeshCore
{

class MeshExport AbstractPolygonTriangulator
{
public:
    AbstractPolygonTriangulator();
    virtual ~AbstractPolygonTriangulator();

    /** Sets the polygon to be triangulated. */
    void SetPolygon(const std::vector<Base::Vector3f>& raclPoints);
    /** Get the polygon points to be triangulated. The points may be
     * projected onto its average plane.
     */
    std::vector<Base::Vector3f> GetPolygon() const;
    /** The triangulation algorithm may create new points when
     * calling Triangulate(). This method returns these added
     * points.
     * @note: Make sure to have called ProjectOntoSurface() before using
     * this method if you want the surface points the new points are lying on.
     */
    std::vector<Base::Vector3f> AddedPoints() const;
    /** If the points of the polygon set by SetPolygon() doesn't lie in a 
     * plane this method can be used to project the points in a common plane.
     * This method must be called directly after SetPolygon() and before
     * Triangulate(). The method returns the normal of the fitted plane and
     * the inverse transformation matrix.
     */
    Base::Vector3f TransformToFitPlane(Base::Matrix4D& rInverse);
    /** Computes the triangulation of a polygon. The resulting facets can
     * be accessed by GetTriangles() or GetFacets().
     */
    virtual bool Triangulate() = 0;
    /** If points were added then we get the 3D points by projecting the added
     * 2D points onto a surface which fits into the given points.
     */
    virtual void ProjectOntoSurface(const std::vector<Base::Vector3f>&);
    /** Returns the geometric triangles of the polygon. */
    const std::vector<MeshGeomFacet>& GetTriangles() const { return _triangles;}
    /** Returns the topologic facets of the polygon. */
    const std::vector<MeshFacet>& GetFacets() const { return _facets;}
    /** Returns the length of the polygon */
    float GetLength() const;

protected:
    std::vector<Base::Vector3f> _points;
    std::vector<Base::Vector3f> _newpoints;
    std::vector<MeshGeomFacet>  _triangles;
    std::vector<MeshFacet>      _facets;
};

/**
 * The EarClippingTriangulator embeds an efficient algorithm to triangulate
 * polygons taken from http://www.flipcode.com/files/code/triangulate.cpp.
 */
class MeshExport EarClippingTriangulator : public AbstractPolygonTriangulator
{
public:
    EarClippingTriangulator();
    ~EarClippingTriangulator();

    bool Triangulate();

private:
    /**
    * Static class to triangulate any contour/polygon (without holes) efficiently.
    * The original code snippet was submitted to FlipCode.com by John W. Ratcliff 
    * (jratcliff@verant.com) on July 22, 2000.
    * The original vector of 2d points is replaced by a vector of 3d points where the
    * z-ccordinate is ignored. This is because the algorithm is often used for 3d points 
    * projected to a common plane. The result vector of 2d points is replaced by an 
    * array of indices to the points of the polygon.
    */
    class Triangulate
    {
    public:
        // triangulate a contour/polygon, places results in STL vector
        // as series of triangles.indicating the points
        static bool Process(const std::vector<Base::Vector3f> &contour,
            std::vector<unsigned long> &result);

        // compute area of a contour/polygon
        static float Area(const std::vector<Base::Vector3f> &contour);

        // decide if point Px/Py is inside triangle defined by
        // (Ax,Ay) (Bx,By) (Cx,Cy)
        static bool InsideTriangle(float Ax, float Ay, float Bx, float By,
            float Cx, float Cy, float Px, float Py);

        static bool _invert;
    private:
        static bool Snip(const std::vector<Base::Vector3f> &contour,
            int u,int v,int w,int n,int *V);
    };
};

class MeshExport QuasiDelaunayTriangulator : public EarClippingTriangulator
{
public:
    QuasiDelaunayTriangulator();
    ~QuasiDelaunayTriangulator();

    bool Triangulate();
};

class MeshExport DelaunayTriangulator : public AbstractPolygonTriangulator
{
public:
    DelaunayTriangulator();
    ~DelaunayTriangulator();

    bool Triangulate();
};

class MeshExport FlatTriangulator : public AbstractPolygonTriangulator
{
public:
    FlatTriangulator();
    ~FlatTriangulator();

    bool Triangulate();
    void ProjectOntoSurface(const std::vector<Base::Vector3f>&);
};

class MeshExport ConstraintDelaunayTriangulator : public AbstractPolygonTriangulator
{
public:
    ConstraintDelaunayTriangulator(float area);
    ~ConstraintDelaunayTriangulator();

    bool Triangulate();

private:
    float fMaxArea;
};
#if 0
/**
 * The MeshPolygonTriangulation embeds an efficient algorithm to triangulate polygons taken from
 * http://www.flipcode.com/files/code/triangulate.cpp.
 */
class MeshExport MeshPolygonTriangulation
{
public:
    MeshPolygonTriangulation();
    MeshPolygonTriangulation(const std::vector<Base::Vector3f>& raclPoints);
    virtual ~MeshPolygonTriangulation();

    /** Computes the triangulation of a polygon. The resulting facets can be accessed by
    * GetTriangles() or GetFacets().
    */
    bool Compute();
    /** The Compute() method returns a valid triangulation of a polygon though it might produce
    * sometimes degenerated triangles. So this method performs several "swap edge" operations to 
    * the output of Compute() to get more regular triangles.
    */
    bool ComputeQuasiDelaunay();
    /**
    * Computes a constrained Delaunay triangulation (CDT) of the input polygon. 
    * To get high quality triangles this algorithm may create new vertices which are added to \a newPoints and
    * create triangles with a maximum area of \a fMaxArea.
    * @note The user must transform these points back into world coordinates. 
    */
    bool ComputeConstrainedDelaunay(float fMaxArea, std::vector<Base::Vector3f>& newPoints);
    /** If the points of the polygon set by SetPolygon() doesn't lie in a plane this method
    * can be used to project the points in a common plane. This method must be called directly 
    * after SetPolygon() and before Compute() or ComputeQuasiDelaunay(). The method returns
    * the normal of the fitted plane and the inverse transformation matrix.
    */
    Base::Vector3f TransformToFitPlane(Base::Matrix4D& rInverse);
    /** Sets the polygon to be triangulated. */
    void SetPolygon(const std::vector<Base::Vector3f>& raclPoints);
    /** Get the polygon points to be triangulated. The points may be projected onto 
     * its balance plane.
     */
    std::vector<Base::Vector3f> GetPolygon() const;
    /** Returns the geometric triangles of the polygon. */
    const std::vector<MeshGeomFacet>& GetTriangles() const { return _triangles;}
    /** Returns the topologic facets of the polygon. */
    const std::vector<MeshFacet>& GetFacets() const { return _facets;}

private:
    std::vector<Base::Vector3f> _points;
    std::vector<MeshGeomFacet>  _triangles;
    std::vector<MeshFacet>      _facets;

    /**
    * Static class to triangulate any contour/polygon (without holes) efficiently.
    * The original code snippet was submitted to FlipCode.com by John W. Ratcliff 
    * (jratcliff@verant.com) on July 22, 2000.
    * The original vector of 2d points is replaced by a vector of 3d points where the
    * z-ccordinate is ignored. This is because the algorithm is often used for 3d points 
    * projected to a common plane. The result vector of 2d points is replaced by an 
    * array of indices to the points of the polygon.
    */
    class Triangulate
    {
    public:
        // triangulate a contour/polygon, places results in STL vector
        // as series of triangles.indicating the points
        static bool Process(const std::vector<Base::Vector3f> &contour, std::vector<unsigned long> &result);

        // compute area of a contour/polygon
        static float Area(const std::vector<Base::Vector3f> &contour);

        // decide if point Px/Py is inside triangle defined by
        // (Ax,Ay) (Bx,By) (Cx,Cy)
        static bool InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);

        static bool _invert;
    private:
        static bool Snip(const std::vector<Base::Vector3f> &contour,int u,int v,int w,int n,int *V);
    };
};
#endif
} // namespace MeshCore


#endif  // MESH_TRIANGULATION_H 
