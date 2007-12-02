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
# include <sstream>
#endif

#include <Base/Builder3D.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/PyCXX/Objects.hxx>

#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Iterator.h"
#include "Core/Info.h"
#include "Core/TopoAlgorithm.h"
#include "Core/Evaluation.h"
#include "Core/Degeneration.h"
#include "Core/SetOperations.h"

#include "Mesh.h"
#include "MeshPy.h"

using namespace Mesh;

TYPESYSTEM_SOURCE(Mesh::MeshObject, Data::ComplexGeoData);

MeshObject::MeshObject()
{
}

MeshObject::MeshObject(const MeshCore::MeshKernel& Kernel)
  : _kernel(Kernel)
{
    // copy the mesh structure
}

MeshObject::MeshObject(const MeshCore::MeshKernel& Kernel, const Base::Matrix4D &Mtrx)
  : ComplexGeoData(Mtrx),_kernel(Kernel)
{
    // copy the mesh structure
}

MeshObject::MeshObject(const MeshObject& mesh)
  : ComplexGeoData(mesh._Mtrx),_kernel(mesh._kernel)
{
    // copy the mesh structure
}

MeshObject::~MeshObject()
{
}


void MeshObject::movePoint(unsigned long index, const Base::Vector3d& v)
{
    _kernel.MovePoint(index,transformToInside(v));
}



Base::Vector3d MeshObject::getPointNormal(unsigned long index)
{
    std::vector<Base::Vector3f> temp = _kernel.CalcVertexNormals();
    return transformToOutside(temp[index]);
}

void MeshObject::operator = (const MeshObject& mesh)
{
    if (this != &mesh) {
        // copy the mesh structure
        setTransform(mesh._Mtrx);
        this->_kernel = mesh._kernel;
    }
}

std::string MeshObject::representation() const
{
    std::stringstream str;
    MeshCore::MeshInfo info(_kernel);
    info.GeneralInformation(str);
    return str.str();
}

std::string MeshObject::topologyInfo() const
{
    std::stringstream str;
    MeshCore::MeshInfo info(_kernel);
    info.TopologyInformation(str);
    return str.str();
}

unsigned long MeshObject::countPoints() const
{
    return _kernel.CountPoints();
}

unsigned long MeshObject::countFacets() const
{
    return _kernel.CountFacets();
}

unsigned long MeshObject::countEdges () const
{
    return _kernel.CountEdges();
}

bool MeshObject::isSolid() const
{
    MeshCore::MeshEvalSolid cMeshEval(_kernel);
    return cMeshEval.Evaluate();
}

MeshCore::MeshFacetIterator MeshObject::FacetIterator() const
{
    MeshCore::MeshFacetIterator it = _kernel.FacetIterator();
    it.Transform(_Mtrx);
    return it;
}

MeshCore::MeshPointIterator MeshObject::PointIterator() const
{
    MeshCore::MeshPointIterator it = _kernel.PointIterator();
    it.Transform(_Mtrx);
    return it;
}

unsigned int MeshObject::getMemSize (void) const
{
    return _kernel.GetMemSize();
}

void MeshObject::Save (Base::Writer &writer) const
{
    // this is handled by the property class
}

void MeshObject::SaveDocFile (Base::Writer &writer) const
{
    _kernel.Write(writer.Stream());
}

void MeshObject::Restore(Base::XMLReader &reader)
{
    // this is handled by the property class
}

void MeshObject::RestoreDocFile(Base::Reader &reader)
{
    _kernel.Read(reader);
}

void MeshObject::save(const char* file) const
{
    MeshCore::MeshOutput aWriter(_kernel);
    aWriter.SaveAny(file);
}

void MeshObject::save(std::ostream& out) const
{
    _kernel.Write(out);
}

void MeshObject::load(const char* file)
{
    MeshCore::MeshInput aReader(_kernel);
    aReader.LoadAny(file);
}

void MeshObject::load(std::istream& in)
{
    _kernel.Read(in);
}

void MeshObject::addFacet(const MeshCore::MeshGeomFacet& facet)
{
    _kernel.AddFacet(facet);
}

void MeshObject::addFacets(const std::vector<MeshCore::MeshGeomFacet>& facets)
{
    _kernel.AddFacets(facets);
}

unsigned long MeshObject::countComponents() const
{
    std::vector<std::vector<unsigned long> > segments;
    MeshCore::MeshComponents comp(_kernel);
    comp.SearchForComponents(MeshCore::MeshComponents::OverEdge,segments);
    return segments.size();
}

void MeshObject::removeComponents(unsigned long count)
{
    MeshCore::MeshTopoAlgorithm(_kernel).RemoveComponents(count);
}

