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
# include <qtoolbar.h>
#endif

#include "Workbench.h"
#include "WorkbenchPy.h"
#include "MenuManager.h"
#include "ToolBarManager.h"
#include "Application.h"
#include "Command.h"
#include "CommandBarManager.h"

#include <App/Application.h>

#include <Base/Parameter.h>
#include <Base/Interpreter.h>

using namespace Gui;

/** \defgroup workbench Workbench concept
 *
 * FreeCAD provides the possibility to have workbenches for each module. A workbench changes the appearance of the main window
 * in that way that it defines toolbars, command bars or menus (later on dockable windows, ...) that are shown to the user.
 *
 * The idea behind this concept is keep the main window as simple as possible and to show to the user only the important and not dozens of
 * functions. 
 *
 * \section stepbystep Step by step
 * Here is a short description how you can add its own workbench to a module.
 *
 * \subsection newClass Inherit either from Workbench or StdWorkbench
 * First you have to subclass either Workbench or StdWorkbench and reimplement the methods setupMenuBar(), setupToolBars() and setupCommandBars().
 * The difference between both classes is that these methods of Workbench are pure virtual while StdWorkbench already defines the most important
 * functions, such as the 'File', 'Edit', ..., 'Help' menus with their common functions.
 * If your class derives from Workbench then you have to define your menus, toolbars and command bars from scratch while deriving from StdWorkbench
 * you have the possibility to add your preferred functions or even remove some unneeded functions.
 * \code
 * 
 * class MyWorkbench : public StdWorkbench
 * {
 *  ...
 * protected:
 *   MenuItem* setupMenuBar() const
 *   {
 *     MenuItem* root = StdWorkbench::setupMenuBar();
 *     // your changes
 *     return root;
 *   }
 *   ToolBarItem* setupToolBars() const
 *   {
 *     ToolBarItem* root = StdWorkbench::setupToolBars();
 *     // your changes
 *     return root;
 *   }
 *   ToolBarItem* setupCommandBars() const
 *   {
 *     ToolBarItem* root = StdWorkbench::setupCommandBars();
 *     // your changes
 *     return root;
 *   }
 * };
 * 
 * \endcode
 * \code
 * 
 * class MyWorkbench : public Workbench
 * {
 *  ...
 * protected:
 *   MenuItem* setupMenuBar() const
 *   {
 *     MenuItem* root = new MenuItem;
 *     // setup from scratch
 *     return root;
 *   }
 *   ToolBarItem* setupToolBars() const
 *   {
 *     ToolBarItem* root = new ToolBarItem;
 *     // setup from scratch
 *     return root;
 *   }
 *   ToolBarItem* setupCommandBars() const
 *   {
 *     ToolBarItem* root = new ToolBarItem;
 *     // setup from scratch
 *     return root;
 *   }
 * };
 * 
 * \endcode
 *
 * + Default & Custom toolbars
 * + dock windows
 * + Python workbench loader InitGui.py
 * + FreeCADGui.AddWorkbench
 * + Limitations (name, file operations, mod. of default stuff n.p.)
 * + GetIcon
 * + register wb in C++
 * + register wb in Python
 * This is the base class for the workbench facility. Each FreeCAD module can provide its own 
 * workbench implementation. The workbench defines which GUI elements (such as toolbars, menus, 
 * dockable windows, ...) are added to the mainwindow and which gets removed or hidden. 
 * When a workbench object gets activated the first time the module - it stands for - gets 
 * loaded into RAM.
 * 
 * Workbench
 * StdWorkbench
 * PythonWorkbench
 * 
 */

Workbench::Workbench()
  : _name(QString::null)
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

ToolBarItem* Workbench::importCustomBars( const char* node ) const
{
  const char* szName = this->name().latin1();
  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench")->GetGroup( szName )->GetGroup( node );
  
  ToolBarItem* root = new ToolBarItem;
  std::vector<FCHandle<ParameterGrp> > hGrps = hGrp->GetGroups();
  CommandManager& rMgr = Application::Instance->commandManager();
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
      Command* pCmd = rMgr.getCommandByName( cmd );
      if ( !pCmd ) // unknown command
      {
        // try to find out the appropriate module name
        QString pyMod = QString("%1Gui").arg(mod);
        try{
          Base::Interpreter().loadModule( pyMod.latin1() );
        }
        catch( const Base::Exception& ) {
        }
      }

      *bar << cmd;
    }
  }

  return root;
}

