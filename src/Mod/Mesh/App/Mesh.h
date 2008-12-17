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


#ifndef MESH_MESH_H
#define MESH_MESH_H

#include <vector>
#include <list>
#include <set>
#include <string>
#include <map>

#include <Base/Matrix.h>
#include <Base/Vector3D.h>

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>
#include <App/ComplexGeoData.h>

#include "Core/MeshKernel.h"
#include "Core/Iterator.h"
#include "MeshPoint.h"
#include "Facet.h"
#include "MeshPoint.h"
#include "Segment.h"

namespace Py {
class List;
}

namespace MeshCore {
class AbstractPolygonTriangulator;
}

namespace Mesh
{
/**
 * The MeshObject class provides an interface for the underlying MeshKernel class and
 * most of its algorithm on it.
 * @note Each instance of MeshObject has its own instance of a MeshKernel so it's not possible
 * that several instances of MeshObject manage one instance of MeshKernel.
 */
class MeshExport MeshObject : public Data::ComplexGeoData
{
    TYPESYSTEM_HEADER();

public:
    // typedef needed for cross-section
    typedef std::pair<Base::Vector3f, Base::Vector3f> Plane;
    typedef std::list<std::vector<Base::Vector3f> > Polylines;

    MeshObject();
    explicit MeshObject(const MeshCore::MeshKernel& Kernel);
    explicit MeshObject(const MeshCore::MeshKernel& Kernel, const Base::Matrix4D &Mtrx);
    MeshObject(const MeshObject&);
    ~MeshObject();

    void operator = (const MeshObject&);

    /** @name Subelement management */
    //@{
    /** Sub type list
     *  List of different subelement types
     *  its NOT a list of the subelements itself
     */
    virtual std::vector<const char*> getElementTypes(void) const;
    /// get the subelement by type and number
    virtual Data::Segment* getSubElement(const char* Type, unsigned long) const;
    //@}


    void setTransform(const Base::Matrix4D& rclTrf);
    Base::Matrix4D getTransform(void) const;

    /**
     * Swaps the content of \a Kernel and the internal mesh kernel.
     */
    void swap(MeshCore::MeshKernel& Kernel);

    /** @name Querying */
    //@{
    std::string representation() const;
    std::string topologyInfo() const;
    unsigned long countPoints() const;
    unsigned long countFacets() const;
    unsigned long countEdges () const;
    unsigned long countSegments() const;
    bool isSolid() const;
    MeshPoint getPoint(unsigned long) const;
    Facet getFacet(unsigned long) const;
    double getSurface() const;
    double getVolume() const;
    //@}

    void setKernel(const MeshCore::MeshKernel& m);
    MeshCore::MeshKernel& getKernel(void)
    { return _kernel; }
    const MeshCore::MeshKernel& getKernel(void) const
    { return _kernel; }

    virtual Base::BoundBox3d getBoundBox(void)const;

    /** @name I/O */
    //@{
    // Implemented from Persistence
    unsigned int getMemSize (void) const;
    void Save (Base::Writer &writer) const;
    void SaveDocFile (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);
    void RestoreDocFile(Base::Reader &reader);
    void save(const char* file) const;
    void save(std::ostream&) const;
    void load(const char* file);
    void load(std::istream&);
    //@}

    /** @name Manipulation */
    //@{
    void addFacet(const MeshCore::MeshGeomFacet& facet);
    void addFacets(const std::vector<MeshCore::MeshGeomFacet>& facets);
    void addFacets(const std::vector<MeshCore::MeshFacet> &facets,
                   const std::vector<Base::Vector3f>& points);
    void addFacets(const std::vector<Data::ComplexGeoData::FacetTopo> &facets,
                   const std::vector<Base::Vector3d>& points);
    void setFacets(const std::vector<MeshCore::MeshGeomFacet>& facets);
    void setFacets(const std::vector<Data::ComplexGeoData::FacetTopo> &facets,
                   const std::vector<Base::Vector3d>& points);
    /**
     * Combines two independant mesh objects.
     * @note The mesh object we want to add must not overlap or intersect with
     * this mesh object.
     */
    void addMesh(const MeshObject&);
    /**
     * Combines two independant mesh objects.
     * @note The mesh object we want to add must not overlap or intersect with
     * this mesh object.
     */
    void addMesh(const MeshCore::MeshKernel&);
    void deleteFacets(const std::vector<unsigned long>& removeIndices);
    void deletePoints(const std::vector<unsigned long>& removeIndices);
    unsigned long countComponents() const;
    void removeComponents(unsigned long);
    void fillupHoles(unsigned long, int, MeshCore::AbstractPolygonTriangulator&);
    void offset(float fSize);
    void offsetSpecial2(float fSize);
    void offsetSpecial(float fSize, float zmax, float zmin);
    /// clears the Mesh
    void clear(void);
    void transformToEigenSystem();
    void movePoint(unsigned long, const Base::Vector3d& v);
    void setPoint(unsigned long, const Base::Vector3d& v);
    void smooth(int iterations, float d_max);
    Base::Vector3d getPointNormal(unsigned long) const;
    void crossSections(const std::vector<Plane>&, std::vector<Polylines> &sections,
                       float fMinEps = 1.0e-2f, bool bConnectPolygons = false) const;
    //@}

