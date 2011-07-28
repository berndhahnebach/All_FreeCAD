/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <Standard_math.hxx>
# include <Poly_Polygon3D.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Geom_Circle.hxx>
# include <Geom_TrimmedCurve.hxx>
# include <Inventor/actions/SoGetBoundingBoxAction.h>
# include <Inventor/SoPath.h>
# include <Inventor/SbBox3f.h>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoLineDetail.h>
# include <Inventor/details/SoPointDetail.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoImage.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoMarkerSet.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoAsciiText.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoVertexProperty.h>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoText2.h>
# include <Inventor/nodes/SoFont.h>

/// Qt Include Files
# include <QAction>
# include <QMenu>
# include <QMessageBox>
# include <QImage>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>
#include <Base/Console.h>
#include <Base/Vector3D.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/MenuManager.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include <Mod/Part/App/Geometry.h>
#include <Mod/Sketcher/App/SketchFlatInterface.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/Sketcher/App/Sketch.h>

#include "ViewProviderSketch.h"
#include "DrawSketchHandler.h"
#include "TaskDlgEditSketch.h"

using namespace SketcherGui;
using namespace Sketcher;

SbColor sCurveColor             (1.0f,1.0f,1.0f);
SbColor sCurveConstructionColor (0.2f,1.0f,0.2f);
SbColor sPointColor             (0.5f,0.5f,0.5f);
SbColor sConstraintColor        (0.0f,0.8f,0.0f);

SbColor ViewProviderSketch::PreselectColor(0.1f, 0.1f, 0.8f);
SbColor ViewProviderSketch::SelectColor   (0.1f, 0.1f, 0.8f);

//**************************************************************************
// Edit data structure

/// Data structure while edit the sketch
struct EditData {
    EditData():
    sketchHandler(0),
    DragPoint(-1),
    DragCurve(-1),
    DragConstraint(-1),
    PreselectPoint(-1),
    PreselectCurve(-1),
    //ActSketch(0),
    EditRoot(0),
    PointsMaterials(0),
    CurvesMaterials(0),
    PointsCoordinate(0),
    CurvesCoordinate(0),
    CurveSet(0),
    PointSet(0)
    {}

    // pointer to the active handler for new sketch objects
    DrawSketchHandler *sketchHandler;

    // dragged point
    int DragPoint;
    // dragged curve
    int DragCurve;
    // dragged constraint
    int DragConstraint;

    SbColor PreselectOldColor;
    int PreselectPoint;
    int PreselectCurve;
    int PreselectConstraint;
    // pointer to the Solver
    Sketcher::Sketch ActSketch;
    // container to track our own selected parts
    std::set<int> SelPointSet;
    std::set<int> SelCurvSet;
    std::set<int> SelConstraintSet;

    // helper data structure for the constraint rendering
    std::vector<ConstraintType> vConstrType;

    // nodes for the visuals
    SoSeparator   *EditRoot;
    SoMaterial    *PointsMaterials;
    SoMaterial    *CurvesMaterials;
    SoMaterial    *EditCurvesMaterials;
    SoCoordinate3 *PointsCoordinate;
    SoCoordinate3 *CurvesCoordinate;
    SoCoordinate3 *EditCurvesCoordinate;
    SoLineSet     *CurveSet;
    SoLineSet     *EditCurveSet;
    SoMarkerSet   *PointSet;

    SoText2       * textX;
    SoTranslation * textPos;

    SoGroup       * constrGroup;
};


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProvider2DObject)


ViewProviderSketch::ViewProviderSketch()
  : edit(0),
    Mode(STATUS_NONE)
{
    sPixmap = "Sketcher_NewSketch";
    LineColor.setValue(1,1,1);
    PointColor.setValue(1,1,1);
    PointSize.setValue(4);
}

ViewProviderSketch::~ViewProviderSketch()
{
}

// handler management ***************************************************************
void ViewProviderSketch::activateHandler(DrawSketchHandler *newHandler)
{
    assert(edit);
    assert(edit->sketchHandler == 0);
    edit->sketchHandler = newHandler;
    Mode = STATUS_SKETCH_UseHandler;
    edit->sketchHandler->sketchgui = this;
    edit->sketchHandler->activated(this);
}

/// removes the active handler
void ViewProviderSketch::purgeHandler(void)
{
    assert(edit);
    assert(edit->sketchHandler != 0);
    delete(edit->sketchHandler);
    edit->sketchHandler = 0;
    Mode = STATUS_NONE;
}

// **********************************************************************************

bool ViewProviderSketch::keyPressed(bool pressed, int key)
{
    switch (key)
    {
    case SoKeyboardEvent::ESCAPE:
        {
            // make the handler quit but not the edit mode
            if (edit && edit->sketchHandler) {
                if (!pressed)
                    edit->sketchHandler->quit();
                return true;
            }
            return false;
        }
    }

    return true; // handle all other key events
}

void ViewProviderSketch::getCoordsOnSketchPlane(double &u, double &v,const SbVec3f &point, const SbVec3f &normal)
{
    // Plane form
    Base::Vector3d R0(0,0,0),RN(0,0,1),RX(1,0,0),RY(0,1,0);

    // move to position of Sketch
    Base::Placement Plz = getSketchObject()->Placement.getValue();
    R0 = Plz.getPosition() ;
    Base::Rotation tmp(Plz.getRotation());
    tmp.multVec(RN,RN);
    tmp.multVec(RX,RX);
    tmp.multVec(RY,RY);
    Plz.setRotation(tmp);

    // line
    Base::Vector3d R1(point[0],point[1],point[2]),RA(normal[0],normal[1],normal[2]);
    // intersection point on plane
    Base::Vector3d S = R1 + ((RN * (R0-R1))/(RN*RA))*RA;

    // distance to x Axle of the sketch
    S.TransformToCoordinateSystem(R0,RX,RY);

    u = S.x;
    v = S.y;
}

