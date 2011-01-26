/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include <QEventLoop>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentMap>

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <Base/Exception.h>
#include <Base/FutureWatcherProgress.h>
#include <Base/Sequencer.h>
#include <Base/Tools.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Points/App/PointsFeature.h>
#include <Mod/Points/App/PointsGrid.h>

#include "InspectionFeature.h"


using namespace Inspection;

InspectActualMesh::InspectActualMesh(const MeshCore::MeshKernel& rMesh) : _iter(rMesh)
{
    this->_count = rMesh.CountPoints();
}

InspectActualMesh::~InspectActualMesh()
{
}

unsigned long InspectActualMesh::countPoints() const
{
    return this->_count;
}

Base::Vector3f InspectActualMesh::getPoint(unsigned long index)
{
    _iter.Set(index);
    return *_iter;
}

// ----------------------------------------------------------------

InspectActualPoints::InspectActualPoints(const Points::PointKernel& rPoints) : _rKernel(rPoints)
{
}

unsigned long InspectActualPoints::countPoints() const
{
    return _rKernel.size();
}

Base::Vector3f InspectActualPoints::getPoint(unsigned long index)
{
    Base::Vector3d p = _rKernel.getPoint(index);
    return Base::Vector3f((float)p.x,(float)p.y,(float)p.z);
}

// ----------------------------------------------------------------

InspectNominalMesh::InspectNominalMesh(const MeshCore::MeshKernel& rMesh, float offset) : _iter(rMesh)
{
    // Max. limit of grid elements
    float fMaxGridElements=8000000.0f;
    Base::BoundBox3f box = rMesh.GetBoundBox();

    // estimate the minimum allowed grid length
    float fMinGridLen = (float)pow((box.LengthX()*box.LengthY()*box.LengthZ()/fMaxGridElements), 0.3333f);
    float fGridLen = 5.0f * MeshCore::MeshAlgorithm(rMesh).GetAverageEdgeLength();

    // We want to avoid to get too small grid elements otherwise building up the grid structure would take
    // too much time and memory. 
    // Having quite a dense grid speeds up more the following algorithms extremely. Due to the issue above it's
    // always a compromise between speed and memory usage.
    fGridLen = std::max<float>(fMinGridLen, fGridLen);

    // build up grid structure to speed up algorithms
    _pGrid = new MeshCore::MeshFacetGrid(rMesh, fGridLen);
    _box = box;
    _box.Enlarge(offset);
}

InspectNominalMesh::~InspectNominalMesh()
{
    delete this->_pGrid;
}

float InspectNominalMesh::getDistance(const Base::Vector3f& point)
{
    if (!_box.IsInBox(point))
        return FLT_MAX; // must be inside bbox

    std::vector<unsigned long> indices;
    //_pGrid->GetElements(point, indices);
    if (indices.empty()) {
        std::set<unsigned long> inds;
        _pGrid->MeshGrid::SearchNearestFromPoint(point, inds);
        indices.insert(indices.begin(), inds.begin(), inds.end());
    }

    float fMinDist=FLT_MAX;
    bool positive = true;
    for (std::vector<unsigned long>::iterator it = indices.begin(); it != indices.end(); ++it) {
        _iter.Set(*it);
        float fDist = _iter->DistanceToPoint(point);
        if (fabs(fDist) < fabs(fMinDist)) {
            fMinDist = fDist;
            positive = point.DistanceToPlane(_iter->_aclPoints[0], _iter->GetNormal()) > 0;
        }
    }

    if (!positive)
        fMinDist = -fMinDist;
    return fMinDist;
}

// ----------------------------------------------------------------

InspectNominalFastMesh::InspectNominalFastMesh(const MeshCore::MeshKernel& rMesh, float offset) : _iter(rMesh)
{
    // Max. limit of grid elements
    float fMaxGridElements=8000000.0f;
    Base::BoundBox3f box = rMesh.GetBoundBox();

    // estimate the minimum allowed grid length
    float fMinGridLen = (float)pow((box.LengthX()*box.LengthY()*box.LengthZ()/fMaxGridElements), 0.3333f);
    float fGridLen = 5.0f * MeshCore::MeshAlgorithm(rMesh).GetAverageEdgeLength();

    // We want to avoid to get too small grid elements otherwise building up the grid structure would take
    // too much time and memory. 
    // Having quite a dense grid speeds up more the following algorithms extremely. Due to the issue above it's
    // always a compromise between speed and memory usage.
    fGridLen = std::max<float>(fMinGridLen, fGridLen);

    // build up grid structure to speed up algorithms
    _pGrid = new MeshCore::MeshFacetGrid(rMesh, fGridLen);
    _box = box;
    _box.Enlarge(offset);
    max_level = (unsigned long)(offset/fGridLen);
}

