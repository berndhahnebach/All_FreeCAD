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
# include <sstream>

#endif


#include <strstream>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include "FeatureView.h"

using namespace Drawing;


//===========================================================================
// FeatureView
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureView, App::AbstractFeature)



FeatureView::FeatureView(void) 
{
  static const char *group = "Drawing view";
  ADD_PROPERTY_TYPE(X ,(0),group,App::Prop_None,"X position of the view on the drawing in modeing units (mm)");
  ADD_PROPERTY_TYPE(Y ,(0),group,App::Prop_None,"Y position of the view on the drawing in modeing units (mm)");
  ADD_PROPERTY_TYPE(Scale ,(0),group,App::Prop_None,"Scale factor of the view");

  ADD_PROPERTY_TYPE(Direction ,(0,0,1.0),group,App::Prop_None,"Projection direction");

  ADD_PROPERTY_TYPE(ViewResult ,(0),group,App::Prop_Output,"Resulting SVG fragment of that view");
}

FeatureView::~FeatureView()
{
}

App::DocumentObjectExecReturn *FeatureView::execute(void)
{
    return App::DocumentObject::StdReturn;
}


//
//PyObject *Feature::getPyObject(void)
//{
// if(PythonObject.is(Py::_None())){
//    // ref counter is set to 1
//    PythonObject.set(new PartFeaturePy(this),true);
//  }
//  return Py::new_reference_to(PythonObject); 
//}


