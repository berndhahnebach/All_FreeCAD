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


#ifndef DLG_REPORT_VIEW_IMP
#define DLG_REPORT_VIEW_IMP

#include "DlgReportView.h"

#ifndef _PreComp_
#endif

namespace Gui {
namespace Dialog {

/** The DlgReportViewImp class implements the available settings for the 
 * report output window to change.
 * \author Werner Mayer
 */
class DlgReportViewImp : public DlgReportViewBase
{
public:
  DlgReportViewImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgReportViewImp();

protected:
  void saveSettings();
  void loadSettings();
};

} // namespace Dialog
} // namespace Gui

#endif //DLG_REPORT_VIEW_IMP
