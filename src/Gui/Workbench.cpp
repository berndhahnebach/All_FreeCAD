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
# include <qaction.h>
# include <qkeysequence.h>
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
#include "Window.h"
#include "Selection.h"

#include <App/Application.h>
#include <App/DocumentObject.h>

#include <Base/Parameter.h>
#include <Base/Interpreter.h>
#define new DEBUG_CLIENTBLOCK
using namespace Gui;

/** \defgroup workbench Workbench Framework
 *
 * FreeCAD provides the possibility to have one or more workbenches for a module. A workbench changes the appearance of the main window
 * in that way that it defines toolbars, command bars, menus or the context menu (later on dockable windows, ...) that are shown to the user.
 *
 * The idea behind this concept is that the user should see only the functions that are required for the task that he is doing at this
 * moment and not to show dozens of unneeded functions which the user never uses.
 *
 * \section stepbystep Step by step
 * Here follows a short description of how an own workbench can be added to a module.
 *
 * \subsection newClass Inherit either from Workbench or StdWorkbench
 * First you have to subclass either Workbench or StdWorkbench and reimplement the methods setupMenuBar(), setupToolBars() and setupCommandBars().
 * The difference between both classes is that these methods of Workbench are pure virtual while StdWorkbench defines already the standard menus and
 * toolbars, such as the 'File', 'Edit', ..., 'Help' menus with their common functions.
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
 * or
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
 * \subsection customizeWorkbench Customizing the workbench
 * If you want to customize your workbench by adding or removing items you can use the ToolBarItem class for customizing toolbars and the MenuItem class
 * for menus. Both classes behave basically the same.
 * To add a new menu item you can do it as follows
 * \code
 *   MenuItem* setupMenuBar() const
 *   {
 *     MenuItem* root = StdWorkbench::setupMenuBar();
 *     // create a sub menu
 *     MenuItem* mySub = new MenuItem; // note: no parent is given
 *     mySub->setCommand( "My &Submenu" );
 *     *mySub << "Std_Undo" << "Std_Redo";
 *
 *     // My menu
 *     MenuItem* myMenu = new MenuItem( root );
 *     myMenu->setCommand( "&My Menu" );
 *     // fill up the menu with some command items
 *     *myMenu << mySub << "Separator" << "Std_Cut" << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo";
 *   }
 * \endcode
 *
 * Toolbars can be cutomized the same way unless that you shouldn't create subitems (there are no subtoolbars).
 *
 * \subsection regWorkbench Register your workbench
 * Once you have implemented your workbench class you have to register it to make it known to the FreeCAD core system. You must make sure that the step
 * of registration is performed only once. A good place to do it is e.g. in the global function initMODULEGui in AppMODULEGui.cpp where MODULE stands
 * for the name of your module. Just add the line
 * \code
 * MODULEGui::MyWorkbench::init();
 * \endcode
 * somewhere there.
 *
 * \subsection itemWorkbench Create an item for your workbench
 * Though your workbench has been registered now,  at this stage you still cannot invoke it yet. Therefore you must create an item in the list of all visible
 * workbenches. To perform this step you must open your InitGui.py (a Python file) and do some adjustments. The file contains already a Python class
 * MODULEWorkbench that implements the Activate() method (it imports the needed library). You can also implement the GetIcon() method to set your own icon for
 * your workbench, if not, the default FreeCAD icon is taken, and finally the most important method GetClassName(). that represents the link between
 * Python and C++. This method must return the name of the associated C++ including namespace. In this case it must the string "ModuleGui::MyWorkbench".
 * At the end you can change the line from
 * \code
 * Gui.AddWorkbenchHandler("MODULE design",MODULEWorkbench())
 * \endcode
 * to
 * \code
 * Gui.AddWorkbenchHandler("My workbench",MODULEWorkbench())
 * \endcode
 * or whatever you want.
 * \note You must make sure to choose a unique name for your workbench (in this example "My workbench"). Since FreeCAD doesn't provide a mechanism for
 * this you have to care on your own.
 *
 * \section moredetails More details and limitations
 * One of the key concepts of the workbench framework is to load a module at runtime when the user needs some function that it
 * provides. So, if the user doesn't need a module it never gets loaded into RAM. This speeds up the startup procedure of
 * FreeCAD and saves memory.
 * At startup FreeCAD scans all module directories and invokes InitGui.py. So an item for a workbench gets created. If the user
 * clicks on such an item the matching module gets loaded, the C++ workbench gets registered and activated.
 *
 * The user is able to modify a workbench (Edit|Customize). E.g. he can add new toolbars or command bars and add his preferred
 * functions to them. But he has only full control over "his" toolbars, the default workbench items cannot be modified or even removed.
 *
 * At the moment only toolbars, command bars and menus are handled by a workbench. In later versions it is also planned to make dock windows
 * and showing special widgets workbench-dependent.
 *
 * FreeCAD provides also the possibility to define pure Python workbenches. Such workbenches are temporarily only and are lost after exiting
 * the FreeCAD session. But if you want to keep your Pyhton workbench you can write a macro and attach it with a user defined button or just
 * perform the macro during the next FreeCAD session.
 * Here follows a short example of how to create and embed a workbench in Python
 * \code
 * w=FreeCADGui.CreateWorkbench("My workbench")               # creates a standard workbench (the same as StdWorkbench in C++)
 * dir(w)                                                     # lists all available function of the object
 * list = ["Std_Test1", "Std_Test2", "Std_Test3"]             # creates a list of new functions
 * w.AppendMenu("Test functions", list)                       # creates a new menu with these functions
 * w.AppendToolbar("Test", list)                              # ... and also a new toolbar
 * FreeCADGui.AddWorkbenchHandler("My workbench", 0)          # Creates an item for our workbenmch now, the second argument can be 0 as we don't
 *                                                            # need our own workbench handler.
 * \endcode
 */

