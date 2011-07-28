/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
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
# include <TopoDS_Shape.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS.hxx>
# include <BRepAdaptor_Surface.hxx>
#endif

#include <Base/Writer.h>
#include <Base/Reader.h>

#include <Mod/Part/App/Geometry.h>

#include <vector>

#include "SketchObject.h"
#include "SketchObjectPy.h"
#include "Sketch.h"

using namespace Sketcher;
using namespace Base;


PROPERTY_SOURCE(Sketcher::SketchObject, Part::Part2DObject)


SketchObject::SketchObject()
{
    ADD_PROPERTY_TYPE(Geometry,           (0)  ,"Sketch",(App::PropertyType)(App::Prop_None),"Sketch geometry");
    ADD_PROPERTY_TYPE(Constraints,        (0)  ,"Sketch",(App::PropertyType)(App::Prop_None),"Sketch constraints");
    ADD_PROPERTY_TYPE(ExternalConstraints,(0,0),"Sketch",(App::PropertyType)(App::Prop_None),"Sketch external constraints");
}

App::DocumentObjectExecReturn *SketchObject::execute(void)
{
    // recalculate support:
    if (Support.getValue() && Support.getValue()->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
    {
        Part::Feature *part = static_cast<Part::Feature*>(Support.getValue());
        Base::Placement ObjectPos = part->Placement.getValue();
        const std::vector<std::string> &sub = Support.getSubValues();
        assert(sub.size()==1);
        // get the selected sub shape (a Face)
        const Part::TopoShape &shape = part->Shape.getValue();
        TopoDS_Shape sh = shape.getSubShape(sub[0].c_str());
        TopoDS_Face face = TopoDS::Face(sh);
        assert(!face.IsNull());

        BRepAdaptor_Surface adapt(face);
        if (adapt.GetType() != GeomAbs_Plane)
            return new App::DocumentObjectExecReturn("Sketch has no planar support!");

        // set sketch position 
        Base::Placement placement = Part2DObject::positionBySupport(face,ObjectPos);
        Placement.setValue(placement);
    }
     
    // and now solve the sketch
    Sketch sketch;

    sketch.setUpSketch(Geometry.getValues(), Constraints.getValues());
 
    // solve the sketch with no fixed points
    if (sketch.solve() != 0)
        return new App::DocumentObjectExecReturn("Solving the sketch failed!",this);

    std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
    Geometry.setValues(geomlist);
    for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it)
        if (*it) delete *it;

    Shape.setValue(sketch.toShape());

    return App::DocumentObject::StdReturn;
}

int SketchObject::setDatum(double Datum, int ConstrNbr)
{
    // set the changed value for the constraint
    assert(ConstrNbr >= 0); 
    const std::vector< Constraint * > &vals = this->Constraints.getValues();
    assert(ConstrNbr < (int)vals.size());
    assert(vals[ConstrNbr]->Type == Distance);

    // copy the list
    std::vector< Constraint * > newVals(vals);
    // clone the changed Constraint
    Constraint *constNew = vals[ConstrNbr]->clone();
    constNew->Value = Datum;
    newVals[ConstrNbr] = constNew;
    this->Constraints.setValues(newVals);
    delete constNew;

    // set up a extra sketch
    Sketch sketch;
    // set the geometry and constraints
    sketch.setUpSketch(Geometry.getValues(), Constraints.getValues());
 
    int ret = sketch.solve();

    if(ret)
        return ret;

    // set the newly solved geometry
    std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
    Geometry.setValues(geomlist);
    for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it)
        if (*it) delete *it;

    return 0;
}

int SketchObject::movePoint(int geoIndex, PointPos pos, const Base::Vector3d& toPoint)
{
    // set up an extra sketch
    Sketch sketch;
    // set the geometry and constraints
    sketch.setUpSketch(Geometry.getValues(), Constraints.getValues());
 
    int ret = sketch.movePoint(geoIndex, pos, toPoint);

    if (ret == 0) {
        std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
        Geometry.setValues(geomlist);
        for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it) {
            if (*it) delete *it;
        }
    }

    return ret;
}

Base::Vector3d SketchObject::getPoint(int geoIndex, PointPos pos)
{
    const std::vector< Part::Geometry * > &geomlist = this->Geometry.getValues();
    assert(geoIndex < (int)geomlist.size());
    Part::Geometry *geo = geomlist[geoIndex];
    if (geo->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
        const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
        if (pos == start)
            return lineSeg->getStartPoint();
        else if (pos == end)
            return lineSeg->getEndPoint();
    } else if (geo->getTypeId() == Part::GeomCircle::getClassTypeId()) {
        const Part::GeomCircle *circle = dynamic_cast<const Part::GeomCircle*>(geo);
        if (pos == mid)
            return circle->getCenter();
    } else if (geo->getTypeId() == Part::GeomArcOfCircle::getClassTypeId()) {
        const Part::GeomArcOfCircle *aoc = dynamic_cast<const Part::GeomArcOfCircle*>(geo);
        if (pos == start)
            return aoc->getStartPoint();
        else if (pos == end)
            return aoc->getEndPoint();
        else if (pos == mid)
            return aoc->getCenter();
    }

    return Base::Vector3d();
}

int SketchObject::addGeometry(const std::vector<Part::Geometry *> &geoList)
{
    return -1;
}

