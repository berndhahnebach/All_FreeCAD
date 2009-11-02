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
# include <QMessageBox>
#endif

#include "Application.h"
#include "MainWindow.h"
#include "DlgRunExternal.h"
#include "FileDialog.h"


using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgRunExternal */

/**
 *  Constructs a DlgRunExternal which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgRunExternal::DlgRunExternal( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl ),process(this),advancedHidden(true)
{
    this->setupUi(this);
	QObject::connect(&process,SIGNAL(finished()),this,SLOT(finished()));
	QObject::connect(ButtonAccept,SIGNAL(clicked()),this,SLOT(accept()));
	QObject::connect(ButtonDiscard,SIGNAL(clicked()),this,SLOT(reject()));
	QObject::connect(ButtonAbort,SIGNAL(clicked()),this,SLOT(abort()));
	QObject::connect(ButtonAdvanced,SIGNAL(clicked()),this,SLOT(advanced()));

	Output->hide();
	ChooseProgramm->hide();
	Programm->hide();


}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgRunExternal::~DlgRunExternal()
{
    // no need to delete child widgets, Qt does it all for us
}


int DlgRunExternal::Do(void)
{
	process.start(ProcName,arguments);

	ButtonAccept->setEnabled(false);
	ButtonDiscard->setEnabled(false);
	return exec();
}
void DlgRunExternal::reject (void)
{
	QDialog::reject();
}

void DlgRunExternal::accept (void)
{

}

void DlgRunExternal::abort (void)
{
	process.terminate();
	QDialog::reject();

}

void DlgRunExternal::advanced (void)
{
	if(advancedHidden){
		Output->show();
		ChooseProgramm->show();
		Programm->show();
		advancedHidden = false;
	}else{
		Output->hide();
		ChooseProgramm->hide();
		Programm->hide();
		advancedHidden = true;
	}

}

void DlgRunExternal::finished ( int exitCode, QProcess::ExitStatus exitStatus )
{
	ButtonAccept->setEnabled(true);
	ButtonDiscard->setEnabled(true);

}

#include "moc_DlgRunExternal.cpp"

