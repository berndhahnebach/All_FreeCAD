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

#ifndef _PreComp_
# include <qstring.h>
#endif

class QPopupMenu;
class QMenuData;

namespace Gui {

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

  void addMenuItem( MenuItem* );
  void removeItem( MenuItem* );

  MenuItem& operator<< ( const QString& command );
  QPtrList<MenuItem> getItems() const;

private:
  QString _name;
  QPtrList<MenuItem> _items;
};

/**
 * @author Werner Mayer
 */
class GuiExport MenuManager
{
public:
  void setup( MenuItem* ) const;
  /// sets up a context menu out of item
  void setupContextMenu( MenuItem* item, QPopupMenu &menu ) const;

  void appendMenuItems( const QString& menu, const QStringList& items );
  void removeMenuItems( const QString& menu, const QStringList& items );
  void appendMenu( const QString& menu );
  void removeMenu( const QString& menu );
  void appendSubmenuItems( const QString& submenu, const QStringList& items );
  void removeSubmenuItems( const QString& submenu, const QStringList& items );
  void appendSubmenu( const QString& menu, const QString& submenu );
  void removeSubmenu( const QString& menu, const QString& submenu );

  static MenuManager& getInstance();
protected:
  MenuManager();
  ~MenuManager();

private:
  QPopupMenu* findSubmenu( QPopupMenu* menu, const QString& submenu, int& );
  QPopupMenu* findMenu( const QString& menu, int&, bool  );
  QPopupMenu* findMenu( QMenuData*, const QString& menu ) const;

  void setup( MenuItem*, QMenuData* ) const;

private:
  static MenuManager* _instance;
};

} // namespace Gui


#endif // MENUMANAGER_H 
