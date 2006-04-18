/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <BRep_Builder.hxx>
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <Geom_Line.hxx>
# include <gp_Pnt.hxx>
# include <TopoDS_Edge.hxx>
#endif

#include "FeaturePartLine.h"

using namespace Part;

PROPERTY_SOURCE(Part::Line, Part::Feature)


Line::Line()
{
  ADD_PROPERTY(b,(0.0f,0.0f,0.0f));
  ADD_PROPERTY(e,(1.0f,1.0f,1.0f));
}

Line::~Line()
{
}

int Line::execute(void)
{
  Base::Vector3f beg = b.getValue();
  Base::Vector3f end = e.getValue();

  // Convert into OCC representation
  gp_Pnt pnt1(beg.x, beg.y, beg.z);
  gp_Pnt pnt2(end.x, end.y, end.z);

  // Create directly the underlying line geometry
  BRepBuilderAPI_MakeEdge makeEdge(pnt1,pnt2);
  
  bool ok = false;
  switch ( makeEdge.Error() )
  {
  case BRepBuilderAPI_EdgeDone:
    ok = true;
    break; // ok
  case BRepBuilderAPI_PointProjectionFailed:
    setError("Point projection failed");
    break;
  case BRepBuilderAPI_ParameterOutOfRange:
    setError("Parameter out of range");
    break;
  case BRepBuilderAPI_DifferentPointsOnClosedCurve:
    setError("Different points on closed curve");
    break;
  case BRepBuilderAPI_PointWithInfiniteParameter:
    setError("Point with infinite parameter");
    break;
  case BRepBuilderAPI_DifferentsPointAndParameter:
    setError("Different point and parameter");
    break;
  case BRepBuilderAPI_LineThroughIdenticPoints:
    setError("Line through identic points");
    break;
  }

  // Error 
  if ( !ok ) return 1;

  TopoDS_Edge edge = makeEdge.Edge();
  setShape(edge);

  return 0;
}

