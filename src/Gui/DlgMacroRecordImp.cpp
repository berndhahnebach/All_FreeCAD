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
# include <qlineedit.h>
# include <qmessagebox.h>
# include <qpushbutton.h>
#endif

#include "Macro.h"
#include "Application.h"
#include "MainWindow.h"
#include "DlgMacroRecordImp.h"
#include "FileDialog.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgMacroRecordImp */

/**
 *  Constructs a DlgMacroRecordImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgMacroRecordImp::DlgMacroRecordImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgMacroRecord( parent, name, modal, fl ), WindowParameter("Macro")
{
  // get the macro home path
  _cMacroPath = getWindowParameter()->GetASCII("MacroPath",App::GetApplication().GetHomePath());
  // check on PATHSEP at the end
  std::string::iterator It = _cMacroPath.end();
  It--;
  if(*It != PATHSEP)
    _cMacroPath += PATHSEP;

  // get a pointer to the macro manager
  _pcMacroMngr = Application::Instance->macroManager();

  // check if a macro recording is in progress
  if(_pcMacroMngr->isOpen())
  {
    PushButtonStart->setEnabled(false);
  }
  else
  {
    PushButtonStop->setEnabled(false);
  }
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgMacroRecordImp::~DlgMacroRecordImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * \todo
 */
void DlgMacroRecordImp::onTieCommandBar()
{
  qWarning( "DlgMacroRecordImp::OnTieCommandBar() not yet implemented!" ); 
}

/**
 * \todo
 */
void DlgMacroRecordImp::onTieToolBar()
{
  qWarning( "DlgMacroRecordImp::OnTieToolBar() not yet implemented!" ); 
}

/**
 * \todo
 */
void DlgMacroRecordImp::onTieKeyboard()
{
  qWarning( "DlgMacroRecordImp::OnTieKeyboard() not yet implemented!" ); 
}

/**
 * Abort the macro.
 */
void DlgMacroRecordImp::cancel()
{
  if(_pcMacroMngr->isOpen())
  {
    _pcMacroMngr->cancel();
  }
  
  reject();
}

/**
 * Starts the record of the macro.
 */
void DlgMacroRecordImp::start()
{
  // test if the path already set
  if(LineEditPath->text().isEmpty())
  {
    QMessageBox::information( getMainWindow(), tr("FreeCAD - Macro recorder"),
                                         tr("Specify first a place to save."));
    reject();
  }
  else
  {
    // search in the macro path first for an already existing macro
    QString fn = (_cMacroPath + LineEditPath->text().latin1()).c_str();
    if ( !fn.endsWith(".FCMacro") ) fn += ".FCMacro";
    QFileInfo fi(fn);
    if ( fi.isFile() && fi.exists() )
    {
      if ( QMessageBox::question( this, tr("Existing macro"), tr("The macro '%1' already exists. Do you want to overwrite?").arg(fn),
                                  QMessageBox::Yes, QMessageBox::No|QMessageBox::Default|QMessageBox::Escape ) == QMessageBox::No )
      return;
    }

    // open the macro recording
    _pcMacroMngr->setRecordGuiCommands(getWindowParameter()->GetBool("RecordGui", true), getWindowParameter()->GetBool("GuiAsComment", true) );
    _pcMacroMngr->open(MacroManager::File,(_cMacroPath + LineEditPath->text().latin1()).c_str());
    accept();
  }
}

/**
 * Stops the record of the macro.and save to the file.
 */
void DlgMacroRecordImp::stop()
{
  if(_pcMacroMngr->isOpen())
  {
    // ends the macrorecording and save the file...
    _pcMacroMngr->commit();
    accept();
  }
}

/**
 * Specify the location to save the macro to.
 */
void DlgMacroRecordImp::onSaveMacro()
{
  QString fn = FileDialog::getSaveFileName(0, "FreeCAD script (*.FCScript)", getMainWindow());

  if (!fn.isEmpty())
  {
    LineEditPath->setText ( fn );
  }
}

#include "DlgMacroRecord.cpp"
#include "moc_DlgMacroRecord.cpp"
#include "moc_DlgMacroRecordImp.cpp"

