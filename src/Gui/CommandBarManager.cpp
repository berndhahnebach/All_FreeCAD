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

#include "CommandBarManager.h"
#include "ToolBarManager.h"
#include "Application.h"
#include "Command.h"
#include "ToolBox.h"

using namespace Gui;
using DockWnd::ToolBox;

CommandBarManager* CommandBarManager::_instance=0;

CommandBarManager* CommandBarManager::getInstance()
{
  if ( !_instance )
    _instance = new CommandBarManager;
  return _instance;
}

void CommandBarManager::destruct()
{
  delete _instance;
  _instance = 0;
}

CommandBarManager::CommandBarManager() : _toolBox(0L)
{
}

CommandBarManager::~CommandBarManager()
{
}

void CommandBarManager::setToolBox( DockWnd::ToolBox* tb )
{
  _toolBox = tb;
}

void CommandBarManager::setup( ToolBarItem* toolBar ) const
{
  if ( !toolBar || !_toolBox )
    return; // empty tool bar

  int ct = _toolBox->count();
  for ( int i=0; i<ct; i++ )
  {
    // get always the first item widget
    QWidget* w = _toolBox->widget(0);
    _toolBox->removeItem(0);
    delete w;
  }

  CommandManager& mgr = Application::Instance->commandManager();
  QList<ToolBarItem*> items = toolBar->getItems();

  for ( QList<ToolBarItem*>::ConstIterator item = items.begin(); item != items.end(); ++item )
  {
    QToolBar* bar = new QToolBar();
    bar->setOrientation(Qt::Vertical);
    bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    bar->setWindowTitle( QObject::tr( (*item)->command() ) ); // i18n
    _toolBox->addItem( bar, bar->windowTitle() );

    QList<ToolBarItem*> subitems = (*item)->getItems();
    for ( QList<ToolBarItem*>::ConstIterator subitem = subitems.begin(); subitem != subitems.end(); ++subitem )
    {
      if ( (*subitem)->command() == "Separator" ) {
        //bar->addSeparator();
      } else {
        mgr.addTo( (*subitem)->command().latin1(), bar );
      }
    }

    // Now set the right size policy for each tool button
    QList<QToolButton*> tool = bar->findChildren<QToolButton*>();
    for (QList<QToolButton*>::Iterator it = tool.begin(); it != tool.end(); ++it) {
      (*it)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      // When setting the horizontal size policy but no icon is set we use the following trick
      // to make the button text left aligned.
      QIcon icon = (*it)->icon();
      if (icon.isNull())
      {
        // Create an icon filled with the button color
        int size = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
        QPixmap p(size, size);
        p.fill(Qt::transparent);
        (*it)->setIcon(p);
      }
    }
  }
}

void CommandBarManager::customSetup( ToolBarItem* toolBar ) const
{return;
  if ( !toolBar || !_toolBox )
    return; // empty menu bar

  int ct = _toolBox->count();
  for ( int i=0; i<ct; i++ )
  {
    // get always the first item widget
    QWidget* w = _toolBox->widget(0);
    _toolBox->removeItem(0);
    delete w;
  }

  CommandManager& mgr = Application::Instance->commandManager();
  QList<ToolBarItem*> items = toolBar->getItems();

  for ( QList<ToolBarItem*>::ConstIterator item = items.begin(); item != items.end(); ++item )
  {
    QToolBar* bar = new QToolBar();
    bar->setOrientation(Qt::Vertical);
    bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    bar->setWindowTitle( QObject::tr( (*item)->command() ) ); // i18n
    _toolBox->addItem( bar, bar->windowTitle() );

    QList<ToolBarItem*> subitems = (*item)->getItems();
    for ( QList<ToolBarItem*>::ConstIterator subitem = subitems.begin(); subitem != subitems.end(); ++subitem )
    {
      if ( (*subitem)->command() == "Separator" )
        bar->addSeparator();
      else
        mgr.addTo( (*subitem)->command().latin1(), bar );
    }
  }
}
