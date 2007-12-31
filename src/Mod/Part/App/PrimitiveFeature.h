/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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



#ifndef PART_PRIMITIVEFEATURE_H
#define PART_PRIMITIVEFEATURE_H

#include <App/PropertyStandard.h>
#include "PartFeature.h"

namespace Part
{

class Plane : public Part::Feature
{
    PROPERTY_HEADER(Part::Plane);

public:
    Plane();

    App::PropertyDistance Length;
    App::PropertyDistance Width;

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Sphere : public Part::Feature
{
    PROPERTY_HEADER(Part::Sphere);

public:
    Sphere();

    App::PropertyFloatConstraint Radius;
    App::PropertyFloatConstraint Angle1;
    App::PropertyFloatConstraint Angle2;
    App::PropertyFloatConstraint Angle3;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Cylinder : public Part::Feature
{
    PROPERTY_HEADER(Part::Cylinder);

public:
    Cylinder();

    App::PropertyDistance Radius;
    App::PropertyDistance Height;
    App::PropertyFloatConstraint Angle;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Cone : public Part::Feature
{
    PROPERTY_HEADER(Part::Cone);

public:
    Cone();

    App::PropertyDistance Radius1;
    App::PropertyDistance Radius2;
    App::PropertyDistance Height;
    App::PropertyFloatConstraint Angle;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Torus : public Part::Feature
{
    PROPERTY_HEADER(Part::Torus);

public:
    Torus();

    App::PropertyFloatConstraint Radius1;
    App::PropertyFloatConstraint Radius2;
    App::PropertyFloatConstraint Angle1;
    App::PropertyFloatConstraint Angle2;
    App::PropertyFloatConstraint Angle3;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

} //namespace Part


#endif // PART_PRIMITIVEFEATURE_H
