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
# include <qstatusbar.h>
#endif


#include "Command.h"
#include "Action.h"
#include "Application.h"
#include "MainWindow.h"
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
  getMainWindow()->tileHorizontal ();
}

bool StdCmdTileHor::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_TileVertical
//===========================================================================
DEF_STD_CMD_A(StdCmdTileVer);

StdCmdTileVer::StdCmdTileVer()
  :CppCommand("Std_TileVertical")
{
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
  getMainWindow()->tile();
}

bool StdCmdTileVer::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_TilePragmatic
//===========================================================================
DEF_STD_CMD_A(StdCmdTilePra);

StdCmdTilePra::StdCmdTilePra()
  :CppCommand("Std_TilePragmatic")
{
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
  getMainWindow()->cascade();
}

bool StdCmdTilePra::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_CloseActiveWindow
//===========================================================================
DEF_STD_CMD_A(StdCmdCloseActiveWindow);

StdCmdCloseActiveWindow::StdCmdCloseActiveWindow()
  :CppCommand("Std_CloseActiveWindow")
{
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Cl&ose");
  sToolTipText  = QT_TR_NOOP("Close active window");
  sWhatsThis    = QT_TR_NOOP("Close active window");
  sStatusTip    = QT_TR_NOOP("Close active window");
  iAccel        = 0;
}

void StdCmdCloseActiveWindow::activated(int iMsg)
{
  getMainWindow()->closeActiveWindow();
}

bool StdCmdCloseActiveWindow::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_CloseAllWindows
//===========================================================================
DEF_STD_CMD_A(StdCmdCloseAllWindows);

StdCmdCloseAllWindows::StdCmdCloseAllWindows()
  :CppCommand("Std_CloseAllWindows")
{
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Close Al&l");
  sToolTipText  = QT_TR_NOOP("Close all windows");
  sWhatsThis    = QT_TR_NOOP("Close all windows");
  sStatusTip    = QT_TR_NOOP("Close all windows");
  iAccel        = 0;
}

void StdCmdCloseAllWindows::activated(int iMsg)
{
  getMainWindow()->closeAllWindows();
}

bool StdCmdCloseAllWindows::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_ActivateNextWindow
//===========================================================================
DEF_STD_CMD_A(StdCmdActivateNextWindow);

StdCmdActivateNextWindow::StdCmdActivateNextWindow()
  :CppCommand("Std_ActivateNextWindow")
{
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
  getMainWindow()->activateNextWindow();
}

bool StdCmdActivateNextWindow::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_ActivatePrevWindow
//===========================================================================
DEF_STD_CMD_A(StdCmdActivatePrevWindow);

StdCmdActivatePrevWindow::StdCmdActivatePrevWindow()
  :CppCommand("Std_ActivatePrevWindow")
{
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
  getMainWindow()->activatePrevWindow();
}

bool StdCmdActivatePrevWindow::isActive(void)
{
  return !( getMainWindow()->windows().isEmpty() );
}

//===========================================================================
// Std_Windows
//===========================================================================
DEF_STD_CMD(StdCmdWindows);

StdCmdWindows::StdCmdWindows()
  :CppCommand("Std_Windows")
{
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
  Gui::Dialog::DlgActivateWindowImp dlg( getMainWindow(), "Windows", true );
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
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Toggles this window");
  sToolTipText  = QT_TR_NOOP("Toggles this window");
  sWhatsThis    = QT_TR_NOOP("Toggles this window");
  sStatusTip    = QT_TR_NOOP("Toggles this window");
  iAccel        = 0;
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
  return new ViewAction(getMainWindow());
}

//===========================================================================
// Std_ViewStatusBar
//===========================================================================

DEF_STD_CMD_AC(StdCmdStatusBar);

StdCmdStatusBar::StdCmdStatusBar()
  :CppCommand("Std_ViewStatusBar", Cmd_Toggle)
{
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Status bar");
  sToolTipText  = QT_TR_NOOP("Toggles the status bar");
  sWhatsThis    = QT_TR_NOOP("Toggles the status bar");
  sStatusTip    = QT_TR_NOOP("Toggles the status bar");
  iAccel        = 0;
}

QAction * StdCmdStatusBar::createAction(void)
{
  QAction *pcAction = CppCommand::createAction();
  activated(0); // hide status bar
  pcAction->setOn(true); // and show it again invoking the toggle action

  return pcAction;
}

void StdCmdStatusBar::activated(int iMsg)
{
  QWidget* w = getMainWindow()->statusBar();
  w->isVisible() ? w->hide() : w->show();
}

bool StdCmdStatusBar::isActive(void)
{
  return true;
}

//===========================================================================
// Std_WindowsMenu
//===========================================================================

DEF_STD_CMD_AC(StdCmdWindowsMenu );

StdCmdWindowsMenu::StdCmdWindowsMenu()
  :CppCommand("Std_WindowsMenu")
{
  sGroup        = QT_TR_NOOP("Standard");
  sMenuText     = QT_TR_NOOP("Activates this window");
  sToolTipText  = QT_TR_NOOP("Activates this window");
  sWhatsThis    = QT_TR_NOOP("Activates this window");
  sStatusTip    = QT_TR_NOOP("Activates this window");
  iAccel        = 0;
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
  return new WindowAction(getMainWindow());
}



//===========================================================================
// Instanciation
//===========================================================================


namespace Gui {

void CreateWindowStdCommands(void)
{
  CommandManager &rcCmdMgr = Application::Instance->commandManager();

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
  rcCmdMgr.addCommand(new StdCmdStatusBar());
}

} // namespace Gui


