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




#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qmessagebox.h>
#	include <qiconview.h> 
#	include <qfiledialog.h>
#	include <qcombobox.h>
# include <Python.h>
#endif

#include "DlgCustomizeImp.h"
#include "DlgToolbarsImp.h"
#include "DlgActionsImp.h"
#include "DlgCmdbarsImp.h"
#include "DlgCommandsImp.h"
#include "Application.h"
#include "Tools.h"
#include <qobjcoll.h>
#include <qtabwidget.h>

/////////////////////////////////////////////////////////////////////////////////////////////

FCPropertyPage::FCPropertyPage() 
{
  bChanged = false;
}

void FCPropertyPage::apply()
{
}

void FCPropertyPage::cancel()
{
}

bool FCPropertyPage::isModified()
{
  return bChanged;
}

void FCPropertyPage::setModified(bool b)
{
  bChanged = b;
}

void FCPropertyPage::slotApply()
{
  if (isModified())
    apply();

  setModified(false);
}

void FCPropertyPage::slotCancel()
{
  cancel();
  setModified(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////

/* 
 *  Constructs a FCDlgCustomize which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgCustomize::FCDlgCustomize( QWidget* parent,  const char* name, bool modal, WFlags fl )
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

  buttonApply = new QPushButton( this, "buttonApply" );
  buttonApply->setProperty( "text", tr( "&Apply" ) );
  buttonApply->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonApply );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setProperty( "text", tr( "&OK" ) );
  buttonOk->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  buttonOk->setProperty( "default", QVariant( TRUE, 0 ) );
  Layout->addWidget( buttonOk );

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
  insertTab(new FCDlgCustomCommandsImp( tabWidget, "tab" ), tr("Commands"));
  
  // second page
  //
  insertTab(new FCDlgCustomToolbarsImp( tabWidget, "tab" ), tr("Toolbars"));
  
  // third page
  //
  insertTab(new FCDlgCustomCmdbarsImp( tabWidget, "tab" ), tr("Command bars"));

  // fourth page
  //
  insertTab(new FCDlgCustomActionsImp( tabWidget, "tab" ), tr("Actions"));


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
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(slotOK()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(slotApply()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgCustomize::~FCDlgCustomize()
{
    // no need to delete child widgets, Qt does it all for us
  for (std::vector<QWidget*>::iterator it = tabPages.begin(); it!=tabPages.end();++it)
    delete (*it);

  tabPages.clear();
}

void FCDlgCustomize::insertTab (QWidget* w, QString name)
{
  w->reparent(tabWidget, QPoint(0,0));
  tabWidget->insertTab( w, name );

  // registrate the new page
  tabPages.push_back(w);
}

void FCDlgCustomize::slotOK()
{
  slotApply();
}

void FCDlgCustomize::slotApply()
{
  QWidget* page = tabWidget->currentPage();
  if (dynamic_cast<FCPropertyPage*>(page) != NULL)
     (dynamic_cast<FCPropertyPage*>(page))->slotApply();

# ifdef _DEBUG
  else
    GetConsole().Warning("Added page does not inherit from class FCPropertyPage");
#endif
}

void FCDlgCustomize::slotCancel()
{
  for (std::vector<QWidget*>::iterator it = tabPages.begin(); it != tabPages.end(); ++it)
  {
    if (dynamic_cast<FCPropertyPage*>(*it) != NULL)
       (dynamic_cast<FCPropertyPage*>(*it))->slotCancel();

# ifdef _DEBUG
    else
      GetConsole().Warning("Added page does not inherit from class FCPropertyPage");
#endif

  }
}

#include "moc_DlgCustomizeImp.cpp"
