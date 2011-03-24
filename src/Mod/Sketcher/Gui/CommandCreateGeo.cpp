/***************************************************************************
 *   Copyright (c) 2010 Jürgen Riegel (juergen.riegel@web.de)              *
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
#endif

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>

#include "ViewProviderSketch.h"
#include "DrawSketchHandler.h"

using namespace std;
using namespace SketcherGui;

/* helper functions ======================================================*/

void ActivateHandler(Gui::Document *doc,DrawSketchHandler *handler)
{
    if (doc) {
        if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom
            (SketcherGui::ViewProviderSketch::getClassTypeId()))
            dynamic_cast<SketcherGui::ViewProviderSketch*>
            (doc->getInEdit())->activateHandler(handler);
    }
}

bool isCreateGeoActive(Gui::Document *doc)
{
    if (doc) {
        // checks if a Sketch Viewprovider is in Edit and is in no special mode
        if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom
            (SketcherGui::ViewProviderSketch::getClassTypeId())) {
            if (dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())->
                getSketchMode() == ViewProviderSketch::STATUS_NONE)
                return true;
        }
    }
    return false;
}

SketcherGui::ViewProviderSketch* getSketchViewprovider(Gui::Document *doc)
{
    if (doc) {
        if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom
            (SketcherGui::ViewProviderSketch::getClassTypeId()) )
            return dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit());
    }
    return 0;
}

/* Sketch commands =======================================================*/

/* XPM */
static const char *cursor_createline[]={
"32 32 2 1",
"# c #646464",
". c None",
"................................",
"................................",
".......#........................",
".......#........................",
".......#........................",
"................................",
".......#........................",
"..###.###.###...................",
".......#..............###.......",
"......................#.#.......",
".......#..............###.......",
".......#.............#..........",
".......#............#...........",
"....................#...........",
"...................#............",
"..................#.............",
"..................#.............",
".................#..............",
"................#...............",
"................#...............",
"...............#................",
"..............#.................",
"..............#.................",
".............#..................",
"..........###...................",
"..........#.#...................",
"..........###...................",
"................................",
"................................",
"................................",
"................................",
"................................"};

class DrawSketchHandlerLine: public DrawSketchHandler
{
public:
    DrawSketchHandlerLine():Mode(STATUS_SEEK_First),EditCurve(2){}
    virtual ~DrawSketchHandlerLine(){}
    /// mode table
    enum LineMode {
        STATUS_SEEK_First,      /**< enum value ----. */  
        STATUS_SEEK_Second,     /**< enum value ----. */  
        STATUS_End
    };

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createline),7,7);
    } 

    virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if (Mode==STATUS_SEEK_Second){
            EditCurve[1] = onSketchPos; 
            sketchgui->drawEdit(EditCurve);
        }
    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_SEEK_First){
            EditCurve[0] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        }
        else {
            EditCurve[1] = onSketchPos;
            sketchgui->drawEdit(EditCurve);
            Mode = STATUS_End;
        }
        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_End){
            unsetCursor();
            resetPositionText();
            Gui::Command::openCommand("Add sketch line");
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[0].fX,EditCurve[0].fY,EditCurve[1].fX,EditCurve[1].fY);
            EditCurve.clear();
            sketchgui->drawEdit(EditCurve);
            sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
        }
        return true;
    }
protected:
    LineMode Mode;
    std::vector<Base::Vector2D> EditCurve;
};



DEF_STD_CMD_A(CmdSketcherCreateLine);

CmdSketcherCreateLine::CmdSketcherCreateLine()
  : Command("Sketcher_CreateLine")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create line");
    sToolTipText    = QT_TR_NOOP("Create a line in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateLine";
    iAccel          = Qt::Key_L;
    eType           = ForEdit;
}

void CmdSketcherCreateLine::activated(int iMsg)
{
    ActivateHandler(getActiveGuiDocument(),new DrawSketchHandlerLine() );
}

bool CmdSketcherCreateLine::isActive(void)
{
    return isCreateGeoActive(getActiveGuiDocument());
}