bool ViewProviderSketch::mouseButtonPressed(int Button, bool pressed, const SbVec3f &point,
                                            const SbVec3f &normal, const SoPickedPoint* pp)
{
    assert(edit);

    double x,y;
    SbVec3f pos = point;
    if (pp) {
        const SoDetail* detail = pp->getDetail();
        if (detail && detail->getTypeId() == SoPointDetail::getClassTypeId()) {
            pos = pp->getPoint();
        }
    }

    getCoordsOnSketchPlane(x,y,pos,normal);

    // Left Mouse button ****************************************************
    if (Button == 1) {
        if (pressed) {
            // Do things depending on the mode of the user interaction
            switch (Mode) {
                case STATUS_NONE:
                    if (edit->PreselectPoint >=0) {
                        //Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
                        Mode = STATUS_SELECT_Point;
                        return true;
                    } else if (edit->PreselectCurve >=0) {
                        //Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
                        Mode = STATUS_SELECT_Edge;
                        return true;
                    } else if (edit->PreselectConstraint >=0) {
                        //Base::Console().Log("start dragging, point:%d\n",this->DragPoint);
                        Mode = STATUS_SELECT_Constraint;
                        return true;

                    } else
                        return false;

                case STATUS_SKETCH_UseHandler:
                    return edit->sketchHandler->pressButton(Base::Vector2D(x,y));
                default:
                    return false;
            }
        }
        else {
            // Do things depending on the mode of the user interaction
            switch (Mode) {
                case STATUS_SELECT_Point:
                    if (pp) {
                        //Base::Console().Log("Select Point:%d\n",this->DragPoint);
                        // Do selection
                        std::stringstream ss;
                        ss << "Vertex" << edit->PreselectPoint;

                        if (Gui::Selection().isSelected(getSketchObject()->getDocument()->getName()
                           ,getSketchObject()->getNameInDocument(),ss.str().c_str()) ) {
                             Gui::Selection().rmvSelection(getSketchObject()->getDocument()->getName()
                                                          ,getSketchObject()->getNameInDocument(), ss.str().c_str());
                        } else {
                            Gui::Selection().addSelection(getSketchObject()->getDocument()->getName()
                                                         ,getSketchObject()->getNameInDocument()
                                                         ,ss.str().c_str()
                                                         ,pp->getPoint()[0]
                                                         ,pp->getPoint()[1]
                                                         ,pp->getPoint()[2]);
                            this->edit->DragPoint = -1;
                            this->edit->DragCurve = -1;
                            this->edit->DragConstraint = -1;
                        }
                    }
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SELECT_Edge:
                    if (pp) {
                        //Base::Console().Log("Select Point:%d\n",this->DragPoint);
                        std::stringstream ss;
                        ss << "Edge" << edit->PreselectCurve;

                        // If edge already selected move from selection
                        if (Gui::Selection().isSelected(getSketchObject()->getDocument()->getName()
                                                       ,getSketchObject()->getNameInDocument(),ss.str().c_str()) ) {
                            Gui::Selection().rmvSelection(getSketchObject()->getDocument()->getName()
                                                         ,getSketchObject()->getNameInDocument(), ss.str().c_str());
                        } else {
                            // Add edge to the selection
                            Gui::Selection().addSelection(getSketchObject()->getDocument()->getName()
                                                         ,getSketchObject()->getNameInDocument()
                                                         ,ss.str().c_str()
                                                         ,pp->getPoint()[0]
                                                         ,pp->getPoint()[1]
                                                         ,pp->getPoint()[2]);
                            this->edit->DragPoint = -1;
                            this->edit->DragCurve = -1;
                            this->edit->DragConstraint = -1;
                        }
                    }
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SELECT_Constraint:
                    if (pp) {

                        std::stringstream ss;
                        ss << "Constraint" << edit->PreselectConstraint;

                        // If the constraint already selected remove
                        if (Gui::Selection().isSelected(getSketchObject()->getDocument()->getName()
                                                       ,getSketchObject()->getNameInDocument(),ss.str().c_str()) ) {
                            Gui::Selection().rmvSelection(getSketchObject()->getDocument()->getName()
                                                         ,getSketchObject()->getNameInDocument(), ss.str().c_str());
                        } else {
                            // Add constraint to current selection
                            Gui::Selection().addSelection(getSketchObject()->getDocument()->getName()
                                                         ,getSketchObject()->getNameInDocument()
                                                         ,ss.str().c_str()
                                                         ,pp->getPoint()[0]
                                                         ,pp->getPoint()[1]
                                                         ,pp->getPoint()[2]);
                            this->edit->DragPoint = -1;
                            this->edit->DragCurve = -1;
                            this->edit->DragConstraint = -1;
                        }
                    }
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SKETCH_DragPoint:
                    if (edit->DragPoint != -1 && pp) {
                        int GeoId;
                        Sketcher::PointPos PosId;
                        getSketchObject()->getGeoVertexIndex(edit->DragPoint, GeoId, PosId);

                        Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.movePoint(%i,%i,App.Vector(%f,%f,0))"
                                               ,getObject()->getNameInDocument()
                                               ,GeoId, PosId, x, y
                                               );
                        edit->PreselectPoint = edit->DragPoint;
                        edit->DragPoint = -1;
                        //updateColor();
                    }
                    resetPositionText();
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SKETCH_DragCurve:
                    if (edit->DragCurve != -1 && pp) {
                        const std::vector<Part::Geometry *> *geomlist;
                        geomlist = &getSketchObject()->Geometry.getValues();
                        Part::Geometry *geo = (*geomlist)[edit->DragCurve];
                        if (geo->getTypeId() == Part::GeomLineSegment::getClassTypeId() ||
                            geo->getTypeId() == Part::GeomArcOfCircle::getClassTypeId() ||
                            geo->getTypeId() == Part::GeomCircle::getClassTypeId()) {
                            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.movePoint(%i,%i,App.Vector(%f,%f,0))"
                                                   ,getObject()->getNameInDocument()
                                                   ,edit->DragCurve, none, x, y
                                                   );
                        }
                        edit->PreselectCurve = edit->DragCurve;
                        edit->DragCurve = -1;
                        //updateColor();
                    }
                    resetPositionText();
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SKETCH_DragConstraint:
                    if (edit->DragConstraint != -1 && pp) {
                        moveConstraint(edit->DragConstraint, Base::Vector2D(x, y));
                        edit->PreselectConstraint = edit->DragConstraint;
                        edit->DragConstraint = -1;
                        //updateColor();
                    }
                    Mode = STATUS_NONE;
                    return true;
                case STATUS_SKETCH_UseHandler:
                    return edit->sketchHandler->releaseButton(Base::Vector2D(x,y));
                case STATUS_NONE:
                default:
                    return false;
            }
        }
    }
    // Right mouse button ****************************************************
    else if (Button == 2) {
        if (pressed) {
            switch (Mode) {
                case STATUS_SKETCH_UseHandler:
                    // make the handler quit
                    edit->sketchHandler->quit();
                    return true;
                case STATUS_NONE:
                    {
                        // A right click shouldn't change the Edit Mode
                        if (edit->PreselectPoint >=0) {
                            return true;
                        } else if (edit->PreselectCurve >=0) {
                            return true;
                        } else if (edit->PreselectConstraint >=0) {
                            return true;
                        } else {
                            //Get Viewer
                            Gui::MDIView* mdi = Gui::Application::Instance->activeDocument()->getActiveView();
                            Gui::View3DInventorViewer * viewer ;
                            viewer = static_cast<Gui::View3DInventor*>(mdi)->getViewer();

                            Gui::MenuItem* geom = new Gui::MenuItem();
                            geom->setCommand("Sketcher geoms");
                            *geom /*<< "Sketcher_CreatePoint"*/
                                << "Sketcher_CreateArc"
                                << "Sketcher_CreateCircle"
                                << "Sketcher_CreateLine"
                                << "Sketcher_CreatePolyline"
                                << "Sketcher_CreateRectangle"
                                /*<< "Sketcher_CreateText"*/
                                /*<< "Sketcher_CreateDraftLine"*/;

                            Gui::Application::Instance->setupContextMenu("View", geom);
                            //Create the Context Menu using the Main View Qt Widget
                            QMenu contextMenu(viewer->getGLWidget());
                            Gui::MenuManager::getInstance()->setupContextMenu(geom, contextMenu);
                            QAction* used = contextMenu.exec(QCursor::pos());

                            return true;
                        }
                    }
                case STATUS_SELECT_Point:
                    break;
                case STATUS_SELECT_Edge:
                    {
                        //Get Viewer
                        Gui::MDIView* mdi = Gui::Application::Instance->activeDocument()->getActiveView();
                        Gui::View3DInventorViewer * viewer ;
                        viewer = static_cast<Gui::View3DInventor*>(mdi)->getViewer();

                        Gui::MenuItem* geom = new Gui::MenuItem();
                        geom->setCommand("Sketcher constraints");
                        *geom << "Sketcher_ConstrainVertical"
                        << "Sketcher_ConstrainHorizontal";

                        // Gets a selection vector
                        std::vector<Gui::SelectionObject> selection = Gui::Selection().getSelectionEx();

                        bool rightClickOnSelectedLine = false;

                        /*
                         * Add Multiple Line Constraints to the menu
                         */
                        // only one sketch with its subelements are allowed to be selected
                        if (selection.size() == 1) {
                            // get the needed lists and objects
                            const std::vector<std::string> &SubNames = selection[0].getSubNames();

                            // Two Objects are selected
                            if (SubNames.size() == 2) {
                                // go through the selected subelements
                                for (std::vector<std::string>::const_iterator it=SubNames.begin();
                                     it!=SubNames.end();++it) {

                                    // If the object selected is of type edge
                                    if (it->size() > 4 && it->substr(0,4) == "Edge") {
                                        // Get the index of the object selected
                                        int index=std::atoi(it->substr(4,4000).c_str());
                                        if (edit->PreselectCurve == index)
                                            rightClickOnSelectedLine = true;
                                    } else {
                                        // The selection is not exclusivly edges
                                        rightClickOnSelectedLine = false;
                                    }
                                } // End of Iteration
                            }
                        }

                        if (rightClickOnSelectedLine) {
                            *geom << "Sketcher_ConstrainParallel"
                                  << "Sketcher_ConstrainPerpendicular";
                        }

                        Gui::Application::Instance->setupContextMenu("View", geom);
                        //Create the Context Menu using the Main View Qt Widget
                        QMenu contextMenu(viewer->getGLWidget());
                        Gui::MenuManager::getInstance()->setupContextMenu(geom, contextMenu);
                        QAction* used = contextMenu.exec(QCursor::pos());

                        return true;
                    }
                case STATUS_SELECT_Constraint:
                case STATUS_SKETCH_DragPoint:
                case STATUS_SKETCH_DragCurve:
                case STATUS_SKETCH_DragConstraint:
                break;
            }
        }
    }

    return false;
}

bool ViewProviderSketch::mouseMove(const SbVec3f &point, const SbVec3f &normal, const SoPickedPoint* pp)
{
    if (!edit)
        return false;
    assert(edit);

    double x,y;
    getCoordsOnSketchPlane(x,y,point,normal);

    int PtIndex,CurvIndex,ConstrIndex;
    bool preselectChanged = detectPreselection(pp,PtIndex,CurvIndex,ConstrIndex);

    switch (Mode) {
        case STATUS_NONE:
            if (preselectChanged)
                updateColor();
            return false;
        case STATUS_SELECT_Point:
            Mode = STATUS_SKETCH_DragPoint;
            edit->DragPoint = edit->PreselectPoint;
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;
            if (edit->DragPoint != -1) {
                int GeoId;
                Sketcher::PointPos PosId;
                getSketchObject()->getGeoVertexIndex(edit->DragPoint, GeoId, PosId);
                edit->ActSketch.initMove(GeoId, PosId);
            }
            return true;
        case STATUS_SELECT_Edge:
            Mode = STATUS_SKETCH_DragCurve;
            edit->DragCurve = edit->PreselectCurve;
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;
            edit->ActSketch.initMove(edit->DragCurve, none);
            return true;
        case STATUS_SELECT_Constraint:
            Mode = STATUS_SKETCH_DragConstraint;
            edit->DragConstraint = edit->PreselectConstraint;
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;
            return true;
        case STATUS_SKETCH_DragPoint:
            if (edit->DragPoint != -1) {
                //Base::Console().Log("Drag Point:%d\n",edit->DragPoint);
                int ret, GeoId;
                Sketcher::PointPos PosId;
                getSketchObject()->getGeoVertexIndex(edit->DragPoint, GeoId, PosId);
                if ((ret=edit->ActSketch.movePoint(GeoId, PosId, Base::Vector3d(x,y,0))) == 0) {
                    setPositionText(Base::Vector2D(x,y));
                    draw(true);
                    signalSolved(0, edit->ActSketch.SolveTime);
                } else {
                    signalSolved(1, edit->ActSketch.SolveTime);
                    //Base::Console().Log("Error solving:%d\n",ret);
                }
            }
            return true;
        case STATUS_SKETCH_DragCurve:
            if (edit->DragCurve != -1) {
                int ret;
                if ((ret=edit->ActSketch.movePoint(edit->DragCurve, none, Base::Vector3d(x,y,0))) == 0) {
                    setPositionText(Base::Vector2D(x,y));
                    draw(true);
                    signalSolved(0, edit->ActSketch.SolveTime);
                } else {
                    signalSolved(1, edit->ActSketch.SolveTime);
                }
            }
            return true;
        case STATUS_SKETCH_DragConstraint:
            if (edit->DragConstraint != -1) {
                moveConstraint(edit->DragConstraint, Base::Vector2D(x,y));
            }
            return true;
        case STATUS_SKETCH_UseHandler:
            if (preselectChanged)
                updateColor();
            edit->sketchHandler->mouseMove(Base::Vector2D(x,y));
            return true;
        default:
            return false;
    }

    return false;
}
void ViewProviderSketch::moveConstraint(int constNum, const Base::Vector2D &Pos)
{
    // are we in edit?
    if (!edit)
        return;

    const std::vector<Sketcher::Constraint*> &ConStr = getSketchObject()->Constraints.getValues();
    Constraint * constr = ConStr[constNum];

    if (constr->Type == Distance) {
        bool tempDraw = true;
        const std::vector<Part::Geometry *> *geomlist;
        std::vector<Part::Geometry *> tempGeo;
        if (tempDraw) {
            tempGeo = edit->ActSketch.getGeometry();
            geomlist = &tempGeo;
        } else {
            geomlist = &getSketchObject()->Geometry.getValues();
        }

        assert(constr->First < int(geomlist->size()));

        // get the geometry

        const Part::Geometry *geo = (*geomlist)[constr->First];
        // Distance can only apply to a GeomLineSegment

        assert(geo->getTypeId()== Part::GeomLineSegment::getClassTypeId());
        const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);

        Base::Vector3d mousePosVec(Pos.fX, Pos.fY, 0);

        // Something wrong with these coordinates
        Base::Vector3d p1 = lineSeg->getStartPoint();
        Base::Vector3d p2 = lineSeg->getEndPoint();

        // calculate the half distance between the start and endpoint
        Base::Vector3d midpos = ((lineSeg->getEndPoint()+lineSeg->getStartPoint())/2);

        // Get a set of vectors perpendicular and tangential to these
        Base::Vector3d dirTmp = (lineSeg->getEndPoint()-lineSeg->getStartPoint()).Normalize();

        // Get Vector between mouse point and mid point
        mousePosVec = mousePosVec - midpos;

        Base::Vector3d temp(0, 0, 0);

        // Get Distance From Sketched Line
        double length = temp.ProjToLine(mousePosVec, dirTmp).Length();

        double crossProduct = ((p2.x - p1.x)*(Pos.fY - p1.y) - (p2.y - p1.y)*(Pos.fX - p1.x));

        // If less than zero, dimension left of Line
        if (crossProduct < 0)
            length *= -1;

        constr->LabelDistance = length;
    }
    draw(true);
}

