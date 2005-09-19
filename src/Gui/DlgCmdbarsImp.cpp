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
# include <qinputdialog.h>
#endif

#include "DlgCmdbarsImp.h"
#include "Application.h"
#include "Command.h"
#include "CustomWidgets.h"
#include "CommandBarManager.h"
#include "Tools.h"
#include "ToolBarManager.h"
#include "Workbench.h"
#include "WorkbenchManager.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomCmdbarsImp */

/**
 *  Constructs a DlgCustomCmdbarsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomCmdbarsImp::DlgCustomCmdbarsImp( QWidget* parent, const char* name, WFlags fl )
  : DlgCustomToolbars(parent, name, fl)
{
  setCaption( tr( "Commandbars" ) );
  if ( WorkbenchManager::instance()->active() )
    _toolBars = WorkbenchManager::instance()->active()->importCustomBars("Commandbars"); 
  refreshToolBarList();
}

/** Destroys the object and frees any allocated resources */
DlgCustomCmdbarsImp::~DlgCustomCmdbarsImp()
{
  if ( _toolBars )
  {
    CommandBarManager::getInstance()->customSetup(_toolBars);
    WorkbenchManager::instance()->active()->exportCustomBars(_toolBars, "Commandbars");
  }
}

/** Discards all changes */
void DlgCustomCmdbarsImp::cancel()
{
}

#include "moc_DlgCmdbarsImp.cpp"