InspectNominalFastMesh::~InspectNominalFastMesh()
{
    delete this->_pGrid;
}

/**
 * This algorithm is not that exact as that from InspectNominalMesh but is by
 * factors faster and sufficient for many cases.
 */
float InspectNominalFastMesh::getDistance(const Base::Vector3f& point)
{
    if (!_box.IsInBox(point))
        return FLT_MAX; // must be inside bbox

    std::set<unsigned long> indices;
#if 0 // a point in a neighbour grid can be nearer
    std::vector<unsigned long> elements;
    _pGrid->GetElements(point, elements);
    indices.insert(elements.begin(), elements.end());
#else
    unsigned long ulX, ulY, ulZ;
    _pGrid->Position(point, ulX, ulY, ulZ);
    unsigned long ulLevel = 0;
    while (indices.size() == 0 && ulLevel <= max_level)
        _pGrid->GetHull(ulX, ulY, ulZ, ulLevel++, indices);
    if (indices.size() == 0 || ulLevel==1)
        _pGrid->GetHull(ulX, ulY, ulZ, ulLevel, indices);
#endif

    float fMinDist=FLT_MAX;
    bool positive = true;
    for (std::set<unsigned long>::iterator it = indices.begin(); it != indices.end(); ++it) {
        _iter.Set(*it);
        float fDist = _iter->DistanceToPoint(point);
        if (fabs(fDist) < fabs(fMinDist)) {
            fMinDist = fDist;
            positive = point.DistanceToPlane(_iter->_aclPoints[0], _iter->GetNormal()) > 0;
        }
    }

    if (!positive)
        fMinDist = -fMinDist;
    return fMinDist;
}

// ----------------------------------------------------------------

InspectNominalPoints::InspectNominalPoints(const Points::PointKernel& Kernel, float offset) : _rKernel(Kernel)
{
    int uGridPerAxis = 50; // totally 125.000 grid elements 
    this->_pGrid = new Points::PointsGrid (Kernel, uGridPerAxis);
}

InspectNominalPoints::~InspectNominalPoints()
{
    delete this->_pGrid;
}

float InspectNominalPoints::getDistance(const Base::Vector3f& point)
{
    //TODO: Make faster
    std::set<unsigned long> indices;
    unsigned long x,y,z;
    Base::Vector3d pointd(point.x,point.y,point.z);
    _pGrid->Position(pointd, x, y, z);
    _pGrid->GetElements(x,y,z,indices);

    double fMinDist=DBL_MAX;
    for (std::set<unsigned long>::iterator it = indices.begin(); it != indices.end(); ++it) {
        Base::Vector3d pt = _rKernel.getPoint(*it);
        double fDist = Base::Distance(pointd, pt);
        if (fDist < fMinDist) {
            fMinDist = fDist;
        }
    }

    return (float)fMinDist;
}

// ----------------------------------------------------------------

// helper class to use Qt's concurrent framework
struct DistanceInspection
{

    DistanceInspection(float radius, InspectActualGeometry*  a,
                       std::vector<InspectNominalGeometry*> n)
                    : radius(radius), actual(a), nominal(n)
    {
    }
    float mapped(unsigned long index)
    {
        Base::Vector3f pnt = actual->getPoint(index);

        float fMinDist=FLT_MAX;
        for (std::vector<InspectNominalGeometry*>::iterator it = nominal.begin(); it != nominal.end(); ++it) {
            float fDist = (*it)->getDistance(pnt);
            if (fabs(fDist) < fabs(fMinDist))
                fMinDist = fDist;
        }

        if (fMinDist > this->radius)
            fMinDist = FLT_MAX;
        else if (-fMinDist > this->radius)
            fMinDist = -FLT_MAX;

        return fMinDist;
    }

    float radius;
    InspectActualGeometry*  actual;
    std::vector<InspectNominalGeometry*> nominal;
};

PROPERTY_SOURCE(Inspection::Feature, App::DocumentObject)

