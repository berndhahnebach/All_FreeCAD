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
//Added by qt3to4:
#include <QEvent>
#include <QCloseEvent>
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

TYPESYSTEM_SOURCE_ABSTRACT(Gui::BaseView,Base::BaseClass);


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
