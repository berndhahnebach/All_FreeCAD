/***************************************************************************
                          DlgCustomizeImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars and button groups
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qbutton.h>
#	include <qmessagebox.h>
#	include <qiconview.h>
#	include <qfiledialog.h>
#	include <qcombobox.h>
#	include <qlayout.h>
#	include <qlabel.h>
#	include <vector>
#endif

#include "DlgCustomizeImp.h"
#include "DlgToolbarsImp.h"
#include "DlgActionsImp.h"
#include "DlgCmdbarsImp.h"
#include "DlgCommandsImp.h"
#include "Tools.h"
#include "../Base/Console.h"
//#include <qobjcoll.h>
#include <qtabwidget.h>

using namespace Gui::Dialog;

/* 
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
  	setName( "FCDlgCustomize" );
  resize( 434, 365 ); 
  setProperty( "caption", tr( "Customize" ) );
  setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
  FCDlgCustomizeBaseLayout = new QGridLayout( this ); 
  FCDlgCustomizeBaseLayout->setSpacing( 6 );
  FCDlgCustomizeBaseLayout->setMargin( 11 );

  Layout = new QHBoxLayout; 
  Layout->setSpacing( 6 );
  Layout->setMargin( 0 );

  buttonHelp = new QPushButton( this, "buttonHelp" );
  buttonHelp->setProperty( "text", tr( "&Help" ) );
  buttonHelp->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonHelp );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout->addItem( spacer );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setProperty( "text", tr( "&OK" ) );
  buttonOk->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  buttonOk->setProperty( "default", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonOk );

  buttonApply = new QPushButton( this, "buttonApply" );
  buttonApply->setProperty( "text", tr( "&Apply" ) );
  buttonApply->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonApply );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setProperty( "text", tr( "&Cancel" ) );
  buttonCancel->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonCancel );

  FCDlgCustomizeBaseLayout->addLayout( Layout, 1, 0 );

  tabWidget = new QTabWidget( this, "tabWidget" );

  // insert new tabs here !!!
  //
  //
  
  // first page
  //
  insertTab(new Gui::Dialog::DlgCustomCommandsImp( tabWidget, "tab" ), tr("Commands"));
  
  // second page
  //
  insertTab(new Gui::Dialog::DlgCustomToolbarsImp( tabWidget, "tab" ), tr("Toolbars"));
  
  // third page
  //
  insertTab(new Gui::Dialog::DlgCustomCmdbarsImp( tabWidget, "tab" ), tr("Command bars"));

  // fourth page
  //
  insertTab(new Gui::Dialog::DlgCustomActionsImp( tabWidget, "tab" ), tr("Actions"));


  FCDlgCustomizeBaseLayout->addWidget( tabWidget, 0, 0 );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

  // tab order
  setTabOrder( tabWidget, buttonHelp );
  setTabOrder( buttonHelp, buttonApply );
  setTabOrder( buttonApply, buttonOk );
  setTabOrder( buttonOk, buttonCancel );

  // connections
  //
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(onOK()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(onApply()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgCustomizeImp::~DlgCustomizeImp()
{
    // no need to delete child widgets, Qt does it all for us
  for (std::vector<QWidget*>::iterator it = tabPages.begin(); it!=tabPages.end();++it)
    delete (*it);

  tabPages.clear();
}

void DlgCustomizeImp::insertTab (QWidget* w, QString name)
{
  w->reparent(tabWidget, QPoint(0,0));
  tabWidget->insertTab( w, name );

  // registrate the new page
  tabPages.push_back(w);
}

void DlgCustomizeImp::onOK()
{
  onApply();
}

void DlgCustomizeImp::onApply()
{
  QWidget* page = tabWidget->currentPage();
  if (dynamic_cast<PropertyPage*>(page) != NULL)
     (dynamic_cast<PropertyPage*>(page))->onApply();

# ifdef FC_DEBUG
  else
	  Base::Console().Warning("Added page does not inherit from class FCPropertyPage");
#endif
}

void DlgCustomizeImp::onCancel()
{
  for (std::vector<QWidget*>::iterator it = tabPages.begin(); it != tabPages.end(); ++it)
  {
    if (dynamic_cast<PropertyPage*>(*it) != NULL)
       (dynamic_cast<PropertyPage*>(*it))->onCancel();

# ifdef FC_DEBUG
    else
		Base::Console().Warning("Added page does not inherit from class FCPropertyPage");
#endif

  }
}

#include "moc_DlgCustomizeImp.cpp"
