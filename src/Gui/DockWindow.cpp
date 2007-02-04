/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "DockWindow.h"
#include "MainWindow.h"

using namespace Gui;

DockWindow::DockWindow( Gui::Document* pcDocument, QWidget *parent)
  : QWidget( parent ), BaseView( pcDocument )
{
}

DockWindow::~DockWindow()
{
}

// --------------------------------------------------------------------

namespace Gui {
struct DockWindowManagerP
{
  QList<QDockWidget*> _clDockWindows;
};
} // namespace Gui

DockWindowManager* DockWindowManager::_instance = 0;

DockWindowManager* DockWindowManager::instance()
{
  if ( _instance == 0 )
    _instance = new DockWindowManager;
  return _instance;
}

void DockWindowManager::destruct()
{
  delete _instance;
  _instance = 0;
}

DockWindowManager::DockWindowManager()
{
  d = new DockWindowManagerP;
}

DockWindowManager::~DockWindowManager()
{
  d->_clDockWindows.clear();
  delete d;
}

/**
 * Adds a new dock window to the main window and embeds the given \a widget.
 */
void DockWindowManager::addDockWindow( const QString& name, QWidget* widget, Qt::DockWidgetArea pos )
{
  // creates the dock widget as container to embed this widget
  MainWindow* mw = getMainWindow();
  QDockWidget* dw = new QDockWidget(mw);

  // set object name and window title needed for i18n stuff
  dw->setObjectName(name);
  dw->setWindowTitle(DockWindow::tr(name));
  dw->setFeatures(QDockWidget::AllDockWidgetFeatures);

  // add the dock widget to the main window
  widget->setParent(dw);
  dw->setWidget(widget);
  mw->addDockWidget(pos, dw);
  d->_clDockWindows.push_back(dw);
}

/**
 * Returns the widget inside the dock window by name.
 * If it does not exist 0 is returned.
 */
QWidget* DockWindowManager::getDockWindow( const QString& name ) const
{
  for (QList<QDockWidget*>::ConstIterator it = d->_clDockWindows.begin(); it != d->_clDockWindows.end(); ++it) {
    if ((*it)->objectName() == name)
      return (*it)->widget();
  }

  return 0;
}

/**
 * Returns a list of all widgets inside the dock windows.
 */
QList<QWidget*> DockWindowManager::getDockWindows() const
{
  QList<QWidget*> docked;
  for (QList<QDockWidget*>::ConstIterator it = d->_clDockWindows.begin(); it != d->_clDockWindows.end(); ++it)
    docked.push_back((*it)->widget());
  return docked;
}

/**
 * Removes the specified dock window with name \name without deleting it.
 */
QWidget* DockWindowManager::removeDockWindow( const QString& name )
{
  QWidget* widget=0;
  for (QList<QDockWidget*>::Iterator it = d->_clDockWindows.begin(); it != d->_clDockWindows.end(); ++it) {
    if ((*it)->objectName() == name) {
      QDockWidget* dw = *it;
      d->_clDockWindows.erase(it);
      getMainWindow()->removeDockWidget(dw);
      // avoid to destruct the embedded widget
      widget = dw->widget();
      widget->setParent(0);
      dw->setWidget(0);
      delete dw; // destruct the QDockWidget, i.e. the parent of the widget
      break;
    }
  }

  return widget;
}

/**
 * Method provided for convenience. Does basically the same as the method above unless that
 * it accepts a pointer.
 */
void DockWindowManager::removeDockWindow( QWidget* widget )
{
  for (QList<QDockWidget*>::Iterator it = d->_clDockWindows.begin(); it != d->_clDockWindows.end(); ++it) {
    if ((*it)->widget() == widget) {
      QDockWidget* dw = *it;
      d->_clDockWindows.erase(it);
      getMainWindow()->removeDockWidget(dw);
      // avoid to destruct the embedded widget
      widget->setParent(0);
      dw->setWidget(0);
      delete dw; // destruct the QDockWidget, i.e. the parent of the widget
      break;
    }
  }
}

/**
 * Forces to show all dock windows with the name specified in the list \a dw.
 */
void DockWindowManager::showDockWindows( const QStringList& dw )
{
  for (QStringList::ConstIterator it = dw.begin(); it != dw.end(); ++it) {
    for (QList<QDockWidget*>::Iterator jt = d->_clDockWindows.begin(); jt != d->_clDockWindows.end(); ++jt) {
      if ((*jt)->objectName() == *it) {
        (*jt)->show();
        break;
      }
    }
  }
}

/**
 * Forces to hide all dock windows with the name specified in the list \a dw.
 */
void DockWindowManager::hideDockWindows( const QStringList& dw )
{
  for (QStringList::ConstIterator it = dw.begin(); it != dw.end(); ++it) {
    for (QList<QDockWidget*>::Iterator jt = d->_clDockWindows.begin(); jt != d->_clDockWindows.end(); ++jt) {
      if ((*jt)->objectName() == *it) {
        (*jt)->hide();
        break;
      }
    }
  }
}

/**
 * Sets the window title for the dockable windows.
 */
void DockWindowManager::languageChanged()
{
  for (QList<QDockWidget*>::Iterator it = d->_clDockWindows.begin(); it != d->_clDockWindows.end(); ++it) {
    (*it)->setWindowTitle(QObject::tr((*it)->objectName()));
  }
}

#include "moc_DockWindow.cpp"
