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

    sketch.setUpSketch(Geometry.getValues(),Constraints.getValues());
 
    // solve the sketch with no fixed points
    double * fixed[2]={0,0};
    if (sketch.solve(fixed) != 0)
        return new App::DocumentObjectExecReturn("Solving the sketch failed!",this);

    std::vector<Part::Geometry *> geomlist = sketch.getGeometry();
    Geometry.setValues(geomlist);
    for (std::vector<Part::Geometry *>::iterator it = geomlist.begin(); it != geomlist.end(); ++it)
        if (*it) delete *it;

    Shape.setValue(sketch.toShape());

    return App::DocumentObject::StdReturn;
}

void SketchObject::setDatum(double Datum, int ConstrNbr)
{
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
    return Geometry.getSize()-1;
}

int SketchObject::delGeometry(int GeoNbr)
{
    const std::vector< Part::Geometry * > &vals = this->Geometry.getValues();
    if (GeoNbr < 0 || GeoNbr >= (int)vals.size())
        return -1;

    std::vector< Part::Geometry * > newVals(vals);
    newVals.erase(newVals.begin()+GeoNbr);

    std::vector< Constraint * > constraints = this->Constraints.getValues();
    std::vector< Constraint * > newConstraints(0);
    for (std::vector<Constraint *>::iterator it = constraints.begin();
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

}

void SketchObject::getGeoVertexIndex(int VertexId,int &GeoId,int &PointPos)
{
    PointPos = VertexId%2 + 1;
    GeoId = (int)VertexId/2;
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
