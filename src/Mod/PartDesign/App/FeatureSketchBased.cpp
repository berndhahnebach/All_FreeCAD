/***************************************************************************
 *   Copyright (c) 2010 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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
# include <BRep_Builder.hxx>
# include <BRepBndLib.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <TopoDS_Compound.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopExp_Explorer.hxx>
#endif


#include "FeatureSketchBased.h"


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

PROPERTY_SOURCE(PartDesign::SketchBased, Part::Feature)

SketchBased::SketchBased()
{
    ADD_PROPERTY(Sketch,(0));
}

TopoDS_Shape SketchBased::makeFace(std::list<TopoDS_Wire>& wires) const
{
    BRepBuilderAPI_MakeFace mkFace(wires.front());
    int countOuterEdges = 0;
    TopExp_Explorer xp;
    for (xp.Init(wires.front(), TopAbs_EDGE); xp.More(); xp.Next()) {
        if (++countOuterEdges > 1)
            break;
    }

    wires.pop_front();
    for (std::list<TopoDS_Wire>::iterator it = wires.begin(); it != wires.end(); ++it) {
        // It seems if the outer or inner wire has multiple edges the orientation should be
        // the same but inner wires with only a single edge should be reversed.
        // If the outer wire has only a single edge then inner wires with multiple edges
        // must be reversed.
        int countEdges = 0;
        TopExp_Explorer xp;
        for (xp.Init(*it, TopAbs_EDGE); xp.More(); xp.Next()) {
            if (++countEdges > 1)
                break;
        }

        if (countOuterEdges > 1) {
            if (countEdges == 1)
                it->Reverse();
        }
        else {
            if (countEdges > 1)
                it->Reverse();
        }

        mkFace.Add(*it);
    }
    return mkFace.Face();
}

TopoDS_Shape SketchBased::makeFace(const std::vector<TopoDS_Wire>& w) const
{
    if (w.empty())
        return TopoDS_Shape();

    //FIXME: Need a safe method to sort wire that the outermost one comes last
    // Currently it's done with the diagonal lengths of the bounding boxes
    std::vector<TopoDS_Wire> wires = w;
    std::sort(wires.begin(), wires.end(), Wire_Compare());
    std::list<TopoDS_Wire> wire_list;
    wire_list.insert(wire_list.begin(), wires.rbegin(), wires.rend());

    // separate the wires into several independent faces
    std::list< std::list<TopoDS_Wire> > sep_wire_list;
    while (!wire_list.empty()) {
        std::list<TopoDS_Wire> sep_list;
        TopoDS_Wire wire = wire_list.front();
        wire_list.pop_front();
        sep_list.push_back(wire);

        Bnd_Box box;
        BRepBndLib::Add(wire, box);
        box.SetGap(0.0);

        std::list<TopoDS_Wire>::iterator it = wire_list.begin();
        while (it != wire_list.end()) {
            Bnd_Box box2;
            BRepBndLib::Add(*it, box2);
            box2.SetGap(0.0);
            if (!box.IsOut(box2)) {
                sep_list.push_back(*it);
                it = wire_list.erase(it);
            }
            else {
                ++it;
            }
        }

        sep_wire_list.push_back(sep_list);
    }

    if (sep_wire_list.size() == 1) {
        std::list<TopoDS_Wire>& wires = sep_wire_list.front();
        return makeFace(wires);
    }
    else if (sep_wire_list.size() > 1) {
        TopoDS_Compound comp;
        BRep_Builder builder;
        builder.MakeCompound(comp);
        for (std::list< std::list<TopoDS_Wire> >::iterator it = sep_wire_list.begin(); it != sep_wire_list.end(); ++it) {
            TopoDS_Shape aFace = makeFace(*it);
            if (!aFace.IsNull())
                builder.Add(comp, aFace);
        }

        return comp;
    }
    else {
        return TopoDS_Shape(); // error
    }
}

}
