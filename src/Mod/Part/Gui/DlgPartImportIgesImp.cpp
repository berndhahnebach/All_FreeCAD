/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"
#ifndef _PreComp_
# include <qlineedit.h>
#endif

#include "../../../Gui/FileDialog.h"
#include "../../../Gui/Application.h"

#include "DlgPartImportIgesImp.h"

/* 
 *  Constructs a DlgPartImportIges which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPartImportIgesImp::DlgPartImportIgesImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgPartImportIges( parent, name, modal, fl )
{
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgPartImportIgesImp::~DlgPartImportIgesImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/* 
 * public slot
 */
void DlgPartImportIgesImp::OnApply()
{
    qWarning( "DlgPartImportIgesImp::OnApply() not yet implemented!" ); 
}

void DlgPartImportIgesImp::onChooseFileName()
{
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "IGES (*.igs *.iges);;All Files (*.*)", ApplicationWindow::Instance);
	if (! fn.isEmpty() )
	{
    FileName->setText(fn);
	}
}




#include "DlgPartImportIges.cpp"
#include "moc_DlgPartImportIges.cpp"
#include "moc_DlgPartImportIgesImp.cpp"
