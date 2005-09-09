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


#ifndef TOOLBAR_MANAGER_H
#define TOOLBAR_MANAGER_H

#ifndef _PreComp_
# include <qstring.h>
#endif

namespace Gui {

class GuiExport ToolBarItem
{
public:
  ToolBarItem();
  ToolBarItem( ToolBarItem* );
  ~ToolBarItem();

  void setCommand( const QString& );
  QString command() const;

  bool hasItems() const;
  ToolBarItem* findItem( const QString& );

  void appendItem( const ToolBarItem* );
  bool insertItem( const ToolBarItem*, const ToolBarItem* );
  void removeItem( const ToolBarItem* );

  ToolBarItem& operator<< ( const ToolBarItem* item );
  ToolBarItem& operator<< ( const QString& command );
  QPtrList<ToolBarItem> getItems() const;

private:
  QString _name;
  QPtrList<ToolBarItem> _items;
};

/**
 * @author Werner Mayer
 */
class GuiExport ToolBarManager
{
public:
  static ToolBarManager* getInstance();
  void setup( ToolBarItem* ) const;
  void customSetup( ToolBarItem* ) const;
  QPtrList<QToolBar> toolBars() const;
  QToolBar* getOrCreateToolBar( const QString& name, bool modify=false ) const;

protected:
  ToolBarManager();
  ~ToolBarManager();

private:
  static ToolBarManager* _instance;
};

} // namespace Gui


#endif // TOOLBAR_MANAGER_H 
