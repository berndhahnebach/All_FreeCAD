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
#ifndef _PreComp_
#endif

#include "DockWindow.h"
#include "MainWindow.h"

using namespace Gui;

DockWindow::DockWindow( Gui::Document* pcDocument, QWidget *parent, const char *name, WFlags f)
  :QWidget( parent,name,f ), BaseView( pcDocument ), _dw( 0L )
{
}

DockWindow::~DockWindow()
{
}

void DockWindow::setCaption ( const QString & s )
{
  if ( _dw )
    _dw->setCaption( s );
  _caption = s;
}

void DockWindow::setDockWindow( QDockWindow* w )
{
  _dw = w;
}

QDockWindow* DockWindow::dockWindow() const
{
  return _dw;
}

void DockWindow::languageChange()
{
  if ( _dw )
    _dw->setCaption( tr( _caption ) );
}

//**************************************************************************
// DockView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


DockView::DockView( Gui::Document* pcDocument,QWidget* parent, const char* name, WFlags f )
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

bool DockView::onHasMsg(const char* pMsg)
{
  return false;
}

// --------------------------------------------------------------------

namespace Gui {
struct DockWindowManagerP
{
  QMap <QString,DockWindow*> _clDocWindows;
};
} // namespace Gui

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
void DockWindowManager::addDockWindow( const QString& name, DockWindow *pcDocWindow, Qt::Dock pos, 
                                       bool stretch, int extWidth, int extHeight )
{
  MainWindow* pApp = getMainWindow();
  d->_clDocWindows[ name ] = pcDocWindow;

  QDockWindow* dw = new QDockWindow(pApp);
  dw->setCloseMode(QDockWindow::Always);
  pcDocWindow->setDockWindow( dw );
  pcDocWindow->setCaption( name );
  pcDocWindow->reparent(dw, QPoint());
  dw->setWidget( pcDocWindow );
  dw->setResizeEnabled(true);

  if ( extWidth > 0 )
    dw->setFixedExtentWidth( extWidth );

  if ( extHeight > 0 )
    dw->setFixedExtentHeight( extHeight );

  switch (pos)
  {
  case Qt::DockTop:
    dw->setHorizontallyStretchable( stretch );
    break;
  case Qt::DockLeft:
    dw->setVerticallyStretchable( stretch );
    pApp->setDockEnabled ( dw, Qt::DockTop, false );
    pApp->setDockEnabled ( dw, Qt::DockBottom, false );
    break;
  case Qt::DockRight:
    dw->setVerticallyStretchable( stretch );
    pApp->setDockEnabled ( dw, Qt::DockTop, false );
    pApp->setDockEnabled ( dw, Qt::DockBottom, false );
    break;
  case Qt::DockBottom:
    dw->setHorizontallyStretchable( stretch );
    pApp->setDockEnabled ( dw, Qt::DockTop, false );
    pApp->setDockEnabled ( dw, Qt::DockLeft, false );
    pApp->setDockEnabled ( dw, Qt::DockRight, false );
    break;
  default:
    dw->setHorizontallyStretchable( stretch );
    dw->setVerticallyStretchable( stretch );
    pApp->setDockEnabled ( dw, Qt::DockTop, false );
    break;
  }
  pApp->addDockWindow( dw, pos );
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
QPtrList<DockWindow> DockWindowManager::getDockWindows()
{
  QPtrList<DockWindow> dockWindows;
  for ( QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.begin(); It!=d->_clDocWindows.end(); ++It)
    dockWindows.append( It.data() );

  return dockWindows;
}

/**
 * Deletes the specified dock window if it exists.
 */
void DockWindowManager::removeDockWindow( const QString& name )
{
  QMap <QString,DockWindow*>::Iterator It = d->_clDocWindows.find( name );
  if( It!=d->_clDocWindows.end() )
  {
    delete It.data();
    d->_clDocWindows.erase(It);
  }
}

#include "moc_DockWindow.cpp"