/* Create Box =======================================================*/

/* XPM */
static const char *cursor_createbox[]={
"32 32 2 1",
"# c #646464",
". c None",
"................................",
"................................",
".......#........................",
".......#........................",
".......#........................",
"................................",
".......#........................",
"..###.###.###...................",
".......#........................",
"................................",
".......#........................",
".......#........................",
".......#........................",
"................................",
"................................",
"..........................###...",
"...........################.#...",
"...........#..............###...",
"...........#...............#....",
"...........#...............#....",
"...........#...............#....",
"...........#...............#....",
"...........#...............#....",
"...........#...............#....",
"..........###..............#....",
"..........#.################....",
"..........###...................",
"................................",
"................................",
"................................",
"................................",
"................................"};

class DrawSketchHandlerBox: public DrawSketchHandler
{
public:
    DrawSketchHandlerBox():Mode(STATUS_SEEK_First),EditCurve(5){}
    virtual ~DrawSketchHandlerBox(){}
    /// mode table
    enum BoxMode {
        STATUS_SEEK_First,      /**< enum value ----. */  
        STATUS_SEEK_Second,     /**< enum value ----. */  
        STATUS_End
    };

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createbox),7,7);
    } 

    virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if(Mode==STATUS_SEEK_Second){
            EditCurve[2] = onSketchPos;
            EditCurve[1] = Base::Vector2D(onSketchPos.fX ,EditCurve[0].fY);
            EditCurve[3] = Base::Vector2D(EditCurve[0].fX,onSketchPos.fY);
            sketchgui->drawEdit(EditCurve);
        }
    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_SEEK_First){
            EditCurve[0] = onSketchPos;
            EditCurve[4] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        }
        else {
            EditCurve[2] = onSketchPos;
            EditCurve[1] = Base::Vector2D(onSketchPos.fX ,EditCurve[0].fY);
            EditCurve[3] = Base::Vector2D(EditCurve[0].fX,onSketchPos.fY);
            sketchgui->drawEdit(EditCurve);
            Mode = STATUS_End;
        }
        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_End){
            unsetCursor();
            resetPositionText();
            Gui::Command::openCommand("Add sketch box");
            int firstCurve = getHighestCurveIndex() + 1;
            // add the four line geos
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[0].fX,EditCurve[0].fY,EditCurve[1].fX,EditCurve[1].fY);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[1].fX,EditCurve[1].fY,EditCurve[2].fX,EditCurve[2].fY);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[2].fX,EditCurve[2].fY,EditCurve[3].fX,EditCurve[3].fY);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[3].fX,EditCurve[3].fY,EditCurve[0].fX,EditCurve[0].fY);
            // add the four coincidents to ty them together
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve,firstCurve+1);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve+1,firstCurve+2);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve+2,firstCurve+3);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve+3,firstCurve);
            // add the horizontal constraints
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Horizontal',%i)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Horizontal',%i)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve+2);
            // add the vertical constraints
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Vertical',%i)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve+1);
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Vertical',%i)) "
                     ,sketchgui->getObject()->getNameInDocument()
                     ,firstCurve+3);

            EditCurve.clear();
            sketchgui->drawEdit(EditCurve);
            sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
        }
        return true;
    }
protected:
    BoxMode Mode;
    std::vector<Base::Vector2D> EditCurve;
};



DEF_STD_CMD_A(CmdSketcherCreateRectangle);

CmdSketcherCreateRectangle::CmdSketcherCreateRectangle()
  : Command("Sketcher_CreateRectangle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create rectangle");
    sToolTipText    = QT_TR_NOOP("Create a rectangle in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateRectangle";
    iAccel          = Qt::Key_R;
    eType           = ForEdit;
}

void CmdSketcherCreateRectangle::activated(int iMsg)
{
    ActivateHandler(getActiveGuiDocument(),new DrawSketchHandlerBox() );
}

bool CmdSketcherCreateRectangle::isActive(void)
{
    return isCreateGeoActive(getActiveGuiDocument());
}


