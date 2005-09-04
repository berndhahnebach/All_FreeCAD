/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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


#include "Command.h"
#include "Application.h"
#include "View.h"
#include "Document.h"
#include "Macro.h"
#include "DlgDisplayPropertiesImp.h"
#include "Selection.h"

#include <Base/Exception.h>
#include <App/Document.h>

using namespace Gui;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//===========================================================================
// Std_OrthographicCamera
//===========================================================================
DEF_STD_CMD_A(StdCmdOrthographicCamera);

StdCmdOrthographicCamera::StdCmdOrthographicCamera()
  :CppCommand("Std_OrthographicCamera")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Orthographic view");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdOrthographicCamera::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"OrthographicCamera\")");
}

bool StdCmdOrthographicCamera::isActive(void)
{
  return true;
}
//===========================================================================
// Std_PerspectiveCamera
//===========================================================================
DEF_STD_CMD_A(StdCmdPerspectiveCamera);

StdCmdPerspectiveCamera::StdCmdPerspectiveCamera()
  :CppCommand("Std_PerspectiveCamera")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Perspective view");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdPerspectiveCamera::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"PerspectiveCamera\")");
}

bool StdCmdPerspectiveCamera::isActive(void)
{
  return true;
}
//===========================================================================
// Std_ToggleVisibility
//===========================================================================
DEF_STD_CMD_A(StdCmdToggleVisibility);

StdCmdToggleVisibility::StdCmdToggleVisibility()
  :CppCommand("Std_ToggleVisibility")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Toggle Visibility");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = Qt::Key_Space;
}

void StdCmdToggleVisibility::activated(int iMsg)
{
  if( getActiveDocument() )
  {
    for(std::vector<App::Feature*>::const_iterator It=Gui::Selection().getSelectedFeatures().begin();It!=Gui::Selection().getSelectedFeatures().end();It++)
    {
      if(getActiveDocument()->isShow(*It))
        getActiveDocument()->setHide(*It);
      else
        getActiveDocument()->setShow(*It);

    }
    Gui::Selection().clearSelection();
  }
}

bool StdCmdToggleVisibility::isActive(void)
{
  return ( Gui::Selection().size() != 0 );
}

//===========================================================================
// Std_SetMaterial
//===========================================================================
DEF_STD_CMD_A(StdCmdSetMaterial);

StdCmdSetMaterial::StdCmdSetMaterial()
  :CppCommand("Std_SetMaterial")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Material");
  sToolTipText  = QT_TR_NOOP("Set the material properties of the selcted feature");
  sWhatsThis    = QT_TR_NOOP("Set the material properties of the selcted feature");
  sStatusTip    = QT_TR_NOOP("Set the material properties of the selcted feature");
  sPixmap       = "Std_Tool1";
  iAccel        = Qt::CTRL+Qt::Key_D;
}

void StdCmdSetMaterial::activated(int iMsg)
{
  Gui::Dialog::DlgDisplayPropertiesImp dlg(this, getAppWnd()->activeView(), "Display", true );
  dlg.exec();
}

bool StdCmdSetMaterial::isActive(void)
{
  return Gui::Selection().size() != 0;
}

//===========================================================================
// Std_ViewBottom
//===========================================================================
DEF_3DV_CMD(StdCmdViewBottom)

StdCmdViewBottom::StdCmdViewBottom()
  :CppCommand("Std_ViewBottom")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Bottom");
  sToolTipText  = QT_TR_NOOP("Set to bottom view");
  sWhatsThis    = QT_TR_NOOP("Set to bottom view");
  sStatusTip    = QT_TR_NOOP("Set to bottom view");
  sPixmap       = "view_bottom";
  iAccel        = Qt::Key_5;
}

void StdCmdViewBottom::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewBottom\")");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_3DV_CMD(StdCmdViewFront);

StdCmdViewFront::StdCmdViewFront()
  :CppCommand("Std_ViewFront")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Front");
  sToolTipText  = QT_TR_NOOP("Set to front view");
  sWhatsThis    = QT_TR_NOOP("Set to front view");
  sStatusTip    = QT_TR_NOOP("Set to front view");
  sPixmap       = "view_front";
  iAccel        = Qt::Key_1;
}

void StdCmdViewFront::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFront\")");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_3DV_CMD(StdCmdViewLeft);

