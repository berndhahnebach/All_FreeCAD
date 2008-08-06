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


#include "PreCompiled.h"

#include "PartFeature.h"

// inclusion of the generated files (generated out of PartFeaturePy.xml)
#include "PartFeaturePy.h"
#include "PartFeaturePy.cpp"

using namespace Part;

// returns a string which represent the object e.g. when printed in python
const char *PartFeaturePy::representation(void) const
{
    return "<Part object>";
}

PyObject *PartFeaturePy::getCustomAttributes(const char* attr) const
{/*
    if ( attr[0]== 'F' && 
         attr[1]== 'a' && 
         attr[2]== 'c' && 
         attr[3]== 'e' && 
         attr[4]>=48 && attr[4]<=57 
         ){
        Part::ShapeSegment* s = dynamic_cast<Part::ShapeSegment*>(this->getFeaturePtr()->Shape.getValue()->getSubElementByName(attr));
        TopoDS_Shape Shape = s->Shape;
        delete s;
        return new TopoShapeFacePy(new TopoShape(Shape));
    }else if ( attr[0]== 'E' && 
               attr[1]== 'd' && 
               attr[2]== 'g' && 
               attr[3]== 'e' && 
               attr[4]>=48 && attr[4]<=57 
         ){
        Part::ShapeSegment* s = dynamic_cast<Part::ShapeSegment*>(getTopoShapePtr()->getSubElementByName(attr));
        TopoDS_Shape Shape = s->Shape;
        delete s;
        return new TopoShapeEdgePy(new TopoShape(Shape));
    }else if ( attr[0]== 'V' && 
               attr[1]== 'e' && 
               attr[2]== 'r' && 
               attr[3]== 't' && 
               attr[4]== 'e' && 
               attr[5]== 'x' && 
               attr[6]>=48 && attr[4]<=57 
               ){
        Part::ShapeSegment* s = dynamic_cast<Part::ShapeSegment*>(getTopoShapePtr()->getSubElementByName(attr));
        TopoDS_Shape Shape = s->Shape;
        delete s;
        return new TopoShapeVertexPy(new TopoShape(Shape));
    }*/
    return 0;
}

int PartFeaturePy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