    /** @name Boolean operations */
    //@{
    MeshObject* unite(const MeshObject&) const;
    MeshObject* intersect(const MeshObject&) const;
    MeshObject* subtract(const MeshObject&) const;
    MeshObject* inner(const MeshObject&) const;
    MeshObject* outer(const MeshObject&) const;
    //@}

    /** @name Topological operations */
    //@{
    void refine();
    void optimizeTopology(float);
    void optimizeEdges();
    void splitEdges();
    void splitEdge(unsigned long, unsigned long, const Base::Vector3f&);
    void splitFacet(unsigned long, const Base::Vector3f&, const Base::Vector3f&);
    void swapEdge(unsigned long, unsigned long);
    void collapseEdge(unsigned long, unsigned long);
    void collapseFacet(unsigned long);
    void collapseFacets(const std::vector<unsigned long>&);
    void insertVertex(unsigned long, const Base::Vector3f& v);
    void snapVertex(unsigned long, const Base::Vector3f& v);
    //@}

    /** @name Mesh validation */
    //@{
    unsigned long countNonUniformOrientedFacets() const;
    void flipNormals();
    void harmonizeNormals();
    void validateIndices();
    void validateDeformations(float fMaxAngle);
    void validateDegenerations();
    void removeDuplicatedPoints();
    void removeDuplicatedFacets();
    bool hasNonManifolds() const;
    void removeNonManifolds();
    bool hasSelfIntersections() const;
    void removeSelfIntersections();
    void removeFoldsOnSurface();
    //@}

    /** @name Mesh segments */
    //@{
    void addSegment(const Segment&);
    void addSegment(const std::vector<unsigned long>&);
    const Segment& getSegment(unsigned long) const;
    Segment& getSegment(unsigned long);
    MeshObject* meshFromSegment(const std::vector<unsigned long>&) const;
    //@}

    /** @name Primitives */
    //@{
    static MeshObject* createMeshFromList(Py::List& list);
    static MeshObject* createSphere(float, int);
    static MeshObject* createEllipsoid(float, float, int);
    static MeshObject* createCylinder(float, float, int, float, int);
    static MeshObject* createCone(float, float, float, int, float, int);
    static MeshObject* createTorus(float, float, int);
    static MeshObject* createCube(float, float, float);
    //@}

public:
    class MeshExport const_point_iterator
    {
    public:
        const_point_iterator(const MeshObject*, unsigned long index);
        const_point_iterator(const const_point_iterator& pi);
        ~const_point_iterator();

        const_point_iterator& operator=(const const_point_iterator& fi);
        const MeshPoint& operator*();
        const MeshPoint* operator->();
        bool operator==(const const_point_iterator& fi) const;
        bool operator!=(const const_point_iterator& fi) const;
        const_point_iterator& operator++();
        const_point_iterator& operator--();
    private:
        void dereference();
        const MeshObject* _mesh;
        MeshPoint _point;
        MeshCore::MeshPointIterator _p_it;
    };

    class MeshExport const_facet_iterator
    {
    public:
        const_facet_iterator(const MeshObject*, unsigned long index);
        const_facet_iterator(const const_facet_iterator& fi);
        ~const_facet_iterator();

        const_facet_iterator& operator=(const const_facet_iterator& fi);
        Facet& operator*();
        Facet* operator->();
        bool operator==(const const_facet_iterator& fi) const;
        bool operator!=(const const_facet_iterator& fi) const;
        const_facet_iterator& operator++();
        const_facet_iterator& operator--();
    private:
        void dereference();
        const MeshObject* _mesh;
        Facet _facet;
        MeshCore::MeshFacetIterator _f_it;
    };

    /** @name Iterator */
    //@{
    const_point_iterator points_begin() const
    { return const_point_iterator(this, 0); }
    const_point_iterator points_end() const
    { return const_point_iterator(this, countPoints()); }

    const_facet_iterator facets_begin() const
    { return const_facet_iterator(this, 0); }
    const_facet_iterator facets_end() const
    { return const_facet_iterator(this, countFacets()); }

    typedef std::vector<Segment>::const_iterator const_segment_iterator;
    const_segment_iterator segments_begin() const
    { return _segments.begin(); }
    const_segment_iterator segments_end() const
    { return _segments.end(); }
    //@}

    // friends
    friend class Segment;

private:
    void deletedFacets(const std::vector<unsigned long>& remFacets);
    void updateMesh(const std::vector<unsigned long>&);
    void updateMesh();

private:
    Base::Matrix4D _Mtrx;
    MeshCore::MeshKernel _kernel;
    std::vector<Segment> _segments;
    static float Epsilon;
};

} // namespace Mesh


#endif // MESH_MESH_H
