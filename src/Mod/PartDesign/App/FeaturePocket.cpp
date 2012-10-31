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
# include <gp_Dir.hxx>
# include <gp_Pln.hxx>
# include <BRep_Builder.hxx>
# include <BRep_Tool.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <BRepBndLib.hxx>
# include <BRepPrimAPI_MakePrism.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <Geom_Plane.hxx>
# include <Handle_Geom_Surface.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopoDS_Solid.hxx>
# include <TopExp_Explorer.hxx>
# include <BRepAlgoAPI_Cut.hxx>
# include <BRepPrimAPI_MakeHalfSpace.hxx>
# include <BRepAlgoAPI_Common.hxx>
#endif

#include <Base/Placement.h>
#include <App/Document.h>

#include "FeaturePocket.h"


using namespace PartDesign;

const char* Pocket::TypeEnums[]= {"Length","UpToLast","UpToFirst","ThroughAll","UpToFace",NULL};

PROPERTY_SOURCE(PartDesign::Pocket, PartDesign::Subtractive)

Pocket::Pocket()
{
    ADD_PROPERTY(Type,((long)0));
    Type.setEnums(TypeEnums);
    ADD_PROPERTY(Length,(100.0));
    ADD_PROPERTY(FaceName,(""));
}

short Pocket::mustExecute() const
{
    if (Placement.isTouched() ||
        Length.isTouched() ||
        FaceName.isTouched())
        return 1;
    return Subtractive::mustExecute();
}

