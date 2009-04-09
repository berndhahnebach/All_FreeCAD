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

class Primitive : public Part::Feature
{
    PROPERTY_HEADER(Part::Primitive);

public:
    Primitive();
    virtual ~Primitive();

    // Note: Location and Axis is different to Placement
    App::PropertyVector Location;
    App::PropertyVector Axis;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void) = 0;
    short mustExecute() const;
    //@}
};

class Plane : public Primitive
{
    PROPERTY_HEADER(Part::Plane);

public:
    Plane();

    App::PropertyLength Length;
    App::PropertyLength Width;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Sphere : public Primitive
{
    PROPERTY_HEADER(Part::Sphere);

public:
    Sphere();

    App::PropertyFloatConstraint Radius;
    App::PropertyFloatConstraint Angle1;
    App::PropertyFloatConstraint Angle2;
    App::PropertyFloatConstraint Angle3;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Ellipsoid : public Primitive
{
    PROPERTY_HEADER(Part::Ellipsoid);

public:
    Ellipsoid();

    App::PropertyFloatConstraint Radius1;
    App::PropertyFloatConstraint Radius2;
    App::PropertyFloatConstraint Angle1;
    App::PropertyFloatConstraint Angle2;
    App::PropertyFloatConstraint Angle3;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Cylinder : public Primitive
{
    PROPERTY_HEADER(Part::Cylinder);

public:
    Cylinder();

    App::PropertyLength Radius;
    App::PropertyLength Height;
    App::PropertyFloatConstraint Angle;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Cone : public Primitive
{
    PROPERTY_HEADER(Part::Cone);

public:
    Cone();

    App::PropertyLength Radius1;
    App::PropertyLength Radius2;
    App::PropertyLength Height;
    App::PropertyFloatConstraint Angle;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

class Torus : public Primitive
{
    PROPERTY_HEADER(Part::Torus);

public:
    Torus();

    App::PropertyFloatConstraint Radius1;
    App::PropertyFloatConstraint Radius2;
    App::PropertyFloatConstraint Angle1;
    App::PropertyFloatConstraint Angle2;
    App::PropertyFloatConstraint Angle3;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    //@}
};

} //namespace Part


#endif // PART_PRIMITIVEFEATURE_H