bool ViewProviderSketch::isConstraintAtPosition(const Base::Vector3d &constrPos, const SoNode * constraint)
{
    assert(edit);
    Gui::MDIView* mdi = Gui::Application::Instance->activeDocument()->getActiveView();
    Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(mdi)->getViewer();

    SoRayPickAction rp(viewer->getViewportRegion());
    rp.setRadius(0.1f);

    rp.setRay(SbVec3f(constrPos.x, constrPos.y,constrPos.z) ,SbVec3f(0, 0, 1) );
    //problem
    rp.apply(edit->constrGroup); // We could narrow it down to just the SoGroup containing the constraints

    // returns a copy of the point
    SoPickedPoint* pp = rp.getPickedPoint();

    if (pp) {
        SoPath * path = pp->getPath();
        int length = path->getLength();
        SoNode * tailFather = path->getNode(length-2);
        SoNode * tailFather2 = path->getNode(length-3);

        // checking if a constraint is the same as the one selected
        if (tailFather2 == constraint || tailFather == constraint) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void ViewProviderSketch::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    // are we in edit?
    if (edit) {
        std::string temp;
        if (msg.Type == Gui::SelectionChanges::ClrSelection) {
            // if something selected in this object?
            if (edit->SelPointSet.size() > 0 || edit->SelCurvSet.size() > 0 || edit->SelConstraintSet.size() > 0) {
                // clear our selection and update the color of the viewed edges and points
                edit->SelPointSet.clear();
                edit->SelCurvSet.clear();
                edit->SelConstraintSet.clear();
                updateColor();
            }
        }
        else if (msg.Type == Gui::SelectionChanges::AddSelection) {
            // is it this object??
            if (strcmp(msg.pDocName,getSketchObject()->getDocument()->getName())==0
                && strcmp(msg.pObjectName,getSketchObject()->getNameInDocument())== 0) {
                    if (msg.pSubName) {
                        std::string shapetype(msg.pSubName);
                        if (shapetype.size() > 4 && shapetype.substr(0,4) == "Edge") {
                            int index=std::atoi(&shapetype[4]);
                            edit->SelCurvSet.insert(index);
                            updateColor();
                        }
                        else if (shapetype.size() > 6 && shapetype.substr(0,6) == "Vertex") {
                            int index=std::atoi(&shapetype[6]);
                            edit->SelPointSet.insert(index);
                            updateColor();
                        }
                        else if (shapetype.size() > 10 && shapetype.substr(0,10) == "Constraint") {
                            int index=std::atoi(&shapetype[10]);
                            edit->SelConstraintSet.insert(index);
                            updateColor();
                        }
                    }
            }
        }
        else if (msg.Type == Gui::SelectionChanges::RmvSelection) {
            // Are there any objects selected
            if (edit->SelPointSet.size() > 0 || edit->SelCurvSet.size() > 0 || edit->SelConstraintSet.size() > 0) {
                // is it this object??
                if (strcmp(msg.pDocName,getSketchObject()->getDocument()->getName())==0
                    && strcmp(msg.pObjectName,getSketchObject()->getNameInDocument())== 0) {
                    if (msg.pSubName) {
                        std::string shapetype(msg.pSubName);
                        if (shapetype.size() > 4 && shapetype.substr(0,4) == "Edge") {
                            int index=std::atoi(&shapetype[4]);
                            edit->SelCurvSet.erase(index);
                            updateColor();
                        }
                        else if (shapetype.size() > 6 && shapetype.substr(0,6) == "Vertex") {
                            int index=std::atoi(&shapetype[6]);
                            edit->SelPointSet.erase(index);
                            updateColor();
                        }
                        else if (shapetype.size() > 10 && shapetype.substr(0,10) == "Constraint") {
                            int index=std::atoi(&shapetype[10]);
                            edit->SelConstraintSet.erase(index);
                            updateColor();
                        }
                    }
                }
            }
        }
        else if (msg.Type == Gui::SelectionChanges::SetSelection) {
            // remove all items
            //selectionView->clear();
            //std::vector<SelectionSingleton::SelObj> objs = Gui::Selection().getSelection(Reason.pDocName);
            //for (std::vector<SelectionSingleton::SelObj>::iterator it = objs.begin(); it != objs.end(); ++it) {
            //    // build name
            //    temp = it->DocName;
            //    temp += ".";
            //    temp += it->FeatName;
            //    if (it->SubName && it->SubName[0] != '\0') {
            //        temp += ".";
            //        temp += it->SubName;
            //    }
            //    new QListWidgetItem(QString::fromAscii(temp.c_str()), selectionView);
            //}
        }

    }
}

bool ViewProviderSketch::detectPreselection(const SoPickedPoint* Point, int &PtIndex, int &CurvIndex, int &ConstrIndex)
{
    assert(edit);

    PtIndex = -1;
    CurvIndex = -1;
    ConstrIndex = -1;

    if (Point) {
        //Base::Console().Log("Point pick\n");
        SoPath * path = Point->getPath();
        SoNode * tail = path->getTail();
        SoNode * tailFather = path->getNode(path->getLength()-2);
        SoNode * tailFather2 = path->getNode(path->getLength()-3);

        // checking for a hit in the points
        if (tail == edit->PointSet) {
            const SoDetail* point_detail = Point->getDetail(edit->PointSet);
            if (point_detail && point_detail->getTypeId() == SoPointDetail::getClassTypeId()) {
                // get the index
                PtIndex = static_cast<const SoPointDetail*>(point_detail)->getCoordinateIndex();
            }
        } else {
            // checking for a hit in the Curves
            if (tail == edit->CurveSet) {
                const SoDetail* curve_detail = Point->getDetail(edit->CurveSet);
                if (curve_detail && curve_detail->getTypeId() == SoLineDetail::getClassTypeId()) {
                    // get the index
                    CurvIndex = static_cast<const SoLineDetail*>(curve_detail)->getLineIndex();
                }
            } else {
                // checking if a constraint is hit
                if (tailFather2 == edit->constrGroup)
                    for (int i=0; i < edit->constrGroup->getNumChildren(); i++)
                        if (edit->constrGroup->getChild(i) == tailFather) {
                            ConstrIndex = i;
                            //Base::Console().Log("Constr %d pick\n",i);
                            break;
                        }
            }
        }

        if (ConstrIndex >= 0) { // if a constraint is hit
            std::stringstream ss;
            ss << "Constraint" << ConstrIndex;
            Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                         ,getSketchObject()->getNameInDocument()
                                         ,ss.str().c_str()
                                         ,Point->getPoint()[0]
                                         ,Point->getPoint()[1]
                                         ,Point->getPoint()[2]);
            if (ConstrIndex != edit->PreselectConstraint) {
                edit->PreselectConstraint = ConstrIndex;
                edit->PreselectPoint = -1;
                edit->PreselectCurve = -1;
                return true;
            } else
                return false;
        } else if (PtIndex >= 0) { // if a point is hit
            std::stringstream ss;
            ss << "Vertex" << PtIndex;
            Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                         ,getSketchObject()->getNameInDocument()
                                         ,ss.str().c_str()
                                         ,Point->getPoint()[0]
                                         ,Point->getPoint()[1]
                                         ,Point->getPoint()[2]);

            if (PtIndex != edit->PreselectPoint) {
                edit->PreselectPoint = PtIndex;
                edit->PreselectCurve = -1;
                edit->PreselectConstraint = -1;
                return true;
            } else
                return false;
        } else if (CurvIndex >= 0) {  // if a curve is hit
            if (CurvIndex != edit->PreselectCurve) {
                edit->PreselectCurve = CurvIndex;
                edit->PreselectPoint = -1;
                edit->PreselectConstraint = -1;
                std::stringstream ss;
                ss << "Edge" << CurvIndex;
                Gui::Selection().setPreselect(getSketchObject()->getDocument()->getName()
                                             ,getSketchObject()->getNameInDocument()
                                             ,ss.str().c_str()
                                             ,Point->getPoint()[0]
                                             ,Point->getPoint()[1]
                                             ,Point->getPoint()[2]);

                return true;
            } else {
                Gui::Selection().setPreselectCoord(Point->getPoint()[0]
                                                  ,Point->getPoint()[1]
                                                  ,Point->getPoint()[2]);
            }
        } else if ((CurvIndex < 0 && PtIndex < 0) && (edit->PreselectCurve >= 0 || edit->PreselectPoint >= 0)) {
            edit->PreselectCurve = -1;
            edit->PreselectPoint = -1;
            edit->PreselectConstraint = -1;
            return true;
        }

    } else if (edit->PreselectCurve >= 0 || edit->PreselectPoint >= 0 || edit->PreselectConstraint >= 0) {
        edit->PreselectCurve = -1;
        edit->PreselectPoint = -1;
        edit->PreselectConstraint = -1;
        return true;
    }

    return false;
}

