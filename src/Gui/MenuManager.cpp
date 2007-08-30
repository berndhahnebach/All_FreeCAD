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

MenuItem::MenuItem(MenuItem* item)
{
    if (item)
        item->appendItem(this);
}

MenuItem::~MenuItem()
{
    clear();
}

void MenuItem::setCommand(const QString& name)
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

MenuItem* MenuItem::findItem(const QString& name)
{
    if (_name == name)
    {
        return this;
    }
    else
    {
        for (QList<MenuItem*>::Iterator it = _items.begin(); it != _items.end(); ++it)
        {
            if ((*it)->_name == name)
                return *it;
        }
    }

    return 0;
}

MenuItem* MenuItem::copy() const
{
    MenuItem* root = new MenuItem;
    root->setCommand(command());

    QList<MenuItem*> items = getItems();
    for (QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it)
    {
        root->appendItem((*it)->copy());
    }

    return root;
}

uint MenuItem::count() const
{
    return _items.count();
}

void MenuItem::appendItem(MenuItem* item)
{
    _items.push_back(item);
}

bool MenuItem::insertItem(MenuItem* before, MenuItem* item)
{
    int pos = _items.indexOf(before);
    if (pos != -1)
    {
        _items.insert(pos, item);
        return true;
    }
    else
        return false;
}

void MenuItem::removeItem(MenuItem* item)
{
    int pos = _items.indexOf(item);
    if (pos != -1)
        _items.removeAt(pos);
}

void MenuItem::clear()
{
    for (QList<MenuItem*>::Iterator it = _items.begin(); it != _items.end(); ++it)
        delete *it;
    _items.clear();
}

MenuItem& MenuItem::operator << (const QString& command)
{
    MenuItem* item = new MenuItem(this);
    item->setCommand(command);
    return *this;
}

MenuItem& MenuItem::operator << (MenuItem* item)
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

void MenuManager::setup(MenuItem* menuItems) const
{
    if (!menuItems)
        return; // empty menu bar

    QMenuBar* menuBar = getMainWindow()->menuBar();
    //menuBar->setUpdatesEnabled(false);
    
    QList<MenuItem*> items = menuItems->getItems();
    QList<QAction*> actions = menuBar->actions();
    for (QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it)
    {
        // search for the menu action
        QAction* action = findAction(actions, (*it)->command());
        if (!action) {
            // There must be not more than one separator in the menu bar, so
            // we can safely remove it if available and append it at the end
            if ((*it)->command() == "Separator") {
                action = menuBar->addSeparator();
            } else {
                // create a new menu
                QByteArray menuName = (*it)->command().toUtf8();
                QMenu* menu = menuBar->addMenu(QObject::trUtf8((const char*)menuName));
                action = menu->menuAction();
            }

            // set the menu user data
            action->setData((*it)->command());
        } else {
            // put the menu at the ende
            menuBar->removeAction(action);
            menuBar->addAction(action);
            action->setVisible(true);
            int index = actions.indexOf(action);
            actions.removeAt(index);
        }

        // flll up the menu
        if (!action->isSeparator())
            setup(*it, action->menu());
    }

    // hide all menus which we don't need for the moment
    for (QList<QAction*>::Iterator it = actions.begin(); it != actions.end(); ++it) {
        (*it)->setVisible(false);
    }

    // enable update again
    //menuBar->setUpdatesEnabled(true);
}

void MenuManager::setup(MenuItem* item, QMenu* menu) const
{
    CommandManager& mgr = Application::Instance->commandManager();
    QList<MenuItem*> items = item->getItems();
    QList<QAction*> actions = menu->actions();
    for (QList<MenuItem*>::ConstIterator it = items.begin(); it != items.end(); ++it)
    {
        // search for the menu item
        QAction* action = findAction(actions, (*it)->command());
        if (!action) {
            if ((*it)->command() == "Separator") {
                action = menu->addSeparator();
            } else {
                if ((*it)->hasItems()) {
                    // Creste a submenu
                    QByteArray menuName = (*it)->command().toUtf8();
                    QMenu* submenu = menu->addMenu(QObject::trUtf8((const char*)menuName));
                    action = submenu->menuAction();
                } else {
                    // Check if action was added successfully
                    if (mgr.addTo((const char*)(*it)->command().toAscii(), menu))
                        action = menu->actions().last();
                }
            }

            // set the menu user data
            if (action) action->setData((*it)->command());
        } else {
            // put the menu item at the ende
            menu->removeAction(action);
            menu->addAction(action);
            int index = actions.indexOf(action);
            actions.removeAt(index);
        }

        // fill up the submenu
        if ((*it)->hasItems())
            setup(*it, action->menu());
    }

    // remove all menu items which we don't need for the moment
    for (QList<QAction*>::Iterator it = actions.begin(); it != actions.end(); ++it) {
        menu->removeAction(*it);
    }
}

void MenuManager::retranslate() const
{
    QMenuBar* menuBar = getMainWindow()->menuBar();
    QList<QAction*> actions = menuBar->actions();
    for (QList<QAction*>::Iterator it = actions.begin(); it != actions.end(); ++it) {
        if ((*it)->menu())
            retranslate((*it)->menu());
    }
}

void MenuManager::retranslate(QMenu* menu) const
{
    // Custom actions with a sub-menu do the translation themselves and we must not do that
    // here. Such actions have the appropriate command name set as user data.
    // Note: This workaround works as long as a command doesn't match with its menu text
    CommandManager& mgr = Application::Instance->commandManager();
    QByteArray menuName = menu->menuAction()->data().toByteArray();
    if (mgr.getCommandByName(menuName))
        return;
    menu->setTitle(QObject::trUtf8(menuName.constData()));
    QList<QAction*> actions = menu->actions();
    for (QList<QAction*>::Iterator it = actions.begin(); it != actions.end(); ++it) {
        if ((*it)->menu()) {
            retranslate((*it)->menu());
        }
    }
}

QAction* MenuManager::findAction(const QList<QAction*>& acts, const QString& item) const
{
    for (QList<QAction*>::ConstIterator it = acts.begin(); it != acts.end(); ++it) {
        if ((*it)->data().toString() == item)
            return *it;
    }

    return 0; // no item with the user data found
}

void MenuManager::setupContextMenu(MenuItem* item, QMenu &menu) const
{
    setup(item, &menu);
}
