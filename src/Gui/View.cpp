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


#include "View.h"
#include "Command.h"
#include "Document.h"
#include "Application.h"


using namespace Gui;

//**************************************************************************
// BaseView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

std::vector<Gui::ViewProvider*> BaseView::_vpcViewProvider;

BaseView::BaseView( Gui::Document* pcDocument)
:_pcDocument(pcDocument),
 bIsDetached(false)
{
  if(pcDocument){
    pcDocument->attachView(this);
    bIsPassiv = false;
  }else{
    ApplicationWindow::Instance->attachView(this);
    bIsPassiv = true;
  }
}

BaseView::~BaseView()
{
//  assert (bIsDetached);
  if(!bIsDetached && !ApplicationWindow::Instance->isClosing() )
  {
    onClose();
  }
}

void BaseView::onClose(void)
{
  if(bIsDetached) return;

  if(bIsPassiv){
    ApplicationWindow::Instance->detachView(this);
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
    ApplicationWindow::Instance->DetachView(this);
*/
}

void BaseView::setDocument(Gui::Document* pcDocument)
{
  Gui::Document* pcOldDocument;
  // detach and attache the observer
  if(_pcDocument)
    _pcDocument->detachView(this, true);
  if(pcDocument)
    pcDocument->attachView(this,true);	

  // set the new document as the active one
  pcOldDocument = _pcDocument;
  _pcDocument = pcDocument;

  // tell the view
  onNewDocument(pcOldDocument,_pcDocument);
}

/// returns the document the view is attached to
App::Document* BaseView::getAppDocument()
{
  if(!_pcDocument) return 0;
  return _pcDocument->getDocument();
}

/// Register a new View provider
void BaseView::addViewProvider(Gui::ViewProvider* pcProvider)
{
  _vpcViewProvider.push_back(pcProvider);
}


//**************************************************************************
// MDIView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



MDIView::MDIView( Gui::Document* pcDocument,QWidget* parent, const char* name, int wflags )
  :QMainWindow(parent, name, wflags), BaseView(pcDocument)
{
}

MDIView::~MDIView()
{
}


/// recife a message
bool MDIView::onMsg(const char* pMsg,const char** ppReturn)
{
  return false;
}

bool MDIView::onHasMsg(const char* pMsg)
{
  return false;
}

void MDIView::closeEvent(QCloseEvent *e)
{
  if(bIsPassiv){
    if(canClose() ){
      e->accept();
      QMainWindow::closeEvent(e);
    }
  }else{
    if(getGuiDocument()->isLastView())
    {
      getGuiDocument()->canClose(e);

      if(e->isAccepted ())
        QMainWindow::closeEvent(e);
    }else
      e->accept();
  }
}

void MDIView::setActive(void)
{
  ApplicationWindow::Instance->viewActivated(this);
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

void MDIView::setFullScreenMode( bool b )
{
  // set fullscreen mode
  if ( b )
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
    ApplicationWindow::Instance->addWindow( this );

    releaseKeyboard();
  }
}

void MDIView::keyPressEvent ( QKeyEvent* e )
{
  // Note: if the widget is in fullscreen mode then we can return to normal mode either
  // by pressing F or ESC. Since keyboard is grabbed accelerators don't work any more
  // (propably accelerators don't work at all - even without having grabbed the keyboard!?)
  if ( isFullScreen() )
  {
    // use Command's API to hold toogled state consistent
    if ( e->key() == Key_F || e->key() == Key_Escape )
    {
      Command* pcCmd = ApplicationWindow::Instance->commandManager().getCommandByName( "Std_ViewFullScreen" );
      if( pcCmd )
      {
        pcCmd->toggleCommand( "Std_ViewFullScreen", false);
      }
    }
  }
  else
  {
    QMainWindow::keyPressEvent( e );
  }
}

#include "moc_View.cpp"