void MeshObject::fillupHoles(unsigned long length, float maxArea)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.FillupHoles(length, maxArea);
}

void MeshObject::offset(float fSize)
{
    std::vector<Base::Vector3f> normals = _kernel.CalcVertexNormals();

    unsigned int i = 0;
    // go through all the vertex normals
    for (std::vector<Base::Vector3f>::iterator It= normals.begin();It != normals.end();It++,i++)
        // and move each mesh point in the normal direction
        _kernel.MovePoint(i,It->Normalize() * fSize);
    _kernel.RecalcBoundBox();
}

void MeshObject::offsetSpecial2(float fSize)
{
    Base::Builder3D builder;  
    std::vector<Base::Vector3f> PointNormals= _kernel.CalcVertexNormals();
    std::vector<Base::Vector3f> FaceNormals;
    std::set<unsigned long> fliped;

    MeshCore::MeshFacetIterator it(_kernel);
    for (it.Init(); it.More(); it.Next())
        FaceNormals.push_back(it->GetNormal().Normalize());

    unsigned int i = 0;

    // go through all the vertex normals
    for (std::vector<Base::Vector3f>::iterator It= PointNormals.begin();It != PointNormals.end();It++,i++){
        builder.addSingleLine(_kernel.GetPoint(i),_kernel.GetPoint(i)+It->Normalize() * fSize);
        // and move each mesh point in the normal direction
        _kernel.MovePoint(i,It->Normalize() * fSize);
    }
    _kernel.RecalcBoundBox();

    MeshCore::MeshTopoAlgorithm alg(_kernel);

    for (int l= 0; l<1 ;l++) {
        for ( it.Init(),i=0; it.More(); it.Next(),i++) {
            if (it->IsFlag(MeshCore::MeshFacet::INVALID))
                continue;
            // calculate the angle between them
            float angle = acos((FaceNormals[i] * it->GetNormal()) / (it->GetNormal().Length() * FaceNormals[i].Length()));
            if (angle > 1.6) {
                builder.addSinglePoint(it->GetGravityPoint(),4,1,0,0);
                fliped.insert(it.Position());
            }
        }
        
        // if there no flipped triangels -> stop
        //int f =fliped.size();
        if (fliped.size() == 0)
            break;
      
        for( std::set<unsigned long>::iterator It= fliped.begin();It!=fliped.end();++It)
            alg.CollapseFacet(*It);
        fliped.clear();
    }

    alg.Cleanup();

    // search for intersected facets
    MeshCore::MeshEvalSelfIntersection eval(_kernel);
    std::vector<unsigned long > faces;
    eval.GetIntersections(faces);
    builder.saveToLog();
}

void MeshObject::offsetSpecial(float fSize, float zmax, float zmin)
{
    std::vector<Base::Vector3f> normals = _kernel.CalcVertexNormals();

    unsigned int i = 0;
    // go through all the vertex normals
    for (std::vector<Base::Vector3f>::iterator It= normals.begin();It != normals.end();It++,i++) {
        Base::Vector3f Pnt = _kernel.GetPoint(i);
        if (Pnt.z < zmax && Pnt.z > zmin) {
            Pnt.z = 0;
            _kernel.MovePoint(i,Pnt.Normalize() * fSize);
        }
        else {
            // and move each mesh point in the normal direction
            _kernel.MovePoint(i,It->Normalize() * fSize);
        }
    }
}

void MeshObject::clear(void)
{
    _kernel.Clear();
    setTransform(Base::Matrix4D());
}

void MeshObject::transformToEigenSystem()
{
    MeshCore::MeshEigensystem cMeshEval(_kernel);
    cMeshEval.Evaluate();
    this->setTransform(cMeshEval.Transform());
}

void MeshObject::unite(const MeshObject& mesh)
{
    MeshCore::SetOperations setOp(_kernel, mesh._kernel, _kernel, MeshCore::SetOperations::Union, 1.0e-5);
    setOp.Do();
}

void MeshObject::intersect(const MeshObject& mesh)
{
    MeshCore::SetOperations setOp(_kernel, mesh._kernel, _kernel, MeshCore::SetOperations::Intersect, 1.0e-5);
    setOp.Do();
}

void MeshObject::subtract(const MeshObject& mesh)
{
    MeshCore::SetOperations setOp(_kernel, mesh._kernel, _kernel, MeshCore::SetOperations::Difference, 1.0e-5);
    setOp.Do();
}

void MeshObject::inner(const MeshObject& mesh)
{
    MeshCore::SetOperations setOp(_kernel, mesh._kernel, _kernel, MeshCore::SetOperations::Inner, 1.0e-5);
    setOp.Do();
}

