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

#include <Mod/Sketcher/App/SketchObjectSF.h>

#include "ViewProviderSketchSF.h"
#include "ViewProviderSketch.h"
#include "DrawSketchHandler.h"

using namespace std;
using namespace SketcherGui;

/* helper functions ======================================================*/

void ActivateHandler(Gui::Document *doc,DrawSketchHandler *handler)
{
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
			    ->activateHandler(handler);
}

bool isCreateGeoActive(Gui::Document *doc)
{
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}

SketcherGui::ViewProviderSketch* getSketchViewprovider(Gui::Document *doc)
{
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketch::getClassTypeId()) )
			return dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit());
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
	enum LineMode{
		STATUS_SEEK_First,      /**< enum value ----. */  
		STATUS_SEEK_Second,     /**< enum value ----. */  
        STATUS_End
	};

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createline),4,4);
    } 

    virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if(Mode==STATUS_SEEK_Second){
            EditCurve[1] = onSketchPos; 
            sketchgui->drawEdit(EditCurve);
        }
    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if(Mode==STATUS_SEEK_First){
            EditCurve[0] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        }else{
            EditCurve[1] = onSketchPos;
            sketchgui->drawEdit(EditCurve);
            Mode = STATUS_End;
        }
        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if(Mode==STATUS_End){
            unsetCursor();
            EditCurve.clear();
            resetPositionText();
            sketchgui->drawEdit(EditCurve);
            Gui::Command::openCommand("add sketch line");
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[0].fX,EditCurve[0].fY,EditCurve[1].fX,EditCurve[1].fY);
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
	:Command("Sketcher_CreateLine")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create line");
    sToolTipText    = QT_TR_NOOP("Create a line in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateLine";
    iAccel          = Qt::Key_L;

}

void CmdSketcherCreateLine::activated(int iMsg)
{
	ActivateHandler(getActiveGuiDocument(),new DrawSketchHandlerLine() );
}

bool CmdSketcherCreateLine::isActive(void)
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
		:Mode(STATUS_SEEK_First),EditCurve(2),firstPoint(-1),previousCurve(-1){}
    virtual ~DrawSketchHandlerLineSet(){}
    /// mode table
	enum LineMode{
		STATUS_SEEK_First,      /**< enum value ----. */  
		STATUS_SEEK_Second,     /**< enum value ----. */  
        STATUS_Do,
        STATUS_Close
	};

    virtual void activated(ViewProviderSketch *sketchgui)
    {
        setCursor(QPixmap(cursor_createlineset),4,4);
    } 

   virtual void mouseMove(Base::Vector2D onSketchPos)
    {
        setPositionText(onSketchPos);
        if(Mode==STATUS_SEEK_Second || Mode==STATUS_Do || Mode==STATUS_Close){
            EditCurve[1] = onSketchPos; 
            sketchgui->drawEdit(EditCurve);
        }
    }

    virtual bool pressButton(Base::Vector2D onSketchPos)
    {
        if(Mode==STATUS_SEEK_First){
			// remember our first point
			firstPoint = getHighestVertexIndex() + 1;
			firstCurve = getHighestCurveIndex() + 1;
            EditCurve[0] = onSketchPos;
            Mode = STATUS_SEEK_Second;
        }else{
            EditCurve[1] = onSketchPos;
            sketchgui->drawEdit(EditCurve);
			applyCursor();
			if (EditCurve[1] == EditCurve[0]){
                // set the old cursor
				unsetCursor();	
                // empty the edit draw
	            EditCurve.clear();
                resetPositionText();
				sketchgui->drawEdit(EditCurve);
				sketchgui->purgeHandler(); // no code after this line, Handler get deleted in ViewProvider
			}if(sketchgui->getPreselectPoint() == firstPoint){
				Mode = STATUS_Close;

			}else
				Mode = STATUS_Do;
        }
        return true;
    }

    virtual bool releaseButton(Base::Vector2D onSketchPos)
    {
        if(Mode==STATUS_Do || Mode==STATUS_Close){
            // open the transaction 
            Gui::Command::openCommand("add sketch line");
            // issue the geometry
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addGeometry(Part.Line(App.Vector(%f,%f,0),App.Vector(%f,%f,0)))",
                      sketchgui->getObject()->getNameInDocument(),
                      EditCurve[0].fX,EditCurve[0].fY,EditCurve[1].fX,EditCurve[1].fY);
            // issue the constraint
            if(previousCurve != -1){
                Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.addConstraint(Sketcher.Constraint('Coincident',%i,2,%i,1)) "
                          ,sketchgui->getObject()->getNameInDocument()
                          ,previousCurve-1,previousCurve
                          );

            }
            if(Mode==STATUS_Do){
                //remember the vertex for the next rounds constraint...
			    previousCurve = getHighestCurveIndex() + 1;
                // setup for the next line segment 
                EditCurve[0] = onSketchPos;
			    Mode = STATUS_SEEK_Second;
			    applyCursor();
            }else{ //Mode==STATUS_Close
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
	:Command("Sketcher_CreatePolyline")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create polyline");
    sToolTipText    = QT_TR_NOOP("Create a polyline in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePolyline";
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


DEF_STD_CMD_A(CmdSketcherCreateArc);

CmdSketcherCreateArc::CmdSketcherCreateArc()
	:Command("Sketcher_CreateArc")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create arc");
    sToolTipText    = QT_TR_NOOP("Create an arc in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateArc";
}


void CmdSketcherCreateArc::activated(int iMsg)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()) )
			dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())->setSketchMode(ViewProviderSketchSF::STATUS_SKETCH_CreateArc);
      
}

