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

#include "PartFeature.h"
#include "PartFeaturePy.h"

using namespace Part;


//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Part::Feature, App::AbstractFeature)


Feature::Feature(void) : _featurePy(0)
{
}

Feature::~Feature()
{
  if ( _featurePy )
  {
    _featurePy->setInvalid();
    _featurePy->DecRef();
  }
}

int Feature::execute(void)
{
  return 0;
}


void Feature::setShape(const TopoDS_Shape &Shape)
{
  _Shape = Shape;
}

TopoDS_Shape Feature::getShape(void)
{
  return _Shape;
}


Base::PyObjectBase *Feature::GetPyObject(void)
{
  if ( !_featurePy ) {
    _featurePy = new PartFeaturePy(this);
  }

  _featurePy->IncRef();
  return _featurePy;
}