void MeshObject::outer(const MeshObject& mesh)
{
    MeshCore::SetOperations setOp(_kernel, mesh._kernel, _kernel, MeshCore::SetOperations::Outer, 1.0e-5);
    setOp.Do();
}

void MeshObject::refine()
{
    unsigned long cnt = _kernel.CountFacets();
    MeshCore::MeshFacetIterator cF(_kernel);
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    for (unsigned long i=0; i<cnt; i++) {
        cF.Set(i);
        if (!cF->IsDeformed())
            topalg.InsertVertexAndSwapEdge(i, cF->GetGravityPoint(), 0.1f);
    }
}

void MeshObject::optimizeTopology(float fMaxAngle)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.OptimizeTopology(fMaxAngle);
}

void MeshObject::optimizeEdges()
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.AdjustEdgesToCurvatureDirection();
}

void MeshObject::splitEdges()
{
    std::vector<std::pair<unsigned long, unsigned long> > adjacentFacet;
    MeshCore::MeshAlgorithm alg(_kernel);
    alg.ResetFacetFlag(MeshCore::MeshFacet::VISIT);
    const MeshCore::MeshFacetArray& rFacets = _kernel.GetFacets();
    for (MeshCore::MeshFacetArray::_TConstIterator pF = rFacets.begin(); pF != rFacets.end(); ++pF) {
        int id=2;
        if (pF->_aulNeighbours[id] != ULONG_MAX) {
            const MeshCore::MeshFacet& rFace = rFacets[pF->_aulNeighbours[id]];
            if (!pF->IsFlag(MeshCore::MeshFacet::VISIT) && !rFace.IsFlag(MeshCore::MeshFacet::VISIT)) {
                pF->SetFlag(MeshCore::MeshFacet::VISIT);
                rFace.SetFlag(MeshCore::MeshFacet::VISIT);
                adjacentFacet.push_back(std::make_pair(pF-rFacets.begin(), pF->_aulNeighbours[id]));
            }
        }
    }

    MeshCore::MeshFacetIterator cIter(_kernel);
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    for (std::vector<std::pair<unsigned long, unsigned long> >::iterator it = adjacentFacet.begin(); it != adjacentFacet.end(); ++it) {
        cIter.Set(it->first);
        Base::Vector3f mid = 0.5f*(cIter->_aclPoints[0]+cIter->_aclPoints[2]);
        topalg.SplitEdge(it->first, it->second, mid);
    }
}

void MeshObject::splitEdge(unsigned long facet, unsigned long neighbour, const Base::Vector3f& v)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.SplitEdge(facet, neighbour, v);
}

void MeshObject::splitFacet(unsigned long facet, const Base::Vector3f& v1, const Base::Vector3f& v2)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.SplitFacet(facet, v1, v2);
}

void MeshObject::swapEdge(unsigned long facet, unsigned long neighbour)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.SwapEdge(facet, neighbour);
}

void MeshObject::collapseEdge(unsigned long facet, unsigned long neighbour)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.CollapseEdge(facet, neighbour);
}

void MeshObject::collapseFacet(unsigned long facet)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.CollapseFacet(facet);
}

void MeshObject::collapseFacets(const std::vector<unsigned long>& facets)
{
    MeshCore::MeshTopoAlgorithm alg(_kernel);
    for (std::vector<unsigned long>::const_iterator it = facets.begin(); it != facets.end(); ++it) {
        alg.CollapseFacet(*it);
    }
}

void MeshObject::insertVertex(unsigned long facet, const Base::Vector3f& v)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.InsertVertex(facet, v);
}

void MeshObject::snapVertex(unsigned long facet, const Base::Vector3f& v)
{
    MeshCore::MeshTopoAlgorithm topalg(_kernel);
    topalg.SnapVertex(facet, v);
}

unsigned long MeshObject::countNonUnifomOrientedFacets() const
{
    MeshCore::MeshEvalOrientation cMeshEval(_kernel);
    std::vector<unsigned long> inds = cMeshEval.GetIndices();
    return inds.size();
}

void MeshObject::flipNormals()
{
    MeshCore::MeshTopoAlgorithm alg(_kernel);
    alg.FlipNormals();
}

void MeshObject::harmonizeNormals()
{
    MeshCore::MeshTopoAlgorithm alg(_kernel);
    alg.HarmonizeNormals();
}

bool MeshObject::hasNonManifolds() const
{
    MeshCore::MeshEvalTopology cMeshEval(_kernel);
    return !cMeshEval.Evaluate();
}