int SketchObject::addGeometry(const Part::Geometry *geo)
{
    const std::vector< Part::Geometry * > &vals = Geometry.getValues();

    std::vector< Part::Geometry * > newVals(vals);
    Part::Geometry *geoNew = geo->clone();
    newVals.push_back(geoNew);
    Geometry.setValues(newVals);
    delete geoNew;
    rebuildVertexIndex();
    return Geometry.getSize()-1;
}

int SketchObject::delGeometry(int GeoNbr)
{
    const std::vector< Part::Geometry * > &vals = this->Geometry.getValues();
    if (GeoNbr < 0 || GeoNbr >= (int)vals.size())
        return -1;

    std::vector< Part::Geometry * > newVals(vals);
    newVals.erase(newVals.begin()+GeoNbr);

    const std::vector< Constraint * > &constraints = this->Constraints.getValues();
    std::vector< Constraint * > newConstraints(0);
    for (std::vector<Constraint *>::const_iterator it = constraints.begin();
         it != constraints.end(); ++it) {
        if ((*it)->First != GeoNbr && (*it)->Second != GeoNbr) {
            if ((*it)->First > GeoNbr)
                (*it)->First -= 1;
            if ((*it)->Second > GeoNbr)
                (*it)->Second -= 1;
            newConstraints.push_back(*it);
        }
    }

    this->Constraints.setValues(newConstraints);
    this->Geometry.setValues(newVals);
    rebuildVertexIndex();
    return 0;
}

int SketchObject::addConstraints(const std::vector<Constraint *> &ConstraintList)
{
    return -1;
}

int SketchObject::addConstraint(const Constraint *constraint)
{
    const std::vector< Constraint * > &vals = this->Constraints.getValues();

    std::vector< Constraint * > newVals(vals);
    Constraint *constNew = constraint->clone();
    newVals.push_back(constNew);
    this->Constraints.setValues(newVals);
    delete constNew;
    return this->Constraints.getSize()-1;
}

int SketchObject::delConstraint(int ConstrNbr)
{
    const std::vector< Constraint * > &vals = this->Constraints.getValues();
    if (ConstrNbr < 0 || ConstrNbr >= (int)vals.size())
        return -1;

    std::vector< Constraint * > newVals(vals);
    newVals.erase(newVals.begin()+ConstrNbr);
    this->Constraints.setValues(newVals);
    return 0;
}

int SketchObject::delConstraintOnPoint(int PointNbr)
{
    int GeoId;
    PointPos PosId;
    getGeoVertexIndex(PointNbr, GeoId, PosId);

    const std::vector< Constraint * > &vals = this->Constraints.getValues();

    std::vector< Constraint * > newVals(0);
    for (std::vector<Constraint *>::const_iterator it = vals.begin(); it != vals.end(); ++it) {
        if ((*it)->Type == Sketcher::Coincident)
            if (((*it)->First == GeoId && (*it)->FirstPos == PosId)
               || ((*it)->Second == GeoId && (*it)->SecondPos == PosId))
               continue;
        newVals.push_back(*it);
    }
    if (newVals.size() < vals.size())
        this->Constraints.setValues(newVals);
    return 0;
}

void SketchObject::rebuildVertexIndex(void)
{
    VertexId2GeoId.resize(0);
    VertexId2PosId.resize(0);
    int i=0;
    const std::vector< Part::Geometry * > &geometry = this->Geometry.getValues();
    for (std::vector< Part::Geometry * >::const_iterator it = geometry.begin();
         it != geometry.end(); ++it,i++) {
        if ((*it)->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(start);
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(end);
        } else if ((*it)->getTypeId() == Part::GeomCircle::getClassTypeId()) {
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(mid);
        } else if ((*it)->getTypeId() == Part::GeomArcOfCircle::getClassTypeId()) {
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(mid);
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(start);
            VertexId2GeoId.push_back(i);
            VertexId2PosId.push_back(end);
        }
    }
}

PyObject *SketchObject::getPyObject(void)
{
    if (PythonObject.is(Py::_None())){
        // ref counter is set to 1
        PythonObject = Py::Object(new SketchObjectPy(this),true);
    }
    return Py::new_reference_to(PythonObject); 
}

unsigned int SketchObject::getMemSize(void) const
{
    return 0;
}

void SketchObject::Save(Writer &writer) const
{
    // save the father classes
    Part::Part2DObject::Save(writer);
}

void SketchObject::Restore(XMLReader &reader)
{
    // read the father classes
    Part::Part2DObject::Restore(reader);
    rebuildVertexIndex();
}

void SketchObject::getGeoVertexIndex(int VertexId, int &GeoId, PointPos &PosId)
{
    if (VertexId < 0 || VertexId >= (int)VertexId2GeoId.size()) {
        GeoId = -1;
        PosId = none;
        return;
    }
    GeoId = VertexId2GeoId[VertexId];
    PosId = VertexId2PosId[VertexId];
}

// Python Sketcher feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Sketcher::SketchObjectPython, Sketcher::SketchObject)
template<> const char* Sketcher::SketchObjectPython::getViewProviderName(void) const {
    return "SketcherGui::ViewProviderPython";
}
/// @endcond

// explicit template instantiation
template class SketcherExport FeaturePythonT<Sketcher::SketchObject>;
}
