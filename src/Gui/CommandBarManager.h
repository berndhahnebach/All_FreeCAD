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


#ifndef GUI_COMMANDBARMANAGER_H
#define GUI_COMMANDBARMANAGER_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

class Q3ToolBar;

namespace Gui {

class ToolBarItem;


namespace DockWnd 
{
class ToolBox;
}

/**
 * The CommandBarManager class is responsible for the creation of command bars and appending them
 * to the toolbox window.
 * @see ToolBarManager
 * @see MenuManager 
 * @author Werner Mayer
 */
class GuiExport CommandBarManager
{
public:
  /// The one and only instance.
  static CommandBarManager* getInstance();
  static void destruct();
  /** Sets up the command bars of a given workbench. */
  void setup( ToolBarItem* ) const;
  /** Sets up the custom command bars defined by the user of a given workbench. */
  void customSetup( ToolBarItem* ) const;
  void setToolBox( DockWnd::ToolBox* );

protected:
  CommandBarManager();
  ~CommandBarManager();

private:
  DockWnd::ToolBox* _toolBox;
  static CommandBarManager* _instance;
};

} // namespace Gui


#endif // GUI_COMMANDBARMANAGER_H 