void MeshObject::removeNonManifolds()
{
    MeshCore::MeshEvalTopology cMeshEval(_kernel);
    if (!cMeshEval.Evaluate()) {
        MeshCore::MeshFixTopology cMeshFix(_kernel, cMeshEval.GetIndices());
        cMeshFix.Fixup();
    }
}

bool MeshObject::hasSelfIntersections() const
{
    MeshCore::MeshEvalTopology cMeshEval(_kernel);
    return !cMeshEval.Evaluate();
}

void MeshObject::removeSelfIntersections()
{
    throw Base::Exception("Not yet implemented");
}

void MeshObject::validateIndices()
{
    MeshCore::MeshEvalNeighbourhood nb(_kernel);
    if (!nb.Evaluate()) {
        MeshCore::MeshFixNeighbourhood fix(_kernel);
        fix.Fixup();
    }

    MeshCore::MeshEvalRangeFacet rf(_kernel);
    if (!rf.Evaluate()) {
        MeshCore::MeshFixRangeFacet fix(_kernel);
        fix.Fixup();
    }

    MeshCore::MeshEvalRangePoint rp(_kernel);
    if (!rp.Evaluate()) {
        MeshCore::MeshFixRangePoint fix(_kernel);
        fix.Fixup();
    }

    MeshCore::MeshEvalCorruptedFacets cf(_kernel);
    if (!nb.Evaluate()) {
        MeshCore::MeshFixCorruptedFacets fix(_kernel);
        fix.Fixup();
    }
}

void MeshObject::validateDeformations(float fMaxAngle)
{
    MeshCore::MeshFixDeformedFacets eval(_kernel, fMaxAngle);
    eval.Fixup();
}

void MeshObject::validateDegenerations()
{
    MeshCore::MeshFixDegeneratedFacets eval(_kernel);
    eval.Fixup();
}

void MeshObject::removeDuplicatedPoints()
{
    MeshCore::MeshFixDuplicatePoints eval(_kernel);
    eval.Fixup();
}

void MeshObject::removeDuplicatedFacets()
{
    MeshCore::MeshFixDuplicateFacets eval(_kernel);
    eval.Fixup();
}

MeshObject* MeshObject::createMeshFromList(Py::List& list)
{
    std::vector<MeshCore::MeshGeomFacet> facets;
    MeshCore::MeshGeomFacet facet;
    int i = 0;
    for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
        Py::List item(*it);
        for (int j = 0; j < 3; j++) {
            Py::Float value(item[j]);
            facet._aclPoints[i][j] = (float)value;
        }
        if (++i == 3) {
            i = 0;
            facet.CalcNormal();
            facets.push_back(facet);
        }
    }

    std::auto_ptr<MeshObject> mesh(new MeshObject);
    //mesh->addFacets(facets);
    mesh->getKernel() = facets;
    return mesh.release();
}

MeshObject* MeshObject::createSphere(float radius, int sampling)
{
    // load the 'BuildRegularGeoms' module
    try {
        Py::Module module(PyImport_ImportModule("BuildRegularGeoms"));
        Py::Dict dict = module.getDict();
        Py::Callable call(dict.getItem("Sphere"));
        Py::Tuple args(2);
        args.setItem(0, Py::Float(radius));
        args.setItem(1, Py::Int(sampling));
        Py::List list(call.apply(args));
        return createMeshFromList(list);
    }
    catch (Py::Exception& e) {
        e.clear();
    }

    return 0;
}

MeshObject* MeshObject::createEllipsoid(float radius1, float radius2, int sampling)
{
    // load the 'BuildRegularGeoms' module
    try {
        Py::Module module(PyImport_ImportModule("BuildRegularGeoms"));
        Py::Dict dict = module.getDict();
        Py::Callable call(dict.getItem("Ellipsoid"));
        Py::Tuple args(3);
        args.setItem(0, Py::Float(radius1));
        args.setItem(1, Py::Float(radius2));
        args.setItem(2, Py::Int(sampling));
        Py::List list(call.apply(args));
        return createMeshFromList(list);
    }
    catch (Py::Exception& e) {
        e.clear();
    }

    return 0;
}

MeshObject* MeshObject::createCylinder(float radius, float length, int closed, float edgelen, int sampling)
{
    // load the 'BuildRegularGeoms' module
    try {
        Py::Module module(PyImport_ImportModule("BuildRegularGeoms"));
        Py::Dict dict = module.getDict();
        Py::Callable call(dict.getItem("Cylinder"));
        Py::Tuple args(5);
        args.setItem(0, Py::Float(radius));
        args.setItem(1, Py::Float(length));
        args.setItem(2, Py::Int(closed));
        args.setItem(3, Py::Float(edgelen));
        args.setItem(4, Py::Int(sampling));
        Py::List list(call.apply(args));
        return createMeshFromList(list);
    }
    catch (Py::Exception& e) {
        e.clear();
    }

    return 0;
}

