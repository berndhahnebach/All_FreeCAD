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

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "DlgMacroExecuteImp.h"
#include "Application.h"
#include "MainWindow.h"
#include "FileDialog.h"
#include "Macro.h"
#include "Document.h"
#include "PythonEditor.h"

#include <App/Application.h>
#include <App/Document.h>

using namespace Gui;
using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgMacroExecuteImp */

/**
 *  Constructs a DlgMacroExecuteImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgMacroExecuteImp::DlgMacroExecuteImp( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl ), WindowParameter( "Macro" )
{
  this->setupUi(this);
  // retrieve the macro path from parameter or use the home path as default
  _cMacroPath = getWindowParameter()->GetASCII("MacroPath",App::GetApplication().GetHomePath());
  fileChooser->setFileName(_cMacroPath.c_str());

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
  macroListBox->clear();
  
  // fill up with the directory
  for (unsigned int i=0; i<d.count(); i++ )
    macroListBox->insertItem(d[i]);
}

/** 
 * Selects a macro file in the list view.
 */
void DlgMacroExecuteImp::on_macroListBox_selectionChanged(Q3ListBoxItem* pcListViewItem)
{
  LineEditMacroName->setText(pcListViewItem->text());

  executeButton->setEnabled(true);
  editButton->setEnabled(true);
  deleteButton->setEnabled(true);
}

/**
 * Executes the selected macro file.
 */
void DlgMacroExecuteImp::on_macroListBox_doubleClicked(Q3ListBoxItem* pcListViewItem)
{
  LineEditMacroName->setText(pcListViewItem->text());
  on_executeButton_clicked();
}

/**
 * Executes the selected macro file.
 */
void DlgMacroExecuteImp::on_executeButton_clicked()
{
  if(!(LineEditMacroName->text().isEmpty() ) )
  {
    accept();

    QDir d( _cMacroPath.c_str() );
    QFileInfo fi( d, LineEditMacroName->text() );
    Application::Instance->macroManager()->run(Gui::MacroManager::File,( fi.filePath() ).latin1());
    // after macro run recalculate the document
    if ( Application::Instance->activeDocument() )
      Application::Instance->activeDocument()->getDocument()->recompute();
  }
}

/**
 * Specify the location of your macro files. The default location is FreeCAD's home path.
 */
void DlgMacroExecuteImp::on_fileChooser_fileNameChanged(const QString& fn)
{
  if (!fn.isEmpty())
  {
    // save the path in the parameters
    _cMacroPath = fn.latin1();
    getWindowParameter()->SetASCII("MacroPath",fn.latin1());
    // fill the list box
    fillUpList();
  }
}

/** 
 * Opens the macro file in an editor.
 */
void DlgMacroExecuteImp::on_editButton_clicked()
{
  int item = macroListBox->currentItem();
  if (item<0) return;

  QDir dir(_cMacroPath.c_str());
  QString file = QString("%1/%2").arg(dir.absPath()).arg(macroListBox->text(item));

  Application::Instance->open( file );
  accept();
}

/** Creates a new macro file. */
void DlgMacroExecuteImp::on_createButton_clicked()
{
  // query file name
  QString fn = QInputDialog::getText( tr("Macro file"), tr("Enter a file name, please:"), QLineEdit::Normal,
                                      QString::null, 0, this, "Macro file");
  if ( !fn.isEmpty() )
  {
    if ( !fn.endsWith(".FCMacro") )
      fn += ".FCMacro";
    QDir dir(_cMacroPath.c_str());
    QFileInfo fi( dir, fn );
    if ( fi.exists() && fi.isFile() )
    {
      QMessageBox::warning( this, tr("Existing file"), tr("'%1'.\nThis file already exists.").arg( fi.fileName() ) );
    }
    else
    {
      QString file = QString("%1/%2").arg(dir.absPath()).arg( fn );
      PythonEditView* edit = new PythonEditView( file, getMainWindow(), "Editor" );
      edit->setWindowTitle( fn );
      edit->resize( 400, 300 );
      getMainWindow()->addWindow( edit );
  
      accept();
    }
  }
}

/** Deletes the selected macro file from your harddisc. */
void DlgMacroExecuteImp::on_deleteButton_clicked()
{
  int item = macroListBox->currentItem();
  if (item<0) return;

  QString fn = macroListBox->text(item);
  int ret = QMessageBox::question( this, tr("Delete macro"), tr("Do you really want to delete the macro '%1'?").arg( fn ), 
                                   QMessageBox::Yes, QMessageBox::No|QMessageBox::Default|QMessageBox::Escape );
  if ( ret == QMessageBox::Yes )
  {
    QDir dir(_cMacroPath.c_str());
    dir.remove( fn );
    macroListBox->takeItem( macroListBox->item(item) );
  }
}

#include "moc_DlgMacroExecuteImp.cpp"