TYPESYSTEM_SOURCE_ABSTRACT(Gui::Workbench, Base::BaseClass)

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

void Workbench::setupContextMenu(const char* recipient,MenuItem* item) const
{
}

bool Workbench::activate()
{
  // Assigns user defined accelerators
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter();
  if ( hGrp->HasGroup("Shortcut") )
  {
    hGrp = hGrp->GetGroup("Shortcut");
    std::map<std::string,std::string> items = hGrp->GetASCIIMap();

    QMap<QString, CommandBase*> nameCommands;
    const CommandManager& cCmdMgr = Application::Instance->commandManager();
    const std::map<std::string,Command*>& sCommands = cCmdMgr.getCommands();

    for (std::map<std::string,Command*>::const_iterator ci = sCommands.begin(); ci != sCommands.end(); ++ci)
    {
      CommandGroup* cmdGrp = dynamic_cast<CommandGroup*>(ci->second);
      if ( cmdGrp )
      {
        int i=0;
        std::vector<CommandItem*> items = cmdGrp->getItems();
        for ( std::vector<CommandItem*>::const_iterator e = items.begin(); e != items.end(); ++e )
        {
          QString sName = QString("%1_%2").arg(ci->second->getName()).arg(i++);
          nameCommands[sName] = *e;
        }
      }
      else
      {
        nameCommands[ci->second->getName()] = ci->second;
      }
    }

    for ( std::map<std::string, std::string>::iterator it = items.begin(); it != items.end(); ++it )
    {
      CommandBase* cmd = nameCommands[ it->first.c_str() ];
      if ( cmd && cmd->getAction() )
      {
        QString str = it->second.c_str();
        QKeySequence shortcut = str;
        cmd->getAction()->setAccel( shortcut );
      }
    }
  }

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

TYPESYSTEM_SOURCE(Gui::StdWorkbench, Gui::Workbench)

StdWorkbench::StdWorkbench()
  : Workbench()
{
}

StdWorkbench::~StdWorkbench()
{
}

void StdWorkbench::setupContextMenu(const char* recipient,MenuItem* item) const
{
  if ( strcmp(recipient,"View") == 0 )
  {
    if ( Gui::Selection().countObjectsOfType(App::DocumentObject::getClassTypeId()) > 0 )
    	*item << "Separator" << "Std_SetMaterial" << "Std_ToggleVisibility" << "Std_RandomColor" << "Separator" << "Std_Delete";
  }
}

MenuItem* StdWorkbench::setupMenuBar() const
{
  static const char* menuItems[] = {
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&File" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&Edit" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&View" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&Tools" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&Windows" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&Help" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&3D View" ),
    QT_TRANSLATE_NOOP( "Gui::MenuManager", "&Zoom" )
  };

  // Setup the default menu bar
  MenuItem* menuBar = new MenuItem;

  // File
  MenuItem* file = new MenuItem( menuBar );
  file->setCommand( menuItems[0] );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_SaveAs" << "Std_ProjectInfo" << "Separator" << "Std_Print"
        << "Separator" << "Std_MRU" << "Separator" << "Std_Quit";

  // Edit
  MenuItem* edit = new MenuItem( menuBar );
  edit->setCommand( menuItems[1] );
  *edit << "Std_Undo" << "Std_Redo" << "Separator" << "Std_Cut" << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Refresh" << "Std_Delete"
        << "Separator" << "Std_DlgPreferences";

  // Standard views
  MenuItem* stdviews = new MenuItem;
  stdviews->setCommand( "Standard views" );
  *stdviews << "Separator" << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
            << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";

  // stereo
  MenuItem* view3d = new MenuItem;
  view3d->setCommand( menuItems[6] );
  *view3d << "Std_ViewIvStereoRedGreen" << "Std_ViewIvStereoQuadBuff" << "Std_ViewIvStereoInterleavedRows" << "Std_ViewIvStereoInterleavedColumns" << "Std_ViewIvStereoOff" << "Separator" << "Std_ViewExample1" << "Std_ViewExample2" << "Std_ViewExample3";

  // zoom
  MenuItem* zoom = new MenuItem;
  zoom->setCommand( menuItems[7] );
  *zoom << "Std_ViewZoomIn" << "Std_ViewZoomOut" << "Separator" << "Std_ViewBoxZoom";

  // View
  MenuItem* view = new MenuItem( menuBar );
  view->setCommand( menuItems[2] );
  *view << "Std_ViewCreate" << "Std_CameraType" << "Separator" << stdviews << "Std_FreezeViews" << "Separator" << view3d << zoom
        << "Std_ToggleVisibility" << "Std_ViewDockUndockFullscreen" << "Std_ToggleClipPlane" << "Separator"
        << "Std_Workbench" << "Std_ToolBarMenu" << "Std_DockViewMenu" << "Separator" << "Std_ViewStatusBar";

  // Tools
  MenuItem* tool = new MenuItem( menuBar );
  tool->setCommand( menuItems[3] );
  *tool << "Std_CommandLine" << "Std_DlgParameter" << "Separator" << "Std_DlgMacroRecord"
        << "Std_DlgMacroStop" << "Std_DlgMacroExecute" << "Std_DlgMacroExecuteDirect" << "Separator" << "Std_ViewScreenShot" << "Separator" << "Std_DlgCustomize";

  // Windows
  MenuItem* wnd = new MenuItem( menuBar );
  wnd->setCommand( menuItems[4] );
  *wnd << "Std_CloseActiveWindow" << "Std_CloseAllWindows" << "Separator" << "Std_ActivateNextWindow"
       << "Std_ActivatePrevWindow" << "Separator" << "Std_TilePragmatic" << "Std_TileHoricontal"
       << "Std_TileVertical" << "Std_WindowsMenu" << "Separator" << "Std_Windows";

  // Separator
  MenuItem* sep = new MenuItem( menuBar );
  sep->setCommand( "Separator" );

  // Help
  MenuItem* help = new MenuItem( menuBar );
  help->setCommand( menuItems[5] );
  *help << "Std_OnlineHelp" << "Std_TipOfTheDay" << "Separator" << "Std_About" << "Std_AboutQt"
        << "Separator" << "Std_WhatsThis" << "Std_DescriptionMode";

  return menuBar;
}

ToolBarItem* StdWorkbench::setupToolBars() const
{
  static const char* toolItems[] = {
    QT_TRANSLATE_NOOP( "Gui::CustomToolBar", "file operations" ),
    QT_TRANSLATE_NOOP( "Gui::CustomToolBar", "Macro recording" ),
    QT_TRANSLATE_NOOP( "Gui::CustomToolBar", "Standard views" )
  };

  ToolBarItem* root = new ToolBarItem;

  // File
  ToolBarItem* file = new ToolBarItem( root );
  file->setCommand( toolItems[0] );
  *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_Print" << "Separator" << "Std_Cut"
        << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo" << "Separator"
        << "Std_Refresh" << "Separator" << "Std_Workbench" << "Std_WhatsThis";

  // Macro
  ToolBarItem* macro = new ToolBarItem( root );
  macro->setCommand( toolItems[1] );
  *macro << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute"<< "Std_DlgMacroExecuteDirect";

  // View
  ToolBarItem* view = new ToolBarItem( root );
  view->setCommand( toolItems[2] );
  *view << "Separator" << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
        << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";

  return root;
}

ToolBarItem* StdWorkbench::setupCommandBars() const
{
  static const char* toolItems[] = {
    QT_TRANSLATE_NOOP( "Gui::CustomToolBar", "Standard views" ),
    QT_TRANSLATE_NOOP( "Gui::CustomToolBar", "Special Ops" )
  };

  ToolBarItem* root = new ToolBarItem;

  // View
  ToolBarItem* view = new ToolBarItem( root );
  view->setCommand( toolItems[0] );
  *view << "Std_ViewFitAll" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewRight"
        << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";
  // Special Ops
  ToolBarItem* macro = new ToolBarItem( root );
  macro->setCommand( toolItems[1] );
  *macro << "Std_DlgParameter" << "Std_DlgPreferences" << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute"
         << "Std_DlgCustomize" << "Std_CommandLine";


  return root;
}

// --------------------------------------------------------------------

TYPESYSTEM_SOURCE(Gui::TestWorkbench, Gui::Workbench)

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

TYPESYSTEM_SOURCE(Gui::PythonWorkbench, Gui::Workbench)

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
  {
    _workbenchPy->setInvalid();
    _workbenchPy->DecRef();
  }
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

void PythonWorkbench::appendMenu( const QStringList& menu, const QStringList& items ) const
{
  if ( menu.empty() || items.empty() )
    return;

  QStringList::ConstIterator jt=menu.begin();
  MenuItem* item = _menuBar->findItem( *jt );
  if ( !item )
  {
    Gui::MenuItem* wnd = _menuBar->findItem( "&Windows" );
    item = new MenuItem;
    item->setCommand( *jt );
    _menuBar->insertItem( wnd, item );
  }

  // create sub menus
  for ( jt++; jt != menu.end(); jt++ )
  {
    MenuItem* subitem = item->findItem( *jt );
    if ( !subitem )
    {
      subitem = new MenuItem(item);
      subitem->setCommand( *jt );
    }
    item = subitem;
  }

  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
    *item << *it;
}

void PythonWorkbench::removeMenu( const QString& menu ) const
{
  MenuItem* item = _menuBar->findItem( menu );
  if ( item ) {
    _menuBar->removeItem( item );
    delete item;
  }
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
  if ( item ) {
    _toolBar->removeItem( item );
    delete item;
  }
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
  if ( item ) {
    _commandBar->removeItem( item );
    delete item;
  }
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