Feature::Feature()
{
    ADD_PROPERTY(SearchRadius,(0.05f));
    ADD_PROPERTY(Thickness,(0.0f));
    ADD_PROPERTY(Actual,(0));
    ADD_PROPERTY(Nominals,(0));
    ADD_PROPERTY(Distances,(0.0f));
}

Feature::~Feature()
{
}

short Feature::mustExecute() const
{
    if (SearchRadius.isTouched())
        return 1;
    if (Thickness.isTouched())
        return 1;
    if (Actual.isTouched())
        return 1;
    if (Nominals.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn* Feature::execute(void)
{
    App::DocumentObject* pcActual = Actual.getValue();
    if (!pcActual)
        throw Base::Exception("No actual geometry to inspect specified");

    InspectActualGeometry* actual = 0;
    if (pcActual->getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
        Mesh::Feature* mesh = (Mesh::Feature*)pcActual;
        actual = new InspectActualMesh(mesh->Mesh.getValue().getKernel());
    }
    else if (pcActual->getTypeId().isDerivedFrom(Points::Feature::getClassTypeId())) {
        Points::Feature* pts = (Points::Feature*)pcActual;
        actual = new InspectActualPoints(pts->Points.getValue());
    }
    else {
        throw Base::Exception("Unknown geometric type");
    }

    // get a list of nominals
    std::vector<InspectNominalGeometry*> inspectNominal;
    const std::vector<App::DocumentObject*>& nominals = Nominals.getValues();
    for (std::vector<App::DocumentObject*>::const_iterator it = nominals.begin(); it != nominals.end(); ++it) {
        InspectNominalGeometry* nominal = 0;
        if ((*it)->getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
            Mesh::Feature* mesh = (Mesh::Feature*)*it;
            nominal = new InspectNominalMesh(mesh->Mesh.getValue().getKernel(), this->SearchRadius.getValue());
        }
        else if ((*it)->getTypeId().isDerivedFrom(Points::Feature::getClassTypeId())) {
            Points::Feature* pts = (Points::Feature*)*it;
            nominal = new InspectNominalPoints(pts->Points.getValue(), this->SearchRadius.getValue());
        }

        if (nominal)
            inspectNominal.push_back(nominal);
    }

#if 1 // test with some huge data sets
    std::vector<unsigned long> index(actual->countPoints());
    std::generate(index.begin(), index.end(), Base::iotaGen<unsigned long>(0));
    DistanceInspection check(this->SearchRadius.getValue(), actual, inspectNominal);
    QFuture<float> future = QtConcurrent::mapped
        (index, boost::bind(&DistanceInspection::mapped, &check, _1));
    //future.waitForFinished(); // blocks the GUI
    Base::FutureWatcherProgress progress("Inspecting...", actual->countPoints());
    QFutureWatcher<float> watcher;
    QObject::connect(&watcher, SIGNAL(progressValueChanged(int)),
                     &progress, SLOT(progressValueChanged(int)));
    watcher.setFuture(future);

    // keep it responsive during computation
    QEventLoop loop;
    QObject::connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    std::vector<float> vals;
    vals.insert(vals.end(), future.begin(), future.end());
#else
    unsigned long count = actual->countPoints();
    Base::SequencerLauncher seq("Inspecting...", count);

    std::vector<float> vals(count);
    for (unsigned long index = 0; index < count; index++) {
        Base::Vector3f pnt = actual->getPoint(index);

        float fMinDist=FLT_MAX;
        for (std::vector<InspectNominalGeometry*>::iterator it = inspectNominal.begin(); it != inspectNominal.end(); ++it) {
            float fDist = (*it)->getDistance(pnt);
            if (fabs(fDist) < fabs(fMinDist))
                fMinDist = fDist;
        }

        if (fMinDist > this->SearchRadius.getValue())
            fMinDist = FLT_MAX;
        else if (-fMinDist > this->SearchRadius.getValue())
            fMinDist = -FLT_MAX;
        vals[index] = fMinDist;
        seq.next();
    }
#endif

    Distances.setValues(vals);

    delete actual;
    for (std::vector<InspectNominalGeometry*>::iterator it = inspectNominal.begin(); it != inspectNominal.end(); ++it)
        delete *it;

    return 0;
}

// ----------------------------------------------------------------

PROPERTY_SOURCE(Inspection::Group, App::DocumentObjectGroup)


Group::Group()
{
}

Group::~Group()
{
}