void ViewProviderSketch::updateColor(void)
{
    assert(edit);
    //Base::Console().Log("Draw preseletion\n");

    int PtNum = edit->PointsMaterials->diffuseColor.getNum();
    SbColor* pcolor = edit->PointsMaterials->diffuseColor.startEditing();
    int CurvNum = edit->CurvesMaterials->diffuseColor.getNum();
    SbColor* color = edit->CurvesMaterials->diffuseColor.startEditing();

    // colors of the point set
    for (int  i=0;i<PtNum;i++) {
        if (edit->SelPointSet.find(i) != edit->SelPointSet.end())
            pcolor[i] = SelectColor;
        else if (edit->PreselectPoint == i)
            pcolor[i] = PreselectColor;
        else
            pcolor[i] = sPointColor;
    }

    // colors or the curves
    for (int  i=0; i < CurvNum; i++) {
        if (edit->SelCurvSet.find(i) != edit->SelCurvSet.end())
            color[i] = SelectColor;
        else if (edit->PreselectCurve == i)
            color[i] = PreselectColor;
        else
            color[i] = sCurveColor;
    }
    // colors of the constraints
    for (int i=0; i < edit->constrGroup->getNumChildren(); i++) {
        SoSeparator *s = dynamic_cast<SoSeparator *>(edit->constrGroup->getChild(i));
        SoMaterial *m = dynamic_cast<SoMaterial *>(s->getChild(0));
        if (edit->SelConstraintSet.find(i) != edit->SelConstraintSet.end())
            m->diffuseColor = SelectColor;
        else if (edit->PreselectConstraint == i)
            m->diffuseColor = PreselectColor;
        else
            m->diffuseColor = sConstraintColor;
    }

    // end editing
    edit->CurvesMaterials->diffuseColor.finishEditing();
    edit->PointsMaterials->diffuseColor.finishEditing();
}

bool ViewProviderSketch::isPointOnSketch(const SoPickedPoint* pp) const
{
    // checks if we picked a point on the sketch or any other nodes like the grid
    SoPath* path = pp->getPath();
    return path->containsNode(edit->EditRoot);
}

bool ViewProviderSketch::doubleClicked(void)
{
    Gui::Application::Instance->activeDocument()->setEdit(this);
    return true;
}

void ViewProviderSketch::draw(bool temp)
{
    assert(edit);

    // Render Geometry ===================================================
    std::vector<Base::Vector3d> Coords;
    std::vector<Base::Vector3d> Points;
    std::vector<unsigned int> Index;
    std::vector<unsigned int> Color;
    std::vector<unsigned int> PtColor;

    const std::vector<Part::Geometry *> *geomlist;
    std::vector<Part::Geometry *> tempGeo;
    if (temp) {
        tempGeo = edit->ActSketch.getGeometry();
        geomlist = &tempGeo;
    }
    else
        geomlist = &getSketchObject()->Geometry.getValues();

    for (std::vector<Part::Geometry *>::const_iterator it = geomlist->begin(); it != geomlist->end(); ++it) {
        if ((*it)->getTypeId() == Part::GeomLineSegment::getClassTypeId()) { // add a line
            const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(*it);
            // create the definition struct for that geom
            Coords.push_back(lineSeg->getStartPoint());
            Coords.push_back(lineSeg->getEndPoint());
            Points.push_back(lineSeg->getStartPoint());
            Points.push_back(lineSeg->getEndPoint());
            Index.push_back(2);
            Color.push_back(0);
            PtColor.push_back(0);
            PtColor.push_back(0);
        }
        else if ((*it)->getTypeId() == Part::GeomCircle::getClassTypeId()) { // add a circle
            const Part::GeomCircle *circle = dynamic_cast<const Part::GeomCircle*>(*it);
            Handle_Geom_Circle curve = Handle_Geom_Circle::DownCast(circle->handle());

            int countSegments = 50;
            Base::Vector3d center = circle->getCenter();
            double segment = (2 * M_PI) / countSegments;
            for (int i=0; i < countSegments; i++) {
                gp_Pnt pnt = curve->Value(i*segment);
                Coords.push_back(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));
            }

            gp_Pnt pnt = curve->Value(0);
            Coords.push_back(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));

            Index.push_back(countSegments+1);
            Color.push_back(0);
            Points.push_back(center);
            PtColor.push_back(0);
        }
        else if ((*it)->getTypeId() == Part::GeomArcOfCircle::getClassTypeId()) { // add an arc
            const Part::GeomArcOfCircle *arc = dynamic_cast<const Part::GeomArcOfCircle*>(*it);
            Handle_Geom_TrimmedCurve curve = Handle_Geom_TrimmedCurve::DownCast(arc->handle());

            double startangle, endangle;
            arc->getRange(startangle, endangle);
            if (startangle > endangle) // if arc is reversed
                std::swap(startangle, endangle);

            double range = endangle-startangle;
            double factor = range / (2 * M_PI);
            int countSegments = int(50.0 * factor);
            double segment = range / countSegments;

            Base::Vector3d center = arc->getCenter();
            Base::Vector3d start  = arc->getStartPoint();
            Base::Vector3d end    = arc->getEndPoint();

            for (int i=0; i < countSegments; i++) {
                gp_Pnt pnt = curve->Value(startangle);
                Coords.push_back(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));
                startangle += segment;
            }

            // end point
            gp_Pnt pnt = curve->Value(endangle);
            Coords.push_back(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));

            Index.push_back(countSegments+1);
            Color.push_back(0);
            Points.push_back(center);
            Points.push_back(start);
            Points.push_back(end);
            PtColor.push_back(0);
            PtColor.push_back(0);
            PtColor.push_back(0);
        }
        else if ((*it)->getTypeId() == Part::GeomBSplineCurve::getClassTypeId()) { // add a circle
            const Part::GeomBSplineCurve *spline = dynamic_cast<const Part::GeomBSplineCurve*>(*it);
            Handle_Geom_BSplineCurve curve = Handle_Geom_BSplineCurve::DownCast(spline->handle());

            double first = curve->FirstParameter();
            double last = curve->LastParameter();
            if (first > last) // if arc is reversed
                std::swap(first, last);

            double range = last-first;
            int countSegments = 50.0;
            double segment = range / countSegments;

            for (int i=0; i < countSegments; i++) {
                gp_Pnt pnt = curve->Value(first);
                Coords.push_back(Base::Vector3d(pnt.X(), pnt.Y(), pnt.Z()));
                first += segment;
            }

            // end point
            gp_Pnt end = curve->Value(last);
            Coords.push_back(Base::Vector3d(end.X(), end.Y(), end.Z()));

            std::vector<Base::Vector3d> poles = spline->getPoles();
            for (std::vector<Base::Vector3d>::iterator it = poles.begin(); it != poles.end(); ++it) {
                Points.push_back(*it);
                PtColor.push_back(0);
            }

            Index.push_back(countSegments+1);
            Color.push_back(0);
        }
        else {
            ;
        }
    }

    edit->CurveSet->numVertices.setNum(Index.size());
    edit->CurvesCoordinate->point.setNum(Coords.size());
    edit->CurvesMaterials->diffuseColor.setNum(Color.size());
    edit->PointsCoordinate->point.setNum(Points.size());
    edit->PointsMaterials->diffuseColor.setNum(PtColor.size());

    SbVec3f* verts = edit->CurvesCoordinate->point.startEditing();
    int32_t* index = edit->CurveSet->numVertices.startEditing();
    SbColor* color = edit->CurvesMaterials->diffuseColor.startEditing();
    SbVec3f* pverts = edit->PointsCoordinate->point.startEditing();
    SbColor* pcolor = edit->PointsMaterials->diffuseColor.startEditing();

    int i=0; // setting up the line set
    for (std::vector<Base::Vector3d>::const_iterator it = Coords.begin(); it != Coords.end(); ++it,i++)
        verts[i].setValue(it->x,it->y,0.1f);

    i=0; // setting up the indexes of the line set
    for (std::vector<unsigned int>::const_iterator it = Index.begin(); it != Index.end(); ++it,i++)
        index[i] = *it;

    i=0; // color of the line set
    for (std::vector<unsigned int>::const_iterator it = Color.begin(); it != Color.end(); ++it,i++)
        color[i] = (*it == 1 ? sCurveConstructionColor : sCurveColor);

    i=0; // setting up the point set
    for (std::vector<Base::Vector3d>::const_iterator it = Points.begin(); it != Points.end(); ++it,i++)
        pverts[i].setValue(it->x,it->y,0.1f);

    i=0; // color of the point set
    for (std::vector<unsigned int>::const_iterator it = PtColor.begin(); it != PtColor.end(); ++it,i++)
        pcolor[i] = (*it == 1 ? sCurveConstructionColor : sPointColor);

    if (edit->PreselectPoint >= 0 && edit->PreselectPoint < int(Points.size()))
        pcolor[edit->PreselectPoint] = PreselectColor;

    edit->CurvesCoordinate->point.finishEditing();
    edit->CurveSet->numVertices.finishEditing();
    edit->CurvesMaterials->diffuseColor.finishEditing();
    edit->PointsCoordinate->point.finishEditing();
    edit->PointsMaterials->diffuseColor.finishEditing();

    // Render Constraints ===================================================
    const std::vector<Sketcher::Constraint*> &ConStr = getSketchObject()->Constraints.getValues();
    // reset point if the constraint type has changed
