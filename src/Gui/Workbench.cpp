/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qpopupmenu.h>
#endif

#include <Base/Parameter.h>
#include <App/Application.h>

#include "Workbench.h"
#include "WorkbenchPy.h"
#include "MenuManager.h"
#include "ToolBarManager.h"
#include "Application.h"
#include "Command.h"
#include "CommandBarManager.h"

using namespace Gui;

Workbench::Workbench()
  : _name(QString::null), _workbenchPy(0)
{
}

Workbench::~Workbench()
{
}

QString Workbench::name() const
{
  return _name;
}

void Workbench::setName( const QString& name )
{
  _name = name;
}

ToolBarItem* Workbench::importCustomToolBars() const
{
  const char* szName = this->name().latin1();
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench")->GetGroup( szName )->GetGroup("Toolbars");
  
  ToolBarItem* root = new ToolBarItem;
  std::vector<FCHandle<ParameterGrp> > hGrps = hGrp->GetGroups();
  CommandManager& rMgr = ApplicationWindow::Instance->commandManager();
  for ( std::vector<FCHandle<ParameterGrp> >::iterator it = hGrps.begin(); it != hGrps.end(); ++it )
  {
    ToolBarItem* bar = new ToolBarItem( root );
    bar->setCommand( (*it)->GetGroupName() );
   
    // get the elements of the subgroups
    std::map<std::string,std::string> items = hGrp->GetGroup( (*it)->GetGroupName() )->GetASCIIMap();
    for ( std::map<std::string,std::string>::iterator it2 = items.begin(); it2 != items.end(); ++it2 )
    {
      // to save the order a number is prepended
      QString cmd = it2->first.c_str(); // command name
      cmd = cmd.mid(2);
      const char* mod = it2->second.c_str(); // module name
//      Command* pCmd = rMgr.getCommandByName( cmd ); 
      *bar << cmd;
    }
  }

  return root;
}

void Workbench::exportCustomToolBars( ToolBarItem* ) const
{
}

ToolBarItem* Workbench::importCustomCommandBars() const
{
  const char* szName = this->name().latin1();
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench")->GetGroup( szName )->GetGroup("Commandbars");
  
  ToolBarItem* root = new ToolBarItem;
  std::vector<FCHandle<ParameterGrp> > hGrps = hGrp->GetGroups();
  CommandManager& rMgr = ApplicationWindow::Instance->commandManager();
  for ( std::vector<FCHandle<ParameterGrp> >::iterator it = hGrps.begin(); it != hGrps.end(); ++it )
  {
    ToolBarItem* bar = new ToolBarItem( root );
    bar->setCommand( (*it)->GetGroupName() );
   
    // get the elements of the subgroups
    std::map<std::string,std::string> items = hGrp->GetGroup( (*it)->GetGroupName() )->GetASCIIMap();
    for ( std::map<std::string,std::string>::iterator it2 = items.begin(); it2 != items.end(); ++it2 )
    {
      // to save the order a number is prepended
      QString cmd = it2->first.c_str(); // command name
      cmd = cmd.mid(2);
      const char* mod = it2->second.c_str(); // module name
//      Command* pCmd = rMgr.getCommandByName( cmd ); 
      *bar << cmd;
    }
  }

  return root;
}

void Workbench::exportCustomCommandBars( ToolBarItem* ) const
{
}

bool Workbench::activate()
{
  MenuItem* mb = setupMenuBar();
  MenuManager::getInstance()->setup( mb );
  delete mb;

  ToolBarItem* tb = setupToolBars();
  ToolBarManager::getInstance()->setup( tb );
  delete tb;
  ToolBarItem* cw = importCustomToolBars();
  ToolBarManager::getInstance()->customSetup(cw);
  delete cw;

  ToolBarItem* cb = setupCommandBars();
  CommandBarManager::getInstance()->setup( cb );
  delete cb;
  ToolBarItem* cc = importCustomCommandBars();
  CommandBarManager::getInstance()->customSetup(cc);
  delete cc;

  return true;
}

Base::PyObjectBase* Workbench::GetPyObject()
{
  if ( !_workbenchPy )
  {
    _workbenchPy = new WorkbenchPy(this);
  }

  return _workbenchPy;
}

void Workbench::appendMenuItems( const QString& menu, const QStringList& items ) const
{
  MenuManager::getInstance()->appendMenuItems( menu, items );
}

void Workbench::removeMenuItems( const QString& menu, const QStringList& items ) const
{
  MenuManager::getInstance()->removeMenuItems( menu, items );
}

void Workbench::appendMenu( const QString& menu ) const
{
  MenuManager::getInstance()->appendMenu( menu );
}

void Workbench::removeMenu( const QString& menu ) const
{
  MenuManager::getInstance()->removeMenu( menu );
}

void Workbench::appendSubmenuItems( const QString& submenu, const QStringList& items ) const
{
  MenuManager::getInstance()->appendSubmenuItems( submenu, items );
}

void Workbench::removeSubmenuItems( const QString& submenu, const QStringList& items ) const
{
  MenuManager::getInstance()->removeSubmenuItems( submenu, items );
}

void Workbench::appendSubmenu( const QString& menu, const QString& submenu ) const
{
  MenuManager::getInstance()->appendSubmenu( menu, submenu );
}

void Workbench::removeSubmenu( const QString& menu, const QString& submenu ) const
{
  MenuManager::getInstance()->removeSubmenu( menu, submenu );
}

void Workbench::appendContextMenuItems() const
{
}

void Workbench::removeContextMenuItems() const
{
}

void Workbench::appendContextMenu() const
{
}

void Workbench::removeContextMenu() const
{
}

void Workbench::appendToolbarItems() const
{
}

