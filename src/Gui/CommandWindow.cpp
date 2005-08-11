/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#include "Action.h"
#include "Application.h"
#include "View.h"
#include "Document.h"
#include "DlgActivateWindowImp.h"

#include "../Base/Exception.h"
#include "../App/Document.h"
#include "Macro.h"

using namespace Gui;


//===========================================================================
// Std_TileHoricontal
//===========================================================================
DEF_STD_CMD_A(StdCmdTileHor);

StdCmdTileHor::StdCmdTileHor()
  :CppCommand("Std_TileHoricontal")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Tile &Horizontally");
  sToolTipText  = QT_TR_NOOP("Tile the windows horizontally");
  sWhatsThis    = QT_TR_NOOP("Tile the windows horizontally");
  sStatusTip    = QT_TR_NOOP("Tile the windows horizontally");
  sPixmap       = "Std_WindowTileHor";
  iAccel        = 0;
}

void StdCmdTileHor::activated(int iMsg)
{
  getAppWnd()->tileHorizontal ();
}

bool StdCmdTileHor::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_TileVertical
//===========================================================================
DEF_STD_CMD_A(StdCmdTileVer);

StdCmdTileVer::StdCmdTileVer()
  :CppCommand("Std_TileVertical")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Tile Vertically");
  sToolTipText  = QT_TR_NOOP("Tile the windows vertically");
  sWhatsThis    = QT_TR_NOOP("Tile the windows vertically");
  sStatusTip    = QT_TR_NOOP("Tile the windows vertically");
  sPixmap       = "Std_WindowTileVer";
  iAccel        = 0;
}

void StdCmdTileVer::activated(int iMsg)
{
  getAppWnd()->tile();
}

bool StdCmdTileVer::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_TilePragmatic
//===========================================================================
DEF_STD_CMD_A(StdCmdTilePra);

StdCmdTilePra::StdCmdTilePra()
  :CppCommand("Std_TilePragmatic")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Cascade");
  sToolTipText  = QT_TR_NOOP("Tile pragmatic");
  sWhatsThis    = QT_TR_NOOP("Tile pragmatic");
  sStatusTip    = QT_TR_NOOP("Tile pragmatic");
  sPixmap       = "Std_WindowCascade";
  iAccel        = 0;
}

void StdCmdTilePra::activated(int iMsg)
{
  getAppWnd()->cascade();
}

bool StdCmdTilePra::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_CloseActiveWindow
//===========================================================================
DEF_STD_CMD_A(StdCmdCloseActiveWindow);

StdCmdCloseActiveWindow::StdCmdCloseActiveWindow()
  :CppCommand("Std_CloseActiveWindow")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Cl&ose");
  sToolTipText  = QT_TR_NOOP("Close active window");
  sWhatsThis    = QT_TR_NOOP("Close active window");
  sStatusTip    = QT_TR_NOOP("Close active window");
  iAccel        = 0;
}

void StdCmdCloseActiveWindow::activated(int iMsg)
{
  getAppWnd()->closeActiveWindow();
}

bool StdCmdCloseActiveWindow::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_CloseAllWindows
//===========================================================================
DEF_STD_CMD_A(StdCmdCloseAllWindows);

StdCmdCloseAllWindows::StdCmdCloseAllWindows()
  :CppCommand("Std_CloseAllWindows")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Close Al&l");
  sToolTipText  = QT_TR_NOOP("Close all windows");
  sWhatsThis    = QT_TR_NOOP("Close all windows");
  sStatusTip    = QT_TR_NOOP("Close all windows");
  iAccel        = 0;
}

void StdCmdCloseAllWindows::activated(int iMsg)
{
  getAppWnd()->closeAllWindows();
}

bool StdCmdCloseAllWindows::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_ActivateNextWindow
//===========================================================================
DEF_STD_CMD_A(StdCmdActivateNextWindow);

StdCmdActivateNextWindow::StdCmdActivateNextWindow()
  :CppCommand("Std_ActivateNextWindow")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Ne&xt");
  sToolTipText  = QT_TR_NOOP("Activate next window");
  sWhatsThis    = QT_TR_NOOP("Activate next window");
  sStatusTip    = QT_TR_NOOP("Activate next window");
  sPixmap       = "Std_WindowNext";
  iAccel        = 0;
}

void StdCmdActivateNextWindow::activated(int iMsg)
{
  getAppWnd()->activateNextWindow();
}

bool StdCmdActivateNextWindow::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_ActivatePrevWindow
//===========================================================================
DEF_STD_CMD_A(StdCmdActivatePrevWindow);

StdCmdActivatePrevWindow::StdCmdActivatePrevWindow()
  :CppCommand("Std_ActivatePrevWindow")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Pre&vious");
  sToolTipText  = QT_TR_NOOP("Activate previous window");
  sWhatsThis    = QT_TR_NOOP("Activate previous window");
  sStatusTip    = QT_TR_NOOP("Activate previous window");
  sPixmap       = "Std_WindowPrev";
  iAccel        = 0;
}

void StdCmdActivatePrevWindow::activated(int iMsg)
{
  getAppWnd()->activatePrevWindow();
}

bool StdCmdActivatePrevWindow::isActive(void)
{
  return !( getAppWnd()->windows().isEmpty() );
}

//===========================================================================
// Std_Windows
//===========================================================================
DEF_STD_CMD(StdCmdWindows);

