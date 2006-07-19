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
# include <qapplication.h>
# include <qregexp.h>
#endif


#include "View.h"
#include "Command.h"
#include "Document.h"
#include "Application.h"
#include "MainWindow.h"

using namespace Gui;

//**************************************************************************
// BaseView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BaseView::BaseView( Gui::Document* pcDocument)
  :_pcDocument(pcDocument), bIsDetached(false)
{
  if(pcDocument){
    pcDocument->attachView(this);
    bIsPassive = false;
  }else{
    Application::Instance->attachView(this);
    bIsPassive = true;
  }
}

BaseView::~BaseView()
{
//  assert (bIsDetached);
  if(!bIsDetached && !Application::Instance->isClosing() )
  {
    onClose();
  }
}

void BaseView::onClose(void)
{
  if(bIsDetached) return;

  if(bIsPassive){
    Application::Instance->detachView(this);
    if(_pcDocument)
      _pcDocument->detachView(this, true);
  }else{
    if(_pcDocument)
      _pcDocument->detachView(this);
  }

  _pcDocument = 0;
  bIsDetached = true;
  /*
  if(_pcDocument)
    _pcDocument->DetachView(this);
  else
    Application::Instance->DetachView(this);
*/
}

void BaseView::setDocument(Gui::Document* pcDocument)
{
  Gui::Document* pcOldDocument;
  // detaches and attaches the observer
  if(_pcDocument)
    _pcDocument->detachView(this, true);
  if(pcDocument)
    pcDocument->attachView(this,true);	

  // set the new document as the active one
  pcOldDocument = _pcDocument;
  _pcDocument = pcDocument;
}

/// returns the document the view is attached to
App::Document* BaseView::getAppDocument() const
{
  if(!_pcDocument) return 0;
  return _pcDocument->getDocument();
}


//**************************************************************************
// MDIView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



MDIView::MDIView( Gui::Document* pcDocument,QWidget* parent, const char* name, int wflags )
  :QMainWindow(parent, name, wflags), BaseView(pcDocument),_actualMode(Normal)
{
}

MDIView::~MDIView()
{
}

void MDIView::onRename(Gui::Document *pDoc)
{
  if(!bIsPassive)
  {
    // Try to separate document name and view number if there is one
    QString cap = caption();
    // Either with dirty flag ...
    QRegExp rx("(\\s\\:\\s\\d+\\s\\*)$");
    int pos = rx.searchRev(cap);
    if ( pos == -1 ) {
      // ... or not
      rx.setPattern("(\\s\\:\\s\\d+)$");
    }
    if ( pos != -1 ) {
      cap = cap.left(pos);
      cap += rx.cap();
      setCaption(cap);
    } else {
      setCaption(QString(pDoc->getDocument()->getName()));
    }
  }
}


/// recife a message
bool MDIView::onMsg(const char* pMsg,const char** ppReturn)
{
  return false;
}

bool MDIView::onHasMsg(const char* pMsg) const
{
  return false;
}

void MDIView::closeEvent(QCloseEvent *e)
{
  if(bIsPassive){
    if(canClose() ){
      e->accept();
      // avoid flickering
      getMainWindow()->removeWindow(this);
      QMainWindow::closeEvent(e);
    }
  }else{
    if(getGuiDocument()->isLastView())
    {
      getGuiDocument()->canClose(e);

      if(e->isAccepted ())
      {
        // avoid flickering
        getMainWindow()->removeWindow(this);
        QMainWindow::closeEvent(e);
      }
    }else
      e->accept();
  }
}

void MDIView::setActive(void)
{
  Application::Instance->viewActivated(this);
}

void MDIView::print( QPrinter* printer )
{
  // print command specified but print methode not overriden!
  assert(0);
}

QSize MDIView::minimumSizeHint () const
{
  return QSize(100, 80);
}

void MDIView::setCaption ( const QString& cap )
{
  QMainWindow::setCaption( cap );
  getMainWindow()->tabChanged( this );
}