Restart:
    // check if a new constraint arrived
    if (ConStr.size() != edit->vConstrType.size())
        rebuildConstraintsVisual();
    assert(int(ConStr.size()) == edit->constrGroup->getNumChildren());
    assert(int(edit->vConstrType.size()) == edit->constrGroup->getNumChildren());
    // go through the constraints and update the position
    i = 0;
    for (std::vector<Sketcher::Constraint*>::const_iterator it = ConStr.begin(); it != ConStr.end(); ++it,i++) {
        // check if the type has changed
        if ((*it)->Type != edit->vConstrType[i]) {
            // clearing the type vector will force a rebuild of the visual nodes
            edit->vConstrType.clear();
            goto Restart;
        }
        // root separator for this constraint
        SoSeparator *sep = dynamic_cast<SoSeparator *>(edit->constrGroup->getChild(i));
        const Constraint *Constr = *it;
        // distinquish different constraint types to build up
        switch (Constr->Type) {
            case DistanceX:
            case DistanceY:
                {
                    assert(Constr->First < int(geomlist->size()));

                    Base::Vector3d pnt1(0.,0.,0.), pnt2(0.,0.,0.);
                    if (Constr->First != -1 && Constr->Second !=-1 &&
                        Constr->FirstPos != none && Constr->SecondPos != none) {
                        pnt1 = getSketchObject()->getPoint(Constr->First, Constr->FirstPos);
                        pnt2 = getSketchObject()->getPoint(Constr->Second, Constr->SecondPos);
                    } else if (Constr->First != -1 && Constr->FirstPos != none) {
                        pnt2 = getSketchObject()->getPoint(Constr->First, Constr->FirstPos);
                    } else if (Constr->First != -1) {
                        const Part::Geometry *geo = (*geomlist)[Constr->First];
                        if (geo->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
                            const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
                            pnt1 = lineSeg->getStartPoint();
                            pnt2 = lineSeg->getEndPoint();
                        }
                    }

                    SbVec3f pos;
                    if (Constr->Type == DistanceX) {
                        SbVec3f p1(pnt1.x,(pnt1.y+pnt2.y)/2,0);
                        SbVec3f p2(pnt2.x,(pnt1.y+pnt2.y)/2,0);
                        SbVec3f yvec(0,1,0);
                        if ((pnt1.y+pnt2.y)/2 < 0)
                            yvec *= -1;
                        pos = (p1 + p2)/2 + 10*yvec;
                        // set the line coordinates:
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(0,p1+yvec*8);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(1,p1+yvec*12);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(2,p2);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(3,p2+yvec*12);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(4,p1+yvec*10);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(5,p2+yvec*10);
                    } else if (Constr->Type == DistanceY) {
                        SbVec3f p1((pnt1.x+pnt2.x)/2,pnt1.y,0);
                        SbVec3f p2((pnt1.x+pnt2.x)/2,pnt2.y,0);
                        SbVec3f xvec(1,0,0);
                        if ((pnt1.x+pnt2.x)/2 < 0)
                            xvec *= -1;
                        pos = (p1 + p2)/2 + 10*xvec;
                        // set the line coordinates:
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(0,p1+xvec*8);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(1,p1+xvec*12);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(2,p2);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(3,p2+xvec*12);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(4,p1+xvec*10);
                        dynamic_cast<SoCoordinate3 *>(sep->getChild(1))->point.set1Value(5,p2+xvec*10);
                    }
                    // set position of datum
                    dynamic_cast<SoTranslation *>(sep->getChild(3))->translation = pos;
                    dynamic_cast<SoText2 *>(sep->getChild(4))->string = SbString().sprintf("%.2f",Constr->Value);
                }
                break;
        case Horizontal: // write the new position of the Horizontal constraint Same as vertical position.
        case Vertical: // write the new position of the Vertical constraint
            {
                assert(Constr->First < int(geomlist->size()));
                // get the geometry
                const Part::Geometry *geo = (*geomlist)[Constr->First];
                // Vertical can only be a GeomLineSegment
                assert(geo->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);

                // calculate the half distance between the start and endpoint
                Base::Vector3d midpos = ((lineSeg->getEndPoint()+lineSeg->getStartPoint())/2);

                //Get a set of vectors perpendicular and tangential to these
                Base::Vector3d dir = (lineSeg->getEndPoint()-lineSeg->getStartPoint()).Normalize();
                Base::Vector3d norm(-dir.y,dir.x,0);
                Base::Vector3d constrPos;

                int multiplier = 0;
                do {
                    // Calculate new position of constraint
                    constrPos = midpos + (norm * 5) + (dir * (multiplier * 5));
                    multiplier++; // Increment the multiplier
                }
                while (isConstraintAtPosition(constrPos, edit->constrGroup->getChild(i)));

                dynamic_cast<SoTranslation *>(sep->getChild(1))->translation = SbVec3f(constrPos.x, constrPos.y, 0.0f);
            }
            break;
        case Parallel:
            {
                assert(Constr->First < int(geomlist->size()));
                assert(Constr->Second < int(geomlist->size()));
                // get the geometry
                const Part::Geometry *geo1 = (*geomlist)[Constr->First];
                const Part::Geometry *geo2 = (*geomlist)[Constr->Second];
                // Parallel can only apply to a GeomLineSegment
                assert(geo1->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                assert(geo2->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                const Part::GeomLineSegment *lineSeg1 = dynamic_cast<const Part::GeomLineSegment*>(geo1);
                const Part::GeomLineSegment *lineSeg2 = dynamic_cast<const Part::GeomLineSegment*>(geo2);

                // calculate the half distance between the start and endpoint
                Base::Vector3d midpos1 = ((lineSeg1->getEndPoint()+lineSeg1->getStartPoint())/2);

                //Get a set of vectors perpendicular and tangential to these
                Base::Vector3d dir1 = (lineSeg1->getEndPoint()-lineSeg1->getStartPoint()).Normalize();

                Base::Vector3d norm1(-dir1.y,dir1.x,0);
                Base::Vector3d constrPos1;

                // calculate the half distance between the start and endpoint
                Base::Vector3d midpos2 = ((lineSeg2->getEndPoint()+lineSeg2->getStartPoint())/2);

                //Get a set of vectors perpendicular and tangential to these
                Base::Vector3d dir2 = (lineSeg2->getEndPoint()-lineSeg2->getStartPoint()).Normalize();
                Base::Vector3d norm2(-dir2.y,dir2.x,0);
                Base::Vector3d constrPos2;

                int multiplier = 0;
                do {
                    // Calculate new position of constraint
                    constrPos1 = midpos1 + (norm1 * 5) + (dir1 * (multiplier * 5));
                    multiplier++; // Increment the multiplier
                }
                while (isConstraintAtPosition(constrPos1, edit->constrGroup->getChild(i)));

                multiplier = 0;
                do {
                    // Calculate new position of constraint
                    constrPos2 = midpos2 + (norm2 * 5) + (dir2 * (multiplier * 5));
                    multiplier++; // Increment the multiplier
                }
                while (isConstraintAtPosition(constrPos2, edit->constrGroup->getChild(i)));

                constrPos2 = constrPos2 - constrPos1;
                constrPos2 = constrPos2 - Base::Vector3d(2, -2, 0);
                dynamic_cast<SoText2 *>(sep->getChild(4))->string = SbString().sprintf("%i",i+1);
                dynamic_cast<SoText2 *>(sep->getChild(8))->string = SbString().sprintf("%i",i+1);

                dynamic_cast<SoTranslation *>(sep->getChild(1))->translation =  SbVec3f(constrPos1.x, constrPos1.y, 0.0f);
                dynamic_cast<SoTranslation *>(sep->getChild(3))->translation =  SbVec3f( 2, -2, 0.0f);
                dynamic_cast<SoTranslation *>(sep->getChild(5))->translation =  SbVec3f(constrPos2.x, constrPos2.y, 0.0f);
                dynamic_cast<SoTranslation *>(sep->getChild(7))->translation =  SbVec3f(2, -2, 0.0f);
            }
            break;
        case Distance:
            {
                assert(Constr->First < int(geomlist->size()));
                // get the geometry
                const Part::Geometry *geo = (*geomlist)[Constr->First];
                // Distance can only apply to a GeomLineSegment
                assert(geo->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                const Part::GeomLineSegment *lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo);
                // calculate the half distance between the start and endpoint

                // calculate the half distance between the start and endpoint
                Base::Vector3d midpos = ((lineSeg->getEndPoint()+lineSeg->getStartPoint())/2);

                //Get a set of vectors perpendicular and tangential to these
                Base::Vector3d dirTmp = (lineSeg->getEndPoint()-lineSeg->getStartPoint()).Normalize();
                Base::Vector3d norm(-dirTmp.y,dirTmp.x,0);
                Base::Vector3d posTmp;

                SbVec3f p1(lineSeg->getStartPoint().x,lineSeg->getStartPoint().y,0);
                SbVec3f p2(lineSeg->getEndPoint().x,lineSeg->getEndPoint().y,0);
                SbVec3f dir = p2 - p1;
                SbVec3f pos = p1 + dir/2;
                dir.normalize();
                // rotate direction perpendicular
                SbVec3f normOrig = SbVec3f (-dir[1],dir[0],0);
                // set the line coordinates:

                // Horizontal unit vector
                Base::Vector3d horizontal(1, 0, 0);

                // Get magnitude of angle between horizontal
                double angle = dirTmp.GetAngle(horizontal);

                // Direction of y is negative angle will be negative.
                if (dirTmp.y < 0)
                    angle *= -1;

                SoAsciiText *asciiText = dynamic_cast<SoAsciiText *>(sep->getChild(4));
                asciiText->string = SbString().sprintf("%.2f",Constr->Value);

                // Get Bounding box dimensions for Datum text
                Gui::MDIView* mdi = Gui::Application::Instance->activeDocument()->getActiveView();
                Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(mdi)->getViewer();

                // [FIX ME] Its an attempt to find the height of the text using the bounding box, but is in correct.
                SoGetBoundingBoxAction bbAction(viewer->getViewportRegion());
                bbAction.apply(sep->getChild(4));

                float bx,by,bz;
                bbAction.getBoundingBox().getSize(bx,by,bz);
                SbVec3f textBB(bx,by,bz);
                // bbAction.setCenter(, true)
                // This is the bounding box containing the width and height of text

                SbVec3f textBBCenter = bbAction.getBoundingBox().getCenter();
                double length = (Constr->LabelDistance) ? Constr->LabelDistance : 10;

                posTmp = midpos + norm * (length - (textBBCenter[1] / 2));

                // set position and roation of Datus Text
                SoTransform *transform = dynamic_cast<SoTransform *>(sep->getChild(2));
                transform->translation.setValue(SbVec3f(posTmp.x, posTmp.y, 0));
                transform->rotation.setValue(SbVec3f(0, 0 , 1), angle);

                // Get the datum nodes
                SoSeparator * sepDatum = dynamic_cast<SoSeparator *>(sep->getChild(1));
                SoCoordinate3 *datumCord = dynamic_cast<SoCoordinate3 *>(sepDatum->getChild(0));

                // [Fixme] This should be made neater - compute the vertical datum line length
                float datumVertical = (length < 0) ? length - 2 : length + 2;

                // Calculate coordinates for vertical datum lines
                datumCord->point.set1Value(0,p1);
                datumCord->point.set1Value(1,p1 + normOrig * datumVertical);
                datumCord->point.set1Value(2,p2);
                datumCord->point.set1Value(3,p2 + normOrig * datumVertical);

                // Calculate the coordiates for the horziontal datum lines
                datumCord->point.set1Value(4,p1  + normOrig * (float)length);
                datumCord->point.set1Value(5,pos + normOrig * (float)length - dir * ((textBB[0]) / 4 ) );
                datumCord->point.set1Value(6,pos + normOrig * (float)length + dir * ((textBB[0]) / 4) );
                datumCord->point.set1Value(7,p2  + normOrig * (float)length);

                // Use the coordinates calculated earlier to the lineset
                SoLineSet *datumLineSet = dynamic_cast<SoLineSet *>(sepDatum->getChild(1));
                datumLineSet->numVertices.set1Value(0,2);
                datumLineSet->numVertices.set1Value(1,2);
                datumLineSet->numVertices.set1Value(2,2);
                datumLineSet->numVertices.set1Value(3,2);
            }
            break;
        case Tangent:
            {
                assert(Constr->First < int(geomlist->size()));
                assert(Constr->Second < int(geomlist->size()));
                // get the geometry
                const Part::Geometry *geo1 = (*geomlist)[Constr->First];
                const Part::Geometry *geo2 = (*geomlist)[Constr->Second];
                // Parallel can only apply to a GeomLineSegment

                const Part::GeomLineSegment *lineSeg = 0;
                const Part::GeomCircle *circle1 = 0;
                const Part::GeomCircle *circle2 = 0;
                const Part::GeomArcOfCircle *arcOfCircle1 = 0;
                const Part::GeomArcOfCircle *arcOfCircle2 = 0;

                //[Fix me] There is probably a nicer way of doing this.
                //Assign pointers to the first part of the geometry
                if (geo1->getTypeId() == Part::GeomLineSegment::getClassTypeId()) {
                    lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo1);
                }
                else if (geo1->getTypeId()== Part::GeomArcOfCircle::getClassTypeId()) {
                    arcOfCircle1= dynamic_cast<const Part::GeomArcOfCircle*>(geo1);
                }
                else if (geo1->getTypeId()== Part::GeomCircle::getClassTypeId()) {
                    circle1 = dynamic_cast<const Part::GeomCircle*>(geo1);
                }
                // Assign the second type of geometry
                if (geo2->getTypeId()== Part::GeomLineSegment::getClassTypeId()) {
                    lineSeg = dynamic_cast<const Part::GeomLineSegment*>(geo2);
                }
                else if (geo2->getTypeId()== Part::GeomCircle::getClassTypeId()) {
                    if (circle1) {
                        circle2 = dynamic_cast<const Part::GeomCircle*>(geo2);
                    } else {
                        circle1 = dynamic_cast<const Part::GeomCircle*>(geo2);
                    }
                }
                else if (geo2->getTypeId()== Part::GeomArcOfCircle::getClassTypeId()) {
                    if (arcOfCircle1) {
                        arcOfCircle2 = dynamic_cast<const Part::GeomArcOfCircle*>(geo2);
                    } else {
                        arcOfCircle1 = dynamic_cast<const Part::GeomArcOfCircle*>(geo2);
                    }
                }

                // Select the pairs that we would like to show
                Base::Vector3d pos;
                if (lineSeg && circle1) {
                    Base::Vector3d lineEndToCenter = circle1->getCenter() - lineSeg->getStartPoint();

                    pos = lineSeg->getEndPoint() - lineSeg->getStartPoint();

                    // Get Scalar / Inner product
                    float length = (lineEndToCenter.x * pos.x + lineEndToCenter.y * pos.y) / pos.Length();
                    Base::Vector3d dir = pos;
                    dir.Normalize();
                    Base::Vector3d norm(-dir.y, dir.x, 0);
                    pos = lineSeg->getStartPoint() + dir * length + norm * 5;
                }
                else if (lineSeg && arcOfCircle1) {
                    Base::Vector3d lineEndToCenter = arcOfCircle1->getCenter() - lineSeg->getStartPoint();

                    pos = lineSeg->getEndPoint() - lineSeg->getStartPoint();

                    // Get Scalar / Inner product
                    float length = (lineEndToCenter.x * pos.x + lineEndToCenter.y * pos.y) / pos.Length();
                    Base::Vector3d dir = pos;
                    dir.Normalize();
                    Base::Vector3d norm(-dir.y, dir.x, 0);
                    pos = lineSeg->getStartPoint() + dir * length + norm * 5;
                }
                else {
                    // Other Behaviour hasn't been implemented
                }
                dynamic_cast<SoTranslation *>(sep->getChild(1))->translation =  SbVec3f(pos.x, pos.y, 0.0f);
            }
            break;
        case Angle:
            break;
        case Perpendicular:
            {
                assert(Constr->First < int(geomlist->size()));
                assert(Constr->Second < int(geomlist->size()));
                // get the geometry
                const Part::Geometry *geo1 = (*geomlist)[Constr->First];
                const Part::Geometry *geo2 = (*geomlist)[Constr->Second];
                // Parallel can only apply to a GeomLineSegment
                assert(geo1->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                assert(geo2->getTypeId()== Part::GeomLineSegment::getClassTypeId());
                const Part::GeomLineSegment *lineSeg1 = dynamic_cast<const Part::GeomLineSegment*>(geo1);
                const Part::GeomLineSegment *lineSeg2 = dynamic_cast<const Part::GeomLineSegment*>(geo2);

                // calculate the half distance between the start and endpoint
                Base::Vector3d midpos1 = ((lineSeg1->getEndPoint()+lineSeg1->getStartPoint())/2);

                //Get a set of vectors perpendicular and tangential to these
                Base::Vector3d dir1 = (lineSeg1->getEndPoint()-lineSeg1->getStartPoint()).Normalize();
                Base::Vector3d norm1(-dir1.y,dir1.x,0);
                Base::Vector3d constrPos1;

                // calculate the half distance between the start and endpoint
                Base::Vector3d midpos2 = ((lineSeg2->getEndPoint()+lineSeg2->getStartPoint())/2);

                //Get a set of vectors perpendicular and tangential to these
                Base::Vector3d dir2 = (lineSeg2->getEndPoint()-lineSeg2->getStartPoint()).Normalize();
                Base::Vector3d norm2(-dir2.y,dir2.x,0);
                Base::Vector3d constrPos2;

                int multiplier = 0;
                do {
                    // Calculate new position of constraint
                    constrPos1 = midpos1 + (norm1 * 5) + (dir1 * (multiplier * 5));
                    multiplier++; // Increment the multiplier
                }
                while (isConstraintAtPosition(constrPos1, edit->constrGroup->getChild(i)));

                multiplier = 0;
                do {
                    // Calculate new position of constraint
                    constrPos2 = midpos2 + (norm2 * 5) + (dir2 * (multiplier * 5));
                    multiplier++; // Increment the multiplier
                }
                while (isConstraintAtPosition(constrPos2, edit->constrGroup->getChild(i)));

                constrPos2 = constrPos2 - constrPos1;
                constrPos2 = constrPos2 - Base::Vector3d(2, -2, 0);
                dynamic_cast<SoText2 *>(sep->getChild(4))->string = SbString().sprintf("%i",i+1);
                dynamic_cast<SoText2 *>(sep->getChild(8))->string = SbString().sprintf("%i",i+1);

                dynamic_cast<SoTranslation *>(sep->getChild(1))->translation =  SbVec3f(constrPos1.x, constrPos1.y, 0.0f);
                dynamic_cast<SoTranslation *>(sep->getChild(3))->translation =  SbVec3f( 2, -2, 0.0f);
                dynamic_cast<SoTranslation *>(sep->getChild(5))->translation =  SbVec3f(constrPos2.x, constrPos2.y, 0.0f);
                dynamic_cast<SoTranslation *>(sep->getChild(7))->translation =  SbVec3f(2, -2, 0.0f);
            }
            break;
        case Coincident: // nothing to do for coincident
        case None:
            break;
        }
    }

    //edit->ActSketch.Cons

    // delete the cloned objects
    for (std::vector<Part::Geometry *>::iterator it = tempGeo.begin(); it != tempGeo.end(); ++it)
        if (*it) delete *it;

    Gui::MDIView* mdi = Gui::Application::Instance->activeDocument()->getActiveView();
    if (mdi && mdi->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        static_cast<Gui::View3DInventor*>(mdi)->getViewer()->render();
    }
}

void ViewProviderSketch::rebuildConstraintsVisual(void)
{
    const std::vector<Sketcher::Constraint*> &ConStr = getSketchObject()->Constraints.getValues();
    // clean up
    edit->constrGroup->removeAllChildren();
    edit->vConstrType.clear();

    // Needs putting in better place
    //Constraint Icons Size scaled by (width) in px
    const int constraintImageSize = 16;

    for (std::vector<Sketcher::Constraint*>::const_iterator it = ConStr.begin(); it != ConStr.end(); ++it) {
        // root separator for one constraint
        SoSeparator *sep = new SoSeparator();
        // no caching for fluctuand data structures
        sep->renderCaching = SoSeparator::OFF;
        // every constrained visual node gets its own material for preselection and selection
        SoMaterial *Material = new SoMaterial;
        Material->diffuseColor = sConstraintColor;
        sep->addChild(Material);

        // destiquish different constraint types to build up
        switch ((*it)->Type) {
            case DistanceX:
            case DistanceY:
                {
                    // nodes for the datum lines
                    sep->addChild(new SoCoordinate3);
                    SoLineSet *lineSet = new SoLineSet;
                    lineSet->numVertices.set1Value(0,2);
                    lineSet->numVertices.set1Value(1,2);
                    lineSet->numVertices.set1Value(2,2);
                    sep->addChild(lineSet);

                    // text node for Coordinate Value
                    sep->addChild(new SoTranslation());
                    SoText2 *text = new SoText2();
                    text->justification = SoText2::LEFT;
                    text->string = "";
                    sep->addChild(text);
                    edit->vConstrType.push_back((*it)->Type);
                }
                break;
            case Horizontal: // add a Text node with the "H" for that constraint
                {
                    //Create the Image Nodes
                    SoImage *constraintIcon = new SoImage();
                    QImage image = Gui::BitmapFactory().pixmap("Constraint_Horizontal").toImage();

                    // Scale Image
                    image = image.scaledToWidth(constraintImageSize);
                    SoSFImage icondata = SoSFImage();
                    Gui::BitmapFactory().convert(image, icondata);

                    int nc = 4;
                    SbVec2s iconSize = SbVec2s(image.width(), image.height());

                    constraintIcon->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));

                    constraintIcon->vertAlignment = SoImage::HALF;
                    constraintIcon->horAlignment = SoImage::CENTER;

                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon);

                    // remember the type of this constraint node
                    edit->vConstrType.push_back(Horizontal);
                }
                break;
            case Vertical:
                {
                    //Create the Image Nodes
                    SoImage *constraintIcon = new SoImage();
                    QImage image = Gui::BitmapFactory().pixmap("Constraint_Vertical").toImage();

                    //Scale Image
                    image = image.scaledToWidth(constraintImageSize);
                    SoSFImage icondata = SoSFImage();
                    Gui::BitmapFactory().convert(image, icondata);

                    int nc = 4;
                    SbVec2s iconSize = SbVec2s(image.width(), image.height());

                    constraintIcon->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));

                    //Set Image Alignment to Center
                    constraintIcon->vertAlignment = SoImage::HALF;
                    constraintIcon->horAlignment = SoImage::CENTER;

                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon);

                    // remember the type of this constraint node
                    edit->vConstrType.push_back(Vertical);
                }
                break;
            case Coincident: // no visual for coincident so far
                edit->vConstrType.push_back(Coincident);
                break;
            case Parallel:
                {
                    // Create the Image Nodes
                    SoImage *constraintIcon = new SoImage();
                    SoImage *constraintIcon2 = new SoImage();

                    QImage image = Gui::BitmapFactory().pixmap("Constraint_Parallel").toImage();

                    //Scale Image
                    image = image.scaledToWidth(constraintImageSize);

                    SoSFImage icondata = SoSFImage();
                    Gui::BitmapFactory().convert(image, icondata);

                    int nc = 4;
                    SbVec2s iconSize = SbVec2s(image.width(), image.height());

                    constraintIcon->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));
                    constraintIcon2->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));

                    //Set Image Alignment to center
                    constraintIcon->vertAlignment = SoImage::HALF;
                    constraintIcon->horAlignment = SoImage::CENTER;
                    constraintIcon2->vertAlignment = SoImage::HALF;
                    constraintIcon2->horAlignment = SoImage::CENTER;

                    SoText2 *indexText1 = new SoText2();
                    SoText2 *indexText2 = new SoText2();

                    // Add new nodes to Constraint Seperator
                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon);
                    sep->addChild(new SoTranslation());
                    sep->addChild(indexText1);
                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon2);
                    sep->addChild(new SoTranslation());
                    sep->addChild(indexText2);

                    // remember the type of this constraint node
                    edit->vConstrType.push_back(Parallel);
                }
                break;
            case Tangent:
                {
                    // no visual for tangent so far
                    // Create the Image Nodes
                    SoImage *constraintIcon = new SoImage();

                    QImage image = Gui::BitmapFactory().pixmap("Constraint_Tangent").toImage();

                    //Scale Image
                    image = image.scaledToWidth(constraintImageSize);

                    SoSFImage icondata = SoSFImage();
                    Gui::BitmapFactory().convert(image, icondata);

                    int nc = 4;
                    SbVec2s iconSize = SbVec2s(image.width(), image.height());

                    constraintIcon->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));

                    //Set Image Alignment to center
                    constraintIcon->vertAlignment = SoImage::HALF;
                    constraintIcon->horAlignment = SoImage::CENTER;

                    SoText2 *indexText1 = new SoText2();

                    // Add new nodes to Constraint Seperator
                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon);
                    sep->addChild(new SoTranslation());
                    sep->addChild(indexText1);

                    edit->vConstrType.push_back(Tangent);
                }
                break;
            case Perpendicular:
                {
                    // Create the Image Nodes
                    SoImage *constraintIcon = new SoImage();
                    SoImage *constraintIcon2 = new SoImage();

                    QImage image = Gui::BitmapFactory().pixmap("Constraint_Perpendicular").toImage();

                    //Scale Image
                    image = image.scaledToWidth(constraintImageSize);

                    SoSFImage icondata = SoSFImage();
                    Gui::BitmapFactory().convert(image, icondata);

                    int nc = 4;
                    SbVec2s iconSize = SbVec2s(image.width(), image.height());

                    constraintIcon->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));
                    constraintIcon2->image.setValue(iconSize, 4, icondata.getValue(iconSize, nc));

                    //Set Image Alignment to center
                    constraintIcon->vertAlignment = SoImage::HALF;
                    constraintIcon->horAlignment = SoImage::CENTER;
                    constraintIcon2->vertAlignment = SoImage::HALF;
                    constraintIcon2->horAlignment = SoImage::CENTER;

                    SoText2 *indexText1 = new SoText2();
                    SoText2 *indexText2 = new SoText2();

                    // Add new nodes to Constraint Seperator
                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon);
                    sep->addChild(new SoTranslation());
                    sep->addChild(indexText1);
                    sep->addChild(new SoTranslation());
                    sep->addChild(constraintIcon2);
                    sep->addChild(new SoTranslation());
                    sep->addChild(indexText2);

                    // remember the type of this constraint node
                    edit->vConstrType.push_back(Perpendicular);
                }
                break;
            case Distance:
                {
                    SoSeparator * sepDatum = new SoSeparator();
                    sepDatum->addChild(new SoCoordinate3());
                    SoLineSet *lineSet = new SoLineSet;

                    // text node for Distance Value
                    sepDatum->addChild(lineSet);

                    sep->addChild(sepDatum);

                    // Add the datum text
                    sep->addChild(new SoTransform());

                    // add font for the datum text
                    SoFont *font = new SoFont();
                    font->size = 5;
                    font->name = "Helvetica, Arial, FreeSans:bold";

                    sep->addChild(font);
                    SoAsciiText *text = new SoAsciiText();
                    text->justification =  SoAsciiText::CENTER;
                    //SoText2 *text = new SoText2();
                    //text->justification = SoText2::CENTER;
                    text->string = "";
                    sep->addChild(text);

                    edit->vConstrType.push_back(Distance);
                }
                break;
            default:
                edit->vConstrType.push_back(None);
            }

        edit->constrGroup->addChild(sep);
    }
}

