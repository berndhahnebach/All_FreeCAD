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


#ifndef DLGMACRORECORDIMP_H
#define DLGMACRORECORDIMP_H

#include "DlgMacroRecord.h"
#include "Window.h"

class FCMacroManager;

namespace Gui {
namespace Dialog {

/**
 * The DlgMacroRecordImp class implements a dialog to record a macro.
 * \author Jürgen Riegel
 */
class DlgMacroRecordImp : public DlgMacroRecord, public Gui::WindowParameter
{ 
    Q_OBJECT

public:
  DlgMacroRecordImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  virtual ~DlgMacroRecordImp();

public:
  virtual void onTieCommandBar();
  virtual void onTieToolBar();
  virtual void onTieKeyboard();
  virtual void cancel();
  virtual void start();
  virtual void stop();
  virtual void onSaveMacro();

protected:
  /// conviniance pointer
  FCMacroManager* _pcMacroMngr; 
  std::string _cMacroPath; // Macro file to save in
};

} // namespace Dialog
} // namespace Gui

#endif // DLGMACRORECORDIMP_H