// ======================================================================================

/* XPM */
static const char *cursor_createlineset[]={
"32 32 2 1",
"# c #646464",
". c None",
"................................",
"................................",
".......#........................",
".......#........................",
".......#........................",
"................................",
".......#........................",
"..###.###.###...................",
".......#..............###.......",
"......................#.#.......",
".......#..............###.......",
".......#.............#..#.......",
".......#............#....#......",
"....................#....#......",
"...................#......#.....",
"..................#.......#.....",
"..................#........#....",
".................#.........#....",
"................#..........###..",
"................#..........#.#..",
"......#........#...........###..",
".......#......#.................",
"........#.....#.................",
".........#...#..................",
"..........###...................",
"..........#.#...................",
"..........###...................",
"................................",
"................................",
"................................",
"................................",
"................................"};

class DrawSketchHandlerLineSet: public DrawSketchHandler
{
public:
    DrawSketchHandlerLineSet()
      : Mode(STATUS_SEEK_First),EditCurve(2),firstPoint(-1),previousCurve(-1){}
    virtual ~DrawSketchHandlerLineSet(){}
    /// mode table
    enum LineMode {
        STATUS_SEEK_First,      /**< enum value ----. */  
        STATUS_SEEK_Second,     /**< enum value ----. */  
        STATUS_Do,
        STATUS_Close
    };

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createlineset),7,7);
    } 

    virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if (Mode==STATUS_SEEK_Second || Mode==STATUS_Do || Mode==STATUS_Close){
            EditCurve[1] = onSketchPos; 
            sketchgui->drawEdit(EditCurve);
        }
    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_SEEK_First){
            // remember our first point
            firstPoint = getHighestVertexIndex() + 1;
            firstCurve = getHighestCurveIndex() + 1;
            EditCurve[0] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        }
        else {
            EditCurve[1] = onSketchPos;
            sketchgui->drawEdit(EditCurve);
            applyCursor();
            if (EditCurve[1] == EditCurve[0]) {
                // set the old cursor
                unsetCursor();
                // empty the edit draw
                EditCurve.clear();
                resetPositionText();
                sketchgui->drawEdit(EditCurve);
                sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
            }
            if (sketchgui->getPreselectPoint() == firstPoint)
                Mode = STATUS_Close;
            else
                Mode = STATUS_Do;
        }
        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_Do || Mode==STATUS_Close){
            // open the transaction 
            Gui::Command::openCommand("add sketch wire");
            // issue the geometry
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[0].fX,EditCurve[0].fY,EditCurve[1].fX,EditCurve[1].fY);
            // issue the constraint
            if (previousCurve != -1) {
                Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                          ,sketchgui->getObject()->getNameInDocument()
                          ,previousCurve-1,previousCurve
                          );

            }
            if (Mode==STATUS_Do) {
                //remember the vertex for the next rounds constraint...
                previousCurve = getHighestCurveIndex() + 1;
                // setup for the next line segment 
                EditCurve[0] = onSketchPos;
                Mode = STATUS_SEEK_Second;
                applyCursor();
            }
            else { //Mode==STATUS_Close
                // close the loop by constrain to the first curve point
                Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                          ,sketchgui->getObject()->getNameInDocument()
                          ,previousCurve,firstCurve
                          );
                unsetCursor();
                // empty the edit draw
                EditCurve.clear();
                resetPositionText();
                sketchgui->drawEdit(EditCurve);
                sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
            }
        } 
        return true;
    }
protected:
    LineMode Mode;
    std::vector<Base::Vector2D> EditCurve;
    Base::Vector2D lastPos;
    int firstPoint;
    int firstCurve;
    int previousCurve;
};


DEF_STD_CMD_A(CmdSketcherCreatePolyline);

CmdSketcherCreatePolyline::CmdSketcherCreatePolyline()
  : Command("Sketcher_CreatePolyline")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create polyline");
    sToolTipText    = QT_TR_NOOP("Create a polyline in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePolyline";
    eType           = ForEdit;
}