void ViewProviderSketch::drawEdit(const std::vector<Base::Vector2D> &EditCurve)
{
    assert(edit);

    edit->EditCurveSet->numVertices.setNum(1);
    edit->EditCurvesCoordinate->point.setNum(EditCurve.size());
    SbVec3f* verts = edit->EditCurvesCoordinate->point.startEditing();
    int32_t* index = edit->EditCurveSet->numVertices.startEditing();

    int i=0; // setting up the line set
    for (std::vector<Base::Vector2D>::const_iterator it = EditCurve.begin(); it != EditCurve.end(); ++it,i++)
        verts[i].setValue(it->fX,it->fY,0.1f);

    index[0] = EditCurve.size();
    edit->EditCurvesCoordinate->point.finishEditing();
    edit->EditCurveSet->numVertices.finishEditing();
}

void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProvider2DObject::updateData(prop);

    if (edit && (prop == &(getSketchObject()->Geometry) || &(getSketchObject()->Constraints))) {
        edit->ActSketch.setUpSketch(getSketchObject()->Geometry.getValues(),
                                    getSketchObject()->Constraints.getValues());
        if (edit->ActSketch.solve() == 0)
            signalSolved(0,edit->ActSketch.SolveTime);
        else
            signalSolved(1,edit->ActSketch.SolveTime);
        draw(true);
    }
    if (edit && &(getSketchObject()->Constraints)) {
        // send the signal for the TaskDlg.
        signalConstraintsChanged();
    }
}

