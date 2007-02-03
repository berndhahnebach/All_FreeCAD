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

#include "MenuManager.h"
#include "Application.h"
#include "MainWindow.h"
#include "Command.h"

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
  clear();
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
    for ( QList<MenuItem*>::Iterator it = _items.begin(); it != _items.end(); ++it )
    {
      if ( (*it)->_name == name )
        return *it;
    }
  }

  return 0;
}

MenuItem* MenuItem::copy() const
{
  MenuItem* root = new MenuItem;
  root->setCommand( command() );

  QList<MenuItem*> items = getItems();
  for ( QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it )
  {
    root->appendItem( (*it)->copy() );
  }

  return root;
}

uint MenuItem::count() const
{
  return _items.count();
}

void MenuItem::appendItem( MenuItem* item )
{
  _items.push_back( item );
}

bool MenuItem::insertItem( MenuItem* before, MenuItem* item)
{
  QList<MenuItem*>::Iterator it = _items.find( before );
  if ( it != _items.end() )
  {
    _items.insert( it, item );
    return true;
  }
  else
    return false;
}

void MenuItem::removeItem( MenuItem* item )
{
  QList<MenuItem*>::Iterator it = _items.find( item );
  if ( it != _items.end() )
    _items.erase( it );
}

void MenuItem::clear()
{
  for ( QList<MenuItem*>::Iterator it = _items.begin(); it != _items.end(); ++it )
    delete *it;
  _items.clear();
}

MenuItem& MenuItem::operator<< ( const QString& command )
{
  MenuItem* item = new MenuItem(this);
  item->setCommand( command );
  return *this;
}

MenuItem& MenuItem::operator<< ( MenuItem* item )
{
  appendItem(item);
  return *this;
}

QList<MenuItem*> MenuItem::getItems() const
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

void MenuManager::destruct()
{
  delete _instance;
  _instance = 0;
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

  // clear menubar
  QMenuBar* bar = getMainWindow()->menuBar();
  bar->clear();

  QList<MenuItem*> items = menuBar->getItems();
  for ( QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it )
  {
    if ( (*it)->command() == "Separator" ) {
      bar->addSeparator();
    } else {
      QMenu* menu = bar->addMenu(QObject::tr((*it)->command()));
      menu->setObjectName((*it)->command());
      setup(*it, menu);
    }
  }
}

void MenuManager::setup( MenuItem* item, QMenu* menu ) const
{
  CommandManager& mgr = Application::Instance->commandManager();

  QList<MenuItem*> items = item->getItems();
  for ( QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it )
  {
    if ( (*it)->hasItems() )
    {
      QMenu* submenu = menu->addMenu(QObject::tr((*it)->command()));
      submenu->setObjectName((*it)->command());
      setup( (*it), submenu );
    }
    else
    {
      if ( (*it)->command() == "Separator" )
        menu->addSeparator();
      else
        mgr.addTo( (*it)->command().latin1(), menu );
    }
  }
}

void MenuManager::setupContextMenu( MenuItem* item, QMenu &menu ) const
{
  setup(item, &menu);
}
