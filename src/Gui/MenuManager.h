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


#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <qstring.h>

class QPopupMenu;
class QMenuData;

namespace Gui {

class MainWindow;

class GuiExport MenuItem
{
public:
  MenuItem();
  MenuItem( MenuItem* );
  ~MenuItem();

  void setCommand( const QString& );
  QString command() const;

  bool hasItems() const;
  MenuItem* findItem( const QString& );
  MenuItem* copy() const;
  uint count() const;

  void appendItem( const MenuItem* );
  bool insertItem( const MenuItem*, const MenuItem* );
  void removeItem( const MenuItem* );
  void clear();

  MenuItem& operator<< ( const MenuItem* item );
  MenuItem& operator<< ( const QString& command );
  QPtrList<MenuItem> getItems() const;

private:
  QString _name;
  QPtrList<MenuItem> _items;
};

/**
 * The MenuManager class is responsible for the creation of menus and context menus and appending them
 * to the main window.
 * @see CommandBarManager
 * @see ToolBarManager 
 * @author Werner Mayer
 */
class GuiExport MenuManager
{
public:
  /** Sets up the menus of a given workbench. */
  void setup( MenuItem* ) const;
  /// sets up a context menu out of item
  void setupContextMenu( MenuItem* item, QPopupMenu &menu ) const;

  /// The one and only instance.
  static MenuManager* getInstance();

protected:
  MenuManager();
  ~MenuManager();
  void languageChange() const;

private:
  QPopupMenu* findMenu( QMenuData*, const QString& menu ) const;
  void setup( MenuItem*, QMenuData* ) const;

private:
  static MenuManager* _instance;

  friend class MainWindow;
};

} // namespace Gui


#endif // MENUMANAGER_H 
