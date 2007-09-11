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


#include <Base/Exception.h>
#include <boost/regex.hpp>
#include <iostream>

#include "FeaturePage.h"

using namespace Drawing;
using namespace std;

  
//===========================================================================
// FeatureView
//===========================================================================

PROPERTY_SOURCE(Drawing::FeaturePage, App::AbstractFeature)

const char *group = "Drawing view";

FeaturePage::FeaturePage(void) 
{
  static const char *group = "Drawing view";

  ADD_PROPERTY_TYPE(Views ,(0),group,App::Prop_None,"List of views in this page");

  ADD_PROPERTY_TYPE(PageResult ,(""),group,App::Prop_Output,"Resulting SVG document of that page");
  ADD_PROPERTY_TYPE(Template   ,(""),group,App::Prop_None  ,"Template for the page");
}

FeaturePage::~FeaturePage()
{
}

int FeaturePage::execute(void)
{
  const char* text = "lskdfjlsd";
  const char* regex = "lskdflds";
  boost::regex e(regex);
  boost::smatch what;
  if(boost::regex_match(string(text), what, e))
  {
  }
  return 0;
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


