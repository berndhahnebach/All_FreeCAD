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


#ifndef DLGCMDBARS_IMP_H
#define DLGCMDBARS_IMP_H

#include "DlgToolbarsImp.h"

class FCCommand;

namespace Gui {
class CustomToolBar;

namespace Dialog {

/** This class implements the creation of user defined commandbars.
 * A commandbar is the same as a toolbar - a collection of several 
 * QAction objects - unless a commandbar is placed in a QToolBox object,
 * while a toolbar is placed in the dock areas of the main window..
 * So commandbars are predestinated to save place on your desktop.
 * @see DlgCustomToolbars
 * @see DlgCustomToolbarsImp
 * \author Werner Mayer
 */
class DlgCustomCmdbarsImp : public DlgCustomToolbars
{ 
  Q_OBJECT

public:
  DlgCustomCmdbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgCustomCmdbarsImp();

protected:
  void apply();
  void cancel();
  void updateData();

protected slots:
  void onCreateToolbar();
  void onDeleteToolbar();
};

} // namespace Dialog
} // namespace Gui

#endif
