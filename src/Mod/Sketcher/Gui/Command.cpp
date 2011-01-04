/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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
# include <TopoDS_Shape.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <QMessageBox>
#endif

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/DlgEditFileIncludeProptertyExternal.h>
#include <Gui/SelectionFilter.h>

#include <Mod/Sketcher/App/SketchObjectSF.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/Part/App/Part2DObject.h>

#include "ViewProviderSketch.h"

using namespace std;
using namespace SketcherGui;
using namespace Part;

/* Sketch commands =======================================================*/
DEF_STD_CMD_A(CmdSketcherNewSketch);

CmdSketcherNewSketch::CmdSketcherNewSketch()
	:Command("Sketcher_NewSketch")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Create sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}


void CmdSketcherNewSketch::activated(int iMsg)
{
    Gui::SelectionFilter SketchFilter("SELECT Sketcher::SketchObject COUNT 1");
    Gui::SelectionFilter FaceFilter  ("SELECT Part::Feature SUBELEMENT Face COUNT 1");

    if (SketchFilter.match()) {
        Sketcher::SketchObject *Sketch = static_cast<Sketcher::SketchObject*>(SketchFilter.Result[0][0].getObject());
        openCommand("Edit Sketch");
        doCommand(Gui,"Gui.activeDocument().setEdit('%s')",Sketch->getNameInDocument());
    }else if (FaceFilter.match()) {
        // get the selected object
        Part::Feature *part = static_cast<Part::Feature*>(FaceFilter.Result[0][0].getObject());
        Base::Placement ObjectPos = part->Placement.getValue();
        const std::vector<std::string> &sub = FaceFilter.Result[0][0].getSubNames();
        assert (sub.size()==1);
        // get the selected sub shape (a Face)
        const Part::TopoShape &shape = part->Shape.getValue();
        TopoDS_Shape sh = shape.getSubShape(sub[0].c_str());
        TopoDS_Face face = TopoDS::Face(sh);
        assert(!face.IsNull());

        BRepAdaptor_Surface adapt(face);
        if(adapt.GetType() != GeomAbs_Plane){
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No planar support"),
                QObject::tr("You need a planar face as support for a Sketch!"));
            return;
        }
        Base::Placement placement = Part2DObject::positionBySupport(face,ObjectPos);
        double a,b,c;
        placement.getRotation().getYawPitchRoll(a,b,c);
 
        // create Sketch on Face
        std::string FeatName = getUniqueObjectName("Sketch");

        openCommand("Create a Sketch on Face");
        doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObject','%s')",FeatName.c_str());
        doCommand(Gui,"App.activeDocument().%s.Placement = FreeCAD.Placement(FreeCAD.Vector(%f,%f,%f),FreeCAD.Rotation(%f,%f,%f))",FeatName.c_str(),placement.getPosition().x,placement.getPosition().y,placement.getPosition().z,a,b,c);
        //doCommand(Gui,"Gui.activeDocument().activeView().setCamera('%s')",cam.c_str());
        doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());




    }else {
        // do the standard Z+ new Sketch
        const char camstring[] = "#Inventor V2.1 ascii \\n OrthographicCamera { \\n viewportMapping ADJUST_CAMERA \\n position 0 0 87 \\n orientation 0 0 1  0 \\n nearDistance 37 \\n farDistance 137 \\n aspectRatio 1 \\n focalDistance 87 \\n height 119 }";
        std::string FeatName = getUniqueObjectName("Sketch");

        std::string cam(camstring);

        openCommand("Create a new Sketch");
        doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObject','%s')",FeatName.c_str());
        doCommand(Gui,"Gui.activeDocument().activeView().setCamera('%s')",cam.c_str());
        doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
    }
 
}

bool CmdSketcherNewSketch::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}



DEF_STD_CMD_A(CmdSketcherLeaveSketch);

CmdSketcherLeaveSketch::CmdSketcherLeaveSketch()
	:Command("Sketcher_LeaveSketch")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Leave sketch");
    sToolTipText    = QT_TR_NOOP("Close the editing of the sketch");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_LeaveSketch";
}

void CmdSketcherLeaveSketch::activated(int iMsg)
{
    openCommand("Sketch changed");
    doCommand(Gui,"Gui.activeDocument().resetEdit()");
    doCommand(Doc,"App.ActiveDocument.recompute()");
    commitCommand();

}

bool CmdSketcherLeaveSketch::isActive(void)
{
	Gui::Document *doc = getActiveGuiDocument();
	if(doc)
		// checks if a Sketch Viewprovider is in Edit and is in no special mode
		if(doc->getInEdit() && doc->getInEdit()->isDerivedFrom(ViewProviderSketch::getClassTypeId()))
			if(dynamic_cast<SketcherGui::ViewProviderSketch*>(doc->getInEdit())
				->getSketchMode() == ViewProviderSketch::STATUS_NONE)
			return true;
	return false;
}

// Sketchflat integration ++++++++++++++++++++++++++++++++++++++++++++++++

DEF_STD_CMD_A(CmdSketcherNewSketchSF);

CmdSketcherNewSketchSF::CmdSketcherNewSketchSF()
	:Command("Sketcher_NewSketchSF")
{
    sAppModule      = "Sketcher";
    sGroup          = QT_TR_NOOP("Sketcher");
    sMenuText       = QT_TR_NOOP("Sketchflat sketch");
    sToolTipText    = QT_TR_NOOP("Create a new sketchflat sketch by starting externel editor");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "Sketcher_NewSketch";
}

void CmdSketcherNewSketchSF::activated(int iMsg)
{

    std::string FeatName = getUniqueObjectName("Sketch");

    openCommand("Create a new Sketch");
    doCommand(Doc,"App.activeDocument().addObject('Sketcher::SketchObjectSF','%s')",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.SketchFlatFile = App.getResourceDir()+'Mod/Sketcher/Templates/Sketch.skf'",FeatName.c_str());
  
    Sketcher::SketchObjectSF *obj = static_cast<Sketcher::SketchObjectSF *>(getDocument()->getObject( FeatName.c_str() ));

    Gui::Dialog::DlgEditFileIncludePropertyExternal dlg((obj->SketchFlatFile),Gui::getMainWindow());
    dlg.ProcName = QString::fromUtf8((App::Application::Config()["AppHomePath"] + "bin/sketchflat.exe").c_str());
    dlg.Do();

    commitCommand();
    getDocument()->recompute();
}

bool CmdSketcherNewSketchSF::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


void CreateSketcherCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdSketcherNewSketch());
    rcCmdMgr.addCommand(new CmdSketcherLeaveSketch());
    rcCmdMgr.addCommand(new CmdSketcherNewSketchSF());

 }
