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
# include <qlistbox.h>
#endif

#include "DlgActivateWindowImp.h"
#include "Application.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgActivateWindowImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgActivateWindowImp::DlgActivateWindowImp( QWidget* parent, const char* name, bool modal, WFlags fl )
  : DlgActivateWindowBase( parent, name, modal, fl )
{
  QWidgetList windows = ApplicationWindow::Instance->windows();
  QWidget* activeWnd = ApplicationWindow::Instance->windows( QWorkspace::StackingOrder ).last();

  int active = -1;
  for ( int i=0; i<(int)windows.count(); i++ )
  {
    listBox2->insertItem( windows.at(i)->caption() );
    if ( windows.at(i) == activeWnd )
      active = i;
  }

  if ( active > -1 )
    listBox2->setCurrentItem( active );

  listBox2->setFocus();

  connect( listBox2, SIGNAL( doubleClicked ( QListBoxItem * ) ), this, SLOT( accept() ) );
  connect( listBox2, SIGNAL( returnPressed ( QListBoxItem * ) ), this, SLOT( accept() ) );
}

/** Destroys the object and frees any allocated resources */
DlgActivateWindowImp::~DlgActivateWindowImp()
{
}

/**
 * Activates the MDI window you wish and closes the dialog.
 */
void DlgActivateWindowImp::accept()
{
  QString activated = listBox2->currentText();

  QWidgetList windows = ApplicationWindow::Instance->windows();
  for ( int i=0; i<(int)windows.count(); i++ )
  {
    if( windows.at(i)->caption() == activated )
    {
      windows.at(i)->setFocus();
      break;
    }
  }

  DlgActivateWindowBase::accept();
}

#include "DlgActivateWindow.cpp"
#include "moc_DlgActivateWindow.cpp"