void ViewProviderSketch::onChanged(const App::Property* prop)
{
    // call father
    PartGui::ViewProvider2DObject::onChanged(prop);
}

void ViewProviderSketch::attach(App::DocumentObject *pcFeat)
{
    ViewProviderPart::attach(pcFeat);
}

void ViewProviderSketch::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act = menu->addAction(QObject::tr("Edit sketch"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
}

bool ViewProviderSketch::setEdit(int ModNum)
{
    // When double-clicking on the item for this sketch the
    // object unsets and sets its edit mode without closing
    // the task panel
    Gui::TaskView::TaskDialog* dlg = Gui::Control().activeDialog();
    TaskDlgEditSketch* sketchDlg = qobject_cast<TaskDlgEditSketch*>(dlg);
    if (sketchDlg && sketchDlg->getSketchView() != this)
        sketchDlg = 0; // another sketch left open its task panel
    if (dlg && !sketchDlg) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("A dialog is already open in the task panel"));
        msgBox.setInformativeText(QObject::tr("Do you want to close this dialog?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
            Gui::Control().closeDialog();
        else
            return false;
    }

    // clear the selction (convenience)
    Gui::Selection().clearSelection();

    // create the container for the addtitional edit data
    assert(!edit);
    edit = new EditData();

    // fill up actual constraints and geometry
    edit->ActSketch.setUpSketch(getSketchObject()->Geometry.getValues(), getSketchObject()->Constraints.getValues());

    createEditInventorNodes();
    this->hide(); // avoid that the wires interfere with the edit lines

    ShowGrid.setValue(true);

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View");
    float transparency;
    // set the highlight color
    unsigned long highlight = (unsigned long)(PreselectColor.getPackedValue());
    highlight = hGrp->GetUnsigned("HighlightColor", highlight);
    PreselectColor.setPackedValue((uint32_t)highlight, transparency);
    // set the selection color
    highlight = (unsigned long)(SelectColor.getPackedValue());
    highlight = hGrp->GetUnsigned("SelectionColor", highlight);
    SelectColor.setPackedValue((uint32_t)highlight, transparency);

    draw();

    // start the edit dialog
    if (sketchDlg)
        Gui::Control().showDialog(sketchDlg);
    else
        Gui::Control().showDialog(new TaskDlgEditSketch(this));

    return true;
}

void ViewProviderSketch::createEditInventorNodes(void)
{
    assert(edit);

    edit->EditRoot = new SoSeparator;
    pcRoot->addChild(edit->EditRoot);
    edit->EditRoot->renderCaching = SoSeparator::OFF ;

    // stuff for the points ++++++++++++++++++++++++++++++++++++++
    edit->PointsMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->PointsMaterials);

    SoMaterialBinding *MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::PER_VERTEX;
    edit->EditRoot->addChild(MtlBind);

    edit->PointsCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->PointsCoordinate);

    SoDrawStyle *DrawStyle = new SoDrawStyle;
    DrawStyle->pointSize = 8;
    edit->EditRoot->addChild(DrawStyle);
    edit->PointSet = new SoMarkerSet;
    edit->PointSet->markerIndex = SoMarkerSet::CIRCLE_FILLED_7_7;
    edit->EditRoot->addChild(edit->PointSet);

    // stuff for the Curves +++++++++++++++++++++++++++++++++++++++
    edit->CurvesMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->CurvesMaterials);

    MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::PER_FACE;
    edit->EditRoot->addChild(MtlBind);

    edit->CurvesCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->CurvesCoordinate);

    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 3;
    edit->EditRoot->addChild(DrawStyle);

    edit->CurveSet = new SoLineSet;

    edit->EditRoot->addChild(edit->CurveSet);

    // stuff for the EditCurves +++++++++++++++++++++++++++++++++++++++
    edit->EditCurvesMaterials = new SoMaterial;
    edit->EditRoot->addChild(edit->EditCurvesMaterials);

    edit->EditCurvesCoordinate = new SoCoordinate3;
    edit->EditRoot->addChild(edit->EditCurvesCoordinate);

    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 3;
    edit->EditRoot->addChild(DrawStyle);

    edit->EditCurveSet = new SoLineSet;
    edit->EditRoot->addChild(edit->EditCurveSet);

    // stuff for the edit coordinates ++++++++++++++++++++++++++++++++++++++
    SoMaterial *EditMaterials = new SoMaterial;
    EditMaterials->diffuseColor = SbColor(0,0,1);
    edit->EditRoot->addChild(EditMaterials);

    SoSeparator *Coordsep = new SoSeparator();
    // no caching for fluctuand data structures
    Coordsep->renderCaching = SoSeparator::OFF;

    SoFont *font = new SoFont();
    font->size = 15.0;
    Coordsep->addChild(font);

    edit->textPos = new SoTranslation();
    Coordsep->addChild(edit->textPos);

    edit->textX = new SoText2();
    edit->textX->justification = SoText2::LEFT;
    edit->textX->string = "";
    Coordsep->addChild(edit->textX);
    edit->EditRoot->addChild(Coordsep);

    // group node for the Constraint visual +++++++++++++++++++++++++++++++++++
    MtlBind = new SoMaterialBinding;
    MtlBind->value = SoMaterialBinding::OVERALL ;
    edit->EditRoot->addChild(MtlBind);

    // add font for the text shown constraints
    font = new SoFont();
    font->size = 15.0;
    edit->EditRoot->addChild(font);

    // use small line with for the Constraints
    DrawStyle = new SoDrawStyle;
    DrawStyle->lineWidth = 1;
    edit->EditRoot->addChild(DrawStyle);

    // add the group where all the constraints has its SoSeparator
    edit->constrGroup = new SoGroup();
    edit->EditRoot->addChild(edit->constrGroup);
}