void MDIView::setCurrentViewMode( ViewMode b )
{
  // set fullscreen mode
  if ( b == Normal )
  {
    if(_actualMode == FullScreen)
    {
      showNormal();
      clearWFlags ( WType_TopLevel );
      getMainWindow()->addWindow( this );
      releaseKeyboard();
    }else if(_actualMode == TopLevel)
    {/*
      WFlags f = getWFlags();
      setWFlags( f | WType_TopLevel );
    reparent( 0, WType_TopLevel | WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WStyle_Minimize | WStyle_Maximize |
	      // preserve some widget flags
	      (getWFlags() & 0xffff0000),
	      mapToGlobal( QPoint( 0, 0) ));
    const QRect screen = qApp->desktop()->screenGeometry( qApp->desktop()->screenNumber( this ) );
    //move( screen.topLeft() );
    //resize( screen.size() );
    raise();
    show();
    QEvent e( QEvent::ShowNormal );
    QApplication::sendEvent( this, &e );
#if defined(Q_WS_X11)
    extern void qt_wait_for_window_manager( QWidget* w ); // defined in qwidget_x11.cpp
    qt_wait_for_window_manager( this );
#endif

    setActiveWindow();*/
      clearWFlags ( WType_TopLevel );
      getMainWindow()->addWindow( this );
    }
    _actualMode = Normal;
    update();
  }else if ( b == FullScreen ){
    if(_actualMode == Normal)
    {
      WFlags f = getWFlags();
      setWFlags( f | WType_TopLevel );
      showFullScreen();
      grabKeyboard();
    }else if(_actualMode == TopLevel)
    {
      showFullScreen();
      grabKeyboard();
    }
    _actualMode = FullScreen;
    update();
  }else if ( b == TopLevel ){
    if(_actualMode == FullScreen)
    {
      showNormal();
      releaseKeyboard();
    }else if(_actualMode == Normal)
    {
      WFlags f = getWFlags();
      setWFlags( f | WType_TopLevel );
    reparent( 0, WType_TopLevel | WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WStyle_Minimize | WStyle_Maximize |
	      // preserve some widget flags
	      (getWFlags() & 0xffff0000),
	      mapToGlobal( QPoint( 0, 0) ));
    //const QRect screen = qApp->desktop()->screenGeometry( qApp->desktop()->screenNumber( this ) );
    //move( screen.topLeft() );
    //resize( screen.size() );
    raise();
    show();
    QEvent e( QEvent::ShowNormal );
    QApplication::sendEvent( this, &e );
#if defined(Q_WS_X11)
    extern void qt_wait_for_window_manager( QWidget* w ); // defined in qwidget_x11.cpp
    qt_wait_for_window_manager( this );
#endif

    setActiveWindow();
    }
    _actualMode = TopLevel;
    update();
  }

  /*
  if ( b = Normal )
  {
    WFlags f = getWFlags();
    setWFlags( f | WType_TopLevel );
    showFullScreen();

    grabKeyboard();
  }
  else
  {
    showNormal();
    clearWFlags ( WType_TopLevel );
    getMainWindow()->addWindow( this );

    releaseKeyboard();
  }
  */
}

void MDIView::keyPressEvent ( QKeyEvent* e )
{
  // Note: if the widget is in fullscreen mode then we can return to normal mode either
  // by pressing D or ESC. Since keyboard is grabbed accelerators don't work any more
  // (propably accelerators don't work at all - even without having grabbed the keyboard!?)

  if ( _actualMode != Normal )
  {
    // use Command's API to hold toogled state consistent
    if ( e->key() == Key_D || e->key() == Key_Escape )
    {
      setCurrentViewMode(Normal);
    }
    else if ( e->key() == Key_U )
    {
      setCurrentViewMode(TopLevel);
    }
    else if ( e->key() == Key_F )
    {
      setCurrentViewMode(FullScreen);
    }
    else
    {
      QMainWindow::keyPressEvent( e );
    }
  }
  else // the occupied key events F,D and U are are "eaten" by the main window
  {
    QMainWindow::keyPressEvent( e );
  }
}

#include "moc_View.cpp"
