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
# include <qmenubar.h>
# include <qpopupmenu.h>
#endif

#include "MenuManager.h"
#include "Application.h"
#include "Command.h"
#include "CustomWidgets.h"

using namespace Gui;


MenuItem::MenuItem()
{
}

MenuItem::MenuItem( MenuItem* item )
{
  if ( item )
    item->appendItem( this );
}

MenuItem::~MenuItem()
{
  MenuItem* item;
  for ( item = _items.first(); item; item = _items.next() )
  {
    delete item;
  }

  _items.clear();
}

void MenuItem::setCommand( const QString& name )
{
  _name = name;
}

QString MenuItem::command() const
{
  return _name;
}

bool MenuItem::hasItems() const
{
  return _items.count() > 0;
}

MenuItem* MenuItem::findItem( const QString& name )
{
  if ( _name == name )
  {
    return this;
  }
  else
  {
    MenuItem* item;
    for ( item = _items.first(); item; item = _items.next() )
    {
      if ( item->_name == name )
        return item;
    }
  }

  return 0;
}

void MenuItem::appendItem( const MenuItem* item )
{
  _items.append( item );
}

bool MenuItem::insertItem( const MenuItem* before, const MenuItem* item)
{
  int pos = _items.find( before );
  if ( pos > -1 )
  {
    return _items.insert( pos, item );
  }
  else
    return false;
}

void MenuItem::removeItem( const MenuItem* item )
{
  _items.remove( item );
}

MenuItem& MenuItem::operator<< ( const QString& command )
{
  MenuItem* item = new MenuItem(this);
  item->setCommand( command );
  return *this;
}

MenuItem& MenuItem::operator<< ( const MenuItem* item )
{
  appendItem(item);
  return *this;
}


QPtrList<MenuItem> MenuItem::getItems() const
{
  return _items;
}

// -----------------------------------------------------------

MenuManager* MenuManager::_instance=0;

MenuManager* MenuManager::getInstance()
{
  if ( !_instance )
    _instance = new MenuManager;
  return _instance;
}

MenuManager::MenuManager()
{
}

MenuManager::~MenuManager()
{
}

void MenuManager::setup( MenuItem* menuBar ) const
{
  if ( !menuBar )
    return; // empty menu bar

  CommandManager& mgr = ApplicationWindow::Instance->commandManager();
  QMenuBar* bar = ApplicationWindow::Instance->menuBar();
  bar->clear();
  QPtrList<MenuItem> items = menuBar->getItems();

  MenuItem* item;
  for ( item = items.first(); item; item = items.next() )
  {
    if ( item->command() == "Separator" )
      bar->insertSeparator();
    else
      setup( item, bar );
  }

  languageChange();
}

void MenuManager::setup( MenuItem* item, QMenuData* data ) const
{
  CommandManager& mgr = ApplicationWindow::Instance->commandManager();

  QPopupMenu* menu = findMenu( data, item->command() );
  if ( !menu )
  {
    menu = new CustomPopupMenu( ApplicationWindow::Instance, item->command() );
    int id = data->insertItem(item->command(), menu);
  }
  else
  {
    menu->clear();
  }

  QPtrList<MenuItem> items = item->getItems();
  MenuItem* subitem;
  for ( subitem = items.first(); subitem; subitem = items.next() )
  {
    if ( subitem->hasItems() )
    {
      setup( subitem, menu );
    }
    else
    {
      if ( subitem->command() == "Separator" )
        menu->insertSeparator();
      else
        mgr.addTo( subitem->command().latin1(), menu );
    }
  }
}

void MenuManager::setupContextMenu( MenuItem* item, QPopupMenu &menu ) const
{
  CommandManager& mgr = ApplicationWindow::Instance->commandManager();

  QPtrList<MenuItem> items = item->getItems();
  MenuItem* subitem;
  for ( subitem = items.first(); subitem; subitem = items.next() )
  {
    if ( subitem->hasItems() )
    {
      setup( subitem, &menu );
    }
    else
    {
      if ( subitem->command() == "Separator" )
        menu.insertSeparator();
      else
        mgr.addTo( subitem->command().latin1(), &menu );
    }
  }
}


QPopupMenu* MenuManager::findMenu( QMenuData* menu, const QString& name ) const
{
  uint cnt = menu->count();
  for ( uint i=0; i<cnt; i++ )
  {
    int id = menu->idAt( i );
    QMenuItem* item = menu->findItem( id );
    if ( item && item->popup() )
    {
      if ( menu->text( id ) == name )
      {
        return item->popup();
      }
    }
  }

  return 0;
}

void MenuManager::appendMenuItems( const QString& menu, const QStringList& items )
{
  int id;
  QPopupMenu* popup = findMenu( menu, id, false );

  if ( popup )
  {
    CommandManager& mgr = ApplicationWindow::Instance->commandManager();
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
      mgr.addTo( (*it).latin1(), popup );
  }
}

