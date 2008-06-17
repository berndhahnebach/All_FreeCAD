/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <BRepPrimAPI_MakeBox.hxx>
#endif


#include <Base/Console.h>
#include "FeaturePartBox.h"


using namespace Part;


PROPERTY_SOURCE(Part::Box, Part::Feature)


Box::Box()
{
    ADD_PROPERTY(x,(0.0));
    ADD_PROPERTY(y,(0.0));
    ADD_PROPERTY(z,(0.0));
    ADD_PROPERTY(l,(100.0));
    ADD_PROPERTY(h,(100.0));
    ADD_PROPERTY(w,(100.0));
}

short Box::mustExecute() const
{
    if (x.isTouched() ||
        y.isTouched() ||
        z.isTouched() ||
        l.isTouched() ||
        h.isTouched() ||
        w.isTouched() )
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Box::execute(void)
{
    double X = x.getValue();
    double Y = y.getValue();
    double Z = z.getValue();
    double L = l.getValue();
    double H = h.getValue();
    double W = w.getValue();

    if ( L < gp::Resolution() ) 
        return new App::DocumentObjectExecReturn("Length of L too small");

    if ( H < gp::Resolution() ) 
        return new App::DocumentObjectExecReturn("Height of H too small");

    if ( W < gp::Resolution() ) 
      return new App::DocumentObjectExecReturn("Width of W too small");

    // Build a box using the dimension and position attributes
    BRepPrimAPI_MakeBox mkBox( gp_Pnt( X, Y, Z ), L, H, W );
    TopoDS_Shape ResultShape = mkBox.Shape();
    this->Shape.setValue(ResultShape);

    return App::DocumentObject::StdReturn;
}
