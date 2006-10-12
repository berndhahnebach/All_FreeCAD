/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qdatetime.h>
# include <qlabel.h>
# include <qlineedit.h>
# include <qtextedit.h>
#endif

#include <App/Document.h>
#include <App/PropertyStandard.h>

#include "DlgProjectInformationImp.h"
#include "Document.h"
#define new DEBUG_CLIENTBLOCK
using namespace Gui::Dialog;

/**
 *  Constructs a Gui::Dialog::DlgProjectInformationImp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgProjectInformationImp::DlgProjectInformationImp( App::Document* doc, QWidget* parent, const char* name, bool modal, WFlags fl )
  : DlgProjectInformation( parent, name, modal, fl ), _doc(doc)
{
  lineEditName->setText( doc->Name.getValue() );
  lineEditPath->setText( doc->FileName.getValue() );
  lineEditCreator->setText( doc->CreatedBy.getValue() );
  lineEditDate->setText( doc->CreationDate.getValue() );
  lineEditLastMod->setText( doc->LastModifiedBy.getValue() );
  lineEditLastModDate->setText( doc->LastModifiedDate.getValue() );
  lineEditCompany->setText( doc->Company.getValue() );

  // When saving the text to XML the newlines get lost. So we store also the newlines as '\n'.
  // See also accept().
  QStringList lines = QStringList::split( "\\n", doc->Comment.getValue(), true );
  QString text = lines.join("\n");
  textEditComment->setText( text );
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgProjectInformationImp::~DlgProjectInformationImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Applies the changes to the project information of the given document.
 */
void DlgProjectInformationImp::accept()
{
  _doc->CreatedBy.setValue( lineEditCreator->text().latin1() );
  _doc->LastModifiedBy.setValue( lineEditCreator->text().latin1() );
  _doc->Company.setValue( lineEditCompany->text().latin1() );

  // Replace newline escape sequence trough '\\n' string
  QStringList lines = QStringList::split("\n", textEditComment->text(), true );
  QString text = lines.join("\\n");
  _doc->Comment.setValue( text.isEmpty() ? "" : text.latin1() );
  
  DlgProjectInformation::accept();
}

#include "DlgProjectInformation.cpp"
#include "moc_DlgProjectInformation.cpp"