StdCmdViewLeft::StdCmdViewLeft()
  :CppCommand("Std_ViewLeft")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Left");
  sToolTipText  = QT_TR_NOOP("Set to left view");
  sWhatsThis    = QT_TR_NOOP("Set to left view");
  sStatusTip    = QT_TR_NOOP("Set to left view");
  sPixmap       = "view_left";
  iAccel        = Qt::Key_6;
}

void StdCmdViewLeft::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewLeft\")");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_3DV_CMD(StdCmdViewRear);

StdCmdViewRear::StdCmdViewRear()
  :CppCommand("Std_ViewRear")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Rear");
  sToolTipText  = QT_TR_NOOP("Set to rear view");
  sWhatsThis    = QT_TR_NOOP("Set to rear view");
  sStatusTip    = QT_TR_NOOP("Set to rear view");
  sPixmap       = "view_back";
  iAccel        = Qt::Key_4;
}

void StdCmdViewRear::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRear\")");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_3DV_CMD(StdCmdViewRight);

StdCmdViewRight::StdCmdViewRight()
  :CppCommand("Std_ViewRight")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Right");
  sToolTipText  = QT_TR_NOOP("Set to right view");
  sWhatsThis    = QT_TR_NOOP("Set to right view");
  sStatusTip    = QT_TR_NOOP("Set to right view");
  sPixmap       = "view_right";
  iAccel        = Qt::Key_3;
}

void StdCmdViewRight::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRight\")");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_3DV_CMD(StdCmdViewTop);

StdCmdViewTop::StdCmdViewTop()
  :CppCommand("Std_ViewTop")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Top");
  sToolTipText  = QT_TR_NOOP("Set to top view");
  sWhatsThis    = QT_TR_NOOP("Set to top view");
  sStatusTip    = QT_TR_NOOP("Set to top view");
  sPixmap       = "view_top";
  iAccel        = Qt::Key_2;
}

void StdCmdViewTop::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewTop\")");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_3DV_CMD(StdCmdViewAxo);

StdCmdViewAxo::StdCmdViewAxo()
  :CppCommand("Std_ViewAxo")
{
  sAppModule  = "";
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Axometric");
  sToolTipText= QT_TR_NOOP("Set to axometric view");
  sWhatsThis  = QT_TR_NOOP("Set to axometric view");
  sStatusTip  = QT_TR_NOOP("Set to axometric view");
  sPixmap     = "view_axo";
  iAccel      = Qt::Key_0;
}

void StdCmdViewAxo::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewAxo\")");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_STD_CMD_A(StdCmdViewFitAll);

StdCmdViewFitAll::StdCmdViewFitAll()
  :CppCommand("Std_ViewFitAll")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Fit all");
  sToolTipText  = QT_TR_NOOP("Fits the whole content on the screen");
  sWhatsThis    = QT_TR_NOOP("Fits the whole content on the screen");
  sStatusTip    = QT_TR_NOOP("Fits the whole content on the screen");
  sPixmap       = "view_fitall";
  iAccel        = 0;
}

void StdCmdViewFitAll::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFit\")");
}

bool StdCmdViewFitAll::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("ViewFit");
}


//===========================================================================
// Std_ViewFullScreen
//===========================================================================
DEF_STD_CMD_A(StdViewFullScreen);

StdViewFullScreen::StdViewFullScreen()
  :CppCommand("Std_ViewFullScreen", Cmd_Toggle)
{
  sAppModule  = "";
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Fullscreen");
  sToolTipText= QT_TR_NOOP("Display the active view in fullscreen");
  sWhatsThis  = QT_TR_NOOP("Display the active view in fullscreen");
  sStatusTip  = QT_TR_NOOP("Display the active view in fullscreen");
//  sPixmap     = "view_fitall";
  iAccel      = Qt::Key_F;
}

void StdViewFullScreen::activated(int iMsg)
{
  MDIView* view = getActiveDocument()->getActiveView();
  if ( view && view->isFullScreen() )
    view->setFullScreenMode( false );
  else
    view->setFullScreenMode( true );
}