void Workbench::removeToolbarItems() const
{
}

void Workbench::appendToolbar() const
{
}

void Workbench::removeToolbar() const
{
}

void Workbench::appendCommandbarItems() const
{
}

void Workbench::removeCommandbarItems() const
{
}

void Workbench::appendCommandbar() const
{
}

void Workbench::removeCommandbar() const
{
}

void Workbench::appendDockWindow() const
{
}

void Workbench::removeDockWindow() const
{
}

// --------------------------------------------------------------------

StdWorkbench::StdWorkbench()
  : Workbench()
{
}

StdWorkbench::~StdWorkbench()
{
}

MenuItem* StdWorkbench::setupMenuBar() const
{
  // Setup the default menu bar
  MenuItem* menuBar = new MenuItem;

  // File
  MenuItem* file = new MenuItem( menuBar );
  file->setCommand( "&File" );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_SaveAs" << "Separator" << "Std_Print"
        << "Separator" << "Std_MRU" << "Separator" << "Std_Quit";

  // Edit
  MenuItem* edit = new MenuItem( menuBar );
  edit->setCommand( "&Edit" );
  *edit << "Std_Cut" << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo"
        << "Separator" << "Std_DlgPreferences";

  // View
  MenuItem* view = new MenuItem( menuBar );
  view->setCommand( "&View" );
  *view << "Std_ViewCreateInventor" << "Separator" << "Std_ToggleVisibility" << "Std_ViewFullScreen" 
        << "Separator" << "Std_ViewMenu" << "Std_ViewStatusBar";
  
  // Tools
  MenuItem* tool = new MenuItem( menuBar );
  tool->setCommand( "&Tools" );
  *tool << "Std_CommandLine" << "Std_DlgParameter" << "Std_OCAFBrowser" << "Separator" << "Std_DlgMacroRecord" 
        << "Std_DlgMacroStop" << "Std_DlgMacroExecute" << "Separator" << "Std_DlgCustomize";

  // Windows
  MenuItem* wnd = new MenuItem( menuBar );
  wnd->setCommand( "&Windows" );
  *wnd << "Std_CloseActiveWindow" << "Std_CloseAllWindows" << "Separator" << "Std_ActivateNextWindow" 
       << "Std_ActivatePrevWindow" << "Separator" << "Std_TilePragmatic" << "Std_TileHoricontal" 
       << "Std_TileVertical" << "Std_WindowsMenu" << "Separator" << "Std_Windows";

  // Separator
  MenuItem* sep = new MenuItem( menuBar );
  sep->setCommand( "Separator" );

  // Help
  MenuItem* help = new MenuItem( menuBar );
  help->setCommand( "&Help" );
  *help << "Std_OnlineHelp" << "Std_TipOfTheDay" << "Separator" << "Std_About" << "Std_AboutQt"
        << "Separator" << "Std_WhatsThis" << "Std_DescriptionMode";

  return menuBar;
}

ToolBarItem* StdWorkbench::setupToolBars() const
{
  ToolBarItem* root = new ToolBarItem;
  
  // File
  ToolBarItem* file = new ToolBarItem( root );
  file->setCommand( "file operations" );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_Print" << "Separator" << "Std_Cut"
        << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo" << "Separator"
        << "Std_Workbench" << "Std_WhatsThis";

  // Macro
  ToolBarItem* macro = new ToolBarItem( root );
  macro->setCommand( "Macro recording" );
  *macro << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute";

  // View
  ToolBarItem* view = new ToolBarItem( root );
  view->setCommand( "Standard views" );
  *view << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
        << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";

  return root;
}

ToolBarItem* StdWorkbench::setupCommandBars() const
{
  ToolBarItem* root = new ToolBarItem;
  
  // View
  ToolBarItem* view = new ToolBarItem( root );
  view->setCommand( "Standard views" );
  *view << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
        << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";
  // Special Ops
  ToolBarItem* macro = new ToolBarItem( root );
  macro->setCommand( "Special Ops" );
  *macro << "Std_DlgParameter" << "Std_DlgPreferences" << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute"
         << "Std_DlgCustomize" << "Std_CommandLine";


  return root;
}

// --------------------------------------------------------------------

TestWorkbench::TestWorkbench()
  : StdWorkbench()
{
}

TestWorkbench::~TestWorkbench()
{
}

MenuItem* TestWorkbench::setupMenuBar() const
{
  // Setup the default menu bar
  MenuItem* menuBar = StdWorkbench::setupMenuBar();

  MenuItem* item = menuBar->findItem( "&Help" );
  item->removeItem(item->findItem("Std_WhatsThis"));

  // Test commands
  MenuItem* test = new MenuItem;
  menuBar->insertItem( item, test );
  test->setCommand( "Test &Commands" );
  *test << "Std_Test1" << "Std_Test2" << "Std_Test3" << "Std_Test4" << "Std_Test5" 
        << "Std_Test6" << "Std_Test7" << "Std_Test8";

  // Inventor View
  MenuItem* opiv = new MenuItem;
  menuBar->insertItem( item, opiv );
  opiv->setCommand( "&Inventor View" );
  *opiv << "Std_ViewExample1" << "Std_ViewExample2" << "Std_ViewExample3" << "Std_ViewIvDecorationOn" 
        << "Std_ViewIvDecorationOff" << "Std_ViewIvStereoOn" << "Std_ViewIvStereoOff" << "Std_ViewIvIssueCamPos";

  return menuBar;
}

ToolBarItem* TestWorkbench::setupToolBars() const
{
  return 0;
}

ToolBarItem* TestWorkbench::setupCommandBars() const
{
  return 0;
}

