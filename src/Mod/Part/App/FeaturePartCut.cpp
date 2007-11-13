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
#endif


#include "FeaturePartCut.h"

#include <BRepAlgoAPI_Cut.hxx>


using namespace Part;

PROPERTY_SOURCE(Part::Cut, Part::Feature)


Cut::Cut(void)
{
    ADD_PROPERTY(Base,(0));
    ADD_PROPERTY(Tool,(0));
}

short Cut::mustExecute() const
{
    if (Base.getValue() && Tool.getValue())
        return -1;
    return 0;
}

App::DocumentObjectExecReturn *Cut::execute(void)
{
    Part::Feature *pcFirst  = dynamic_cast<Part::Feature*>(Base.getValue());
    Part::Feature *pcSecond = dynamic_cast<Part::Feature*>(Tool.getValue());

    // Now, let's get the TopoDS_Shape
    TopoDS_Shape OriginalShape  = pcFirst->getShape();
    TopoDS_Shape ToolShape = pcSecond->getShape();

    // STEP 2:
    // Let's call for algorithm computing a cut operation:
    BRepAlgoAPI_Cut mkCut(OriginalShape, ToolShape);
    // Let's check if the Cut has been successfull:
    if (!mkCut.IsDone()) 
        return new App::DocumentObjectExecReturn("Cut operation failed");

    TopoDS_Shape ResultShape = mkCut.Shape();
    setShape(ResultShape);

    return App::DocumentObject::StdReturn;
}




