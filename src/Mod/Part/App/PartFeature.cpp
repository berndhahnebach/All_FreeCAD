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


#include <strstream>
#include <Base/Console.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>

#include "PartFeature.h"
#include "PartFeaturePy.h"
#include "TopologyPy.h"

using namespace Part;


PROPERTY_SOURCE(Part::Feature, App::DocumentObject)


Feature::Feature(void) 
{
    ADD_PROPERTY(Shape, (TopoDS_Shape()));
}

Feature::~Feature()
{
}

short Feature::mustExecute(void) const
{
    return 0;
}

App::DocumentObjectExecReturn *Feature::execute(void)
{
    return App::DocumentObject::StdReturn;
}


void Feature::setShape(const TopoDS_Shape &Shape)
{
    this->Shape.setValue(Shape);
}

TopoDS_Shape Feature::getShape(void)
{
    return Shape.getValue();
}

PyObject *Feature::getPyObject(void)
{
    if (PythonObject.is(Py::_None())){
        // ref counter is set to 1
        PythonObject.set(new PartFeaturePy(this),true);
    }
    return Py::new_reference_to(PythonObject); 
}

