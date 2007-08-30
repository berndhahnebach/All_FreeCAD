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

void ToolBarManager::setup(ToolBarItem* toolBarItems) const
{
    if (!toolBarItems)
        return; // empty menu bar

    QList<ToolBarItem*> items = toolBarItems->getItems();
    QList<QToolBar*> toolbars = toolBars();
    for (QList<ToolBarItem*>::ConstIterator it = items.begin(); it != items.end(); ++it) {
        // search for the toolbar
        QToolBar* toolbar = findToolBar(toolbars, (*it)->command());
        if (!toolbar) {
            QByteArray toolbarName = (*it)->command().toUtf8();
            toolbar = getMainWindow()->addToolBar(QObject::trUtf8((const char*)toolbarName)); // i18n
            toolbar->setObjectName((*it)->command());
        } else {
            int index = toolbars.indexOf(toolbar);
            toolbars.removeAt(index);
        }

        // setup the toolbar
        setup(*it, toolbar);
    }

    // remove and destroy all unneeded toolbars
    for (QList<QToolBar*>::Iterator it = toolbars.begin(); it != toolbars.end(); ++it) {
        getMainWindow()->removeToolBar(*it);
        delete *it;
    }
}

void ToolBarManager::setup(ToolBarItem* item, QToolBar* toolbar) const
{
    CommandManager& mgr = Application::Instance->commandManager();
    QList<ToolBarItem*> items = item->getItems();
    QList<QAction*> actions = toolbar->actions();
    for (QList<ToolBarItem*>::ConstIterator it = items.begin(); it != items.end(); ++it) {
        // search for the action item
        QAction* action = findAction(actions, (*it)->command());
        if (!action) {
            if ((*it)->command() == "Separator") {
                action = toolbar->addSeparator();
            } else {
                // Check if action was added successfully
                if (mgr.addTo((const char*)(*it)->command().toAscii(), toolbar))
                    action = toolbar->actions().last();
            }

            // set the tool button user data
            if (action) action->setData((*it)->command());
        } else {
            // Note: For toolbars we do not remove and readd the actions
            // because this causes flicker effects. So, it could happen that the order of 
            // buttons doesn't match with the order of commands in the workbench.
            int index = actions.indexOf(action);
            actions.removeAt(index);
        }
    }

    // remove all tool buttons which we don't need for the moment
    for (QList<QAction*>::Iterator it = actions.begin(); it != actions.end(); ++it) {
        toolbar->removeAction(*it);
    }
}

void ToolBarManager::retranslate() const
{
    QList<QToolBar*> toolbars = toolBars();
    for (QList<QToolBar*>::Iterator it = toolbars.begin(); it != toolbars.end(); ++it) {
        QByteArray toolbarName = (*it)->objectName().toUtf8();
        (*it)->setWindowTitle(QObject::trUtf8(toolbarName.constData()));
    }
}

QToolBar* ToolBarManager::findToolBar(const QList<QToolBar*>& toolbars, const QString& item) const
{
    for (QList<QToolBar*>::ConstIterator it = toolbars.begin(); it != toolbars.end(); ++it) {
        if ((*it)->objectName() == item)
            return *it;
    }

    return 0; // no item with the user data found
}

QAction* ToolBarManager::findAction(const QList<QAction*>& acts, const QString& item) const
{
    for (QList<QAction*>::ConstIterator it = acts.begin(); it != acts.end(); ++it) {
        if ((*it)->data().toString() == item)
            return *it;
    }

    return 0; // no item with the user data found
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
