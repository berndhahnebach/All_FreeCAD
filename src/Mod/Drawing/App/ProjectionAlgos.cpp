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
#include <BRepMesh.hxx>


#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Mod/Part/App/PartFeature.h>

#include "ProjectionAlgos.h"

using namespace Drawing;
using namespace std;


//===========================================================================
// FeatureView
//===========================================================================



ProjectionAlgos::ProjectionAlgos(const TopoDS_Shape &Input,const Base::Vector3f &Dir) 
:Input(Input), Direction(Dir)
{
	execute();
}

ProjectionAlgos::~ProjectionAlgos()
{
}

void ProjectionAlgos::execute(void)
{

    Handle( HLRBRep_Algo ) brep_hlr = new HLRBRep_Algo;
    brep_hlr->Add( Input );

    gp_Ax2 transform(gp_Pnt(0,0,0),gp_Dir(Direction.x,Direction.y,Direction.z));
    HLRAlgo_Projector projector( transform );
    brep_hlr->Projector( projector );
    brep_hlr->Update();
    brep_hlr->Hide();

    // extracting the result sets:
    HLRBRep_HLRToShape shapes( brep_hlr );

    V  = shapes.VCompound       ();// hard edge visibly
    V1 = shapes.Rg1LineVCompound();// Smoth edges visibly
    VN = shapes.RgNLineVCompound();// contour edges visibly
    VO = shapes.OutLineVCompound();// contours apparents visibly
    VI = shapes.IsoLineVCompound();// isoparamtriques   visibly
    H  = shapes.HCompound       ();// hard edge       invisibly
    H1 = shapes.Rg1LineHCompound();// Smoth edges  invisibly
    HN = shapes.RgNLineHCompound();// contour edges invisibly
    HO = shapes.OutLineHCompound();// contours apparents invisibly
    HI = shapes.IsoLineHCompound();// isoparamtriques   invisibly

}

std::string  ProjectionAlgos::getSVG(SvgExtractioType type)
{
	std::stringstream result;


	BRepMesh::Mesh(V1,0.1);
    result  << "<g" 
            //<< " id=\"" << ViewName << "\"" << endl
		    << "   stroke=\"rgb(0, 0, 0)\"" << endl 
			<< "   stroke-width=\"0.15\"" << endl
			<< "   stroke-linecap=\"butt\"" << endl
			<< "   stroke-linejoin=\"miter\"" << endl
            << "   fill=\"none\"" << endl
            << "  >" << endl
			<< Edges2SVG(V1)
			<< "</g>" << endl;
	BRepMesh::Mesh(V,0.1);
    result  << "<g" 
            //<< " id=\"" << ViewName << "\"" << endl
		    << "   stroke=\"rgb(0, 0, 0)\"" << endl 
			<< "   stroke-width=\"0.35\"" << endl
			<< "   stroke-linecap=\"butt\"" << endl
			<< "   stroke-linejoin=\"miter\"" << endl
            << "   fill=\"none\"" << endl
            << "  >" << endl
			<< Edges2SVG(V)
			<< "</g>" << endl;

	return result.str();
}

std::string ProjectionAlgos::Edges2SVG(const TopoDS_Shape &Input)
{
	std::stringstream result;

    TopExp_Explorer edges( Input, TopAbs_EDGE );
    for (int i = 1 ; edges.More(); edges.Next(),i++ ) {
      TopoDS_Edge edge = TopoDS::Edge( edges.Current() );
      TopLoc_Location location;
      Handle( Poly_Polygon3D ) polygon = BRep_Tool::Polygon3D( edge, location );
      if ( !polygon.IsNull() ) {
        const TColgp_Array1OfPnt& nodes = polygon->Nodes();
         char c = 'M';
        result << "<path id= \"" /*<< ViewName*/ << i << "\" d=\" "; 
        for ( int i = nodes.Lower(); i<= nodes.Upper(); i++ ){
            result << c << " " << nodes(i).X() << " " << nodes(i).Y()<< " " ; 
            c = 'L';
        }
        result << "\" />" << endl;
      }
    }
	return result.str();
}