void CmdSketcherCreatePolyline::activated(int iMsg)
{
    ActivateHandler(getActiveGuiDocument(),new DrawSketchHandlerLineSet() );
}

bool CmdSketcherCreatePolyline::isActive(void)
{
    return isCreateGeoActive(getActiveGuiDocument());
}

// ======================================================================================

/* XPM */
static const char *cursor_createarc[]={
"32 32 2 1",
"# c #646464",
". c None",
"................................",
"................................",
".......#..........###...........",
".......#..........#.#...........",
".......#..........###...........",
".....................##.........",
".......#..............##........",
"..###.###.###..........#........",
".......#................#.......",
"........................##......",
".......#.................#......",
".......#.................#......",
".......#..................#.....",
"..........................#.....",
"..........................#.....",
"..........................#.....",
"..........................#.....",
"..........................#.....",
"..........................#.....",
".........................#......",
".........................#......",
"........................#.......",
"........................#.......",
"...###.................#........",
"...#.#................#.........",
"...###...............#..........",
"......##...........##...........",
".......###.......##.............",
"..........#######...............",
"................................",
"................................",
"................................"};

class DrawSketchHandlerArc : public DrawSketchHandler
{
public:
    DrawSketchHandlerArc()
      : Mode(STATUS_SEEK_First),EditCurve(2){}
    virtual ~DrawSketchHandlerArc(){}
    /// mode table
    enum LineMode {
        STATUS_SEEK_First,      /**< enum value ----. */
        STATUS_SEEK_Second,     /**< enum value ----. */
        STATUS_SEEK_Third,      /**< enum value ----. */
        STATUS_End
    };

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createarc),7,7);
    }

    virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if (Mode==STATUS_SEEK_Second) {
            float dx_ = onSketchPos.fX - EditCurve[0].fX;
            float dy_ = onSketchPos.fY - EditCurve[0].fY;
            for (int i=0; i < 16; i++) {
                float angle = i*M_PI/16.0;
                float dx = dx_ * cos(angle) + dy_ * sin(angle);
                float dy = -dx_ * sin(angle) + dy_ * cos(angle);
                EditCurve[1+i] = Base::Vector2D(EditCurve[0].fX + dx, EditCurve[0].fY + dy);
                EditCurve[17+i] = Base::Vector2D(EditCurve[0].fX - dx, EditCurve[0].fY - dy);
            }
            EditCurve[33] = EditCurve[1];
            sketchgui->drawEdit(EditCurve);
        }
        else if (Mode==STATUS_SEEK_Third) {
            float angle1 = atan2(onSketchPos.fY - CenterPoint.fY,
                                 onSketchPos.fX - CenterPoint.fX) - startAngle;
            float angle2 = angle1 + (angle1 < 0. ? 2 : -2) * M_PI ;
            arcAngle = abs(angle1-arcAngle) < abs(angle2-arcAngle) ? angle1 : angle2;
            for (int i=1; i <= 29; i++) {
                float angle = i*arcAngle/29.0;
                float dx = rx * cos(angle) - ry * sin(angle);
                float dy = rx * sin(angle) + ry * cos(angle);
                EditCurve[i] = Base::Vector2D(CenterPoint.fX + dx, CenterPoint.fY + dy);
            }
            sketchgui->drawEdit(EditCurve);
        }

    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_SEEK_First){
            CenterPoint = onSketchPos;
            EditCurve.resize(34);
            EditCurve[0] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        }
        else if (Mode==STATUS_SEEK_Second){
            EditCurve.resize(31);
            EditCurve[0] = onSketchPos;
            EditCurve[30] = CenterPoint;
            rx = EditCurve[0].fX - CenterPoint.fX;
            ry = EditCurve[0].fY - CenterPoint.fY;
            startAngle = atan2(ry, rx);
            arcAngle = 0.;
            Mode = STATUS_SEEK_Third;
        }
        else {
            EditCurve.resize(30);
            float angle1 = atan2(onSketchPos.fY - CenterPoint.fY,
                                 onSketchPos.fX - CenterPoint.fX) - startAngle;
            float angle2 = angle1 + (angle1 < 0. ? 2 : -2) * M_PI ;
            arcAngle = abs(angle1-arcAngle) < abs(angle2-arcAngle) ? angle1 : angle2;
            if (arcAngle > 0)
                endAngle = startAngle + arcAngle;
            else {
                endAngle = startAngle;
                startAngle += arcAngle;
            }

            sketchgui->drawEdit(EditCurve);
            applyCursor();
            Mode = STATUS_End;
        }

        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_End) {
            unsetCursor();
            resetPositionText();
            Gui::Command::openCommand("Add sketch arc");
            Gui::Command::doCommand(Gui::Command::Doc,
                "App.ActiveDocument.%s.addGeometry(Part.ArcOfCircle"
                "(Part.Circle(App.Vector(%f,%f,0),App.Vector(0,0,1),%f),"
                "%f,%f))",
                      sketchgui->getObject()->getNameInDocument(),
                      CenterPoint.fX, CenterPoint.fY, sqrt(rx*rx + ry*ry),
                      startAngle, endAngle); //arcAngle > 0 ? 0 : 1);
            EditCurve.clear();
            sketchgui->drawEdit(EditCurve);
            sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
        }
        return true;
    }
