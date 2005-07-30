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
# include <TDF_Label.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>


#include "PointsFeature.h"
#include "PointsFeaturePy.h"

using namespace Points;


//===========================================================================
// Feature
//===========================================================================

PointsFeature::PointsFeature()
{
}

PointsFeature::~PointsFeature()
{
}

void PointsFeature::InitLabel(const TDF_Label &rcLabel)
{
}

Standard_Integer PointsFeature::Execute(TFunction_Logbook& log)
{
  return 0;
}

void PointsFeature::Validate(TFunction_Logbook& log)
{
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

