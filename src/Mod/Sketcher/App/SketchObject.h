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



#ifndef SKETCHER_SKETCHOBJECT_H
#define SKETCHER_SKETCHOBJECT_H

#include <App/PropertyStandard.h>
#include <App/PropertyFile.h>
#include <App/FeaturePython.h>

#include <Mod/Part/App/Part2DObject.h>
#include <Mod/Part/App/PropertyGeometryList.h>
#include <Mod/Sketcher/App/PropertyConstraintList.h>

namespace Sketcher
{

class AppSketcherExport SketchObject :public Part::Part2DObject
{
    PROPERTY_HEADER(Sketcher::SketchObject);

public:
    SketchObject();

    /// Property
    Part    ::PropertyGeometryList   Geometry;
    Sketcher::PropertyConstraintList Constraints;
    App     ::PropertyLinkSubList    ExternalConstraints;
    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "SketcherGui::ViewProviderSketch";
    }
    //@}

    /// add unspecified geometry
    int addGeometry(const Part::Geometry *geo);
    /// add unspecified geometry
    int addGeometry(const std::vector<Part::Geometry *> &geoList);
    /// delete geometry
    int delGeometry(int GeoNbr);
    /// add all constraints in the list
    int addConstraints(const std::vector<Constraint *> &ConstraintList);
    /// add constraint
    int addConstraint(const Constraint *constraint);
    /// delete constraint
    int delConstraint(int ConstrNbr);

    /// set the datum of a Distance or Angle constraint
    void setDatum(double Datum, int ConstrNbr);

    /** retrieves for a Vertex number the corosponding GeoId and PointPosition
    */
    void getGeoVertexIndex(int VertexId, int &GeoId, int &PointPos);

    // from base class
    virtual PyObject *getPyObject(void);
    virtual unsigned int getMemSize (void) const;
    virtual void Save (Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

};

typedef App::FeaturePythonT<SketchObject> SketchObjectPython;

} //namespace Sketcher


#endif // SKETCHER_SKETCHOBJECT_H
