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

#include "Workbench.h"
#include "WorkbenchPy.h"
#include "MenuManager.h"
#include "ToolBarManager.h"
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

MenuItem* Workbench::setupMenuBar() const
{
  // Setup the default menu bar
  MenuItem* menuBar = new MenuItem;

  // File
  MenuItem* file = new MenuItem( menuBar );
  file->setCommand( QObject::tr("&File") );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_SaveAs" << "Separator" << "Std_Print"
        << "Separator" << "Std_MRU" << "Separator" << "Std_Quit";


  // Edit
  MenuItem* edit = new MenuItem( menuBar );
  edit->setCommand( QObject::tr("&Edit") );
  *edit << "Std_Cut" << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo"
        << "Separator" << "Std_DlgPreferences";

  // View
  MenuItem* view = new MenuItem( menuBar );
  view->setCommand( QObject::tr("&View") );
  *view << "Std_ViewCreateInventor" << "Std_ViewCreateOCC" << "Separator" << "Std_ViewFullScreen" 
        << "Separator" << "Std_ViewMenu";
  
  // Tools
  MenuItem* tool = new MenuItem( menuBar );
  tool->setCommand( QObject::tr("&Tools") );
  *tool << "Std_CommandLine" << "Std_DlgParameter" << "Std_OCAFBrowser" << "Separator" << "Std_DlgMacroRecord" 
        << "Std_DlgMacroStop" << "Std_DlgMacroExecute" << "Separator" << "Std_DlgCustomize";

  // Test commands
  MenuItem* test = new MenuItem( menuBar );
  test->setCommand( QObject::tr("Test &Commands") );
  *test << "Std_Test1" << "Std_Test2" << "Std_Test3" << "Std_Test4" << "Std_Test5" 
        << "Std_Test6" << "Std_Test7" << "Std_Test8";

#if 1
  MenuItem* hallo = new MenuItem ( test );
  hallo->setCommand("Test_Test");
  *hallo << "Std_New" << "Std_Open" << "Separator";
  *test << "Separator" << "Std_Quit";
#endif

  // Inventor View
  MenuItem* opiv = new MenuItem( menuBar );
  opiv->setCommand( QObject::tr("&Inventor View") );
  *opiv << "Std_ViewExample1" << "Std_ViewExample2" << "Std_ViewExample3" << "Std_ViewIvDecorationOn" 
        << "Std_ViewIvDecorationOff" << "Std_ViewIvStereoOn" << "Std_ViewIvStereoOff" << "Std_ViewIvIssueCamPos";

  // Windows
  MenuItem* wnd = new MenuItem( menuBar );
  wnd->setCommand( QObject::tr("&Windows") );
  *wnd << "Std_TilePragmatic" << "Std_TileHoricontal" << "Std_TileVertical" << "Std_WindowsMenu" 
       << "Separator" << "Std_Windows";

  // Separator
  MenuItem* sep = new MenuItem( menuBar );
  sep->setCommand( "Separator" );

  // Help
  MenuItem* help = new MenuItem( menuBar );
  help->setCommand( QObject::tr("&Help") );
  *help << "Std_OnlineHelp" << "Std_TipOfTheDay" << "Separator" << "Std_About" << "Std_AboutQt"
        << "Separator" << "Std_WhatsThis";

  return menuBar;
}

ToolBarItem* Workbench::setupToolBars() const
{
  ToolBarItem* root = new ToolBarItem;
  
  // File
  ToolBarItem* file = new ToolBarItem( root );
  file->setCommand( QObject::tr("file operations") );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_Print" << "Separator" << "Std_Cut"
        << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo" << "Separator"
        << "Std_Workbench" << "Std_WhatsThis";

  // Macro
  ToolBarItem* macro = new ToolBarItem( root );
  macro->setCommand( QObject::tr("Macro recording") );
  *macro << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute";

  // View
  ToolBarItem* view = new ToolBarItem( root );
  view->setCommand( QObject::tr("Standard views") );
  *view << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
        << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";

  return root;
}

ToolBarItem* Workbench::setupCommandBars() const
{
  ToolBarItem* root = new ToolBarItem;
  
  // File
  ToolBarItem* file = new ToolBarItem( root );
  file->setCommand( QObject::tr("file operations") );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_Print" << "Separator" << "Std_Cut"
        << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo" << "Separator"
        << "Std_Workbench" << "Std_WhatsThis";

  // Macro
  ToolBarItem* macro = new ToolBarItem( root );
  macro->setCommand( QObject::tr("Macro recording") );
  *macro << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute";

  // View
  ToolBarItem* view = new ToolBarItem( root );
  view->setCommand( QObject::tr("Standard views") );
  *view << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
        << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";

  return root;
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
  MenuManager::getInstance().appendMenuItems( menu, items );
}

void Workbench::removeMenuItems( const QString& menu, const QStringList& items ) const
{
  MenuManager::getInstance().removeMenuItems( menu, items );
}

void Workbench::appendMenu( const QString& menu ) const
{
  MenuManager::getInstance().appendMenu( menu );
}

void Workbench::removeMenu( const QString& menu ) const
{
  MenuManager::getInstance().removeMenu( menu );
}

void Workbench::appendSubmenuItems( const QString& submenu, const QStringList& items ) const
{
  MenuManager::getInstance().appendSubmenuItems( submenu, items );
}

void Workbench::removeSubmenuItems( const QString& submenu, const QStringList& items ) const
{
  MenuManager::getInstance().removeSubmenuItems( submenu, items );
}

void Workbench::appendSubmenu( const QString& menu, const QString& submenu ) const
{
  MenuManager::getInstance().appendSubmenu( menu, submenu );
}

void Workbench::removeSubmenu( const QString& menu, const QString& submenu ) const
{
  MenuManager::getInstance().removeSubmenu( menu, submenu );
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

bool StdWorkbench::activate()
{
  MenuItem* menuBar = setupMenuBar();

  MenuItem* item = menuBar->findItem( QObject::tr("&Help"));
  item->removeItem(item->findItem("Std_WhatsThis"));

  item = menuBar->findItem( QObject::tr("&Inventor View") );
  menuBar->removeItem( item );

  MenuManager::getInstance().setup( menuBar );
  delete menuBar;

  ToolBarItem* tb = setupToolBars();
  ToolBarManager::getInstance()->setup( tb );
  delete tb;

  ToolBarItem* cb = setupCommandBars();
  CommandBarManager::getInstance()->setup( cb );
  delete cb;

  return true;
}
