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

DockWindow::DockWindow( Gui::Document* pcDocument, QWidget *parent, const char *name, Qt::WFlags f)
  :QWidget( parent,name,f ), BaseView( pcDocument ), _dw( 0L )
{
}

DockWindow::~DockWindow()
{
}

void DockWindow::setCaption ( const QString & s )
{
  if ( _dw )
    _dw->setCaption( tr(s) );
  _caption = s;
}

QDockWidget* DockWindow::dockWindow() const
{
  return _dw;
}
#if 0
void DockWindow::setFixedExtentWidth(int w)
{
  if (!_dw||!_dw->area())
    return; // not yet attached to a dock area
  Q3DockArea* area = _dw->area();
  QList<Q3DockWindow *> dws = area->dockWindowList();

  int ct=0;
  int sum=0;
  for (QList<Q3DockWindow *>::const_iterator dw = dws.begin(); dw != dws.end(); ++dw)
  {
    if ( (*dw)->isVisible() && (*dw) != _dw )
    {
      ct++;
      sum += (*dw)->width();
    }
  }

  if ( ct == 0 || sum <= w )
    return; // sorry, cannot enlarge other windows

  int lw = (sum - w)/ct;
  for (QList<Q3DockWindow *>::const_iterator dw = dws.begin(); dw != dws.end(); ++dw)
  {
    if ( (*dw)->isVisible() && (*dw) != _dw )
      (*dw)->setFixedExtentWidth( lw );
  }

  _dw->setFixedExtentWidth(w);
  area->QWidget::layout()->invalidate();
  area->QWidget::layout()->activate();
}

void DockWindow::setFixedExtentHeight(int h)
{
  if (!_dw||!_dw->area())
    return; // not yet attached to a dock area
  Q3DockArea* area = _dw->area();
  QList<Q3DockWindow *> dws = area->dockWindowList();

  int ct=0;
  int sum=0;
  for (QList<Q3DockWindow *>::const_iterator dw = dws.begin(); dw != dws.end(); ++dw)
  {
    if ( (*dw)->isVisible() && (*dw) != _dw )
    {
      ct++;
      sum += (*dw)->height();
    }
  }

  if ( ct == 0 || sum <= h )
    return; // sorry, cannot enlarge other windows

  int lh = (sum - h)/ct;
  for (QList<Q3DockWindow *>::const_iterator dw = dws.begin(); dw != dws.end(); ++dw)
  {
    if ( (*dw)->isVisible() && (*dw) != _dw )
      (*dw)->setFixedExtentHeight( lh );
  }

  _dw->setFixedExtentHeight(h);
  area->QWidget::layout()->invalidate();
  area->QWidget::layout()->activate();
}
#endif
void DockWindow::languageChange()
{
  if ( _dw )
    _dw->setCaption( tr( _caption ) );
}

//**************************************************************************
// DockView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


DockView::DockView( Gui::Document* pcDocument,QWidget* parent, const char* name, Qt::WFlags f )
  :DockWindow( pcDocument, parent, name, f)
{
}

DockView::~DockView()
{
}


/// receive a message
bool DockView::onMsg(const char* pMsg)
{
  return false;
}

bool DockView::onHasMsg(const char* pMsg) const
{
  return false;
}

// --------------------------------------------------------------------

DockContainer::DockContainer( QWidget* parent, const char* name, Qt::WFlags fl )
  : DockWindow( 0L, parent, name, fl )
{
  sv = new Q3ScrollView( this );
  sv->setResizePolicy( Q3ScrollView::AutoOneFit );
  sv->setFrameStyle( Q3Frame::NoFrame );
  sv->viewport()->setBackgroundMode( Qt::PaletteBase );
  Q3GridLayout* pGrid = new Q3GridLayout(this);
  pGrid->addWidget(sv, 0, 0);
}

DockContainer::~DockContainer()
{
  delete sv;
}

/**
 * Inserts the widget \a w to the container. To fit into the dockable window area the widget \w
 * should reimplement QWidget::sizeHint() with the minimum required size.
 */
void DockContainer::setDockedWidget( QWidget* w )
{
  sv->addChild( w );
}

// --------------------------------------------------------------------