protected:
    LineMode Mode;
    std::vector<Base::Vector2D> EditCurve;
    Base::Vector2D CenterPoint;
    float rx, ry, startAngle, endAngle, arcAngle;
};

DEF_STD_CMD_A(CmdSketcherCreateArc);

CmdSketcherCreateArc::CmdSketcherCreateArc()
  : Command("Sketcher_CreateArc")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create arc");
    sToolTipText    = QT_TR_NOOP("Create an arc in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateArc";
    eType           = ForEdit;
}

void CmdSketcherCreateArc::activated(int iMsg)
{
    ActivateHandler(getActiveGuiDocument(),new DrawSketchHandlerArc() );
}

bool CmdSketcherCreateArc::isActive(void)
{
    return isCreateGeoActive(getActiveGuiDocument());
}

// ======================================================================================

/* XPM */
static const char *cursor_createcircle[]={
"32 32 2 1",
"# c #646464",
". c None",
"................................",
"................................",
".......#........................",
".......#........................",
".......#........................",
"................................",
".......#........................",
"..###.###.###...................",
".......#.......#######..........",
".............##.......##........",
".......#....#...........#.......",
".......#...#.............#......",
".......#..#...............#.....",
".........#.................#....",
"........#...................#...",
"........#...................#...",
".......#.....................#..",
".......#.....................#..",
".......#.........###.........#..",
".......#.........#.#.........#..",
".......#.........###.........#..",
".......#.....................#..",
".......#.....................#..",
"........#...................#...",
"........#...................#...",
".........#.................#....",
"..........#...............#.....",
"...........#.............#......",
"............#...........#.......",
".............##.......##........",
"...............#######..........",
"................................"};

