/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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


#include <HLRBRep_Algo.hxx>
#include <TopoDS_Shape.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
//#include <BRepAPI_MakeOutLine.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <BRep_Tool.hxx>


#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Mod/Part/App/PartFeature.h>

#include "FeatureViewPart.h"

using namespace Drawing;
using namespace std;


//===========================================================================
// FeatureView
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureViewPart, Drawing::FeatureView)


FeatureViewPart::FeatureViewPart(void) 
{
  static const char *group = "Shape view";

  ADD_PROPERTY_TYPE(Source ,(0),group,App::Prop_None,"Shape to view");
}

FeatureViewPart::~FeatureViewPart()
{
}

App::DocumentObjectExecReturn *FeatureViewPart::execute(void)
{
    std::stringstream result;

    App::DocumentObject* link = Source.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");
    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");
    TopoDS_Shape shape = static_cast<Part::Feature*>(link)->Shape.getShape()._Shape;
    if (shape.IsNull())
        return new App::DocumentObjectExecReturn("Linked shape object is empty");

    Handle( HLRBRep_Algo ) brep_hlr = new HLRBRep_Algo;
    brep_hlr->Add( shape );

    gp_Ax2 transform(gp_Pnt(0,0,0),gp_Dir(0,0,1));
    HLRAlgo_Projector projector( transform );
    brep_hlr->Projector( projector );
    brep_hlr->Update();
    brep_hlr->Hide();

    // extracting the result sets:
    HLRBRep_HLRToShape shapes( brep_hlr );

    TopoDS_Shape VisiblyEdges;
    VisiblyEdges = shapes.VCompound();

    TopoDS_Shape HiddenEdges;
    HiddenEdges = shapes.HCompound();

    //HLRBRep_HLRToShape Tool(Hider);
    //TopoDS_Shape V  = Tool.VCompound       ();// artes vives       vues
    //TopoDS_Shape V1 = Tool.Rg1LineVCompound();// artes rgulires  vues
    //TopoDS_Shape VN = Tool.RgNLineVCompound();// artes de couture  vues
    //TopoDS_Shape VO = Tool.OutLineVCompound();// contours apparents vus
    //TopoDS_Shape VI = Tool.IsoLineVCompound();// isoparamtriques   vues
    //TopoDS_Shape H  = Tool.HCompound       ();// artes vives       caches
    //TopoDS_Shape H1 = Tool.Rg1LineHCompound();// artes rgulires  caches
    //TopoDS_Shape HN = Tool.RgNLineHCompound();// artes de couture  caches
    //TopoDS_Shape HO = Tool.OutLineHCompound();// contours apparents cachs
    //TopoDS_Shape HI = Tool.IsoLineHCompound();// isoparamtriques   caches

    result << "<g" << endl
           //<< "  id=\"" << ViweName << "\"" << endl
           << "  fill=\"none\"" << endl
           //<< "  transform=\"" << ViweName << "\"" << endl
           << "  >" << endl;

    TopExp_Explorer edges( VisiblyEdges, TopAbs_EDGE );

    for ( ; edges.More(); edges.Next() ) {
      TopoDS_Edge edge = TopoDS::Edge( edges.Current() );
      TopLoc_Location location;
      Handle( Poly_Polygon3D ) polygon = BRep_Tool::Polygon3D( edge, location );
      if ( !polygon.IsNull() ) {
        const TColgp_Array1OfPnt& nodes = polygon->Nodes();
        //glBegin( GL_LINE_STRIP );
        char c = 'M';
        result << "<path d=\" "; 
        for ( int i = nodes.Lower(); i<= nodes.Upper(); i++ ){
            result << c << " " << nodes(i).X() << " " << nodes(i).Y()<< " " ; 
            c = 'L';
          //glVertex3f( nodes(i).X(), nodes(i).Y(), nodes(i).Z() );
        }
        result << "\" />" << endl;
        //glEnd();
      }
    }

    result << "</g>" << endl;

    // Apply the resulting fragment
    ViewResult.setValue(result.str().c_str());

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