namespace Gui {
struct DockWindowManagerP
{
  QMap <QString,DockWindow*> _clDocWindows;
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
  d->_clDocWindows.clear();
  delete d;
}

/**
 * Adds a new dock window.
 */
void DockWindowManager::addDockWindow( const QString& name, DockWindow *pcDocWindow, Qt::DockWidgetArea pos )
{
  MainWindow* pApp = getMainWindow();
  d->_clDocWindows[ name ] = pcDocWindow;

  QDockWidget* dw = new QDockWidget(pApp);
  dw->setObjectName(name);
  dw->setFeatures(QDockWidget::AllDockWidgetFeatures);
  pcDocWindow->_dw = dw;
  pcDocWindow->setCaption( name );
  pcDocWindow->reparent(dw, QPoint());
  dw->setWidget( pcDocWindow );

  pApp->addDockWidget( pos, dw );
}

/**
 * Returns the dock window bar by name.
 * If it does not exist it returns 0.
 */
DockWindow* DockWindowManager::getDockWindow( const QString& name )
{
  QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.find( name );

  if (It!=d->_clDocWindows.end())
    return It.data();
  else
    return 0L;
}

/**
 * Returns a vector of all dock windows.
 */
Q3PtrList<DockWindow> DockWindowManager::getDockWindows()
{
  Q3PtrList<DockWindow> dockWindows;
  for ( QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.begin(); It!=d->_clDocWindows.end(); ++It)
    dockWindows.append( It.data() );

  return dockWindows;
}

/**
 * Removes the specified dock window with name \name without deleting it.
 */
void DockWindowManager::removeDockWindow( const QString& name )
{
  QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.find( name );
  if( It!=d->_clDocWindows.end() )
  {
    QDockWidget* dw = It.data()->dockWindow();
    getMainWindow()->removeDockWidget( dw );
    // avoid to destruct the DockWindow object
    dw->removeChild( It.data() );
    d->_clDocWindows.erase(It);
    delete dw; // destruct the QDockWindow
  }
}

/**
 * Method provided for convenience. Does basically the same as the method above unless that
 * it accepts a pointer.
 */
void DockWindowManager::removeDockWindow( DockWindow* dock )
{
  for ( QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.begin(); It != d->_clDocWindows.end(); ++It ) {
    if ( It.data() == dock ) {
      QDockWidget* dw = dock->dockWindow();
      getMainWindow()->removeDockWidget( dw );
      // avoid to destruct the DockWindow object
      dw->removeChild( dock );
      d->_clDocWindows.erase(It);
      delete dw; // destruct the QDockWindow
      break;
    }
  }
}

/**
 * Removes the dock window to the docked widget \a w and destructs it with all its children, especially \a w.
 * @note The widget \a w becomes invalid after the usage of this method.
 */
void DockWindowManager::removeDockedWidget( QWidget* docked )
{
  for ( QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.begin(); It != d->_clDocWindows.end(); ++It ) {
    bool ok = false;
    QObjectList l = It.data()->queryList( docked->className(), docked->name() );
    QObjectList::const_iterator it( l.begin() ); // iterate over the widgets of the same type and name as 'docked'
    QObject *obj;
    while ( *it != 0 ) {
        // for each found object...
        if ( *it == docked ) {
          ok = true;
          obj = *it;
          break;
        }
        ++it;
    }

    // if we have a DockWindow containing 'docked'
    if ( It.data() == docked || ok ) {
      QDockWidget* dw = It.data()->dockWindow();
      getMainWindow()->removeDockWidget( dw );
      d->_clDocWindows.erase(It);
      // destruct the QDockWindow and all its children, especially 'docked'
      delete dw;
      break;
    }
  }
}

/**
 * Forces to show all dock windows with the name specified in the list \a dw.
 */
void DockWindowManager::showDockWindows( const QStringList& dw )
{
  for ( QStringList::ConstIterator it = dw.begin(); it != dw.end(); ++it )
  {
    DockWindow* w = getDockWindow( *it );
    if ( w && w->dockWindow() ) w->dockWindow()->show();
  }
}

/**
 * Forces to hide all dock windows with the name specified in the list \a dw.
 */
void DockWindowManager::hideDockWindows( const QStringList& dw )
{
  for ( QStringList::ConstIterator it = dw.begin(); it != dw.end(); ++it )
  {
    DockWindow* w = getDockWindow( *it );
    if ( w && w->dockWindow() ) w->dockWindow()->hide();
  }
}

#include "moc_DockWindow.cpp"
