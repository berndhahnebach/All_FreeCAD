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


#ifndef GUI_DIALOG_DLGINPUTDIALOGIMP_H
#define GUI_DIALOG_DLGINPUTDIALOGIMP_H

#include "DlgInputDialog.h"

namespace Gui {
namespace Dialog {

/**
 * The DlgInputDialogImp dialog class does basically the same as Qt's QInputDialog
 * unless that it provides no static function but the application programmer must
 * create an instance and prepare it. This requires a little more work but increases
 * the flexibility.
 * \author Werner Mayer
 */
class GuiExport DlgInputDialogImp : public DlgInputDialog
{
public:
  enum Type { LineEdit, SpinBox, FloatSpinBox, ComboBox };

  DlgInputDialogImp( const QString& label, QWidget* parent = 0, const char* name = 0, 
    bool modal = TRUE, Type = LineEdit );
  ~DlgInputDialogImp();

  void setType( Type t );
  Type type() const;

  Gui::SpinBox *getSpinBox() const;
  Gui::FloatSpinBox *getFloatSpinBox() const;
  QLineEdit *getLineEdit() const;
  QComboBox *getComboBox() const;

protected:
  void textChanged( const QString &s );
  void tryAccept();
  Type inputtype;
};

} // namespace Dialog
} // namespace Gui

#endif // GUI_DIALOG_DLGINPUTDIALOGIMP_H
