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


#ifndef __DLGSETTINGS3DVIEWIMP_H
#define __DLGSETTINGS3DVIEWIMP_H

#include "DlgSettings3DView.h"

namespace Gui {
namespace Dialog {

/**
 * The DlgSettings3DViewImp class implements a preference page to change settings
 * for the Inventor viewer.
 * \author Jürgen Riegel
 */
class DlgSettings3DViewImp : public DlgSettings3DView
{ 
  Q_OBJECT

public:
  DlgSettings3DViewImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgSettings3DViewImp();

protected:
  void saveSettings();
  void loadSettings();
};

} // namespace Dialog
} // namespace Gui

#endif // __DLGSETTINGS3DVIEWIMP_H
