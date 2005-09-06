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


#ifndef COMMANDBAR_MANAGER_H
#define COMMANDBAR_MANAGER_H

#ifndef _PreComp_
# include <qstring.h>
# include <qptrlist.h>
#endif

namespace Gui {

class ToolBarItem;


namespace DockWnd 
{
class ToolBox;
}

/**
 * @author Werner Mayer
 */
class GuiExport CommandBarManager
{
public:
  static CommandBarManager* getInstance();
  void setup( ToolBarItem* ) const;
  void setToolBox( DockWnd::ToolBox* );
  QPtrList<QToolBar> commandBars() const;
  QToolBar* getOrCreateCommandBar( const QString& name, bool activate=false );

protected:
  CommandBarManager();
  ~CommandBarManager();

private:
  DockWnd::ToolBox* _toolBox;
  static CommandBarManager* _instance;
};

} // namespace Gui


#endif // COMMANDBAR_MANAGER_H 