bool StdViewFullScreen::isActive(void)
{
  return (getActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewCreateOCC
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreateOCC);

StdCmdViewCreateOCC::StdCmdViewCreateOCC()
  :CppCommand("Std_ViewCreateOCC")
{
  sAppModule  = "";
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Create new OCC View");
  sToolTipText= QT_TR_NOOP("Creates a new OpenCasCade view window for the active document");
  sWhatsThis  = QT_TR_NOOP("Creates a new OpenCasCade view window for the active document");
  sStatusTip  = QT_TR_NOOP("Creates a new OpenCasCade view window for the active document");
  sPixmap     = "view_fitall";
  iAccel      = 0;
}

void StdCmdViewCreateOCC::activated(int iMsg)
{
  getActiveDocument()->createView("View3DOCC");
}

bool StdCmdViewCreateOCC::isActive(void)
{
  return (getActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewCreateInventor
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreateInventor);

StdCmdViewCreateInventor::StdCmdViewCreateInventor()
  :CppCommand("Std_ViewCreateInventor")
{
  sAppModule  = "";
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Create new Inventor View");
  sToolTipText= QT_TR_NOOP("Creates a new Inventor view window for the active document");
  sWhatsThis  = QT_TR_NOOP("Creates a new Inventor view window for the active document");
  sStatusTip  = QT_TR_NOOP("Creates a new Inventor view window for the active document");
  sPixmap     = "view_fitall";
  iAccel      = 0;
}

void StdCmdViewCreateInventor::activated(int iMsg)
{
  getActiveDocument()->createView("View3DIv");
}

bool StdCmdViewCreateInventor::isActive(void)
{
  return (getActiveDocument()!=NULL);
}

//===========================================================================
// Std_ViewExample1
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample1);

StdCmdViewExample1::StdCmdViewExample1()
  :CppCommand("Std_ViewExample1")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #1");
  sToolTipText  = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sWhatsThis    = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sStatusTip    = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdViewExample1::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example1\")");
}

bool StdCmdViewExample1::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Example1");
}
//===========================================================================
// Std_ViewExample2
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample2);

StdCmdViewExample2::StdCmdViewExample2()
  :CppCommand("Std_ViewExample2")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #2");
  sToolTipText  = QT_TR_NOOP("Shows spheres an dragglights");
  sWhatsThis    = QT_TR_NOOP("Shows spheres an dragglights");
  sStatusTip    = QT_TR_NOOP("Shows spheres an dragglights");
  sPixmap       = "Std_Tool2";
  iAccel        = 0;
}

void StdCmdViewExample2::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example2\")");
}

bool StdCmdViewExample2::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Example2");
}

//===========================================================================
// Std_ViewExample3
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample3);

StdCmdViewExample3::StdCmdViewExample3()
  :CppCommand("Std_ViewExample3")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #3");
  sToolTipText  = QT_TR_NOOP("Shows a animated texture");
  sWhatsThis    = QT_TR_NOOP("Shows a animated texture");
  sStatusTip    = QT_TR_NOOP("Shows a animated texture");
  sPixmap       = "Std_Tool3";
  iAccel        = 0;
}

void StdCmdViewExample3::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example3\")");
}

bool StdCmdViewExample3::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("Example3");
}

//===========================================================================
// Std_ViewIvDecorationOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvDecorationOn);

StdCmdViewIvDecorationOn::StdCmdViewIvDecorationOn()
  :CppCommand("Std_ViewIvDecorationOn")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Decoration on");
  sToolTipText  = QT_TR_NOOP("Examiner viewer decoration on");
  sWhatsThis    = QT_TR_NOOP("Examiner viewer decoration on");
  sStatusTip    = QT_TR_NOOP("Examiner viewer decoration on");
  sPixmap       = "Std_Tool4";
  iAccel        = 0;
}

void StdCmdViewIvDecorationOn::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetDecorationOn\")");
}

bool StdCmdViewIvDecorationOn::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetDecorationOn");
}

//===========================================================================
// Std_ViewIvDecorationOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvDecorationOff);

StdCmdViewIvDecorationOff::StdCmdViewIvDecorationOff()
  :CppCommand("Std_ViewIvDecorationOff")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Decoration Off");
  sToolTipText  = QT_TR_NOOP("Examiner viewer decoration Off");
  sWhatsThis    = QT_TR_NOOP("Examiner viewer decoration Off");
  sStatusTip    = QT_TR_NOOP("Examiner viewer decoration Off");
  sPixmap       = "Std_Tool5";
  iAccel        = 0;
}

void StdCmdViewIvDecorationOff::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetDecorationOff\")");
}

bool StdCmdViewIvDecorationOff::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetDecorationOff");
}


//===========================================================================
// Std_ViewIvStereoOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOff);

StdCmdViewIvStereoOff::StdCmdViewIvStereoOff()
  :CppCommand("Std_ViewIvStereoOff")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Off");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing off");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing off");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing off");
  sPixmap       = "Std_Tool6";
  iAccel        = 0;
}

