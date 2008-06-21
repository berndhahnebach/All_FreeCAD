/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef PART_FEATUREPARTCIRCLE_H
#define PART_FEATUREPARTCIRCLE_H

#include <App/PropertyStandard.h>
#include "PartFeature.h"

#if 1
#include <App/PropertyGeo.h>
#include <gp_Circ.hxx>
#endif

namespace Part
{
#if 1
/** The line property class.
 * @author Werner Mayer
 */
class AppPartExport PropertyCircle : public App::Property
{
    TYPESYSTEM_HEADER();

public:
    PropertyCircle();
    ~PropertyCircle();

    /** @name Getter/setter */
    //@{
    /// set the line
    void setValue( const gp_Circ& circle );
    /// get the line
    const gp_Circ& getValue(void) const;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject(void);
    void setPyObject(PyObject *value);
    //@}

    /** @name Save/restore */
    //@{
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);

    App::Property *Copy(void) const;
    void Paste(const App::Property &from);
    unsigned int getMemSize (void) const;
    //@}

private:
    gp_Circ _circle;
};
#endif 
class Circle : public Part::Feature
{
    PROPERTY_HEADER(Part::Circle);

public:
    Circle();
    virtual ~Circle();

    App::PropertyFloat Radius;
    App::PropertyAngle Angle0;
    App::PropertyAngle Angle1;
#if 1
    PropertyCircle Circ;
#endif

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const;
    void onChanged(const App::Property*);
    //@}
};

} //namespace Part

#endif // PART_FEATUREPARTCIRCLE_H
