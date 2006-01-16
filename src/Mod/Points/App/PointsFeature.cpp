/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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

#include <Base/Console.h>
#include <Base/Exception.h>


#include "PointsFeature.h"
#include "PointsFeaturePy.h"

using namespace Points;


//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Points::PointsFeature, App::Feature)

PointsFeature::PointsFeature()
{
  // set default display mode
  _showMode = "Point";
}

PointsFeature::~PointsFeature()
{
}

void PointsFeature::Save (short indent, std::ostream &str)
{
  // save parent
  Feature::Save(indent,str);
  //reinterpret_cast<App::Feature*>(this)->Save(indent,str);

  _Points.Save(indent,str);

}

void PointsFeature::Restore(Base::Reader &reader)
{
  // save parent
  Feature::Restore(reader);

  _Points.Restore(reader);


}

int PointsFeature::execute(void)
{
  return 0;
}



void PointsFeature::setPoints(const PointsWithProperty& New)
{
  _Points = New;
  Touch();
}

Base::PyObjectBase *PointsFeature::GetPyObject(void)
{
  return new PointsFeaturePy(this);
}