void StdCmdViewIvStereoOff::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOff\")");
}

bool StdCmdViewIvStereoOff::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetStereoOff");
}


//===========================================================================
// Std_ViewIvStereoOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOn);

StdCmdViewIvStereoOn::StdCmdViewIvStereoOn()
  :CppCommand("Std_ViewIvStereoOn")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo On");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing on");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing on");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing on");
  sPixmap       = "Std_Tool7";
  iAccel        = 0;
}

void StdCmdViewIvStereoOn::activated(int iMsg)
{
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOn\")");
}

bool StdCmdViewIvStereoOn::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetStereoOn");
}


//===========================================================================
// Std_ViewIvStereoOn
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvIssueCamPos);

StdCmdViewIvIssueCamPos::StdCmdViewIvIssueCamPos()
  :CppCommand("Std_ViewIvIssueCamPos")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Issue camera position");
  sToolTipText  = QT_TR_NOOP("Issue the camera posiotion to the console and to a macro, to easily recall this position");
  sWhatsThis    = QT_TR_NOOP("Issue the camera posiotion to the console and to a macro, to easily recall this position");
  sStatusTip    = QT_TR_NOOP("Issue the camera posiotion to the console and to a macro, to easily recall this position");
  sPixmap       = "Std_Tool8";
  iAccel        = 0;
}

void StdCmdViewIvIssueCamPos::activated(int iMsg)
{
  std::string Temp,Temp2;
  unsigned int pos;

  const char* ppReturn=0;
  getAppWnd()->sendMsgToActiveView("GetCamera",&ppReturn);

  // remove the #inventor line...
  Temp2 = ppReturn;
  pos = Temp2.find_first_of("\n");
  Temp2.erase(0,pos);

  // remove all returns
  while((pos=Temp2.find('\n')) != std::string::npos)
    Temp2.replace(pos,1," ");

  // build up the command string
  Temp += "FreeCADGui.SendMsgToActiveView(\"SetCamera ";
  Temp += Temp2;
  Temp += "\")";

  Base::Console().Message("%s",Temp.c_str());

  getAppWnd()->macroManager()->addLine(MacroManager::Gui,"FreeCAD.DocGet().Update()");
  getAppWnd()->macroManager()->addLine(MacroManager::Gui,Temp.c_str());
  //doCommand(Command::Gui,Temp.c_str());
}

bool StdCmdViewIvIssueCamPos::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("SetStereoOn");
}




//===========================================================================
// Instanciation
//===========================================================================


namespace Gui {

void CreateViewStdCommands(void)
{
  CommandManager &rcCmdMgr = ApplicationWindow::Instance->commandManager();

  // views
  rcCmdMgr.addCommand(new StdCmdViewBottom());
  rcCmdMgr.addCommand(new StdCmdViewFront());
  rcCmdMgr.addCommand(new StdCmdViewLeft());
  rcCmdMgr.addCommand(new StdCmdViewRear());
  rcCmdMgr.addCommand(new StdCmdViewRight());
  rcCmdMgr.addCommand(new StdCmdViewTop());
  rcCmdMgr.addCommand(new StdCmdViewAxo());
  rcCmdMgr.addCommand(new StdCmdViewFitAll());

  rcCmdMgr.addCommand(new StdCmdViewExample1());
  rcCmdMgr.addCommand(new StdCmdViewExample2());
  rcCmdMgr.addCommand(new StdCmdViewExample3());

  rcCmdMgr.addCommand(new StdCmdViewIvDecorationOff());
  rcCmdMgr.addCommand(new StdCmdViewIvDecorationOn());

  rcCmdMgr.addCommand(new StdCmdViewIvStereoOn());
  rcCmdMgr.addCommand(new StdCmdViewIvStereoOff());

  rcCmdMgr.addCommand(new StdCmdViewIvIssueCamPos());

  rcCmdMgr.addCommand(new StdCmdViewCreateOCC());
  rcCmdMgr.addCommand(new StdCmdViewCreateInventor());

  rcCmdMgr.addCommand(new StdViewFullScreen());
  rcCmdMgr.addCommand(new StdCmdSetMaterial());
  rcCmdMgr.addCommand(new StdCmdToggleVisibility());
  rcCmdMgr.addCommand(new StdCmdPerspectiveCamera());
  rcCmdMgr.addCommand(new StdCmdOrthographicCamera());
}

} // namespace Gui


