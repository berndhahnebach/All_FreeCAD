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



#ifndef PART_PART2DOBJECT_H
#define PART_PART2DOBJECT_H

#include <App/PropertyStandard.h>

#include "PartFeature.h"

class TopoDS_Face;

namespace Part
{

/** 2D Shape 
  * This is a specialiced Version of the PartShape for use with 
  * Flat (2D) geometry. The Z direction have always to be 0.
  * The position and orientation of the Plane this 2D geometry is
  * referenced in is defined by the Placement property. It allso
  * has a link to a supporting Face which defines the position 
  * in space where its located. If the support is changed the
  * static methode positionBySupport() is used to calculate a 
  * new position for the Part2DObject.
  * This object can be used stand alone or for constraint 
  * geometry as its descend Sketcher::SketchObject .
  */

class AppPartExport Part2DObject :public Part::Feature
{
    PROPERTY_HEADER(Part::Part2DObject);

public:
    Part2DObject();

    /// if the 2DObject lies on the Face of an other object this links to it
    App::PropertyLinkSub        Support;

    /** calculate a position out of a support face
      * this methode will calculate the position of the 
      * 2D shape on a supporting Face. The Normal(Orientation) get 
      * extracted from the Face and for the position an educated guess is made,
      * by examining the placement of the support object (not only the face).
      * If the support is changed this methode is called do determine a new 
      * postion of the 2D shape on the supporting Face
      */
    static Base::Placement positionBySupport(const TopoDS_Face &face,const Base::Placement &Place );

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "PartGui::ViewProvider2DObject";
    }
    //@}

};

typedef App::FeaturePythonT<Part2DObject> Part2DObjectPython;

} //namespace Part


#endif // PART_PART2DOBJECT_H