bool CmdSketcherCreateArc::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	// checks if a Sketch Viewprovider is in Edit and is in no special mode
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
	//		if(dynamic_cast<SketcherGui::ViewProviderSketchSF*>(doc->getInEdit())
	//			->getSketchMode() == ViewProviderSketchSF::STATUS_NONE)
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateCircle);

CmdSketcherCreateCircle::CmdSketcherCreateCircle()
	:Command("Sketcher_CreateCircle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create circle");
    sToolTipText    = QT_TR_NOOP("Create a circle in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateCircle";
}


void CmdSketcherCreateCircle::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObjectSF\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateCircle::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreatePoint);

CmdSketcherCreatePoint::CmdSketcherCreatePoint()
	:Command("Sketcher_CreatePoint")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create point");
    sToolTipText    = QT_TR_NOOP("Create a point in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreatePoint";
}


void CmdSketcherCreatePoint::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObjectSF\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreatePoint::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
	//		return true;
	return false;
}






DEF_STD_CMD_A(CmdSketcherCreateRectangle);


CmdSketcherCreateRectangle::CmdSketcherCreateRectangle()
	:Command("Sketcher_CreateRectangle")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create rectangle");
    sToolTipText    = QT_TR_NOOP("Create a rectangle in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateRectangle";
}


void CmdSketcherCreateRectangle::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObjectSF\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateRectangle::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateText);

CmdSketcherCreateText::CmdSketcherCreateText()
	:Command("Sketcher_CreateText")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create text");
    sToolTipText    = QT_TR_NOOP("Create text in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_CreateText";
}


void CmdSketcherCreateText::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObjectSF\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateText::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
	//		return true;
	return false;
}


DEF_STD_CMD_A(CmdSketcherCreateDraftLine);

CmdSketcherCreateDraftLine::CmdSketcherCreateDraftLine()
	:Command("Sketcher_CreateDraftLine")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create draft line");
    sToolTipText    = QT_TR_NOOP("Create a draft line in the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_DraftLine";
}


void CmdSketcherCreateDraftLine::activated(int iMsg)
{
    //openCommand("Sketcher Create a new Sketch");
    //doCommand(Doc,"App.activeDocument().addObject(\"Sketcher::SketchObjectSF\",\"Sketch\")");
    //commitCommand();
      
}

bool CmdSketcherCreateDraftLine::isActive(void)
{
	//Gui::Document *doc = getActiveGuiDocument();
	//if(doc)
	//	if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(SketcherGui::ViewProviderSketchSF::getClassTypeId()))
	//		return true;
	return false;
}



void CreateSketcherCommandsCreateGeo(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

	rcCmdMgr.addCommand(new CmdSketcherCreatePoint());
	rcCmdMgr.addCommand(new CmdSketcherCreateArc());
	rcCmdMgr.addCommand(new CmdSketcherCreateCircle());
	rcCmdMgr.addCommand(new CmdSketcherCreateLine());
	rcCmdMgr.addCommand(new CmdSketcherCreatePolyline());
	rcCmdMgr.addCommand(new CmdSketcherCreateRectangle());
	rcCmdMgr.addCommand(new CmdSketcherCreateText());
	rcCmdMgr.addCommand(new CmdSketcherCreateDraftLine());

 }
