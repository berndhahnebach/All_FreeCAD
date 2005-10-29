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
#include "MainWindow.h"
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

MenuItem* MenuItem::copy() const
{
  MenuItem* root = new MenuItem;
  root->setCommand( command() );

  QPtrList<MenuItem> items = getItems();
  MenuItem* item;
  for ( item=items.first(); item; item=items.next() )
  {
    root->appendItem( item->copy() );
  }

  return root;
}

uint MenuItem::count() const
{
  return _items.count();
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

void MenuItem::clear()
{
  MenuItem* item;
  for ( item = _items.first(); item; item = _items.next() )
  {
    delete item;
  }

  _items.clear();
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

  QMenuBar* bar = getMainWindow()->menuBar();
  // Cannot call QMenuBar::clear() to prevent the icon and window buttons from being removed ( e.g if a window is in fullscreen mode )
  bool ok = false;
  do {
    ok = false;
    uint ct = bar->count();
    for ( uint i=0; i<ct; i++ )
    {
      int id = bar->idAt( i );
      QMenuItem* item = bar->findItem( id );
      if ( !item )
        continue;

      QWidget* w = item->widget();
      // remove all popup menus and (empty) separators
      if ( item->popup() || (item->isSeparator() && !w) )
      {
        bar->removeItem( id );
        ok = true;
      }
    }
  } while ( ok );

  QPtrList<MenuItem> items = menuBar->getItems();

  MenuItem* item;
  for ( item = items.first(); item; item = items.next() )
  {
    if ( item->command() == "Separator" )
      bar->insertSeparator();
    else
      setup( item, bar );
  }
}

void MenuManager::setup( MenuItem* item, QMenuData* data ) const
{
  CommandManager& mgr = Application::Instance->commandManager();

  QPopupMenu* menu = findMenu( data, item->command() );
  if ( !menu )
  {
    menu = new CustomPopupMenu( getMainWindow(), item->command() );
    data->insertItem( QObject::tr(item->command()), menu);
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
  CommandManager& mgr = Application::Instance->commandManager();

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

void MenuManager::languageChange() const
{
  QMenuBar* mb = getMainWindow()->menuBar();
  uint ct = mb->count();
  for ( uint i=0; i<ct; i++ )
  {
    int id = mb->idAt( i );
    QMenuItem* item = mb->findItem( id );
    if ( item && item->popup() )
    {
      mb->changeItem( id, MainWindow::tr( item->popup()->name() ) );
    }
  }
}