App::DocumentObjectExecReturn *Pocket::execute(void)
{
    // Validate parameters
    double L = Length.getValue();
    if ((std::string(Type.getValueAsString()) == "Length") && (L < Precision::Confusion()))
        return new App::DocumentObjectExecReturn("Pocket: Length of pocket too small");

    Part::Part2DObject* sketch = 0;
    std::vector<TopoDS_Wire> wires;
    TopoDS_Shape support;
    try {
        sketch = getVerifiedSketch();
        wires = getSketchWires();
        support = getSupportShape();
    } catch (const Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    // get the Sketch plane
    Base::Placement SketchPos = sketch->Placement.getValue();
    Base::Rotation SketchOrientation = SketchPos.getRotation();
    Base::Vector3d SketchVector(0,0,1);
    SketchOrientation.multVec(SketchVector,SketchVector);

    this->positionBySketch();
    TopLoc_Location invObjLoc = this->getLocation().Inverted();

    try {
        TopoDS_Face aFace = TopoDS::Face(makeFace(wires));
        if (aFace.IsNull())
            return new App::DocumentObjectExecReturn("Pocket: Creating a face from sketch failed");
        // This is a trick to avoid problems with the cut operation. Sometimes a cut doesn't
        // work as expected if faces are coincident. Thus, we move the face in normal direction
        // but make it longer by one unit in the opposite direction.
        // TODO: Isn't one unit (one millimeter) a lot, assuming someone models a really tiny solid?
        // What about using 2 * Precision::Confusion() ?
        gp_Trsf mov;
        mov.SetTranslation(gp_Vec(SketchVector.x,SketchVector.y,SketchVector.z));
        TopLoc_Location loc(mov);
        aFace.Move(loc);

        // lengthen the vector
        SketchVector *= (Length.getValue()+1);

        // turn around for pockets
        SketchVector *= -1;

        // extrude the face to a solid
        TopoDS_Shape prism;

        if ((std::string(Type.getValueAsString()) == "UpToLast") ||
            (std::string(Type.getValueAsString()) == "UpToFirst") ||
            (std::string(Type.getValueAsString()) == "UpToFace"))
        {
            TopoDS_Face upToFace;
            gp_Dir dir(SketchVector.x,SketchVector.y,SketchVector.z);

            if ((std::string(Type.getValueAsString()) == "UpToLast") ||
                (std::string(Type.getValueAsString()) == "UpToFirst"))
            {
                TopoDS_Shape origFace = makeFace(wires); // original sketch face before moving one unit
                std::vector<Part::cutFaces> cfaces = Part::findAllFacesCutBy(support, origFace, dir);
                if (cfaces.empty())
                      return new App::DocumentObjectExecReturn("No faces found in this direction");

                // Find nearest/furthest face
                std::vector<Part::cutFaces>::const_iterator it, it_near, it_far;
                it_near = it_far = cfaces.begin();
                for (it = cfaces.begin(); it != cfaces.end(); it++)
                    if (it->distsq > it_far->distsq)
                        it_far = it;
                    else if (it->distsq < it_near->distsq)
                        it_near = it;
                upToFace = (std::string(Type.getValueAsString()) == "UpToLast" ? it_far->face : it_near->face);
            } else {
                if (FaceName.isEmpty())
                    return new App::DocumentObjectExecReturn("Cannot extrude up to face: No face selected");

                // Get active object, this is the object that the user referenced when he clicked on the face!
                App::DocumentObject* baseLink = this->getDocument()->getActiveObject();

                if (!baseLink)
                    return new App::DocumentObjectExecReturn("Cannot extrude up to face: No object linked");
                if (!baseLink->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
                    return new App::DocumentObjectExecReturn("Cannot extrude up to face: Linked object is not a Part object");
                Part::Feature *base = static_cast<Part::Feature*>(baseLink);
                const Part::TopoShape& baseShape = base->Shape.getShape();
                if (baseShape._Shape.IsNull())
                    return new App::DocumentObjectExecReturn("Cannot extrude up to face: Cannot work on invalid shape");

                TopoDS_Shape sub = baseShape.getSubShape(FaceName.getValue());
                if (!sub.IsNull() && sub.ShapeType() == TopAbs_FACE)
                    upToFace = TopoDS::Face(sub);
                else
                    return new App::DocumentObjectExecReturn("Cannot extrude up to face: Selection is not a face");

                // Find the origin of this face (i.e. a vertex or a edge in a sketch)
                TopoDS_Shape origin = base->findOriginOf(sub);

                // Validate face
                // TODO: This would also exclude faces that are valid but not cut by the line
                // So for now we trust to the intelligence of the user when picking the face
                /*std::vector<cutFaces> cfaces = findAllFacesCutBy(upToFace, origFace, dir);
                if (cfaces.empty())
                      return new App::DocumentObjectExecReturn("No faces found in this direction");*/
            }

            // Create semi-infinite prism from sketch in direction dir
            dir.Transform(invObjLoc.Transformation());
            BRepPrimAPI_MakePrism PrismMaker(aFace.Moved(invObjLoc),dir,0,0,1);
            if (!PrismMaker.IsDone())
                return new App::DocumentObjectExecReturn("Cannot extrude up to face: Could not extrude the sketch!");

            // Cut off the prism at the face we found
            // Grab any point from the sketch
            TopExp_Explorer exp;
            exp.Init(aFace, TopAbs_VERTEX);
            if (!exp.More())
                return new App::DocumentObjectExecReturn("Cannot extrude up to face: Sketch without points?");
            gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));

            // Create a halfspace from the face, extending in direction of sketch plane
            BRepPrimAPI_MakeHalfSpace mkHalfSpace(upToFace, aPnt);
            if (!mkHalfSpace.IsDone())
                return new App::DocumentObjectExecReturn("Cannot extrude up to face: HalfSpace creation failed");

            // Find common material between halfspace and prism
            BRepAlgoAPI_Common mkCommon(PrismMaker.Shape(), mkHalfSpace.Solid().Moved(invObjLoc));
            if (!mkCommon.IsDone())
                return new App::DocumentObjectExecReturn("Cannot extrude up to face: Common creation failed");

            prism = this->getSolid(mkCommon.Shape());
            if (prism.IsNull())
                return new App::DocumentObjectExecReturn("Cannot extrude up to face: Resulting shape is not a solid");
        } else if (std::string(Type.getValueAsString()) == "ThroughAll") {
            gp_Dir dir(SketchVector.x,SketchVector.y,SketchVector.z);
            dir.Transform(invObjLoc.Transformation());
            BRepPrimAPI_MakePrism PrismMaker(aFace.Moved(invObjLoc),dir,1,0,1); // infinite prism (in both directions!)
            if (!PrismMaker.IsDone())
                return new App::DocumentObjectExecReturn("Could not extrude the sketch!");
            prism = PrismMaker.Shape();
        } else if (std::string(Type.getValueAsString()) == "Length") {
            gp_Vec vec(SketchVector.x,SketchVector.y,SketchVector.z);
            vec.Transform(invObjLoc.Transformation());
            BRepPrimAPI_MakePrism PrismMaker(aFace.Moved(invObjLoc),vec,0,1); // finite prism
            if (!PrismMaker.IsDone())
                return new App::DocumentObjectExecReturn("Could not extrude the sketch!");
            prism = PrismMaker.Shape();
        } else {
            return new App::DocumentObjectExecReturn("Internal error: Unknown type for Pocket feature");
        }

        this->SubShape.setValue(prism);

        // Cut out the pocket
        BRepAlgoAPI_Cut mkCut(support.Moved(invObjLoc), prism);

        // Let's check if the fusion has been successful
        if (!mkCut.IsDone())
            return new App::DocumentObjectExecReturn("Cut with support failed");

        // we have to get the solids (fuse sometimes creates compounds)
        TopoDS_Shape solRes = this->getSolid(mkCut.Shape());
        if (solRes.IsNull())
            return new App::DocumentObjectExecReturn("Resulting shape is not a solid");

        this->Shape.setValue(solRes);

        return App::DocumentObject::StdReturn;
    } catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        if (std::string(e->GetMessageString()) == "TopoDS::Face")
            return new App::DocumentObjectExecReturn("Could not create face from sketch.\n"
                "Intersecting sketch entities or multiple faces in a sketch are not allowed.");
        else
            return new App::DocumentObjectExecReturn(e->GetMessageString());
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}