void MenuManager::removeMenuItems( const QString& menu, const QStringList& items )
{
  int id;
  QPopupMenu* popup = findMenu( menu, id, false );

  if ( popup )
  {
    CommandManager& mgr = ApplicationWindow::Instance->commandManager();
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
      mgr.removeFrom( (*it).latin1(), popup );
  }
}

void MenuManager::appendMenu( const QString& menu )
{
  QPopupMenu *popup = new QPopupMenu( ApplicationWindow::Instance, menu );
  ApplicationWindow::Instance->menuBar()->insertItem( menu, popup );
}

void MenuManager::removeMenu( const QString& menu )
{
  QMenuBar* bar = ApplicationWindow::Instance->menuBar();
  uint cnt = bar->count();
  for ( uint i=0; i<cnt; i++ )
  {
    int id = bar->idAt( i );
    if ( bar->text( id ) == menu )
    {
      ApplicationWindow::Instance->menuBar()->removeItem( id );
      break;
    }
  }
}

void MenuManager::appendSubmenuItems( const QString& submenu, const QStringList& items )
{
  int id;
  QPopupMenu* popup = findMenu( submenu, id, true );

  if ( popup )
  {
    CommandManager& mgr = ApplicationWindow::Instance->commandManager();
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
      mgr.addTo( (*it).latin1(), popup );
  }
}

void MenuManager::removeSubmenuItems( const QString& submenu, const QStringList& items )
{
  int id;
  QPopupMenu* popup = findMenu( submenu, id, true );

  if ( popup )
  {
    CommandManager& mgr = ApplicationWindow::Instance->commandManager();
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
      mgr.removeFrom( (*it).latin1(), popup );
  }
}

void MenuManager::appendSubmenu( const QString& menu, const QString& submenu )
{
  int id;
  QPopupMenu* popup = findMenu( menu, id, true );

  if ( popup )
  {
    QPopupMenu* subpopup = new QPopupMenu( ApplicationWindow::Instance, submenu );
    popup->insertItem( submenu, subpopup );
  }
}

void MenuManager::removeSubmenu( const QString& menu, const QString& submenu )
{
  int id;
  QPopupMenu* popup = findMenu( menu, id, true );

  if ( popup )
  {
    popup->removeItem( id );
  }
}

QPopupMenu* MenuManager::findMenu( const QString& menu, int& index, bool sub )
{
  QMenuBar* bar = ApplicationWindow::Instance->menuBar();
  uint cnt = bar->count();

  // search in the menubar first
  for ( uint i=0; i<cnt; i++ )
  {
    int id = bar->idAt( i );
    QMenuItem* item = bar->findItem( id );
    if ( item && item->popup() )
    {
      if ( bar->text( id ) == menu )
      {
        index = id;
        return item->popup();
      }
    }
  }

  if ( sub )
  {
    // now search recursively in all top-level menus 
    for ( uint j=0; j<cnt; j++ )
    {
      int id = bar->idAt( j );
      QMenuItem* item = bar->findItem( id );
      if ( item && item->popup() )
      {
        // search for 'menu' in popupmenu
        QPopupMenu* popup = findSubmenu( item->popup(), menu, index );
        if ( popup )
        {
          return popup;
        }
      }
    }
  }

  return 0; // sorry, nothing found
}

QPopupMenu* MenuManager::findSubmenu( QPopupMenu* menu, const QString& submenu, int& index )
{
  uint cnt = menu->count();
  for ( uint i=0; i<cnt; i++ )
  {
    int id = menu->idAt( i );
    QMenuItem* item = menu->findItem( id );
    if ( item && item->popup() )
    {
      if ( menu->text( id ) == submenu )
      {
        index = id;
        return item->popup();
      }
      else
      {
        // recursion
        return findSubmenu( item->popup(), submenu, index );
      }
    }
  }

  return 0;
}

void MenuManager::languageChange() const
{
//  QMenuBar* mb = ApplicationWindow::Instance->menuBar();
//  uint cnt = mb->count();
//  for ( uint i=0; i<cnt; i++ )
//  {
//    int id = mb->idAt( i );
//    QMap<int, QString>::ConstIterator it = _menuBar.find( id );
//    if ( it != _menuBar.end() )
//      mb->changeItem( id, ApplicationWindow::tr( it.data() ) );
//  }
  QMenuBar* mb = ApplicationWindow::Instance->menuBar();
  uint ct = mb->count();
  for ( uint i=0; i<ct; i++ )
  {
    int id = mb->idAt( i );
    QMenuItem* item = mb->findItem( id );
    if ( item && item->popup() )
    {
      mb->changeItem( id, ApplicationWindow::tr( item->popup()->name() ) );
    }
  }
}
