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
# include <qbutton.h>
# include <qmessagebox.h>
# include <qiconview.h>
# include <qfiledialog.h>
# include <qcombobox.h>
# include <qlabel.h>
#endif

#include "DlgSettingsImp.h"
#include "PrefWidgets.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgSettingsHtmlViewImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgSettingsHtmlViewImp::DlgSettingsHtmlViewImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettingsBase( parent, name, fl )
{
  append(EnableBookmarks->getHandler());
  append(EnableHistory->getHandler());
  append(NbOfBookmarks->getHandler());
  append(NbOfHistoryItems->getHandler());
  append(LineEditURL->getHandler());
  append(LineEditBrowser->getHandler());
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsHtmlViewImp::~DlgSettingsHtmlViewImp()
{
  // no need to delete child widgets, Qt does it all for us
}

#include "DlgSettings.cpp"
#include "moc_DlgSettings.cpp"
#include "moc_DlgSettingsImp.cpp"
