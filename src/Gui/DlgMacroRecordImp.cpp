/** \file DlgMacroRecordImp.cpp
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qfiledialog.h>
#	include <qlineedit.h>
#	include <qmessagebox.h>
# include <qpushbutton.h>
#endif

#include "Macro.h"
#include "Application.h"
#include "DlgMacroRecordImp.h"


/* 
 *  Constructs a DlgMacroRecordImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgMacroRecordImp::DlgMacroRecordImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgMacroRecord( parent, name, modal, fl )
{
	// get the parameter group of the "Macro Execute" Dialog
	FCHandle<FCParameterGrp> h = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro/");
	// get the macro home path
	_cMacroPath = h->GetASCII("MacroPath",GetApplication().GetHomePath());
	// get a pointer to the macro manager
	_pcMacroMngr = ApplicationWindow::Instance->GetMacroMngr();

	// check if a macro recording is in progress
	if(_pcMacroMngr->IsOpen())
	{
		PushButtonStart->setEnabled(false);

	}else{
		PushButtonStop->setEnabled(false);


	}
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgMacroRecordImp::~DlgMacroRecordImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/* 
 * public slot
 */
void DlgMacroRecordImp::OnTieCommandBar()
{
    qWarning( "DlgMacroRecordImp::OnTieCommandBar() not yet implemented!" ); 
}
/* 
 * public slot
 */
void DlgMacroRecordImp::OnTieToolBar()
{
    qWarning( "DlgMacroRecordImp::OnTieToolBar() not yet implemented!" ); 
}
/* 
 * public slot
 */
void DlgMacroRecordImp::OnTieKeyboard()
{
    qWarning( "DlgMacroRecordImp::OnTieKeyboard() not yet implemented!" ); 
}

void DlgMacroRecordImp::Cancel()
{
	if(_pcMacroMngr->IsOpen())
	{
		_pcMacroMngr->Cancel();
	}
	reject();
}

void DlgMacroRecordImp::Start()
{
	// test if the path already set
	if(LineEditPath->text().isEmpty())
	{
		QMessageBox::information( ApplicationWindow::Instance, tr("FreeCAD - Macro recorder"),
		                                       tr("Specify first a place to save."));
		reject();
	}else{
		// open the macro recording
		_pcMacroMngr->Open(FCMacroManager::File,(_cMacroPath + LineEditPath->text().latin1()).c_str());
		accept();
	}
}

void DlgMacroRecordImp::Stop()
{
	if(_pcMacroMngr->IsOpen())
	{
		// ends the macrorecording and save the file...
		_pcMacroMngr->Commit();
		accept();
	}
}

void DlgMacroRecordImp::File()
{
	QString fn = QFileDialog::getSaveFileName(0, "FreeCAD script (*.FCScript)", ApplicationWindow::Instance);
	if (!fn.isEmpty())
	{
		LineEditPath->setText ( fn );
	}
}



#include "DlgMacroRecord.cpp"
#include "moc_DlgMacroRecord.cpp"
#include "moc_DlgMacroRecordImp.cpp"

