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


#ifndef DLGMACROEXECUTEIMP_H
#define DLGMACROEXECUTEIMP_H

#include "DlgMacroExecute.h"
#include "Window.h"

namespace Gui {
namespace Dialog {

/**
 * The DlgMacroExecuteImp class implements a dialog to execute or edit a
 * recorded macro. 
 * \author Jürgen Riegel
 */
class DlgMacroExecuteImp : public DlgMacroExecute, public Gui::WindowParameter
{ 
    Q_OBJECT

public:
  DlgMacroExecuteImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  virtual ~DlgMacroExecuteImp();

public:
  virtual void onExecute();
  virtual void onNewFolder();
  virtual void onCreate();
  virtual void onEdit();
  virtual void onDelete();

protected:
  void fillUpList(void);
  virtual void onListDoubleClicked( QListViewItem* );
  virtual void onNewListItemPicked( QListViewItem* );

protected:
  std::string _cMacroPath;
};

} // namespace Dialog
} // namespace Gui

#endif // DLGMACROEXECUTEIMP_H
