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

#include "DlgSettings3DViewPartImp.h"
#include "../../../Gui/PrefWidgets.h"
#include "../../../Base/Console.h"

using namespace PartGui;

/**
 *  Constructs a DlgSettings3DViewPartImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettings3DViewPartImp::DlgSettings3DViewPartImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettings3DViewPart( parent, name, fl )
{
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettings3DViewPartImp::~DlgSettings3DViewPartImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Print warning if OpenInventor viewer is used.
 */
void DlgSettings3DViewPartImp::warnInventor(bool b)
{
  if ( b )
    Base::Console().Warning("The inventor Viewer is highly experimental. Usage can cause FreeCAD to crash!\n");
}

void DlgSettings3DViewPartImp::saveSettings()
{
  prefFloatSpinBox1->onSave();
  prefCheckBox8->onSave();
  prefCheckBox3->onSave();
}

void DlgSettings3DViewPartImp::loadSettings()
{
  prefFloatSpinBox1->onRestore();
  prefCheckBox8->onRestore();
  prefCheckBox3->onRestore();
}

#include "DlgSettings3DViewPart.cpp"
#include "moc_DlgSettings3DViewPart.cpp"
#include "moc_DlgSettings3DViewPartImp.cpp"
