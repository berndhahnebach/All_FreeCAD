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


#ifndef DLGACTIONS_IMP_H
#define DLGACTIONS_IMP_H

#include "DlgActions.h"

class FCCommand;

namespace Gui {
namespace Dialog {

/** This class implements the creation of user defined actions executing a recorded macro.
 * It is possible to use these actions in own toolbars or commandbars.
 * @see Gui::Action
 * @see MacroCommand
 * @see FCCommand
 * \author Werner Mayer
 */
class DlgCustomActionsImp : public DlgCustomActionsBase
{ 
Q_OBJECT

public:
  DlgCustomActionsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgCustomActionsImp();

  void show();

protected:
  /** Enables/disables buttons for deletion */
  void onCustomActionsCanDelete( QListViewItem *i );
  /** Shows the setup of the action */
  void onCustomActionsDoubleClicked( QListViewItem *i );
  /** Adds a custom action */
  void onAddCustomAction();
  /** Deletes a custom action */
  void onDelCustomAction();
  /** Opens a iconview to select a pixmap */
  void onCustomActionPixmap();

private:
  /** Shows all actions and their pixmaps if available  */
  void showActions();
   /** Name for the new created action */
  void newActionName();

private:
  bool bShown; /**< For internal use only*/
  QString m_sPixmap; /**< Name of the specified pixmap */
};

} // namespace Dialog
} // namespace Gui

#endif