class DrawSketchHandlerCircle : public DrawSketchHandler
{
public:
    DrawSketchHandlerCircle() : Mode(STATUS_SEEK_First),EditCurve(34){}
    virtual ~DrawSketchHandlerCircle(){}
    /// mode table
    enum LineMode {
        STATUS_SEEK_First,      /**< enum value ----. */
        STATUS_SEEK_Second,     /**< enum value ----. */
        STATUS_Close
    };

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createcircle),7,7);
    } 

    virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if (Mode==STATUS_SEEK_Second) {
            float rx0 = onSketchPos.fX - EditCurve[0].fX;
            float ry0 = onSketchPos.fY - EditCurve[0].fY;
            for (int i=0; i < 16; i++) {
                float angle = i*M_PI/16.0;
                float rx = rx0 * cos(angle) + ry0 * sin(angle);
                float ry = -rx0 * sin(angle) + ry0 * cos(angle);
                EditCurve[1+i] = Base::Vector2D(EditCurve[0].fX + rx, EditCurve[0].fY + ry);
                EditCurve[17+i] = Base::Vector2D(EditCurve[0].fX - rx, EditCurve[0].fY - ry);
            }
            EditCurve[33] = EditCurve[1];
            sketchgui->drawEdit(EditCurve);
        }
    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_SEEK_First){
            EditCurve[0] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        } else {
            EditCurve[1] = onSketchPos;
            Mode = STATUS_Close;
        }
        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if (Mode==STATUS_Close) {
            float rx = EditCurve[1].fX - EditCurve[0].fX;
            float ry = EditCurve[1].fY - EditCurve[0].fY;
            unsetCursor();
            resetPositionText();
            Gui::Command::openCommand("Add sketch circle");
            Gui::Command::doCommand(Gui::Command::Doc,
                "App.ActiveDocument.%s.addGeometry(Part.Circle"
                "(App.Vector(%f,%f,0),App.Vector(0,0,1),%f))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[0].fX, EditCurve[0].fY,
                      sqrt(rx*rx + ry*ry));
            EditCurve.clear();
            sketchgui->drawEdit(EditCurve);
            sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
        }
        return true;
    }
protected:
    LineMode Mode;
    std::vector<Base::Vector2D> EditCurve;
};

DEF_STD_CMD_A(CmdSketcherCreateCircle);

CmdSketcherCreateCircle::CmdSketcherCreateCircle()
  : Command("Sketcher_CreateCircle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create circle");
    sToolTipText    = QT_TR_NOOP("Create a circle in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateCircle";
    eType           = ForEdit;
}

void CmdSketcherCreateCircle::activated(int iMsg)
{
    ActivateHandler(getActiveGuiDocument(),new DrawSketchHandlerCircle() );
}

bool CmdSketcherCreateCircle::isActive(void)
{
    return isCreateGeoActive(getActiveGuiDocument());
}

// ======================================================================================

DEF_STD_CMD_A(CmdSketcherCreatePoint);

CmdSketcherCreatePoint::CmdSketcherCreatePoint()
  : Command("Sketcher_CreatePoint")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create point");
    sToolTipText    = QT_TR_NOOP("Create a point in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePoint";
    eType           = ForEdit;
}

void CmdSketcherCreatePoint::activated(int iMsg)
{
}

bool CmdSketcherCreatePoint::isActive(void)
{
    return false;
}

// ======================================================================================

DEF_STD_CMD_A(CmdSketcherCreateText);

CmdSketcherCreateText::CmdSketcherCreateText()
  : Command("Sketcher_CreateText")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create text");
    sToolTipText    = QT_TR_NOOP("Create text in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateText";
    eType           = ForEdit;
}

void CmdSketcherCreateText::activated(int iMsg)
{
}

bool CmdSketcherCreateText::isActive(void)
{
    return false;
}

// ======================================================================================

DEF_STD_CMD_A(CmdSketcherCreateDraftLine);

CmdSketcherCreateDraftLine::CmdSketcherCreateDraftLine()
  : Command("Sketcher_CreateDraftLine")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create draft line");
    sToolTipText    = QT_TR_NOOP("Create a draft line in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_DraftLine";
    eType           = ForEdit;
}

void CmdSketcherCreateDraftLine::activated(int iMsg)
{
}

bool CmdSketcherCreateDraftLine::isActive(void)
{
    return false;
}


void CreateSketcherCommandsCreateGeo(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    //rcCmdMgr.addCommand(new CmdSketcherCreatePoint());
    rcCmdMgr.addCommand(new CmdSketcherCreateArc());
    rcCmdMgr.addCommand(new CmdSketcherCreateCircle());
    rcCmdMgr.addCommand(new CmdSketcherCreateLine());
    rcCmdMgr.addCommand(new CmdSketcherCreatePolyline());
    rcCmdMgr.addCommand(new CmdSketcherCreateRectangle());
    //rcCmdMgr.addCommand(new CmdSketcherCreateText());
    //rcCmdMgr.addCommand(new CmdSketcherCreateDraftLine());
}
