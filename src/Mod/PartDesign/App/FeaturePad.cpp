/***************************************************************************
 *   Copyright (c) 2009 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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
# include <Bnd_Box.hxx>
# include <gp_Pln.hxx>
# include <BRep_Builder.hxx>
# include <BRepBndLib.hxx>
# include <BRepPrimAPI_MakePrism.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <Geom_Plane.hxx>
# include <Handle_Geom_Surface.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopExp_Explorer.hxx>
#endif


#include "FeaturePad.h"


using namespace PartDesign;

namespace PartDesign {
// sort bounding boxes according to diagonal length
struct Wire_Compare {
    bool operator() (const TopoDS_Wire& w1, const TopoDS_Wire& w2)
    {
        Bnd_Box box1, box2;
        BRepBndLib::Add(w1, box1);
        box1.SetGap(0.0);

        BRepBndLib::Add(w2, box2);
        box2.SetGap(0.0);
        
        return box1.SquareExtent() < box2.SquareExtent();
    }
};
}


PROPERTY_SOURCE(PartDesign::Pad, Part::Feature)

Pad::Pad()
{
    ADD_PROPERTY(Base,(0));
    ADD_PROPERTY(Dir,(Base::Vector3f(0.0f,0.0f,1.0f)));
}

short Pad::mustExecute() const
{
    if (Base.isTouched() ||
        Dir.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Pad::execute(void)
{
    App::DocumentObject* link = Base.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");
    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");
    TopoDS_Shape shape = static_cast<Part::Feature*>(link)->Shape.getShape()._Shape;
    if (shape.IsNull())
        return new App::DocumentObjectExecReturn("Linked shape object is empty");
    TopExp_Explorer ex;
    std::vector<TopoDS_Wire> wires;
    for (ex.Init(shape, TopAbs_WIRE, TopAbs_FACE); ex.More(); ex.Next()) {
        wires.push_back(TopoDS::Wire(ex.Current()));
    }
    if (/*shape.ShapeType() != TopAbs_WIRE*/wires.empty()) // there can be several wires
        return new App::DocumentObjectExecReturn("Linked shape object is not a wire");

	/* Version from the blog
	Handle(Geom_Surface) aSurf = new Geom_Plane (gp::XOY());
	//anti-clockwise circles if too look from surface normal
	Handle(Geom_Curve) anExtC = new Geom_Circle (gp::XOY(), 10.);
	Handle(Geom_Curve) anIntC = new Geom_Circle (gp::XOY(), 5.);
	TopoDS_Edge anExtE = BRepBuilderAPI_MakeEdge (anExtC);
	TopoDS_Edge anIntE = BRepBuilderAPI_MakeEdge (anExtC);
	TopoDS_Wire anExtW = BRepBuilderAPI_MakeWire (anExtE);
	TopoDS_Wire anIntW = BRepBuilderAPI_MakeWire (anIntE);
	BRep_Builder aB;
	TopoDS_Face aFace;
	aB.MakeFace (aFace, aSurf, Precision::Confusion());
	//aB. (aFace, aSurf);
	aB.Add (aFace, anExtW);
	//aB.Add (aFace, anIntW.Reversed()); //material should lie on the right of the inner wire

	this->Shape.setValue(aFace);
	*/

	/* Version from First Application
    Base::Vector3f v = Dir.getValue();
    gp_Vec vec(v.x,v.y,v.z);
	gp_Pln Pln(gp_Pnt(0,0,0),gp_Dir(0,0,1));

    // Now, let's get the TopoDS_Shape
	TopoDS_Wire theWire = TopoDS::Wire(Shape.getShape()._Shape);
	BRepBuilderAPI_MakeFace FaceBuilder = BRepBuilderAPI_MakeFace(Pln,theWire);
	if(FaceBuilder.IsDone()){
		TopoDS_Face FaceProfile = FaceBuilder.Face();
		// Make a solid out of the face
		BRepPrimAPI_MakePrism PrismMaker = BRepPrimAPI_MakePrism(FaceProfile , vec);
		if(PrismMaker.IsDone()){
			this->Shape.setValue(PrismMaker.Shape());
		}
	}
	*/

	Handle(Geom_Surface) aSurf = new Geom_Plane (gp_Pln(gp_Pnt(0,0,0),gp_Dir(0,0,1)));
	//anti-clockwise circles if too look from surface normal
#if 0
	//TopoDS_Wire theWire = TopoDS::Wire(shape);
	//TopoDS_Face aFace = BRepBuilderAPI_MakeFace(theWire);
#else
    //FIXME: Need a safe method to sort wire that the outermost one comes last
    // Currently it's done with the diagonal lengths of the bounding boxes
    std::sort(wires.begin(), wires.end(), Wire_Compare());
    BRepBuilderAPI_MakeFace mkFace(wires.back());
    for (std::vector<TopoDS_Wire>::reverse_iterator it = wires.rbegin()+1; it != wires.rend(); ++it) {
        //it->Reverse(); // Shouldn't inner wires be reversed?
        mkFace.Add(*it);
    }
    TopoDS_Face aFace = mkFace.Face();
#endif

    // extrude the face to a solid
    Base::Vector3f v = Dir.getValue();
    gp_Vec vec(v.x,v.y,v.z);
	BRepPrimAPI_MakePrism PrismMaker(aFace,vec,0,1);
	if(PrismMaker.IsDone()){
		this->Shape.setValue(PrismMaker.Shape());
	}else
        this->Shape.setValue(aFace);

    return App::DocumentObject::StdReturn;
}

