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

#include <Mod/Part/App/Part2DObject.h>

#include <Mod/Part/App/PropertyGeometryList.h>
#include <Mod/Sketcher/App/PropertyConstraintList.h>

namespace Sketcher
{


class SketchObject :public Part::Part2DObject
{
    PROPERTY_HEADER(Sketcher::SketchObject);

public:
    SketchObject();

    /// Property
    Part    ::PropertyGeometryList   Geometry;
    Sketcher::PropertyConstraintList Constraints;

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "SketcherGui::ViewProviderSketch";
    }
    //@}

	// from base class
    virtual PyObject *getPyObject(void);
    virtual unsigned int getMemSize (void) const;
	virtual void Save (Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

};

} //namespace Part


#endif // SKETCHER_SKETCHOBJECT_H
