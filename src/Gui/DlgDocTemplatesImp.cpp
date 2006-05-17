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
# include <qiconview.h> 
# include <qfiledialog.h>
# include <qlineedit.h>
#endif

//#include "DlgDocTemplatesImp.h"
#include "Command.h"
#include "Application.h"
#include "BitmapFactory.h"

#include "../App/Application.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgDocTemplatesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgDocTemplatesImp::DlgDocTemplatesImp(  Gui::Command* pcCmd, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgDocTemplates( parent, name, modal, fl ), _pcCmd(pcCmd)
{
  std::vector<std::string> vTemplates = App::GetApplication().GetAllTemplates();

  QPixmap pixmap(Gui::BitmapFactory().pixmap("Folder32"));
  QString str;
  // cycling through the Templates
  for(std::vector<std::string>::iterator i=vTemplates.begin();i!=vTemplates.end();i++)
  {
    str = (*i).c_str();
    if(str != "")
      (void)new QIconViewItem ( TemplateIconView ,str,pixmap );
  }
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgDocTemplatesImp::~DlgDocTemplatesImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Opens a file dialog to choose a template file. 
 */
void DlgDocTemplatesImp::onChooseFile()
{
  QString s ( QFileDialog::getOpenFileName( QString::null, "FreeCAD Templates (*.FCTempl)", this ) );
  if ( s.isEmpty() )
    return;
  puts(s);
}

/**
 * Creates a new document.
 */
void DlgDocTemplatesImp::onValidate()
{
  QString sTemplate = LineEdit1->text();

  if (sTemplate.isEmpty())
    _pcCmd->doCommand(Gui::Command::Doc,"FreeCAD.New()");		
  else
    _pcCmd->doCommand(Gui::Command::Doc,"FreeCAD.New(\"%s\")",LineEdit1->text().latin1());

  accept();
}

/**
 * \internal
 */
void DlgDocTemplatesImp::onIconDoubleClicked( QIconViewItem* Item )
{
  LineEdit1->setText(Item->text());
  onValidate();
}

/**
 * \internal
 */
void DlgDocTemplatesImp::onViewChanged( QIconViewItem* Item )
{
  LineEdit1->setText(Item->text());
}

/**
 * \todo
 */
void DlgDocTemplatesImp::onEditFile()
{
  qWarning( "DlgDocTemplatesImp::EditFile() not yet implemented!" ); 
}

#include "DlgDocTemplates.cpp"
#include "moc_DlgDocTemplates.cpp"
#include "moc_DlgDocTemplatesImp.cpp"

