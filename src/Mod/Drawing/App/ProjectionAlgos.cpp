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
# include <BRepAdaptor_Curve.hxx>
# include <Geom_Circle.hxx>
# include <gp_Circ.hxx>
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
#include <gp_Vec.hxx>
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
// ProjectionAlgos
//===========================================================================



ProjectionAlgos::ProjectionAlgos(const TopoDS_Shape &Input,const Base::Vector3f &Dir) 
  : Input(Input), Direction(Dir)
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
    brep_hlr->Projector(projector);
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

std::string ProjectionAlgos::getSVG(SvgExtractionType type, float scale)
{
    std::stringstream result;
    if (!H.IsNull() && type==WithHidden) {
        float width = 0.15f/scale;
        BRepMesh::Mesh(H,0.1);
        result  << "<g" 
                //<< " id=\"" << ViewName << "\"" << endl
                << "   stroke=\"rgb(0, 0, 0)\"" << endl 
                << "   stroke-width=\"" << width << "\"" << endl
                << "   stroke-linecap=\"butt\"" << endl
                << "   stroke-linejoin=\"miter\"" << endl
                << "   stroke-dasharray=\"5 3\"" << endl
                << "   fill=\"none\"" << endl
                << "  >" << endl
                << Edges2SVG(H)
                << "</g>" << endl;
    }
    if (!HO.IsNull() && type==WithHidden) {
        float width = 0.15f/scale;
        BRepMesh::Mesh(HO,0.1);
        result  << "<g" 
                //<< " id=\"" << ViewName << "\"" << endl
                << "   stroke=\"rgb(0, 0, 0)\"" << endl 
                << "   stroke-width=\"" << width << "\"" << endl
                << "   stroke-linecap=\"butt\"" << endl
                << "   stroke-linejoin=\"miter\"" << endl
                << "   stroke-dasharray=\"5 3\"" << endl
                << "   fill=\"none\"" << endl
                << "  >" << endl
                << Edges2SVG(HO)
                << "</g>" << endl;
    }
    if (!VO.IsNull()) {
        float width = 0.35f/scale;
        BRepMesh::Mesh(VO,0.1);
        result  << "<g" 
                //<< " id=\"" << ViewName << "\"" << endl
                << "   stroke=\"rgb(0, 0, 0)\"" << endl 
                << "   stroke-width=\"" << width << "\"" << endl
                << "   stroke-linecap=\"butt\"" << endl
                << "   stroke-linejoin=\"miter\"" << endl
                << "   fill=\"none\"" << endl
                << "  >" << endl
                << Edges2SVG(VO)
                << "</g>" << endl;
    }
    if (!V.IsNull()) {
        float width = 0.35f/scale;
        BRepMesh::Mesh(V,0.1);
        result  << "<g" 
                //<< " id=\"" << ViewName << "\"" << endl
                << "   stroke=\"rgb(0, 0, 0)\"" << endl 
                << "   stroke-width=\"" << width << "\"" << endl
                << "   stroke-linecap=\"butt\"" << endl
                << "   stroke-linejoin=\"miter\"" << endl
                << "   fill=\"none\"" << endl
                << "  >" << endl
                << Edges2SVG(V)
                << "</g>" << endl;
    }

    return result.str();
}

std::string ProjectionAlgos::Edges2SVG(const TopoDS_Shape &Input)
{
    std::stringstream result;

    TopExp_Explorer edges( Input, TopAbs_EDGE );
    for (int i = 1 ; edges.More(); edges.Next(),i++ ) {
        const TopoDS_Edge& edge = TopoDS::Edge(edges.Current());
        BRepAdaptor_Curve adapt(edge);
        if (adapt.GetType() == GeomAbs_Circle) {
            printCircle(adapt, result);
        }
        // fallback
        else {
            TopLoc_Location location;
            Handle(Poly_Polygon3D) polygon = BRep_Tool::Polygon3D(edge, location);
            if (!polygon.IsNull()) {
                const TColgp_Array1OfPnt& nodes = polygon->Nodes();
                char c = 'M';
                result << "<path id= \"" /*<< ViewName*/ << i << "\" d=\" "; 
                for (int i = nodes.Lower(); i <= nodes.Upper(); i++){
                    result << c << " " << nodes(i).X() << " " << nodes(i).Y()<< " " ; 
                    c = 'L';
                }
                result << "\" />" << endl;
            }
        }
    }

    return result.str();
}

void ProjectionAlgos::printCircle(const BRepAdaptor_Curve& c, std::ostream& out)
{
    gp_Circ circ = c.Circle();
    const gp_Pnt& p= circ.Location();
    double r = circ.Radius();
    double f = c.FirstParameter();
    double l = c.LastParameter();
    gp_Pnt s = c.Value(f);
    gp_Pnt e = c.Value(l);
    gp_Vec v1(p,s);
    gp_Vec v2(p,e);
    gp_Vec v3(0,0,1);
    double a = v3.DotCross(v1,v2);

    // a full circle
    if (s.SquareDistance(e) < 0.001) {
        out << "<circle cx =\"" << p.X() << "\" cy =\"" 
            << p.Y() << "\" r =\"" << r << "\" />";
    }
    // arc of circle
    else {
        // See also https://developer.mozilla.org/en/SVG/Tutorial/Paths
        char xar = '0'; // x-axis-rotation
        char las = (l-f > D_PI) ? '1' : '0'; // large-arc-flag
        char swp = (a > 0) ? '1' : '0'; // sweep-flag, i.e. clockwise (0) or counter-clockwise (1)
        out << "<path d=\"M" << s.X() <<  " " << s.Y()
            << " A" << r << " " << r << " "
            << xar << " " << las << " " << swp << " "
            << e.X() << " " << e.Y() << "\" />";
    }
}