StdCmdWindows::StdCmdWindows()
  :CppCommand("Std_Windows")
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("&Windows...");
  sToolTipText  = QT_TR_NOOP("Windows list");
  sWhatsThis    = QT_TR_NOOP("Windows list");
  sStatusTip    = QT_TR_NOOP("Windows list");
  //sPixmap     = "";
  iAccel        = 0;
}

void StdCmdWindows::activated(int iMsg)
{
  Gui::Dialog::DlgActivateWindowImp dlg( getAppWnd(), "Windows", true );
  dlg.exec();
}

/*
//===========================================================================
// Std_MDINormal
//===========================================================================
DEF_STD_CMD(StdCmdMDINormal);

StdCmdMDINormal::StdCmdMDINormal()
  :CppCommand("Std_MDINormal")
{
  sAppModule    = "";
  sGroup        = "Standard";
  sMenuText     = "MDI Normal";
  sToolTipText  = "Set the standard MDI mode";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Paste";
  iAccel        = 0;
}


void StdCmdMDINormal::Activated(int iMsg)
{
  getAppWnd()->switchToChildframeMode();
}
*/

//===========================================================================
// Std_MDIToplevel
//===========================================================================
DEF_STD_CMD(StdCmdMDIToplevel);

StdCmdMDIToplevel::StdCmdMDIToplevel()
  :CppCommand("Std_MDIToplevel",Cmd_Toggle)
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("MDI seperate windows");
  sToolTipText  = QT_TR_NOOP("Set the seperate window MDI mode");
  sWhatsThis    = QT_TR_NOOP("Set the seperate window MDI mode");
  sStatusTip    = QT_TR_NOOP("Set the seperate window MDI mode");
  sPixmap       = "TopLevel";
  iAccel        = 0;
}

void StdCmdMDIToplevel::activated(int iMsg)
{
  if(iMsg){
    // switches Tab mode off 
    toggleCommand("Std_MDIToplevel",false);
//    getAppWnd()->switchToToplevelMode();
  }//else
    //getAppWnd()->finishToplevelMode();
//    getAppWnd()->switchToChildframeMode();
}

//===========================================================================
// Std_MDITabed
//===========================================================================
DEF_STD_CMD(StdCmdMDITabbed);

StdCmdMDITabbed::StdCmdMDITabbed()
  :CppCommand("Std_MDITabbed",Cmd_Toggle)
{
  sAppModule    = "";
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("MDI tabbed");
  sToolTipText  = QT_TR_NOOP("Set the tabbed MDI mode");
  sWhatsThis    = QT_TR_NOOP("Set the tabbed MDI mode");
  sStatusTip    = QT_TR_NOOP("Set the tabbed MDI mode");
  sPixmap       = "TopLevel";
  iAccel        = 0;
}

void StdCmdMDITabbed::activated(int iMsg)
{
  if(iMsg){
    // switches Toplevel off 
    toggleCommand("Std_MDIToplevel",false);
//    getAppWnd()->switchToTabPageMode();
  }//else
//    getAppWnd()->switchToChildframeMode();
}


//===========================================================================
// Std_ViewMenu
//===========================================================================

DEF_STD_CMD_AC(StdCmdViewMenu);

StdCmdViewMenu::StdCmdViewMenu()
  :CppCommand("Std_ViewMenu")
{
}

void StdCmdViewMenu::activated(int iMsg)
{
}

bool StdCmdViewMenu::isActive(void)
{
  return true;
}

QAction * StdCmdViewMenu::createAction(void)
{
  return new ViewAction(ApplicationWindow::Instance);
}

//===========================================================================
// Std_WindowsMenu
//===========================================================================

DEF_STD_CMD_AC(StdCmdWindowsMenu );

StdCmdWindowsMenu::StdCmdWindowsMenu()
  :CppCommand("Std_WindowsMenu")
{
}

void StdCmdWindowsMenu::activated(int iMsg)
{
}

bool StdCmdWindowsMenu::isActive(void)
{
  return true;
}

QAction * StdCmdWindowsMenu::createAction(void)
{
  return new WindowAction(ApplicationWindow::Instance);
}



//===========================================================================
// Instanciation
//===========================================================================


namespace Gui {

void CreateWindowStdCommands(void)
{
  CommandManager &rcCmdMgr = ApplicationWindow::Instance->commandManager();

  //rcCmdMgr.addCommand(new StdCmdMDINormal());
  rcCmdMgr.addCommand(new StdCmdMDIToplevel());
  rcCmdMgr.addCommand(new StdCmdMDITabbed());
  rcCmdMgr.addCommand(new StdCmdTileHor());
  rcCmdMgr.addCommand(new StdCmdTileVer());
  rcCmdMgr.addCommand(new StdCmdTilePra());
  rcCmdMgr.addCommand(new StdCmdCloseActiveWindow());
  rcCmdMgr.addCommand(new StdCmdCloseAllWindows());
  rcCmdMgr.addCommand(new StdCmdActivateNextWindow());
  rcCmdMgr.addCommand(new StdCmdActivatePrevWindow());
  rcCmdMgr.addCommand(new StdCmdWindows());
  rcCmdMgr.addCommand(new StdCmdViewMenu());
  rcCmdMgr.addCommand(new StdCmdWindowsMenu());
}

} // namespace Gui


