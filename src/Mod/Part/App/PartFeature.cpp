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
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <TFunction_Logbook.hxx>
#	include <TFunction_DriverTable.hxx>
#	include <TFunction_Function.hxx>
# include <TNaming_Builder.hxx>
# include <TNaming_NamedShape.hxx>
# include <TopoDS_Shape.hxx>
#	include <Standard_GUID.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;

#include "TopologyPy.h"

#include "PartFeature.h"
#include "PartFeaturePy.h"

using namespace Part;


//===========================================================================
// Feature
//===========================================================================

PartFeature::PartFeature(void)
{
  _showMode = "Normal";


}

void PartFeature::initFeature(void)
{
}

Standard_Integer PartFeature::execute(TFunction_Logbook& log)
{
  return 0;
}


void PartFeature::setShape(const TopoDS_Shape &Shape)
{
 	TNaming_Builder B(_cFeatureLabel);
	B.Generated(Shape);
}

TopoDS_Shape PartFeature::getShape(void)
{
  Handle(TNaming_NamedShape) ShapeToViewName;
  if (!( _cFeatureLabel.FindAttribute(TNaming_NamedShape::GetID(),ShapeToViewName) ))
    throw Base::Exception("PartFeature::getShape(): Shape not valid check Feature::getStatus() first!");

  // Now, let's get the TopoDS_Shape of these TNaming_NamedShape:
  return ShapeToViewName->Get();

}


Base::PyObjectBase *PartFeature::GetPyObject(void)
{
  return new PartFeaturePy(this);
}


