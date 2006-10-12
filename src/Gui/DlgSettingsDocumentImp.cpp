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
#endif

#include "DlgSettingsDocumentImp.h"
#include "PrefWidgets.h"
#include "../Base/Console.h"
#define new DEBUG_CLIENTBLOCK
using namespace Gui::Dialog;

/**
 *  Constructs a DlgSettingsDocumentImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettingsDocumentImp::DlgSettingsDocumentImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettingsDocument( parent, name, fl )
{
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsDocumentImp::~DlgSettingsDocumentImp()
{
  // no need to delete child widgets, Qt does it all for us
}


void DlgSettingsDocumentImp::saveSettings()
{
  prefCheckNewDoc->onSave();
  SpinBoxCompression->onSave();

  prefCheckBox2->onSave();
  prefSpinBox2->onSave();
  prefCheckBox2_2->onSave();
  prefCheckBox2_2_2->onSave();
}

void DlgSettingsDocumentImp::loadSettings()
{
  prefCheckNewDoc->onRestore();
  SpinBoxCompression->onRestore();

  prefCheckBox2->onRestore();
  prefSpinBox2->onRestore();
  prefCheckBox2_2->onRestore();
  prefCheckBox2_2_2->onRestore();
}

#include "DlgSettingsDocument.cpp"
#include "moc_DlgSettingsDocument.cpp"
#include "moc_DlgSettingsDocumentImp.cpp"
