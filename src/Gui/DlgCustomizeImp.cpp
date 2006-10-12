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
# include <qlayout.h>
# include <qpushbutton.h>
# include <qtabwidget.h>
#endif

#include "DlgCustomizeImp.h"
#include "MainWindow.h"
#include "WidgetFactory.h"
#define new DEBUG_CLIENTBLOCK
using namespace Gui::Dialog;

QStringList DlgCustomizeImp::_pages;

/* TRANSLATOR Gui::Dialog::DlgCustomizeImp */

/**
 *  Constructs a DlgCustomizeImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomizeImp::DlgCustomizeImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  if ( !name )
    setName( "DlgCustomizeImp" );
  resize( 434, 365 ); 

  setCaption( tr( "Customize" ) );
  setSizeGripEnabled( true );

  customLayout = new QGridLayout( this ); 
  customLayout->setSpacing( 6 );
  customLayout->setMargin( 11 );

  layout = new QHBoxLayout; 
  layout->setSpacing( 6 );
  layout->setMargin( 0 );

  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setText( tr( "&Help" ) );
  layout->addWidget( buttonHelp );

  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout->addItem( spacer );

  buttonClose = new QPushButton( this, "buttonCancel" );
  buttonClose->setText( tr( "&Close" ) );
  layout->addWidget( buttonClose );

  customLayout->addLayout( layout, 1, 0 );

  tabWidget = new QTabWidget( this, "tabWidget" );

  // make sure that pages are ready to create
  GetWidgetFactorySupplier();
  for ( QStringList::Iterator it = _pages.begin(); it!=_pages.end(); ++it )
  {
    addPage( WidgetFactory().createWidget( *it ) );
  }

  customLayout->addWidget( tabWidget, 0, 0 );


  // tab order
  setTabOrder( tabWidget, buttonClose );
  setTabOrder( buttonClose, buttonHelp );

  // connections
  //
  connect( buttonHelp,  SIGNAL ( clicked() ), getMainWindow(), SLOT ( whatsThis() ));
  connect( buttonClose, SIGNAL ( clicked() ), this, SLOT ( reject() ) );
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgCustomizeImp::~DlgCustomizeImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Adds a customize page with its class name \a className.
 * To create this page it must be registered in the WidgetFactory.
 * @see WidgetFactory
 * @see CustomPageProducer
 */
void DlgCustomizeImp::addPage( const QString& className )
{
  _pages.push_back( className );
}

/** Inserts a new tab page with its caption */
void DlgCustomizeImp::addPage ( QWidget* w )
{
  //w->reparent(tabWidget, QPoint(0,0));
  tabWidget->insertTab( w, w->caption() );
}

#include "moc_DlgCustomizeImp.cpp"
