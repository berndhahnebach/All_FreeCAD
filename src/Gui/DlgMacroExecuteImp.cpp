/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qcombobox.h>
# include <qdir.h>
# include <qfileinfo.h>
# include <qlineedit.h>
# include <qlistview.h>
# include <qpushbutton.h>
#endif


#include "DlgMacroExecuteImp.h"
#include "../App/Application.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "FileDialog.h"
#include "Macro.h"
#include "PythonEditor.h"

using namespace Gui;
using namespace Gui::Dialog;

/**
 *  Constructs a DlgMacroExecuteImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgMacroExecuteImp::DlgMacroExecuteImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgMacroExecute( parent, name, modal, fl ), WindowParameter( "Macro" )
{
  // retrieve the macro path from parameter or use the home path as default
  _cMacroPath = getWindowParameter()->GetASCII("MacroPath",App::GetApplication().GetHomePath());
  ComboBoxPath->insertItem(_cMacroPath.c_str());

  // Fill the List box
  fillUpList();
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgMacroExecuteImp::~DlgMacroExecuteImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Fills up the list with all macro files found in the specified location.
 */
void DlgMacroExecuteImp::fillUpList(void)
{
  // lists all files in macro path
  QDir d( _cMacroPath.c_str(),"*.FCMacro" );

  // clean old entries
  MacrosListView->clear();
  
  // fill up with the directory
  for (unsigned int i=0; i<d.count(); i++ )
    (void)new QListViewItem( MacrosListView, d[i], "FCMacro file" );
}

/** 
 * Selects a macro file in the list view.
 */
void DlgMacroExecuteImp::onNewListItemPicked(QListViewItem* pcListViewItem)
{
  LineEditMacroName->setText(pcListViewItem->text(0));

  ExecuteButton->setEnabled(true);
  EditButton->setEnabled(true);
  DeleteButton->setEnabled(true);
}

/**
 * Executes the selected macro file.
 */
void DlgMacroExecuteImp::onListDoubleClicked(QListViewItem* pcListViewItem)
{
  LineEditMacroName->setText(pcListViewItem->text(0));
  onExecute();
}

/**
 * Executes the selected macro file.
 */
void DlgMacroExecuteImp::onExecute()
{
  if(!(LineEditMacroName->text().isEmpty() ) )
  {
    accept();

    QDir d( _cMacroPath.c_str() );
    QFileInfo fi( d, LineEditMacroName->text() );
    ApplicationWindow::Instance->macroManager()->run(Gui::MacroManager::File,( fi.filePath() ).latin1());
  }
}

/**
 * Specify the location of your macro files. The default location is FreeCAD's home path.
 */
void DlgMacroExecuteImp::onNewFolder()
{
  QString fn = FileDialog::getExistingDirectory (_cMacroPath.c_str(), ApplicationWindow::Instance);
  if (!fn.isEmpty())
  {
    _cMacroPath = fn.latin1();
    // combo box
    ComboBoxPath->insertItem(fn);
    ComboBoxPath->setCurrentText(fn);
    // save the path in the parameters
    getWindowParameter()->SetASCII("MacroPath",fn.latin1());
    // fill the list box
    fillUpList();
  }
}

/** 
 * Opens the macro file in an editor.
 */
void DlgMacroExecuteImp::onEdit()
{
  QListViewItem* item = MacrosListView->selectedItem();
  if (!item) return;

  QDir dir(_cMacroPath.c_str());
  QString file = QString("%1/%2").arg(dir.absPath()).arg(item->text(0));
  PythonEditView* edit = new PythonEditView(ApplicationWindow::Instance, "Editor");
  edit->setIcon( Gui::BitmapFactory().pixmap("MacroEditor") );
  edit->setCaption( file );
  edit->resize( 400, 300 );
  ApplicationWindow::Instance->addWindow( edit );
  edit->openFile(file);
  
  accept();
}

/** \todo */
void DlgMacroExecuteImp::onCreate()
{
  DlgMacroExecute::onCreate();
}

/** \todo */
void DlgMacroExecuteImp::onDelete()
{
  DlgMacroExecute::onDelete();
}

#include "DlgMacroExecute.cpp"
#include "moc_DlgMacroExecute.cpp"
#include "moc_DlgMacroExecuteImp.cpp"
