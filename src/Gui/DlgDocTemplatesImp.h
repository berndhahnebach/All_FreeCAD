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


#ifndef DLGDOCTEMPLATESIMP_H
#define DLGDOCTEMPLATESIMP_H

#include "DlgDocTemplates.h"


namespace Gui {
class Command;
namespace Dialog {

/** 
 * The DlgDocTemplatesImp class implements a dialog containing all available document
 * templates to create a new document.
 * \author Jürgen Riegel
 */
class DlgDocTemplatesImp : virtual public DlgDocTemplates
{ 
    Q_OBJECT

public:
  DlgDocTemplatesImp( Gui::Command* pcCmd,QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgDocTemplatesImp();

public:
  virtual void onChooseFile();
  virtual void onValidate();
  virtual void onEditFile();

protected:
  virtual void onViewChanged( QIconViewItem* );
  virtual void onIconDoubleClicked( QIconViewItem* );

protected:
  Gui::Command* _pcCmd;
};

} // namespace Dialog
} // namespace Gui

#endif // DLGDOCTEMPLATESIMP_H