void ViewProviderSketch::unsetEdit(int ModNum)
{
    ShowGrid.setValue(false);

    edit->EditRoot->removeAllChildren();
    pcRoot->removeChild(edit->EditRoot);

    if (edit->sketchHandler) {
        edit->sketchHandler->unsetCursor();
        purgeHandler();
    }
    delete edit;
    edit = 0;

    this->show();

    // and update the sketch
    getSketchObject()->getDocument()->recompute();

    // clear the selction and set the new/edited sketch(convenience)
    Gui::Selection().clearSelection();
    std::string ObjName = getSketchObject()->getNameInDocument();
    std::string DocName = getSketchObject()->getDocument()->getName();
    Gui::Selection().addSelection(DocName.c_str(),ObjName.c_str());

    // when pressing ESC make sure to close the dialog
    Gui::Control().closeDialog();
}

void ViewProviderSketch::setPositionText(const Base::Vector2D &Pos)
{
    char buf[40];
    sprintf( buf, " (%.1f,%.1f)", Pos.fX,Pos.fY );
    edit->textX->string = buf;
    edit->textPos->translation = SbVec3f(Pos.fX,Pos.fY,0.2f);
}

void ViewProviderSketch::resetPositionText(void)
{
    edit->textX->string = "";
}

int ViewProviderSketch::getPreselectPoint(void)const
{
    if (edit)
        return edit->PreselectPoint;
    return -1;
}
int ViewProviderSketch::getPreselectCurve(void)const
{
    if (edit)
        return edit->PreselectCurve;
    return -1;
}

int ViewProviderSketch::getPreselectConstraint(void)const
{
    if (edit)
        return edit->PreselectConstraint;
    return -1;
}

void ViewProviderSketch::setGridSnap(int Type)
{
    assert(edit);
}

Sketcher::SketchObject* ViewProviderSketch::getSketchObject(void) const
{
    return dynamic_cast<Sketcher::SketchObject*>(pcObject);
}

void ViewProviderSketch::delSelected(void)
{
    if (edit) {
        // We must tmp. block the signaling because otherwise we empty the sets while
        // looping through them which may cause a crash
        this->blockConnection(true);
        std::set<int>::const_reverse_iterator rit;
        for (rit = edit->SelConstraintSet.rbegin(); rit != edit->SelConstraintSet.rend(); rit++) {
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.delConstraint(%i)"
                                   ,getObject()->getNameInDocument(), *rit);
        }
        for (rit = edit->SelCurvSet.rbegin(); rit != edit->SelCurvSet.rend(); rit++) {
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.delGeometry(%i)"
                                   ,getObject()->getNameInDocument(), *rit);
        }
        for (rit = edit->SelPointSet.rbegin(); rit != edit->SelPointSet.rend(); rit++) {
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.delConstraintOnPoint(%i)"
                                   ,getObject()->getNameInDocument(), *rit);
        }

        this->blockConnection(false);
        Gui::Selection().clearSelection();
        edit->PreselectCurve = -1;
        edit->PreselectPoint = -1;
        edit->PreselectConstraint = -1;
        updateColor();
    }
}