MeshObject* MeshObject::createCone(float radius1, float radius2, float len, int closed, float edgelen, int sampling)
{
    // load the 'BuildRegularGeoms' module
    try {
        Py::Module module(PyImport_ImportModule("BuildRegularGeoms"));
        Py::Dict dict = module.getDict();
        Py::Callable call(dict.getItem("Cone"));
        Py::Tuple args(6);
        args.setItem(0, Py::Float(radius1));
        args.setItem(1, Py::Float(radius2));
        args.setItem(2, Py::Float(len));
        args.setItem(3, Py::Int(closed));
        args.setItem(4, Py::Float(edgelen));
        args.setItem(5, Py::Int(sampling));
        Py::List list(call.apply(args));
        return createMeshFromList(list);
    }
    catch (Py::Exception& e) {
        e.clear();
    }

    return 0;
}

MeshObject* MeshObject::createTorus(float radius1, float radius2, int sampling)
{
    // load the 'BuildRegularGeoms' module
    try {
        Py::Module module(PyImport_ImportModule("BuildRegularGeoms"));
        Py::Dict dict = module.getDict();
        Py::Callable call(dict.getItem("Toroid"));
        Py::Tuple args(3);
        args.setItem(0, Py::Float(radius1));
        args.setItem(1, Py::Float(radius2));
        args.setItem(2, Py::Int(sampling));
        Py::List list(call.apply(args));
        return createMeshFromList(list);
    }
    catch (Py::Exception& e) {
        e.clear();
    }

    return 0;
}

MeshObject* MeshObject::createCube(float length, float width, float height)
{
    // load the 'BuildRegularGeoms' module
    try {
        Py::Module module(PyImport_ImportModule("BuildRegularGeoms"));
        Py::Dict dict = module.getDict();
        Py::Callable call(dict.getItem("Cube"));
        Py::Tuple args(3);
        args.setItem(0, Py::Float(length));
        args.setItem(1, Py::Float(width));
        args.setItem(2, Py::Float(height));
        Py::List list(call.apply(args));
        return createMeshFromList(list);
    }
    catch (Py::Exception& e) {
        e.clear();
    }

    return 0;
}

// ----------------------------------------------------------------

MeshSegment::MeshSegment(MeshObject* mesh) : _mesh(mesh)
{
}

MeshSegment::MeshSegment(MeshObject* mesh, const std::vector<unsigned long>& inds) : _mesh(mesh), _indices(inds)
{
}

void MeshSegment::addIndices(const std::vector<unsigned long>& inds)
{
    _indices.insert(_indices.end(), inds.begin(), inds.end());
}

const std::vector<unsigned long>& MeshSegment::getIndices() const
{
    return _indices;
}

// ----------------------------------------------------------------------------

MeshSegment::FacetIter::FacetIter(const MeshSegment* segm, std::vector<unsigned long>::const_iterator it)
  : _segment(segm), _it(it)
{
    const MeshCore::MeshKernel& kernel = _segment->_mesh->getKernel();
    this->_f_it = kernel.GetFacets().begin();
}

MeshSegment::FacetIter::FacetIter(const MeshSegment::FacetIter& fi)
  : _segment(fi._segment), _f_it(fi._f_it), _it(fi._it)
{
}

MeshSegment::FacetIter::~FacetIter()
{
}

MeshSegment::FacetIter& MeshSegment::FacetIter::operator=(const MeshSegment::FacetIter& fi)
{
    this->_segment = fi._segment;
    this->_f_it    = fi._f_it;
    this->_it      = fi._it;
    return *this;
}

const MeshCore::MeshFacet& MeshSegment::FacetIter::operator*() const
{
    return *(_f_it + *_it);
}

const MeshCore::MeshFacet* MeshSegment::FacetIter::operator->() const
{
    return &(*(_f_it + *_it));
}

bool MeshSegment::FacetIter::operator==(const MeshSegment::FacetIter& fi) const
{
    return (this->_segment == fi._segment) && (this->_it == fi._it);
}

bool MeshSegment::FacetIter::operator!=(const MeshSegment::FacetIter& fi) const 
{
    return !operator==(fi);
}

MeshSegment::FacetIter& MeshSegment::FacetIter::operator++()
{
    ++(this->_it);
    return *this;
}

MeshSegment::FacetIter& MeshSegment::FacetIter::operator--()
{
    --(this->_it);
    return *this;
}
