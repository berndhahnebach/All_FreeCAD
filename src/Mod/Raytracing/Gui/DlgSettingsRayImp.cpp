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


/*  Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "DlgSettingsRayImp.h"
#include <Gui/PrefWidgets.h>
#include <Base/Console.h>

using namespace RayGui;

/**
 *  Constructs a DlgSettings3DViewImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettingsRayImp::DlgSettingsRayImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettingsRay( parent, name, fl )
{
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsRayImp::~DlgSettingsRayImp()
{
  // no need to delete child widgets, Qt does it all for us
}


void DlgSettingsRayImp::saveSettings()
{
  prefLineEdit1->onSave();
  prefLineEdit2->onSave();
  prefLineEdit3->onSave();
  prefFloatSpinBox1->onSave();
  prefCheckBox8->onSave();
  prefCheckBox9->onSave();
}

void DlgSettingsRayImp::loadSettings()
{
  prefLineEdit1->onRestore();
  prefLineEdit2->onRestore();
  prefLineEdit3->onRestore();
  prefFloatSpinBox1->onRestore();
  prefCheckBox8->onRestore();
  prefCheckBox9->onRestore();
}

#include "DlgSettingsRay.cpp"
#include "moc_DlgSettingsRay.cpp"
#include "moc_DlgSettingsRayImp.cpp"
