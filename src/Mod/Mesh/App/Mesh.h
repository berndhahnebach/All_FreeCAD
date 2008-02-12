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
#include "Facet.h"
#include "MeshPoint.h"

namespace Py {
class List;
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
    MeshObject();
    explicit MeshObject(const MeshCore::MeshKernel& Kernel);
    explicit MeshObject(const MeshCore::MeshKernel& Kernel, const Base::Matrix4D &Mtrx);
    MeshObject(const MeshObject&);
    ~MeshObject();

    void operator = (const MeshObject&);
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
    bool isSolid() const;
    MeshPoint getPoint(unsigned long) const;
    Facet getFacet(unsigned long) const;
    //@}

    /** @name Iterator */
    //@{
    /** Returns an iterator object to go over all facets. */
    MeshCore::MeshFacetIterator FacetIterator() const;
    /** Returns an iterator object to go over all points. */
    MeshCore::MeshPointIterator PointIterator() const;
    //@}

    void setKernel(const MeshCore::MeshKernel& m)
    { _kernel = m; }
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
    unsigned long countComponents() const;
    void removeComponents(unsigned long);
    void fillupHoles(unsigned long, float);
    void offset(float fSize);
    void offsetSpecial2(float fSize);
    void offsetSpecial(float fSize, float zmax, float zmin);
    /// clears the Mesh
    void clear(void);
    void transformToEigenSystem();
    void movePoint(unsigned long, const Base::Vector3d& v);
    void setPoint(unsigned long, const Base::Vector3d& v);
    Base::Vector3d getPointNormal(unsigned long) const;
    //@}

    /** @name Boolean operations */
    //@{
    void unite(const MeshObject&);
    void intersect(const MeshObject&);
    void subtract(const MeshObject&);
    void inner(const MeshObject&);
    void outer(const MeshObject&);
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
    unsigned long countNonUnifomOrientedFacets() const;
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
    class MeshExport FacetIter
    {
    public:
        FacetIter(MeshObject*, unsigned long index);
        FacetIter(const FacetIter& fi);
        ~FacetIter();

        FacetIter& operator=(const FacetIter& fi);
        Facet& operator*();
        Facet* operator->();
        bool operator==(const FacetIter& fi) const;
        bool operator!=(const FacetIter& fi) const;
        FacetIter& operator++();
        FacetIter& operator--();
    private:
        void dereference();
        MeshObject* _mesh;
        Facet _facet;
        MeshCore::MeshFacetIterator _f_it;
    };

    FacetIter facets_begin()
    { return FacetIter(this, 0); }
    FacetIter facets_end()
    { return FacetIter(this, countFacets()); }

private:
    MeshCore::MeshKernel _kernel;
};

} // namespace Mesh


#endif // MESH_MESH_H
