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

#include "DlgSettingsMacroImp.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgSettingsMacroImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettingsMacroImp::DlgSettingsMacroImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettingsMacro( parent, name, fl )
{
  append(FCCheckBox_GuiAsComment->getHandler());
  append(FCCheckBox_RecordGui   ->getHandler());
  append(MacroPath              ->getHandler());
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsMacroImp::~DlgSettingsMacroImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Opens a file dialog to choose a directory.
 */
void DlgSettingsMacroImp::chooseDir()
{
  QString cPath = QFileDialog::getExistingDirectory ();

  MacroPath->setText( cPath.latin1() );
}




#include "DlgSettingsMacro.cpp"
#include "moc_DlgSettingsMacro.cpp"
#include "moc_DlgSettingsMacroImp.cpp"
