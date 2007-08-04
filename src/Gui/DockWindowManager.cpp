/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "DockWindowManager.h"
#include "MainWindow.h"

using namespace Gui;

DockWindowItems::DockWindowItems()
{
}

DockWindowItems::~DockWindowItems()
{
}

void DockWindowItems::addDockWidget(const QString& name, Qt::DockWidgetArea pos)
{
  _items.push_back(qMakePair<QString, Qt::DockWidgetArea>(name, pos));
}

const QList<DockWindowItem>& DockWindowItems::dockWidgets() const
{
  return this->_items;
}

bool DockWindowItems::operator == (const DockWindowItems& that) const
{
  if (this->_items.size() != that._items.size())
    return false;
  QList<QPair<QString, Qt::DockWidgetArea> >::const_iterator it, jt;

  it = this->_items.begin();
  jt = that._items.begin();

  // all items must be equal
  while (it != this->_items.end()) {
    if ((*it).first != (*jt).first || (*it).second != (*jt).second)
      return false;
    ++it;
    ++jt;
  }

  return true;
}

// -----------------------------------------------------------

namespace Gui {
struct DockWindowManagerP
{
  QList<QDockWidget*> _dockedWindows;
  QMap<QString, QPointer<QWidget> > _dockWindows;
  DockWindowItems _dockWindowItems;
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
  for (QMap<QString, QPointer<QWidget> >::Iterator it = d->_dockWindows.begin(); it != d->_dockWindows.end(); ++it) {
    QWidget* dw = it.value();
    if (dw) {
      delete dw;
    }
  }

  d->_dockedWindows.clear();
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
  // Note: By default all dock widgets are hidden but the user can show them manually in the view menu.
  // First, hide immediately the dock widget to avoid flickering, after setting up the dock widgets
  // MainWindow::loadLayoutSettings() is called to restore the layout.
  dw->hide();
  mw->addDockWidget(pos, dw);

  // add the widget to the dock widget
  widget->setParent(dw);
  dw->setWidget(widget);

  // set object name and window title needed for i18n stuff
  dw->setObjectName(name);
  dw->setWindowTitle(QDockWidget::tr(name.toUtf8()));
  dw->setFeatures(QDockWidget::AllDockWidgetFeatures);

  d->_dockedWindows.push_back(dw);
}

/**
 * Returns the widget inside the dock window by name.
 * If it does not exist 0 is returned.
 */
QWidget* DockWindowManager::getDockWindow( const QString& name ) const
{
  for (QList<QDockWidget*>::ConstIterator it = d->_dockedWindows.begin(); it != d->_dockedWindows.end(); ++it) {
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
  for (QList<QDockWidget*>::ConstIterator it = d->_dockedWindows.begin(); it != d->_dockedWindows.end(); ++it)
    docked.push_back((*it)->widget());
  return docked;
}

/**
 * Removes the specified dock window with name \name without deleting it.
 */
QWidget* DockWindowManager::removeDockWindow( const QString& name )
{
  QWidget* widget=0;
  for (QList<QDockWidget*>::Iterator it = d->_dockedWindows.begin(); it != d->_dockedWindows.end(); ++it) {
    if ((*it)->objectName() == name) {
      QDockWidget* dw = *it;
      d->_dockedWindows.erase(it);
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
  for (QList<QDockWidget*>::Iterator it = d->_dockedWindows.begin(); it != d->_dockedWindows.end(); ++it) {
    if ((*it)->widget() == widget) {
      QDockWidget* dw = *it;
      d->_dockedWindows.erase(it);
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
    for (QList<QDockWidget*>::Iterator jt = d->_dockedWindows.begin(); jt != d->_dockedWindows.end(); ++jt) {
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
    for (QList<QDockWidget*>::Iterator jt = d->_dockedWindows.begin(); jt != d->_dockedWindows.end(); ++jt) {
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
  for (QList<QDockWidget*>::Iterator it = d->_dockedWindows.begin(); it != d->_dockedWindows.end(); ++it) {
    (*it)->setWindowTitle(QDockWidget::tr((*it)->objectName().toUtf8()));
  }
}

/**
 * Appends a new \a widget with \a name to the list of available dock widgets. The caller must make sure that
 * the name is unique. If a widget with this name is already registered nothing is done but false is returned,
 * otherwise it is appended and true is returned.
 *
 * As default the following widgets are already registered:
 * \li Std_TreeView
 * \li Std_PropertyView
 * \li Std_ReportView
 * \li Std_ToolBox
 *
 * To avoid name clashes the caller should use names of the form \a module_widgettype, i. e. if a analyse dialog for
 * the mesh module is added the name must then be Mesh_AnalyzeDialog. 
 *
 * To make use of dock windows when a workbench gets loaded the method setupDockWindows() must reimplemented in a 
 * subclass of Gui::Workbench. 
 */
bool DockWindowManager::registerDockWindow(const QString& name, QWidget* widget)
{
  QMap<QString, QPointer<QWidget> >::Iterator it = d->_dockWindows.find(name);
  if (it != d->_dockWindows.end() || !widget)
    return false;
  d->_dockWindows[name] = widget;
  widget->hide(); // hide the widget if not used
  return true;
}

/** Sets up the dock windows of the activated workbench. */
void DockWindowManager::setup( DockWindowItems* items)
{
  // if this configuration is already setup
  if (!items || d->_dockWindowItems == *items) {
    languageChanged();
    return;
  }

  // remove all dock windows
  QList<QDockWidget*> docked = d->_dockedWindows;
  for (QList<QDockWidget*>::Iterator it = docked.begin(); it != docked.end(); ++it) {
    QWidget* w = (*it)->widget();
    removeDockWindow(w);
    w->hide();
  }

  const QList<DockWindowItem>& dw = items->dockWidgets();
  for (QList<DockWindowItem>::ConstIterator it = dw.begin(); it != dw.end(); ++it) {
    QMap<QString, QPointer<QWidget> >::ConstIterator jt = d->_dockWindows.find((*it).first);
    if (jt != d->_dockWindows.end()) {
      addDockWindow(jt.value()->windowTitle(), jt.value(), (*it).second);
      jt.value()->show();
    }
  }

  d->_dockWindowItems = *items;
}