void Workbench::exportCustomBars( ToolBarItem* toolBar, const char* node ) const
{
  const char* szName = this->name().latin1();
  CommandManager& rMgr = Application::Instance->commandManager();

  ParameterGrp::handle hGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench")->GetGroup( szName )->GetGroup( node );
  hGrp->Clear();

  QPtrList<ToolBarItem> items = toolBar->getItems();

  ToolBarItem* item;
  for ( item = items.first(); item; item = items.next() )
  {
    ParameterGrp::handle hSubGrp = hGrp->GetGroup( item->command().latin1() );
    QPtrList<ToolBarItem> subitems = item->getItems();
    ToolBarItem* subitem;

    int pos = 0;
    for ( subitem = subitems.first(); subitem; subitem = subitems.next() )
    {
      Command* pCmd = rMgr.getCommandByName( subitem->command().latin1() );
      QString mod = "unknown";
      if ( pCmd )
        mod = pCmd->getAppModuleName();
      QString key; key.sprintf("%.2d%s", pos++, subitem->command().latin1() );
      hSubGrp->SetASCII( key.latin1(), mod.latin1() );
    }
  }
}

void Workbench::showOrHideToolBars( bool read ) const
{
  QPtrList<QToolBar> bars = ToolBarManager::getInstance()->toolBars();
	QToolBar* bar;
  if ( read )
  {
	  for ( bar=bars.first(); bar; bar=bars.next() )
	  {
	    if ( !App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench")->GetBool( bar->name(), true) )
	  	  bar->hide();
	  }
  }
  else // write
  {
	  for ( bar=bars.first(); bar; bar=bars.next() )
	  {
	    App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Workbench")->SetBool( bar->name(), !bar->isHidden() );
	  }
  }
}

bool Workbench::activate()
{
  // just checks the toolbars if they must be hidden
  showOrHideToolBars( false );

  MenuItem* mb = setupMenuBar();
  MenuManager::getInstance()->setup( mb );
  delete mb;

  ToolBarItem* tb = setupToolBars();
  ToolBarManager::getInstance()->setup( tb );
  delete tb;
  ToolBarItem* cw = importCustomBars("Toolbars");
  ToolBarManager::getInstance()->customSetup(cw);
  delete cw;

  ToolBarItem* cb = setupCommandBars();
  CommandBarManager::getInstance()->setup( cb );
  delete cb;
  ToolBarItem* cc = importCustomBars("Commandbars");
  CommandBarManager::getInstance()->customSetup(cc);
  delete cc;
  
  // just checks the toolbars if they must be hidden
  showOrHideToolBars( true );

  return true;
}

Base::PyObjectBase* Workbench::GetPyObject()
{
  return new WorkbenchPy(this);
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
  *view << "Std_ViewCreateInventor" << "Std_OrthographicCamera" << "Std_PerspectiveCamera" << "Separator" << "Std_ToggleVisibility" << "Std_ViewFullScreen" 
        << "Separator" << "Std_Workbench" << "Std_ToolBarMenu" << "Std_DockViewMenu" << "Separator" << "Std_ViewStatusBar";
  
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

// -----------------------------------------------------------------------

PythonWorkbench::PythonWorkbench() : _workbenchPy(0)
{
  _menuBar = StdWorkbench::setupMenuBar();
  _toolBar = StdWorkbench::setupToolBars();
  _commandBar = new ToolBarItem;
}

PythonWorkbench::~PythonWorkbench()
{
  delete _menuBar;
  delete _toolBar;
  delete _commandBar;
  if ( _workbenchPy )
    _workbenchPy->DecRef();
}

Base::PyObjectBase* PythonWorkbench::GetPyObject()
{
  if ( !_workbenchPy )
  {
    _workbenchPy = new PythonWorkbenchPy(this);
  }

  // Increment every time when this object is returned
  _workbenchPy->IncRef();

  return _workbenchPy;
}

MenuItem* PythonWorkbench::setupMenuBar() const
{
  return _menuBar->copy();
}

ToolBarItem* PythonWorkbench::setupToolBars() const
{
  return _toolBar->copy();
  ToolBarItem* root = new ToolBarItem;
  root->setCommand(_toolBar->command());
  QPtrList<ToolBarItem> bars = _toolBar->getItems();

  ToolBarItem* bar;
  for ( bar = bars.first(); bar; bar = bars.next() )
  {
    ToolBarItem* nbar = new ToolBarItem(root);
    nbar->setCommand(bar->command());
    QPtrList<ToolBarItem> items = bar->getItems(); 

    ToolBarItem* item;
    for ( item = items.first(); item; item = items.next() )
    {
      ToolBarItem* nitem = new ToolBarItem(nbar);
      nitem->setCommand(item->command());
    }
  }

  return root;
}

ToolBarItem* PythonWorkbench::setupCommandBars() const
{
  return _commandBar->copy();
  ToolBarItem* root = new ToolBarItem;
  root->setCommand(_commandBar->command());
  QPtrList<ToolBarItem> bars = _commandBar->getItems();

  ToolBarItem* bar;
  for ( bar = bars.first(); bar; bar = bars.next() )
  {
    ToolBarItem* nbar = new ToolBarItem(root);
    nbar->setCommand(bar->command());
    QPtrList<ToolBarItem> items = bar->getItems(); 

    ToolBarItem* item;
    for ( item = items.first(); item; item = items.next() )
    {
      ToolBarItem* nitem = new ToolBarItem(nbar);
      nitem->setCommand(item->command());
    }
  }

  return root;
}

void PythonWorkbench::appendMenu( const QString& menu, const QStringList& items ) const
{
  MenuItem* item = _menuBar->findItem( menu );
  if ( !item )
  {
    Gui::MenuItem* wnd = _menuBar->findItem( "&Windows" );
    item = new MenuItem;
    item->setCommand( menu );
    _menuBar->insertItem( wnd, item );
  }

  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
    *item << *it;
}

void PythonWorkbench::removeMenu( const QString& menu ) const
{
  MenuItem* item = _menuBar->findItem( menu );
  if ( item )
    _menuBar->removeItem( item );
}

QStringList PythonWorkbench::listMenus() const
{
	QStringList menus;
	QPtrList<MenuItem> items = _menuBar->getItems();
	MenuItem* item;
	for ( item=items.first(); item; item=items.next() )
		menus << item->command();
	return menus;
}

void PythonWorkbench::appendContextMenu( const QString& menu, const QStringList& items ) const
{
}

void PythonWorkbench::removeContextMenu( const QString& menu ) const
{
}

void PythonWorkbench::appendToolbar( const QString& bar, const QStringList& items ) const
{
  ToolBarItem* item = _toolBar->findItem( bar );
  if ( !item )
  {
    item = new ToolBarItem(_toolBar);
    item->setCommand(bar);
  }

  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
    *item << *it;
}

void PythonWorkbench::removeToolbar( const QString& bar) const
{
  ToolBarItem* item = _toolBar->findItem( bar );
  if ( item )
    _toolBar->removeItem( item );
}

QStringList PythonWorkbench::listToolbars() const
{
	QStringList bars;
	QPtrList<ToolBarItem> items = _toolBar->getItems();
	ToolBarItem* item;
	for ( item=items.first(); item; item=items.next() )
		bars << item->command();
	return bars;
}

void PythonWorkbench::appendCommandbar( const QString& bar, const QStringList& items ) const
{
  ToolBarItem* item = _commandBar->findItem( bar );
  if ( !item )
  {
    item = new ToolBarItem(_commandBar);
    item->setCommand(bar);
  }

  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
    *item << *it;
}

void PythonWorkbench::removeCommandbar( const QString& bar ) const
{
  ToolBarItem* item = _commandBar->findItem( bar );
  if ( item )
    _commandBar->removeItem( item );
}

QStringList PythonWorkbench::listCommandbars() const
{
	QStringList bars;
	QPtrList<ToolBarItem> items = _commandBar->getItems();
	ToolBarItem* item;
	for ( item=items.first(); item; item=items.next() )
		bars << item->command();
	return bars;
}
