/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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

#include "Window.h"
#include "Application.h"

#include "../Base/Console.h"
#include "../App/Application.h"

using namespace Gui;

//**************************************************************************
//**************************************************************************
// WindowParameter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

WindowParameter::WindowParameter(const char *name)
{
  FCHandle<FCParameterGrp> h;

  // not allowed to use a Window without a name, see the constructor of a DockWindow or a other QT Widget
  assert(name);
  //printf("Instanceate:%s\n",name);

  // geting the group for the window
  h = GetApplication().GetUserParameter().GetGroup("BaseApp");
  h = h->GetGroup("Preferences");
  //h = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/");

  _handle = h->GetGroup(name);



}

WindowParameter::~WindowParameter()
{

}

void WindowParameter::OnParameterChanged(void)
{
  Base::Console().Warning("WindowParameter::OnParameterChanged(): Parameter has changed and window (%s) has not overriden this function!",_handle->GetGroupName());
}


FCParameterGrp::handle  WindowParameter::GetWindowParameter(void)
{
  return _handle;
}

FCParameterGrp::handle  WindowParameter::GetParameter(void)
{
  return GetApplication().GetUserParameter().GetGroup("BaseApp");
}

ApplicationWindow* WindowParameter::GetAppWnd(void)
{
  return ApplicationWindow::Instance;
}

//**************************************************************************
//**************************************************************************
// FCWindow
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

DockWindow::DockWindow( FCGuiDocument* pcDocument, QWidget *parent, const char *name, WFlags f)
  :QWidget( parent,name,f ), FCBaseView( pcDocument )
{
}

DockWindow::~DockWindow()
{
}

//**************************************************************************
// DockView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


DockView::DockView( FCGuiDocument* pcDocument,QWidget* parent, const char* name, WFlags f )
	:DockWindow( pcDocument, parent, name, f)
{
}

DockView::~DockView()
{
}


/// receive a message
bool DockView::OnMsg(const char* pMsg)
{
	return false;
}

bool DockView::OnHasMsg(const char* pMsg)
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
  ApplicationWindow* pApp = ApplicationWindow::Instance;
  d->_clDocWindows[ name ] = pcDocWindow;

  QDockWindow* dw = new QDockWindow(pApp);
  dw->setCloseMode(QDockWindow::Always);
  dw->setCaption( name );
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

#include "moc_Window.cpp"
