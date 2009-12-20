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
# include <QMessageBox>
#endif

#include <App/Application.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/Document.h>

#include <Mod/Fem/App/FemMeshObject.h>


using namespace std;

//DEF_STD_CMD_A(CmdFemConstraintAxle);
//
//CmdFemConstraintAxle::CmdFemConstraintAxle()
//	:Command("Fem_Create")
//{
//    sAppModule      = "Fem";
//    sGroup          = QT_TR_NOOP("Fem");
//    sMenuText       = QT_TR_NOOP("Place robot...");
//    sToolTipText    = QT_TR_NOOP("Place a robot (experimental!)");
//    sWhatsThis      = sToolTipText;
//    sStatusTip      = sToolTipText;
//    sPixmap         = "Fem_CreateFem";
//}
//
//
//void CmdFemConstraintAxle::activated(int iMsg)
//{
//    std::string FeatName = getUniqueObjectName("Fem");
//    std::string FemPath = "Mod/Fem/Lib/Kuka/kr500_1.wrl";
//    std::string KinematicPath = "Mod/Fem/Lib/Kuka/kr500_1.csv";
//
//    openCommand("Place robot");
//    doCommand(Doc,"App.activeDocument().addObject(\"Fem::FemObject\",\"%s\")",FeatName.c_str());
//    doCommand(Doc,"App.activeDocument().%s.FemVrmlFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),FemPath.c_str());
//    doCommand(Doc,"App.activeDocument().%s.FemKinematicFile = App.getResourceDir()+\"%s\"",FeatName.c_str(),KinematicPath.c_str());
//    doCommand(Doc,"App.activeDocument().%s.Axis2 = -90",FeatName.c_str());
//    doCommand(Doc,"App.activeDocument().%s.Axis3 = 90",FeatName.c_str());
//    updateActive();
//    commitCommand();
//      
//}
//
//bool CmdFemConstraintAxle::isActive(void)
//{
//    return hasActiveDocument();
//}
//
//
//// #####################################################################################################
//
//DEF_STD_CMD_A(CmdFemSimulate);
//
//CmdFemSimulate::CmdFemSimulate()
//	:Command("Fem_Simulate")
//{
//    sAppModule      = "Fem";
//    sGroup          = QT_TR_NOOP("Fem");
//    sMenuText       = QT_TR_NOOP("Simulate a trajectory");
//    sToolTipText    = QT_TR_NOOP("Run a simulation on a trajectory");
//    sWhatsThis      = sToolTipText;
//    sStatusTip      = sToolTipText;
//    sPixmap         = "Fem_Simulate";
//}
//
//
//void CmdFemSimulate::activated(int iMsg)
//{
// 
//    unsigned int n1 = getSelection().countObjectsOfType(Fem::FemObject::getClassTypeId());
//    unsigned int n2 = getSelection().countObjectsOfType(Fem::TrajectoryObject::getClassTypeId());
// 
//    if (n1 != 1 || n2 != 1) {
//        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
//            QObject::tr("Select one Fem and one Trajectory object."));
//        return;
//    }
//
//    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();
//
//    Fem::FemObject *pcFemObject;
//    if(Sel[0].pObject->getTypeId() == Fem::FemObject::getClassTypeId())
//        pcFemObject = dynamic_cast<Fem::FemObject*>(Sel[0].pObject);
//    else if(Sel[1].pObject->getTypeId() == Fem::FemObject::getClassTypeId())
//        pcFemObject = dynamic_cast<Fem::FemObject*>(Sel[1].pObject);
//    std::string RoboName = pcFemObject->getNameInDocument();
//
//    Fem::TrajectoryObject *pcTrajectoryObject;
//    if(Sel[0].pObject->getTypeId() == Fem::TrajectoryObject::getClassTypeId())
//        pcTrajectoryObject = dynamic_cast<Fem::TrajectoryObject*>(Sel[0].pObject);
//    else if(Sel[1].pObject->getTypeId() == Fem::TrajectoryObject::getClassTypeId())
//        pcTrajectoryObject = dynamic_cast<Fem::TrajectoryObject*>(Sel[1].pObject);
//    std::string TrakName = pcTrajectoryObject->getNameInDocument();
//
//    FemGui::TrajectorySimulate dlg(pcFemObject,pcTrajectoryObject,Gui::getMainWindow());
//    dlg.exec();
//      
//}
//
//bool CmdFemSimulate::isActive(void)
//{
//    return hasActiveDocument();
//}



// #####################################################################################################



void CreateFemCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    //rcCmdMgr.addCommand(new CmdFemConstraintAxle());
    //rcCmdMgr.addCommand(new CmdFemSimulate());
 }
