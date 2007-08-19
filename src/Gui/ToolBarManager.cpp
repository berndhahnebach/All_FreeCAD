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

#include "ToolBarManager.h"
#include "MainWindow.h"
#include "Application.h"
#include "Command.h"
#include "BitmapFactory.h"

using namespace Gui;

ToolBarItem::ToolBarItem()
{
}

ToolBarItem::ToolBarItem(ToolBarItem* item)
{
    if ( item )
        item->appendItem(this);
}

ToolBarItem::~ToolBarItem()
{
    clear();
}

void ToolBarItem::setCommand(const QString& name)
{
    _name = name;
}

QString ToolBarItem::command() const
{
    return _name;
}

bool ToolBarItem::hasItems() const
{
    return _items.count() > 0;
}

ToolBarItem* ToolBarItem::findItem(const QString& name)
{
    if ( _name == name ) {
        return this;
    } else {
        for ( QList<ToolBarItem*>::ConstIterator it = _items.begin(); it != _items.end(); ++it ) {
            if ( (*it)->_name == name ) {
                return *it;
            }
        }
    }

    return 0;
}

ToolBarItem* ToolBarItem::copy() const
{
    ToolBarItem* root = new ToolBarItem;
    root->setCommand( command() );

    QList<ToolBarItem*> items = getItems();
    for ( QList<ToolBarItem*>::ConstIterator it = items.begin(); it != items.end(); ++it ) {
        root->appendItem( (*it)->copy() );
    }

    return root;
}

uint ToolBarItem::count() const
{
    return _items.count();
}

void ToolBarItem::appendItem(ToolBarItem* item)
{
    _items.push_back( item );
}

bool ToolBarItem::insertItem( ToolBarItem* before, ToolBarItem* item)
{
    int pos = _items.indexOf(before);
    if (pos != -1) {
        _items.insert(pos, item);
        return true;
    } else
        return false;
}

void ToolBarItem::removeItem(ToolBarItem* item)
{
    int pos = _items.indexOf(item);
    if (pos != -1)
        _items.removeAt(pos);
}

void ToolBarItem::clear()
{
    for ( QList<ToolBarItem*>::Iterator it = _items.begin(); it != _items.end(); ++it ) {
        delete *it;
    }

    _items.clear();
}

ToolBarItem& ToolBarItem::operator << (ToolBarItem* item)
{
    appendItem(item);
    return *this;
}

ToolBarItem& ToolBarItem::operator << (const QString& command)
{
    ToolBarItem* item = new ToolBarItem(this);
    item->setCommand(command);
    return *this;
}

QList<ToolBarItem*> ToolBarItem::getItems() const
{
    return _items;
}

// -----------------------------------------------------------

ToolBarManager* ToolBarManager::_instance=0;

ToolBarManager* ToolBarManager::getInstance()
{
    if ( !_instance )
        _instance = new ToolBarManager;
    return _instance;
}

void ToolBarManager::destruct()
{
    delete _instance;
    _instance = 0;
}

ToolBarManager::ToolBarManager()
{
}

ToolBarManager::~ToolBarManager()
{
}

void ToolBarManager::setup(ToolBarItem* toolBar) const
{
    if ( !toolBar )
        return; // empty menu bar

    // switch updates off to avoid flickering
    getMainWindow()->setUpdatesEnabled(false);
    QList<QToolBar*> tbs = toolBars();
    for ( QList<QToolBar*>::Iterator it = tbs.begin(); it != tbs.end(); ++it ) {
        getMainWindow()->removeToolBar( *it );
        delete *it;
    }

    CommandManager& mgr = Application::Instance->commandManager();
    QList<ToolBarItem*> items = toolBar->getItems();

    for ( QList<ToolBarItem*>::ConstIterator item = items.begin(); item != items.end(); ++item ) {
        QByteArray toolbarName = (*item)->command().toUtf8();
        QToolBar* bar = getMainWindow()->addToolBar(QObject::trUtf8((const char*)toolbarName)); // i18n
        bar->setObjectName((*item)->command());

        QList<ToolBarItem*> subitems = (*item)->getItems();
        for ( QList<ToolBarItem*>::ConstIterator subitem = subitems.begin(); subitem != subitems.end(); ++subitem ) {
            if ( (*subitem)->command() == "Separator" )
                bar->addSeparator();
            else
                mgr.addTo((const char*)(*subitem)->command().toAscii(), bar);
        }
    }
  
    // switch on updates
    getMainWindow()->setUpdatesEnabled(true);
}

QList<QToolBar*> ToolBarManager::toolBars() const
{
    QWidget* mw = getMainWindow();
    QList<QToolBar*> tb;
    QList<QToolBar*> bars = getMainWindow()->findChildren<QToolBar*>();
    for (QList<QToolBar*>::ConstIterator it = bars.begin(); it != bars.end(); ++it) {
        if ((*it)->parentWidget() == mw)
            tb.push_back(*it);
    }

    return tb;
}
